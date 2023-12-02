/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-30 16:05:09
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-02 12:59:57
 * @FilePath: /桌面/myModuo/src/EventLoopThread.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/EventLoopThread..h"
#include <memory>
#include <mutex>

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb,
                                 const std::string &name)
    : exit_(false), loop_(nullptr),
      thread_(std::bind(&EventLoopThread::threadFunc, this),
              name), // 注意这里只是注册线程函数, 名称, 并未启动线程函数
      callback_(std::move(cb)) {}
EventLoopThread::~EventLoopThread() {
  exit_ = true;
  // 不是100%没有冲突, 比如threadFunc中正运行callback_回调,
  // 然后立即析构当前对象. 此时, IO线程函数已经启动, 创建EventLoop了对象,
  // 但还没有修改loop_, 此时loop_一直为NULL 也就是说,
  // 无法通过析构让IO线程退出loop循环, 也无法连接线程.
  if (loop_ != nullptr) {
    loop_->quit();  // 退出IO线程loop循环
    thread_.join(); // 连接线程, 回收资源
  }
}
/* 启动IO线程函数中的loop循环, 返回IO线程中创建的EventLoop对象地址(栈空间) */
EventLoop *EventLoopThread::startLoop() {
  thread_.start(); // 启动底层的新线程

  EventLoop *loop = nullptr;
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (loop_ == nullptr) {
      cond_.wait(lock);
    }
    loop = loop_;
  }
  return loop;
}

// IO线程函数,在一个新开的线程中运行,作用为开启事件循环
/**
 * IO线程函数, 创建EventLoop局部对象, 运行loop循环
 */
void EventLoopThread::threadFunc() {
  EventLoop loop; // 创建一个独立的eventloop，和上面的线程是一一对应的，one loop
                  // per thread// 创建线程函数局部EventLoop对象,
                  // 只有线程函数退出, EventLoop::loop()退出时, 才会释放该对象
  // 运行线程函数初始回调
  if (callback_) {
    callback_(&loop);
  }

  {
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = &loop;
    cond_.notify_one(); // 唤醒等待在cond_条件上的线程(i.e. startLoop的调用线程)
  }

  loop.loop(); // EventLoop loop  => Poller.poll// 运行IO线程循环, 即事件循环,
               // 通常不会退出, 除非调用EventLoop::quit
  std::unique_lock<std::mutex> lock(mutex_);
  loop_ = nullptr;
}