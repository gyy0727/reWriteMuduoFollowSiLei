/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-27 15:41:21
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-03 19:50:46
 * @FilePath: /桌面/myModuo/include/socket.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "InetAddress.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>
class Socket {
public:
  explicit Socket(int sockfd) :socketfd_(sockfd){
    
  }
  ~Socket();
  int fd() { return socketfd_; }
  void bindAddress(const InetAddress &localaddr);
  void listen();
  int accept(InetAddress *peeraddr);

  void shutdownWrite();

  void setTcpNoDelay(bool on);
  void setReuseAddr(bool on);
  void setReusePort(bool on);
  void setKeepAlive(bool on);

private:
  const int socketfd_;
};