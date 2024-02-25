/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 18:38:01
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-03 17:21:51
 * @FilePath: /桌面/myModuo/src/TcpServer.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/TcpServer.h"

static EventLoop *CheckLoopNotNull(EventLoop *loop) {
  if (loop == nullptr) {
    LOG_FATAL("%s:%s:%d mainLoop is null! \n", __FILE__, __FUNCTION__,
              __LINE__);
  }
  return loop;
}
TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr,
                     const std::string &nameArg, Option option)
    : loop_(CheckLoopNotNull(loop)), ipPort_(listenAddr.toIPPORT()),
      name_(nameArg),
      acceptor_(new Acceptor(loop, listenAddr, (option == kReusePort))),
      threadPool_(new EventLoopThreadPool(loop, nameArg)), nextConnId_(1),
      started_(0) { // 当有先用户连接时，会执行TcpServer::newConnection回调
  acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this,
                                                std::placeholders::_1,
                                                std::placeholders::_2));
}
TcpServer::~TcpServer() {}

void TcpServer::setThreadNum(int numThreads) {
  threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {

  if (started_++ == 0) // 防止一个TcpServer对象被start多次
  {
    threadPool_->start(threadInitCallback_); // 启动底层的loop线程池
    loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
  }
}
/**
 * 新建一个TcpConnection对象, 用于连接管理.
 * @details 新建的TcpConnection对象会加入内部ConnectionMap.
 * @param sockfd accept返回的连接fd (accepted socket fd)
 * @param peerAddr 对端ip地址信息
 * @note 必须在所属loop线程运行
 */
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
  // 轮询算法，选择一个subLoop，来管理channel/*
  // 从EventLoop线程池中，取出一个EventLoop对象构造TcpConnection对象，便于均衡各EventLoop负责的连接数　*/
  EventLoop *ioLoop = threadPool_->getNextLoop();
  /*
   *设置连接对象名称, 包含基础名称+ip地址+端口号+连接Id
   * 因为要作为ConnectionMap的key, 要确保运行时唯一性 */
  char buf[64] = {0};
  snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;

  LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s \n",
           name_.c_str(), connName.c_str(), peerAddr.toIPPORT().c_str());

  // 通过sockfd获取其绑定的本机的ip地址和端口信息
  sockaddr_in local;
  ::bzero(&local, sizeof local);
  socklen_t addrlen = sizeof local;
  if (::getsockname(sockfd, (sockaddr *)&local, &addrlen) < 0) {
    LOG_ERROR("sockets::getLocalAddr");
  }
  InetAddress localAddr(local);

  // 根据连接成功的sockfd，创建TcpConnection连接对象
  TcpConnectionPtr conn(new TcpConnection(ioLoop, connName,
                                          sockfd, // Socket Channel
                                          localAddr, peerAddr));
  connections_[connName] = conn;
  // 下面的回调都是用户设置给TcpServer=>TcpConnection=>Channel=>Poller=>notify
  // channel调用回调
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);

  // 设置了如何关闭连接的回调   conn->shutDown()
  conn->setCloseCallback(
      std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

  // 直接调用TcpConnection::connectEstablished
  ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
  loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
  LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s\n",
           name_.c_str(), conn->name().c_str());

  connections_.erase(conn->name());
  EventLoop *ioLoop = conn->getLoop();
  ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}