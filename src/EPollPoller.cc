/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 19:06:25
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-30 14:51:03
 * @FilePath: /桌面/myModuo/src/EPollPoller.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/EPollPoller.h"
#include "../include/Channel.h"
#include "../include/Logger.h"
#include <errno.h>
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>
// channel未添加到poller中
const int kNew = -1; // channel的成员index_ = -1
// channel已添加到poller中
const int kAdded = 1;
// channel从poller中删除
const int kDeleted = 2;
// EPOLL_CLOEXEC 的具体含义是在执行 exec
// 函数时关闭该文件描述符。在多进程的环境中，如果一个进程在执行 exec
// 函数时继续保持某个文件描述符打开，而另一个进程也打开了相同的文件描述符，可能会导致一些问题。通过设置
// EPOLL_CLOEXEC 标志，可以在 exec
// 调用期间自动关闭这个文件描述符，从而避免潜在的问题。
EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize) {
  if (epollfd_ < 0) {
    LOG_FATAL("创建epoll实例失败 error:%d\n", errno);
  }
}
EPollPoller::~EPollPoller() { ::close(epollfd_); }

/* 监听函数, 调用epoll_wait() */
TimeStamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels) {
  // 发生感兴趣事件的文件描述符个数
  int numOfEvents = epoll_wait(epollfd_, &*events_.begin(),
                               static_cast<int>(events_.size()), timeoutMs);
  int saveErrno = errno;
  // 获取当前时间
  TimeStamp now(TimeStamp::now());
  if (numOfEvents > 0) {
    LOG_INFO("%d 个事件发生了感兴趣事件 \n", numOfEvents);
    fillActiveChannels(numOfEvents, activeChannels);
    if (numOfEvents == events_.size()) {
      events_.resize(events_.size() * 2);
    }
  } else if (numOfEvents == 0) {
    LOG_DEBUG("%s 监听epoll超过设定超时时间! \n", __FUNCTION__);
  } else {
    if (saveErrno != EINTR) {
      errno = saveErrno;
      LOG_ERROR("EPollPoller::poll() 发生错误非系统中断!");
    }
  }
  return now;
}

// 更新epoll监听的事件集合 /* ADD/MOD/DEL */
void EPollPoller::updateChannel(Channel *channel) {
  const int index = channel->index();
  LOG_INFO("func=%s => fd=%d events=%d index=%d \n", __FUNCTION__,
           channel->sockfd(), channel->events(), index);

  if (index == kNew || index == kDeleted) {
    if (index == kNew) {
      int fd = channel->sockfd();
      channels_[fd] = channel;
    }

    channel->set_index(kAdded);
    update(EPOLL_CTL_ADD, channel);
  } else // channel已经在poller上注册过了
  {
    int fd = channel->sockfd();
    if (channel->isNoneEvent()) {
      update(EPOLL_CTL_DEL, channel);
      channel->set_index(kDeleted);
    } else {
      update(EPOLL_CTL_MOD, channel);
    }
  }
}
// 移除已完成的事件 /* DEL */
void EPollPoller::removeChannel(Channel *channel) {
  int fd = channel->sockfd();
  channels_.erase(fd);
  LOG_INFO("事件%d已被删除\n", fd);
  int index = channel->index();
  if (index == kAdded) {
    update(EPOLL_CTL_DEL, channel);
  }
  channel->set_index(kNew);
}

/* poll返回后将就绪的fd添加到激活通道中activeChannels */
void EPollPoller::fillActiveChannels(int numEvents,
                                     ChannelList *activeChannels) const {
  for (int i = 0; i < numEvents; i++) {
    Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
    channel->setEvent(events_[i].events);
    activeChannels->push_back(channel);
  }
}
/* 由updateChannel/removeChannel调用，真正执行epoll_ctl()控制epoll的函数 */
void EPollPoller::update(int operation, Channel *channel) {
  epoll_event event;
  bzero(&event, sizeof event);
  int fd = channel->sockfd();
  event.events = channel->events();
  event.data.fd = fd;
  event.data.ptr = channel;
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    LOG_ERROR("更新操作epoll_ctl失败errno=%d\n", errno);
  } else {
    LOG_INFO("更新操作epoll_ctl成功");
  }
}
