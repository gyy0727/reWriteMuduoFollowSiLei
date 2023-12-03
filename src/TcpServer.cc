/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 18:38:01
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-03 14:04:47
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
    : loop_(CheckLoopNotNull(loop)), ipPort_(listenAddr.toIPPORT()), name_(nameArg),
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

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {}
void TcpServer::removeConnection(const TcpConnectionPtr &conn) {}
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {}
