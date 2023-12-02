/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 19:06:19
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-29 14:18:08
 * @FilePath: /桌面/myModuo/include/EPollPoller.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
/**
* IO Multiplexing with epoll(7).
*/
#include "../include/Poller.h"
#include "../include/TimeStamp.h"

#include <sys/epoll.h>
#include <vector>
class EPollPoller : public Poller {
public:
  EPollPoller(EventLoop *loop);
  ~EPollPoller() override;

  /* 监听函数, 调用epoll_wait() */
  TimeStamp poll(int timeoutMs, ChannelList *activeChannels) override;
  // 更新epoll监听的事件集合 /* ADD/MOD/DEL */
  void updateChannel(Channel *channel) override;
  // 移除已完成的事件 /* DEL */
  void removeChannel(Channel *channel) override;

private:
  /* events_数组初始大小 */
  static const int kInitEventListSize = 16;

  /* poll返回后将就绪的fd添加到激活通道中activeChannels */
  void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
  /* 由updateChannel/removeChannel调用，真正执行epoll_ctl()控制epoll的函数 */
  void update(int operation, Channel *channel);

  using EventList = std::vector<epoll_event>;
  /* epoll文件描述符，由epoll_create返回 */
  int epollfd_;
  /* epoll事件数组，为了适配epoll_wait参数要求 */
  EventList events_;
};