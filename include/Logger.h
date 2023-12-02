/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-19 19:20:27
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-11-21 14:04:34
 * @FilePath: /桌面/myModuo/src/Logger.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*************************************************************************
	> File Name: Logger.h
	> Author: muqiu0614
	> Mail: 3155833132@qq.com
	> Created Time: 2023年11月19日 星期日 19时19分42秒
 ************************************************************************/
#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include "NonCopyable.h"
using namespace std;
#define LOG_INFO(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0) 

#define LOG_ERROR(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0) 

#define LOG_FATAL(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
        exit(-1); \
    } while(0) 

#ifdef MUDEBUG
#define LOG_DEBUG(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(DEBUG); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0) 
#else
    #define LOG_DEBUG(logmsgFormat, ...)
#endif

enum LogLevel
{
	INFO, // 普通信息
	ERROR, // 错误信息
	FATAL, // core信息
	DEBUG // 调试信息

};
class Logger : NonCopyable
{
public:
	//获取唯一 的日志对象
	static Logger& instance();
	//设置日志的等级
	void setLogLevel(int level);
	//输出日志
	void log(std::string msg);

private:
	int logLevel_;
	int64_t a;
};
