#include "../include/EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop,
                                         const std::string &nameArg)
    : baseLoop_(baseLoop), name_(nameArg), started_(false), numThreads_(0),
      next_(0) {}
EventLoopThreadPool::~EventLoopThreadPool() {
  // 线程池的变量都是堆栈变量会自动释放
}
/**
 * 启动IO线程池.
 * 只能启动一次, 而且必须是baseLoop_的创建线程调用start().
 * @param cb 线程函数初始回调
 */
void EventLoopThreadPool::start(const ThreadInitCallback &cb) {
  started_ = true; // 标记线程池已启动
                   // 根据用户指定线程数, 创建IO线程组
                   /* create numThreads_ EventLoopThread, added to threads_ */
  for (int i = 0; i < numThreads_; ++i) { // 线程编号范围取决于用户指定的线程数
    char buf[name_.size() + 32];
    snprintf(buf, sizeof(buf), "%s%d", name_.c_str(),
             i); // IO线程名称: 线程池名称 + 线程编号
    EventLoopThread *t = new EventLoopThread(cb, buf);
    threads_.push_back(std::unique_ptr<EventLoopThread>(
        t)); // 将EventLoopThread对象指针 插入threads_数组
    loops_.push_back(
        t->startLoop()); // 启动IO线程, 并将线程函数创建的EventLoop对象地址
                         // 插入loops_数组
  }
  if (numThreads_ == 0 &&
      cb) { // 如果没有创建任何线程, 也会调用回调cb; 否则,
            // 会在新建的线程函数初始化完成后(进入loop循环前)调用
    cb(baseLoop_);
  }
}
/**
 * 从线程池获取下一个event loop
 * @note 默认event loop是baseLoop_ (创建baseLoop_线程,
 * 通常也是创建线程池的线程). 没有调用setThreadNum()设置numThreads_(number of
 * threads)时, numThreads_默认为０, 所有IO操作都默认交由baseLoop_的event
 * loop来完成, 因为没有其他IO线程.
 */
EventLoop *EventLoopThreadPool::getNextLoop() {
  EventLoop *loop = baseLoop_;

  // 如果loops_为空, 则loop指向baseLoop
  // 如果非空, 则按round-robin(RR,
  // 轮叫)的调度方式(从loops_列表中)选择一个EventLoop
  if (!loops_.empty()) {
    // round-robin
    loop = loops_[next_];
    ++next_;
    if (next_ >= loops_.size()) {
      next_ = 0;
    }
  }
  return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
  if (loops_.empty())
    {
        return std::vector<EventLoop*>(1, baseLoop_);
    }
    else
    {
        return loops_;
    }
}
