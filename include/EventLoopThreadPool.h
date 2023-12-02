#pragma once
#include "EventLoopThread..h"
#include "NonCopyable.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>
// 作为线程池对象，绑定了背后的系统资源，如线程，因此是引用语义（不可拷贝）
class EventLoopThreadPool : NonCopyable {
public:
  // 线程池初始化函数
  using ThreadInitCallback = std::function<void(EventLoop *)>;

  EventLoopThreadPool(EventLoop *baseLoop, const std::string &nameArg);
  ~EventLoopThreadPool();
  /* 设置线程数量, 需要在start()之前调用 */
  void setThreadNum(int numThreads) { numThreads_ = numThreads; }
  /* 启动线程池, 设置线程函数初始回调 */
  void start(const ThreadInitCallback &cb = ThreadInitCallback());
  /*
   * valid after calling start()
   * round-robin(轮询)
   */
  // 如果工作在多线程中，baseLoop_默认以轮询的方式分配channel给subloop
  EventLoop *getNextLoop();
/* 获取所有loops(EventLoop数组) */
  std::vector<EventLoop *> getAllLoops();
/* 获取线程池启动状态 */
  bool started() const { return started_; }
  /* 获取线程池名称 */
  const std::string name() const { return name_; }

private:
  // 与Acceptor所属EventLoop相同
  EventLoop *baseLoop_; // EventLoop loop;
  // 线程池名称, 通常由用户指定. 线程池中EventLoopThread名称依赖于线程池名称
  std::string name_;
  // 线程池是否启动标志
  bool started_;
  // 线程数
  int numThreads_;
  // 新连接到来，所选择的EventLoopThread下标
  int next_;
  // IO线程列表
  std::vector<std::unique_ptr<EventLoopThread>> threads_;
  // EventLoop列表, 指向的是EventLoopThread线程函数创建的EventLoop对象
  std::vector<EventLoop *> loops_;
};