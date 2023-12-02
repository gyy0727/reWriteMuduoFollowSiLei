/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-30 15:33:14
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-01 13:03:21
 * @FilePath: /桌面/myModuo/src/Thread.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/Thread.h"
#include <memory>
#include <semaphore.h>
#include <thread>
std::atomic_int Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false), joined_(false), tid_(0), name_(name),
      func_(std::move(func)) {

  setDefaultName();
}
Thread::~Thread() {
  if (started_ && joined_) {
    thread_->detach();
  }
}
void Thread::start() {
  started_ = true;
  sem_t sem;
  sem_init(&sem, false, 0);
  thread_ = std::shared_ptr<std::thread>(new std::thread([&]() -> void {
    tid_ = CurrentThread::tid();
    sem_post(&sem);
    func_();
  }));
  sem_wait(&sem); 
}
void Thread::join() {
  joined_ = true;
  thread_->join();
}

void Thread::setDefaultName() {
  int num = ++numCreated_;
  if (name_.empty()) {
    char buf[32] = {0};
    snprintf(buf, sizeof buf, "Thread%d", num);
    name_ = buf;
  }
}