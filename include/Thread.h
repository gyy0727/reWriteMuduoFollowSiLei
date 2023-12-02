/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-30 15:33:22
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-01 09:25:33
 * @FilePath: /桌面/myModuo/include/Thread.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "CurrentThreadId.h"
#include "NonCopyable.h"
#include <atomic>
#include <functional>
#include <memory>
#include <semaphore.h>
#include <string>
#include <thread>
#include <unistd.h>
class Thread : NonCopyable {
public:
  // 线程函数
  using ThreadFunc = std::function<void()>;

  explicit Thread(ThreadFunc, const std::string &name = std::string());
  ~Thread();

  void start();
  //设置等待线程执行完成
  void join();
  //开启线程
  bool started() const { return started_; }
  //获取线程id
  pid_t tid() const { return tid_; }
  //设置线程名称
  const std::string &name() const { return name_; }
  //统计线程个数
  static int numCreated() { return numCreated_; }

private:
  void setDefaultName();

  bool started_;
  bool joined_;
  std::shared_ptr<std::thread> thread_;
  pid_t tid_;
  ThreadFunc func_;
  std::string name_;
  static std::atomic_int numCreated_;
};