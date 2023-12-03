/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-26 17:38:09
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-02 17:18:13
 * @FilePath: /桌面/myModuo/include/Acceptor.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * @description: 负责处理新链接
 */
/**
 * TCP连接接受器
 * 基础调用为accept(2)/accept4(2)
 */
#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "NonCopyable.h"
#include "Socket.h"
#include <functional>
class Acceptor : NonCopyable {
public:
  using NewConnectionCallback =
      std::function<void(int socketfd, const InetAddress &addr)>;
  Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
  ~Acceptor();
  /* 设置新连接回调 */
  void setNewConnectionCallback(const NewConnectionCallback &cb);
  /* 判断当前是否正在监听端口 */
  bool listening() { return listening_; }
  /* 监听本地端口 */
  void listen();

private:
  // 处理读事件
  void handleRead();
  // 监听状态
  bool listening_;
  // 所属EventLoop
  EventLoop *loop_;
  // 专门用于接受连接的套接字(sock fd)
  Socket acceptSocket_;
  // 专门接受连接通道, 监听conn fd
  Channel acceptChannel_;
  // 新建连接回调
  NewConnectionCallback callback_;
};