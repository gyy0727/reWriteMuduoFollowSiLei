/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-12-02 19:11:23
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-02 19:11:26
 * @FilePath: /桌面/myModuo/src/DefaultPoller.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/EPollPoller.h"
#include "../include/Poller.h"

#include <stdlib.h>

Poller *Poller::newDefaultPoller(EventLoop *loop) {
  if (::getenv("MUDUO_USE_POLL")) {
    return nullptr; // 生成poll的实例
  } else {
    return new EPollPoller(loop); // 生成epoll的实例
  }
}