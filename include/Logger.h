/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-19 19:20:27
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2024-03-24 15:27:28
 * @FilePath: /桌面/myModuo/src/Logger.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*************************************************************************
        > File Name: Logger.h
        > Author: muqiu0614
        > Mail: 3155833132@qq.com
        > Created Time: 2023年11月19日 星期日 19时19分42秒
 ************************************************************************/
#pragma once
#include "../include/json.hpp"

#include "NonCopyable.h"
#include "Singleton.h"
#include "util.h"
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <ostream>
#include <sstream>
#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <vector>

class Logger;
class LoggerManager;
#define LOG_LEVEL(logger, level)                                               \
  if (logger->getLevel() <= level)                                             \
  LogEventWrap(LogEvent::LogEventPtr(new LogEvent(logger, level, __FILE__, __LINE__,   \
                                          0, GetThreadId(), 0,      \
                                          time(0), "新线程")))        \
      .getSS()

/**
 * @brief 使用流式方式将日志级别debug的日志写入到logger
 */
#define LOG_DEBUG(logger) LOG_LEVEL(logger, LogLevel::DEBUG)

/**
 * @brief 使用流式方式将日志级别info的日志写入到logger
 */
#define LOG_INFO(logger) LOG_LEVEL(logger, LogLevel::INFO)

/**
 * @brief 使用流式方式将日志级别warn的日志写入到logger
 */
#define LOG_WARN(logger) LOG_LEVEL(logger, LogLevel::WARN)

/**
 * @brief 使用流式方式将日志级别error的日志写入到logger
 */
#define LOG_ERROR(logger) LOG_LEVEL(logger, LogLevel::ERROR)

/**
 * @brief 使用流式方式将日志级别fatal的日志写入到logger
 */
#define LOG_FATAL(logger) LOG_LEVEL(logger, LogLevel::FATAL)

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 */
#define LOG_FMT_LEVEL(logger, level, fmt, ...)                                 \
  if (logger->getLevel() <= level)                                             \
  LogEventWrap(LogEvent::LogEventPtr(new LogEvent(logger, level, __FILE__, __LINE__,   \
                                          0, GetThreadId(), 0,      \
                                          time(0), "新线程")))        \
      .getEvent()                                                              \
      ->format(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 */
#define LOG_FMT_DEBUG(logger, fmt, ...)                                        \
  LOG_FMT_LEVEL(logger, LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别info的日志写入到logger
 */
#define LOG_FMT_INFO(logger, fmt, ...)                                         \
  LOG_FMT_LEVEL(logger, LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别warn的日志写入到logger
 */
#define LOG_FMT_WARN(logger, fmt, ...)                                         \
  LOG_FMT_LEVEL(logger, LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别error的日志写入到logger
 */
#define LOG_FMT_ERROR(logger, fmt, ...)                                        \
  LOG_FMT_LEVEL(logger, LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别fatal的日志写入到logger
 */
#define LOG_FMT_FATAL(logger, fmt, ...)                                        \
  LOG_FMT_LEVEL(logger, LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @brief 获取主日志器
 */
#define LOG_ROOT() return Logger::LoggerPtr logger("system")

/**
 * @brief 获取name的日志器
 */

#define LOG_NAME(name) LoggerMgr::GetInstance()->getLogger(name)
class LogLevel {
public:
  enum Level {
    /// 未知级别
    UNKNOW = 0,
    /// DEBUG 级别
    DEBUG = 1,
    /// INFO 级别
    INFO = 2,
    /// WARN 级别
    WARN = 3,
    /// ERROR 级别
    ERROR = 4,
    /// FATAL 级别
    FATAL = 5
  };
  static const char *ToString(LogLevel::Level level);
  static LogLevel::Level FromString(const std::string &str);
};

class LogEvent {
public:
  using LogEventPtr = std::shared_ptr<LogEvent>;

  LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
           const char *file, int32_t line, uint32_t elapse, uint32_t thread_id,
           uint32_t fiber_id, uint64_t time, const std::string &thread_name);

  const char *getFile() const { return m_file; }

  int32_t getLine() const { return m_line; }

  uint32_t getElapse() const { return m_elapse; }

  uint32_t getThreadId() const { return m_threadId; }

  uint32_t getFiberId() const { return m_fiberId; }

  uint64_t getTime() const { return m_time; }

  const std::string &getThreadName() const { return m_threadName; }

  std::string getContent() const { return m_ss.str(); }

  std::shared_ptr<Logger> getLogger() const { return m_logger; }

  LogLevel::Level getLevel() const { return m_level; }

  std::stringstream &getSS() { return m_ss; }

  template <typename T = const char *, typename... Arg>
  void format(T fmt, Arg... args) {
    //*  第一次调用snprintf仅获取所需的长度
    int requiredLength = snprintf(nullptr, 0, fmt, args...);

    if (requiredLength >= 0) {
      //* 创建一个足够大的缓冲区来容纳所有字符，包括终止null字符
      std::unique_ptr<char[]> buf(new char[requiredLength + 1]);

      //* 使用snprintf实际格式化字符串
      snprintf(buf.get(), requiredLength + 1, fmt, args...);

      //* 将格式化的字符串添加到字符串流
      m_ss << std::string(buf.get(), requiredLength);
    } else {
      // 处理错误情况
      throw std::runtime_error("Error formatting string.");
    }
  }

private:
  /// 文件名
  const char *m_file = nullptr;
  /// 行号
  int32_t m_line = 0;
  /// 程序启动开始到现在的毫秒数
  uint32_t m_elapse = 0;
  /// 线程ID
  uint32_t m_threadId = 0;
  /// 协程ID
  uint32_t m_fiberId = 0;
  /// 时间戳
  uint64_t m_time = 0;
  /// 线程名称
  std::string m_threadName = "";
  /// 日志内容流
  std::stringstream m_ss;
  /// 日志器
  std::shared_ptr<Logger> m_logger = nullptr;
  /// 日志等级
  LogLevel::Level m_level;
};

class LogFormatter {
public:
  typedef std::shared_ptr<LogFormatter> LogFormatterPtr;

  LogFormatter(const std::string &pattern);

  std::string format(LogEvent::LogEventPtr event);
  std::ostream &format(std::ostream &ofs, LogEvent::LogEventPtr event);

public:
  class FormatItem {
  public:
    typedef std::shared_ptr<FormatItem> FormatItemPtr;

    virtual ~FormatItem() {}

    virtual void format(std::ostream &os, LogEvent::LogEventPtr event) = 0;
  };

  void init();

  bool isError() const { return m_error; }

  const std::string getPattern() const { return m_pattern; }

private:
  /// 日志格式模板
  std::string m_pattern;
  /// 日志格式解析后格式
  std::vector<FormatItem::FormatItemPtr> m_item;
  /// 是否有错误
  bool m_error = false;
};

class LogAppender {
  friend class Logger;

public:
  typedef std::shared_ptr<LogAppender> LogAppenderPtr;

  virtual ~LogAppender() {}

  virtual void log(LogEvent::LogEventPtr event) = 0;

  // virtual std::string toJsonString() = 0;

  void setFormatter(LogFormatter::LogFormatterPtr val);
  LogFormatter::LogFormatterPtr getFormatter() const { return m_formatter; }

  bool hasFormatter() { return m_hasFormatter; }
  LogFormatter::LogFormatterPtr getFormatter();

  LogLevel::Level getLevel() const { return m_level; }

  void setLevel(LogLevel::Level val) { m_level = val; }

protected:
  /// 日志级别
  LogLevel::Level m_level = LogLevel::DEBUG;
  /// 是否有自己的日志格式器
  bool m_hasFormatter = false;
  /// Mutex
  std::mutex m_mutex;
  /// 日志格式器
  LogFormatter::LogFormatterPtr m_formatter;
};

class Logger : public std::enable_shared_from_this<Logger> {
  friend class LoggerManager;

public:
  typedef std::shared_ptr<Logger> LoggerPtr;

  Logger(const std::string &name = "root");
  void log(LogEvent::LogEventPtr event);

  void addAppender(LogAppender::LogAppenderPtr appender);

  void delAppender(LogAppender::LogAppenderPtr appender);

  void clearAppenders();

  LogLevel::Level getLevel() const { return m_level; }

  void setLevel(LogLevel::Level val) { m_level = val; }

  const std::string &getName() const { return m_name; }

  void setFormatter(LogFormatter::LogFormatterPtr val);

  void setFormatter(const std::string &val);

  LogFormatter::LogFormatterPtr getFormatter();

  // std::string toJsonString();

private:
  std::string m_name; /// 日志名称

  LogLevel::Level m_level; /// 日志级别

  std::mutex m_mutex; /// Mutex

  std::list<LogAppender::LogAppenderPtr> m_appenders; /// 日志目标集合

  LogFormatter::LogFormatterPtr m_formatter; /// 日志格式器

  Logger::LoggerPtr m_root; /// 主日志器
};

class StdoutLogAppender : public LogAppender {
public:
  typedef std::shared_ptr<StdoutLogAppender> StdoutLogAppenderPtr;
  void log(LogEvent::LogEventPtr event) override;
  // std::string toJsonString() override;
};

class FileLogAppender : public LogAppender {
public:
  typedef std::shared_ptr<FileLogAppender> FileLogAppenderPtr;
  FileLogAppender(const std::string &filename);
  void log(LogEvent::LogEventPtr event) override;
  // std::string toJsonString() override;
  bool reopen();

private:
  std::string m_filename; /// 文件路径

  std::ofstream m_filestream; /// 文件流

  uint64_t m_lastTime = 0; /// 上次重新打开时间
};

class LogEventWrap {
public:
  LogEventWrap(LogEvent::LogEventPtr e);

  ~LogEventWrap();

  LogEvent::LogEventPtr getEvent() const { return m_event; }

  std::stringstream &getSS();

private:
  LogEvent::LogEventPtr m_event;
};

class LoggerManager {
public:
  LoggerManager();

  Logger::LoggerPtr getLogger(const std::string &name);

  void init();

  Logger::LoggerPtr getRoot() const { return m_root; }

  // std::string toJsonString();

private:
  std::mutex m_mutex; /// Mutex

  std::map<std::string, Logger::LoggerPtr> m_loggers; /// 日志器容器

  Logger::LoggerPtr m_root; /// 主日志器
};

/// 日志器管理类单例模式
typedef Sylar::Singleton<LoggerManager> LoggerMgr;
static Logger::LoggerPtr logger(new Logger("system"));