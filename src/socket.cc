/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-22 15:06:35
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-27 15:41:35
 * @FilePath: /桌面/smallchat/src/socket.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/socket.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include"../include/Logger.h"
Socket::Socket() : len_(0), connectfd_(0), socketfd_(0) {}
Socket::~Socket() {
  close(socketfd_);
  close(connectfd_);
}

int Socket::createSocket() {

  this->socketfd_ = socket(AF_INET, SOCK_STREAM, 0);
  return this->socketfd_;
}

int Socket::bindSocket() {

  int ret = bind(this->socketfd_, (struct sockaddr *)&serverAddr_,
                 sizeof(serverAddr_));
  return ret;
}

int Socket::listenSocket() {

  int ret = listen(this->socketfd_, 128);
  return ret;
}

int Socket::acceptSocket() {

  struct sockaddr_in cliaddr;
  socklen_t client = sizeof(cliaddr);
  this->connectfd_ =
      accept(this->socketfd_, (struct sockaddr *)&cliaddr, &client);
  if (this->connectfd_ == -1) {
    perror("accept操作失败");
    exit(0);
  }
  // 打印客户端的地址信息
  char ip[24] = {0};
  printf("客户端的IP地址: %s, 端口: %d\n",
         inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
         ntohs(cliaddr.sin_port));
  return this->connectfd_;
}

int Socket::readSocket(char *buf) {

  int len = ::read(this->connectfd_, buf, sizeof(buf));

  if (len > 0) {
    printf("客户端say: %s\n", buf);
    memset(buf, 0, sizeof(buf));
    std::cin >> buf;
    writeSocket(buf);

  } else if (len == 0) {
    printf("客户端断开了连接...\n");

  } else {
    perror("read");
  }
  return len;
}

void Socket::writeSocket(char *buf) {

  write(this->connectfd_, buf, sizeof(buf));
  printf("write success\n");
}

void Socket::setServerAddr(sockaddr_in temp) {

  serverAddr_.sin_addr = temp.sin_addr;
  serverAddr_.sin_port = temp.sin_port;
  serverAddr_.sin_family = temp.sin_family;
}