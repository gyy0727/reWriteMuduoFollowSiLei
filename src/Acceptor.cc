/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-26 17:37:55
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-02 17:14:01
 * @FilePath: /桌面/myModuo/src/Acceptor.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/Acceptor.h"
#include <utility>
// 创建一个非阻塞文件描述符
static int createNonblocking() {
  int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  if (sockfd < 0) {
    LOG_FATAL("%s:%s:%d listen socket create err:%d \n", __FILE__, __FUNCTION__,
              __LINE__, errno);
  }
  return sockfd;
}
// TcpServer::start() Acceptor.listen
// 有新用户的连接，要执行一个回调（connfd=》channel=》subloop） baseLoop =>
// acceptChannel_(listenfd) =>
Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr,
                   bool reuseport)
    : listening_(false), loop_(loop), acceptSocket_(createNonblocking()),
      acceptChannel_(loop, acceptSocket_.fd()) {
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.setReusePort(true);
  acceptSocket_.bindAddress(listenAddr);
  acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}
Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}
/* 设置新连接回调 */
void Acceptor::setNewConnectionCallback(const NewConnectionCallback &cb) {
  callback_=std::move(cb);
}

void Acceptor::listen() {
  
    listening_ = true;
    acceptSocket_.listen(); // listen
    acceptChannel_.enableReading(); // acceptChannel_ => Poller
}

// 处理读事件
void Acceptor::handleRead() {
  InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0)
    {
        if (callback_)
        {
            callback_(connfd, peerAddr); // 轮询找到subLoop，唤醒，分发当前的新客户端的Channel
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        LOG_ERROR("%s:%s:%d accept err:%d \n", __FILE__, __FUNCTION__, __LINE__, errno);
        if (errno == EMFILE)
        {
            LOG_ERROR("%s:%s:%d sockfd reached limit! \n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}