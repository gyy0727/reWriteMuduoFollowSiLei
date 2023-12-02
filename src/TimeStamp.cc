/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 13:32:05
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-21 13:53:01
 * @FilePath: /桌面/myModuo/src/TimeStamp.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/TimeStamp.h"
#include <cstddef>
#include <cstdint>
#include <string>
//默认构造
TimeStamp::TimeStamp() : microSecondsSinceEpoch_(0) {}
//赋值构造
TimeStamp::TimeStamp(int64_t microSecondsSinceEpoch_)
    : microSecondsSinceEpoch_(microSecondsSinceEpoch_) {}
//获取当前时间
TimeStamp TimeStamp::now() { return TimeStamp(time(NULL)); }
//格式化输出当前时间
std::string TimeStamp::toString() const {
  char buf[128] = {0};

  tm *tm_time = localtime(&microSecondsSinceEpoch_);
  snprintf(buf, 128, "%4d/%02d/%02d %02d:%02d:%02d", tm_time->tm_year + 1900,
           tm_time->tm_mon + 1, tm_time->tm_mday, tm_time->tm_hour,
           tm_time->tm_min, tm_time->tm_sec);
  return buf;
}