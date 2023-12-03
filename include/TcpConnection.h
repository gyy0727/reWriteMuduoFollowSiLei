/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-12-02 18:58:13
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-03 15:28:37
 * @FilePath: /桌面/myModuo/include/TcpConnection.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "Buffer.h"
#include "Callback.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "NonCopyable.h"
#include "Socket.h"
#include "TimeStamp.h"
#include <atomic>
#include <memory>
#include <string>
/**
 * Tcp连接, 为服务器和客户端使用.
 * 接口类, 因此不要暴露太多细节.
 *
 * @note 继承自std::enable_shared_from_this的类,
 * 可以用getSelf返回用std::shared_ptr管理的this指针
 */
class TcpConnection : NonCopyable,
                      public std::enable_shared_from_this<TcpConnection> {
public:
  TcpConnection(EventLoop *loop, const std::string &name, int sockfd,
                const InetAddress &localAddr, const InetAddress &peerAddr);
  ~TcpConnection();

  EventLoop *getLoop() const { return loop_; }
  const std::string &name() const { return name_; }
  const InetAddress &localAddress() const { return localAddr_; }
  const InetAddress &peerAddress() const { return peerAddr_; }

  bool connected() const { return state_ == kConnected; }

  // 发送数据
  /**
   * 转交给 sendInLoop(const char*, int)
   * 发送消息给对端, 允许在其他线程调用
   * @param message 要发送的消息. StringPiece兼容C/C++风格字符串, 二进制缓存,
   * 提供统一字符串接口
   */
  void send(const std::string &buf);
  // 关闭连接
  void shutdown();

  void setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
  }

  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback &cb) {
    writeCompleteCallback_ = cb;
  }

  void setHighWaterMarkCallback(const HighWaterMarkCallback &cb,
                                size_t highWaterMark) {
    highWaterMarkCallback_ = cb;
    highWaterMark_ = highWaterMark;
  }

  void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }

  // 连接建立
  void connectEstablished();
  // 连接销毁
  void connectDestroyed();

private:
  enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
  void setState(StateE state) { state_ = state; }

  void handleRead(TimeStamp receiveTime); // 处理读事件
  void handleWrite();                     // 处理写事件
  void handleClose();                     // 处理关闭连接事件
  void handleError();                     // 处理错误事件
  // 转交给sendInLoop(const char*, int)
  void sendInLoop(const void *data, size_t len);
  void shutdownInLoop();

  EventLoop
      *loop_; // 这里绝对不是baseLoop， 因为TcpConnection都是在subLoop里面管理的
              // // 所属　EventLoop
  const std::string name_;
  std::atomic_int state_;
  bool reading_; // whether the connection is reading // 连接是否正在监听读事件
                 // we don't expose those classes to client.

  // 这里和Acceptor类似   Acceptor=》mainLoop    TcpConenction=》subLoop
  std::unique_ptr<Socket> socket_; // 连接套接字, 用于对连接进行底层操作
  std::unique_ptr<Channel> channel_; // 通道, 用于绑定要监听的事件

  const InetAddress localAddr_; // 本地IP地址
  const InetAddress peerAddr_;  // 对端IP地址

  ConnectionCallback connectionCallback_;       // 有新连接时的回调
  MessageCallback messageCallback_;             // 有读写消息时的回调
  WriteCompleteCallback writeCompleteCallback_; // 消息发送完成以后的回调
  HighWaterMarkCallback highWaterMarkCallback_; // 水位控制回调
  CloseCallback closeCallback_;
  size_t highWaterMark_; // 设置高水位控制,高水位阈值

  Buffer inputBuffer_;  // 接收数据的缓冲区
  Buffer outputBuffer_; // 发送数据的缓冲区
};