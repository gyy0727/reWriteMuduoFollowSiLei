/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 16:04:55
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-02 18:40:07
 * @FilePath: /桌面/myModuo/src/Channel.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "../include/Channel.h"

const int Channel::kNoneEvent_ = 0;
const int Channel::kReadEvent_ = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent_ = EPOLLOUT;
Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false) {}
Channel::~Channel() {}
/* 处理事件, 监听事件激活时, 由EventLoop::loop调用 */
/**
 * 处理激活的Channel事件
 * @details Poller中监听到激活事件的Channel后, 将其加入激活Channel列表,
 * EventLoop::loop根据激活Channel回调对应事件处理函数.
 * @param recevieTime Poller中调用epoll_wait/poll返回后的时间.
 * 用户可能需要该参数.
 */
void Channel::handleEvent(TimeStamp receiveTime) {

  if (tied_) {
    /*
     * shared_ptr通过RAII方式管理对象资源guard
     * weak_ptr::lock可将weak_ptr提升为shared_ptr, 引用计数+1
     */
    /*
     * 为什么使用 tie?
     * 确保在执行事件处理动作时, 所需的对象不会被释放, 但又不能用shared_ptr,
     * 否则可能导致循环引用. 最好使用weak_ptr, 然后lock提升为shared_ptr,
     * 这样更安全.
     */
    std::shared_ptr<void> guard = tie_.lock();
    if (guard) {
      handleEventWithGuard(receiveTime);
    }
  } else {
    //tied_为false,代表和tcpconnection没关联,不用担心tcpconnection在执行回调函数时被释放
    handleEventWithGuard(receiveTime);
  }
}

void Channel::tie(const std::shared_ptr<void> &obj) {
  tie_ = obj;
  tied_ = true;
}

void Channel::remove() { loop_->removeChannel(this); }

void Channel::update() { loop_->updateChannel(this); }
void Channel::handleEventWithGuard(TimeStamp receiveTime) {
   LOG_INFO("channel handleEvent revents:%d\n", revents_);

    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if (closeCallback_)
        {
            closeCallback_();
        }
    }

    if (revents_ & EPOLLERR)
    {
        if (errorCallback_)
        {
            errorCallback_();
        }
    }

    if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        if (readCallback_)
        {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & EPOLLOUT)
    {
        if (writeCallback_)
        {
            writeCallback_();
        }
    }
}