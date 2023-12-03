/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 16:02:19
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-02 19:00:38
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
  // 除了读事件, 用于其他事件(如写/关闭/错误)回调类型
  using EventCallback = std::function<void()>;
  // 读事件回调类型
  using ReadEventCallback = std::function<void(TimeStamp)>;
  Channel(EventLoop *loop, int fd);
  ~Channel();
  
  void handleEvent(TimeStamp recevieTime);
  /* 设置事件回调，由Channel对象持有者配置Channel事件回调时调用 */
  void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
  void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
  void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
  void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

  /*
将shared_ptr管理的对象系到本地weak_ptr管理的tie_,
可用于保存TcpConnection指针
一个TcpConnection新连接创建的时候 TcpConnection => Channel
*/
  void tie(const std::shared_ptr<void> &obj);

  int sockfd() const { return fd_; }
  int events() const { return events_; }
  void setEvent(int revt) { revents_ = revt; } // used by poller
  //    int revents() const { return revents_; }
  bool isNoneEvent() const { return events_ == kNoneEvent_; }

  /* 使能/禁用 监听 可读/可写事件, 会影响Poller监听的通道列表 */
  void enableReading() {
    events_ |= kReadEvent_;
    update();
  }
  void disableReading() {
    events_ &= ~kReadEvent_;
    update();
  }
  void enableWriting() {
    events_ |= kWriteEvent_;
    update();
  }
  void disableWriting() {
    events_ &= ~kWriteEvent_;
    update();
  }
  void disableAll() {
    events_ = kNoneEvent_;
    update();
  }
  /* 判断是否请求监听 可写事件 */
  bool isWriting() const { return events_ & kWriteEvent_; }
  /* 判断是否请求监听 可读事件 */
  bool isReading() const { return events_ & kReadEvent_; }

  // for Poller
  int index() { return index_; }
  void set_index(int idx) { index_ = idx; }

  EventLoop *ownerLoop() { return loop_; }
  /* 从EventLoop中移除当前通道.
   * 建议在移除前禁用所有事件
   */
  void remove();

private:
  /* 将fd对应事件转化为字符串 */
  static string eventsToString(int fd, int ev);
  /* update()将调用EventLoop::updateChannel更新监听的通道 */
  void update();
  /* 根据不同的事件源激活不同的回调函数，来处理事件 */
  void handleEventWithGuard(TimeStamp receiveTime);
  // 由轮询器使用
  // PollPoller：映射到fd_的poll fds数组的索引
  // EPollPoller：对fd的操作类型：kNew、kAdded、kDeleted
  int index_;
  static const int kNoneEvent_;
  static const int kReadEvent_;
  static const int kWriteEvent_;
  EventLoop *loop_; // 定义channel所属的事件循环
  const int fd_;    // 文件描述符
  int events_;      // 感兴趣事件
  int revents_;     // 实际发生的事件
  /* *
   *使用weak_ptr指向shared_ptr所指对象, 防止循环引用.
   * 通常是生命周期不确定的对象, 如TcpConnection */
  std::weak_ptr<void> tie_;
  bool tied_;          /* weak_ptr tie_绑定对象的标志 */
  bool eventHandling_; /* 正在处理事件的标志 */
  bool addedToLoop_;   /* 加入到loop中, 被监听/处理的标志 */
  ReadEventCallback readCallback_; /* 可读事件回调 */
  EventCallback writeCallback_;    /* 可写事件回调 */
  EventCallback closeCallback_;    /* 关闭事件回调 */
  EventCallback errorCallback_;    /* 错误事件回调 */
};
