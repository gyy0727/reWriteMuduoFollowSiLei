/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 18:55:39
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-02 19:10:34
 * @FilePath: /桌面/myModuo/include/Poller.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
/**
 * IO Multiplexing Interface
 * Support poll(2), epoll(7)
 *
 * Only owner EventLoop IO thread can invoke it, so thread safe is not
 * necessary.
 */
/**
 * IO复用接口
 * 禁止编译器生成copy构造函数和copy assignment
 * 支持poll(2), epoll(7)
 */
#include "../include/EventLoop.h"
#include "../include/NonCopyable.h"
#include "../include/TimeStamp.h"
#include "Channel.h"
#include <unordered_map>
#include <vector>
class Channel;
class EventLoop;
// muduo库中多路事件分发器的核心IO复用模块
class Poller : NonCopyable {
public:
  // 活跃事件的集合,即监听到发生感兴趣事件的集合
  using ChannelList = std::vector<Channel *>;

  Poller(EventLoop *loop);
  virtual ~Poller() = default;

  // Polls the I/O events.
  //  Must be called in the loop thread.
  // poll(2) for PollPoller, epoll_wait(2) for EPollPoller
  // 监听函数，根据激活的通道列表，监听指定fd的相应事件
  // 对于PollPoller会调用epoll_wait(2), 对于EPollPoller会调用poll(2)
  // 给所有IO复用保留统一的接口
  // 返回调用完epoll_wait/poll的当前时间（Timestamp对象）
  virtual TimeStamp poll(int timeoutMs, ChannelList *activeChannels) = 0;

  // Update channel listened 更新监听通道的事件
  virtual void updateChannel(Channel *channel) = 0;
   // 删除监听通道
  virtual void removeChannel(Channel *channel) = 0;

  // /* 判断当前Poller对象是否持有指定通道 */
  bool hasChannel(Channel *channel) const;

  // EventLoop可以通过该接口获取默认的IO复用的具体实现/* 默认创建Poller对象的类函数 */
  static Poller *newDefaultPoller(EventLoop *loop);

protected:
  // map的key：sockfd  value：sockfd所属的channel通道类型,该类型保存fd和需要监听的events，以及各种事件回调函数（可读/可写/错误/关闭等）
  using ChannelMap = std::unordered_map<int, Channel *>;
  ChannelMap channels_; // 要监听的事件的集合,保存所有事件的Channel，一个Channel绑定一个fd

private:
  EventLoop *ownerLoop_; // 定义Poller所属的事件循环EventLoop,事件驱动循环, 用于调用poll监听fd事件
};