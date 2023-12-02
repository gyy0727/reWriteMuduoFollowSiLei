/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 16:02:19
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-30 09:36:04
 * @FilePath: /桌面/myModuo/include/Channel.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "EventLoop.h"
#include "NonCopyable.h"
#include "TimeStamp.h"
#include <functional>
#include <memory>
#include <sys/epoll.h>
class EventLoop;

/**
 * 理清楚  EventLoop、Channel、Poller之间的关系   《= Reactor模型上对应
 * Demultiplex Channel
 * 理解为通道，封装了sockfd和其感兴趣的event，如EPOLLIN、EPOLLOUT事件
 * 还绑定了poller返回的具体事件
 */
class Channel : NonCopyable {
public:
  // 回调操作
  using EventCallback = std::function<void()>;
  // 回调
  using ReadEventCallback = std::function<void(TimeStamp)>;

  Channel(EventLoop *mainLoop, int fd);
  ~Channel();
  // fd得到poller通知以后，处理事件,handleEvent() 会调用相应的 callback 来处理。
  void handleEvent(TimeStamp receiveTime);
  // 设置回调函数对象
  void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
  void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
  void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
  void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }
  int setEvent(int event);
  bool isNoneEvent() const { return event_ == kNoneEvent_; }
  bool isWriting() const { return event_ & kWriteEvent_; }
  bool isReading() const { return event_ & kReadEvent_; }

  // 设置当前事件为可读
  void enableReading() {
    event_ |= kReadEvent_;
    update();
  }
  // 设置当前事件为不可读
  void disableReading() {
    event_ &= ~kReadEvent_;
    update();
  }
  // 设置当前事件为可写
  void enableWriting() {
    event_ |= kWriteEvent_;
    update();
  }
  // 设置当前事件为不可写
  void disableWriting() {
    event_ &= ~kWriteEvent_;
    update();
  }
  // 设置当前事件为不可读写
  void disableAll() {
    event_ = kNoneEvent_;
    update();
  }
  int sockfd() const { return sockfd_; }
  int events() const { return event_; }
  int set_revents(int revt) {
    revent_ = revt;
    return revent_;
  }
  // 实际上执行回调函数的函数
  //  返回当前channel所属的事件循环
  EventLoop *ownerLoop() { return loop_; }
  int index() { return index_; }
  void set_index(int idx) { index_ = idx; }
  void remove();

private:
  void handleEventWithGuard(TimeStamp receiveTime);

  int index_;                      // 给poller操作
  void update();                   // 更新channel
  const int sockfd_;               // 当前channel所封装文件描述符
  EventLoop *loop_;                // 当前channel所属的事件循环
  int event_;                      // 当前channel想监听的事件
  int revent_;                     // channel实际发生的事件
  std::weak_ptr<void> tie_;        // 防止循环引用,指向TcpConnection
  bool tied_;                      // 标志tie_是否指向了TcpConnection
  ReadEventCallback readCallback_; /* 可读事件回调 */
  EventCallback writeCallback_;    /* 可写事件回调 */
  EventCallback closeCallback_;    /* 关闭事件回调 */
  EventCallback errorCallback_;    /* 错误事件回调 */
  static const int kReadEvent_;    // 要epoll监听读事件
  static const int kWriteEvent_;   // 要epoll监听写事件
  static const int kNoneEvent_;    // 不需要epoll监听任何事件
};
