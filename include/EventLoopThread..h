/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-30 16:05:15
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-02 12:26:05
 * @FilePath: /桌面/myModuo/include/EventLoopThread..h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "EventLoop.h"
#include "NonCopyable.h"
#include "Thread.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>

class EventLoop;

class EventLoopThread : NonCopyable {
public:
  using ThreadInitCallback = std::function<void(EventLoop *)>;
  EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                  const std::string &name = std::string());
  ~EventLoopThread();
  /* 启动IO线程函数中的loop循环, 返回IO线程中创建的EventLoop对象地址(栈空间) */
  EventLoop *startLoop();

private:
  // IO线程函数
  void threadFunc();
  EventLoop *loop_ ;
  bool exit_;
  // 线程, 用于实现IO线程中的线程功能
  Thread thread_;
  std::mutex mutex_;
   // 条件变量
  std::condition_variable cond_;
   // 线程函数初始回调
  ThreadInitCallback callback_;
};