/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 14:40:05
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-21 16:01:02
 * @FilePath: /桌面/myModuo/src/InetAddress.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/InetAddress.h"
#include <arpa/inet.h>
#include <cstdint>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
InetAddress::InetAddress(uint16_t port, std::string ip) {}

std::string InetAddress::toIP() const {
  char buf[64] = {0};
  ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  return buf;
}
std::string InetAddress::toIPPORT() const {
  char buf[128];
  ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
  size_t end = strlen(buf);
  uint16_t port = ntohs(addr_.sin_port);
  sprintf(buf + end, ":%u", port);
  return buf;
}
uint16_t InetAddress::toPORT() const {
  return ::ntohs(addr_.sin_port);
}
