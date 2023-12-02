/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 16:04:55
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-29 13:53:52
 * @FilePath: /桌面/myModuo/src/Channel.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "../include/Channel.h"

const int Channel::kNoneEvent_ = 0;
const int Channel::kReadEvent_ = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent_ = EPOLLOUT;
Channel::Channel(EventLoop *mainLoop, int fd)
    : sockfd_(fd), loop_(mainLoop), event_(kNoneEvent_), revent_(kNoneEvent_),
      index_(-1) {}
Channel::~Channel() {}
void Channel::update() {
  // 通过channel所属的EventLoop，调用poller的相应方法，注册fd的events事件
  loop_->updateChannel(this);
}
void Channel::remove() {
  
}
void Channel::handleEvent(TimeStamp receiveTime) {}
void Channel::handleEventWithGuard(TimeStamp receiveTime) {
  
  }
