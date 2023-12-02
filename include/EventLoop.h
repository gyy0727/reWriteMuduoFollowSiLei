/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 17:07:19
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-30 12:50:20
 * @FilePath: /桌面/myModuo/include/EventLoop.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <sys/eventfd.h>
#include <unistd.h>

#include "Channel.h"
#include "CurrentThreadId.h"
#include "NonCopyable.h"
#include "TimeStamp.h"
/**
 * Reactor模式， 每个线程最多一个EventLoop (One loop per thread).
 * 接口类, 不要暴露太多细节给客户
 */
class Channel;
class Poller;
class EventLoop : NonCopyable {
public:
  using Functor = std::function<void()>;

  EventLoop();
  // force out-line dtor, for std::unique_ptr members.
  ~EventLoop();

  // 开启事件循环 /* loop循环, 运行一个死循环.
  // 必须在当前对象的创建线程中运行
  void loop();
  // 退出事件循环
  /*
   * 退出loop循环.
   * 如果通过原始指针(raw pointer)调用, 不是100%线程安全;
   * 为了100%安全, 最好通过shared_ptr<EventLoop>调用
   */
  void quit();
  /*
   * Poller::poll()返回的时间, 通常意味着有数据达到.
   * 对于PollPoller, 是调用完poll(); 对于EPollPoller, 是调用完epoll_wait()
   */
  TimeStamp pollReturnTime() const { return pollReturnTime_; }

  // 在当前loop中执行cb
  /*
   * 在loop线程中, 立即运行回调cb.
   * 如果没在loop线程, 就会唤醒loop, (排队)运行回调cb.
   * 如果用户在同一个loop线程, cb会在该函数内运行; 否则，
   * 会在loop线程中排队运行. 因此, 在其他线程中调用该函数是安全的.
   */
  void runInLoop(Functor cb);
  // 把cb放入队列中，唤醒loop所在的线程，执行cb
  /* 排队回调cb进loop线程.
   * 回调cb在loop中完成polling后运行.
   * 从其他线程调用是安全的.
   */
  void queueInLoop(Functor cb);

  // 用来唤醒loop所在的线程的
  /* 唤醒loop线程, 没有事件就绪时, loop线程可能阻塞在poll()/epoll_wait() */
  void wakeup();

  // EventLoop的方法 =》 Poller的方法
  /* 更新Poller监听的channel, 只能在channel所属loop线程中调用 */
  void updateChannel(Channel *channel);
  /* 移除Poller监听的channel, 只能在channel所属loop线程中调用 */
  void removeChannel(Channel *channel);
  /* 判断Poller是否正在监听channel, 只能在channel所属loop线程中调用 */
  bool hasChannel(Channel *channel);

  // 判断EventLoop对象是否在自己的线程里面
  /* 判断前线程是否创建当前对象的线程.
   * threadId_是创建当前EventLoop对象时, 记录的线程tid
   */
  bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
  /* 唤醒所属loop线程, 也是wakeupFd_的事件回调 */
  // wake up
  void handleRead();
  // /* 处理pending函数 */
  // 执行回调
  void doPendingFunctors();

  using ChannelList = std::vector<Channel *>;

  std::atomic_bool
      looping_; // 原子操作，通过CAS实现的 /* atomic, true表示loop循环执行中 */
  std::atomic_bool quit_; // 标识退出loop循环

  const pid_t
      threadId_; // 记录当前loop所在线程的id/* 线程id, 对象构造时初始化 */

  TimeStamp pollReturnTime_; // poller返回发生事件的channels的时间点 /*
                             // poll()返回时间点 */
  std::unique_ptr<Poller> poller_; /* 轮询器, 用于监听事件 */
  /* 唤醒loop线程的eventfd */ // 主要作用，当mainLoop获取一个新用户的channel，通过轮询算法选择一个subloop，通过该成员唤醒subloop处理channel

  int wakeupFd_;
  /* 用于唤醒loop线程的channel.
   * 不像TimerQueue是内部类, 不应该暴露Channel给客户. */
  std::unique_ptr<Channel> wakeupChannel_;
  /* 临时辅助变量 */
  /* 激活事件的通道列表 */
  ChannelList activeChannels_;
  /* atomic, true表示loop循环正在调用pending函数 */
  std::atomic_bool
      callingPendingFunctors_; // 标识当前loop是否有需要执行的回调操作
  std::vector<Functor> pendingFunctors_; // 存储loop需要执行的所有的回调操作
  std::mutex mutex_; // 互斥锁，用来保护上面vector容器的线程安全操作
};