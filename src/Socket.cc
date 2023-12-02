/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-22 15:06:35
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-02 16:56:07
 * @FilePath: /桌面/smallchat/src/socket.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/Socket.h"
#include "../include/Logger.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
Socket::~Socket() { ::close(socketfd_); }

void Socket::bindAddress(const InetAddress &localaddr) {
  if (0 != ::bind(socketfd_, (sockaddr *)localaddr.getSockAddr(),
                  sizeof(sockaddr_in))) {
    LOG_FATAL("bind socketfd:%d 失败 \n", socketfd_);
  }
}
void Socket::listen() {
  if (0 != ::listen(socketfd_, 1024)) {
    LOG_FATAL("listen sockfd:%d 失败 \n", socketfd_);
  }
}
int Socket::accept(InetAddress *peeraddr) {
  /**
   * 1. accept函数的参数不合法
   * 2. 对返回的connfd没有设置非阻塞
   * Reactor模型 one loop per thread
   * poller + non-blocking IO
   */
  sockaddr_in addr;
  socklen_t len = sizeof addr;
  bzero(&addr, sizeof addr);
  int connfd = ::accept4(socketfd_, (sockaddr *)&addr, &len,
                         SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd >= 0) {
    peeraddr->setSockAddr(addr);
  }
  return connfd;
}
/*  关闭写入。*/
void Socket::shutdownWrite() {
  if (::shutdown(socketfd_, SHUT_WR) < 0) {
    LOG_ERROR("关闭写入端 error");
  }
}
/*    作用：启用或禁用Nagle算法。Nagle算法是一种用于减少小数据报文传输的算法，启用该选项后可以减小网络传输的延迟，但可能增加网络带宽的使用。
    参数：on为布尔值，表示是否启用TCP无延迟算法。*/
void Socket::setTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(socketfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}
/*    作用：设置地址重用选项。允许多个套接字绑定到相同的地址和端口，即使之前的套接字仍然存在。这在一些特定情况下是有用的，例如当一个套接字关闭后，但是还有数据在传输时。
    参数：on为布尔值，表示是否启用地址重用选项。*/
void Socket::setReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(socketfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}
/*    作用：设置端口重用选项。允许多个套接字绑定到相同的地址和端口，但每个套接字必须使用不同的本地地址。这对于实现负载均衡和高并发服务器很有用。
    参数：on为布尔值，表示是否启用端口重用选项。*/
void Socket::setReusePort(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(socketfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);
}
/*作用：启用或禁用TCP保活机制。TCP保活用于检测长时间没有活动的连接，如果连接处于空闲状态，就会发送一些数据以保持连接的活跃性。
参数：on为布尔值，表示是否启用TCP保活机制*/
void Socket::setKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(socketfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof optval);
}