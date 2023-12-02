/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-19 19:55:11
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-21 14:05:17
 * @FilePath: /桌面/myModuo/src/Logger.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE"
 */
#include "../include/Logger.h"
#include "../include/TimeStamp.h"
Logger &Logger::instance() {
  static Logger LOGGER;
  return LOGGER;
}
//设置日志的等级
void Logger::setLogLevel(int level) { logLevel_ = level; }
//输出日志
void Logger::log(std::string msg) {
  switch (logLevel_) {
  case INFO:
    std::cout << "[INFO]";
    break;
  case ERROR:
    std::cout << "[ERROR]";
    break;
  case FATAL:
    std::cout << "[FATAL]";
    break;
  case DEBUG:
    std::cout << "[DEBUG]";
    break;
  default:
    break;
  }

  // 打印时间和msg
  std::cout << TimeStamp::now().toString() << " : " << msg << std::endl;
}
