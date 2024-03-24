/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-19 19:55:11
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2024-03-23 19:17:31
 * @FilePath: /桌面/myModuo/src/Logger.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE"
 */
#include "../include/Logger.h"
#include "../include/TimeStamp.h"
#include <map>
const char *LogLevel::ToString(LogLevel::Level level) {
  switch (level) {
  case LogLevel::DEBUG:
    return "DEBUG";
    break;
  case LogLevel::INFO:
    return "INFO";
    break;
  case LogLevel::WARN:
    return "WARN";
    break;
  case LogLevel::ERROR:
    return "ERROR";
    break;
  case LogLevel::FATAL:
    return "FATAL";
    break;
  default:
    return "UNKONW";
  }
  return "UNKONW";
}
LogLevel::Level LogLevel::FromString(const std::string &str) {
#define XX(level, v)                                                           \
  if (str == #v) {                                                             \
    return LogLevel::level;                                                    \
  }
  XX(DEBUG, debug);
  XX(INFO, info);
  XX(WARN, warn);
  XX(ERROR, error);
  XX(FATAL, fatal);

  XX(DEBUG, DEBUG);
  XX(INFO, INFO);
  XX(WARN, WARN);
  XX(ERROR, ERROR);
  XX(FATAL, FATAL);
  return LogLevel::UNKNOW;
#undef XX
}

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
                   const char *file, int32_t line, uint32_t elapse,
                   uint32_t thread_id, uint32_t fiber_id, uint64_t time,
                   const std::string &thread_name)
    : m_file(file), m_line(line), m_elapse(elapse), m_threadId(thread_id),
      m_fiberId(fiber_id), m_time(time), m_threadName(thread_name),
      m_logger(logger), m_level(level) {
  //*初始化各种变量
  m_logger->setLevel(level);
}

LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern) {
  init(); //*解析字符串
}

class MessageFormatItem : public LogFormatter::FormatItem {
public:
  MessageFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << event->getContent();
  }
};

std::stringstream &LogEventWrap::getSS() { return m_event->getSS(); }

class LevelFormatItem : public LogFormatter::FormatItem {
public:
  LevelFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << LogLevel::ToString(event->getLevel());
  }
};
class ElapseFormatItem : public LogFormatter::FormatItem {
public:
  ElapseFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << event->getElapse();
  }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
  NameFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << event->getLogger()->getName();
  }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
  ThreadIdFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << event->getThreadId();
  }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
  FiberIdFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << event->getFiberId();
  }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
  ThreadNameFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << event->getThreadName();
  }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
  DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
      : m_format(format) {
    if (m_format.empty()) {
      m_format = "%Y-%m-%d %H:%M:%S";
    }
  }

  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    struct tm tm;
    time_t time = event->getTime();
    localtime_r(&time, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), m_format.c_str(), &tm);
    os << buf;
  }

private:
  std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
public:
  FilenameFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << event->getFile();
  }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
  LineFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << event->getLine();
  }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:
  NewLineFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << std::endl;
  }
};

class StringFormatItem : public LogFormatter::FormatItem {
public:
  StringFormatItem(const std::string &str) : m_string(str) {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << m_string;
  }

private:
  std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
  TabFormatItem(const std::string &str = "") {}
  void format(std::ostream &os, LogEvent::LogEventPtr event) override {
    os << "\t";
  }

private:
  std::string m_string;
};
std::string LogFormatter::format(LogEvent::LogEventPtr event) {
  std::stringstream ss;
  for (auto &i : m_item) {
    i->format(ss, event);
  }
  return ss.str();
}
std::ostream &LogFormatter::format(std::ostream &ofs,
                                   LogEvent::LogEventPtr event) {
  for (auto &i : m_item) {
    i->format(ofs, event);
  }
  return ofs;
}

void LogFormatter::init() {
  //*存储解析结果[格式化文本(如%d里面的d),解析出的内容
  // *(如"%Y-%m-%d %H:%M:%S"),是否是需要解析的内容(如"["等则为0)]
  std::vector<std::tuple<std::string, std::string, int>> vec;
  std::string nstr; //*非格式化文本如 "["
  for (size_t i = 0; i < m_pattern.size(); ++i) {
    //*检查第一个字符,不是%则可以确定为用户提供的非格式化文本,如[]
    if (m_pattern[i] != '%') {
      nstr.append(1, m_pattern[i]);
      continue;
    }
    //*走到这一步,证明第一个字符是%,检查第二个字符
    if (i + 1 < m_pattern.size()) {
      if (m_pattern[i + 1] == '%') {
        nstr.append(1, '%');
        vec.push_back(std::make_tuple(nstr, std::string(), 0));
        nstr.clear();
        i = i + 1;
        continue;
      }
    }
    size_t n = i + 1;
    int fmt_status = 0;
    size_t fmt_begin = 0;

    std::string str; //*存储格式化文本,如%d里面的d
    std::string fmt; //*存储%Y-%m-%d %H:%M:%S
    //*while函数负责解析字符串
    if (n < m_pattern.size()) {
      if (isalpha(m_pattern[n])) {
        str.append(1, m_pattern[n]);
        if (n + 1 < m_pattern.size() && !fmt_status &&
            m_pattern[n + 1] == '{') {
          fmt_status = 1;
          size_t m;
          m = n + 2;
          fmt_begin = m;
          while (m <= m_pattern.size()) {
            if (m_pattern[m] == '}') {
              fmt = m_pattern.substr(fmt_begin, m - fmt_begin);
              fmt_status = 0;
              i = m - 1;
              break;
            } else {
              m++;
            }
          }
          if (fmt_status == 1) {
            m_error = true;
            std::cout << "pattern parse error: " << m_pattern << " - "
                      << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple(str, "<<pattern_error>>", 1));
            if (!nstr.empty()) {
              vec.push_back(std::make_tuple(nstr, std::string(), 0));
              nstr.clear();
            }
            if (!str.empty() && !fmt.empty()) {
              vec.push_back(std::make_tuple(str, "error", 1));
              fmt.clear();
              str.clear();
            }
            // vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            i = i + 1;
            continue;
          }
        }
      }
    }
    i++;

    if (!nstr.empty()) {
      vec.push_back(std::make_tuple(nstr, std::string(), 0));
      nstr.clear();
    }
    if (!str.empty() && !fmt.empty()) {
      vec.push_back(std::make_tuple(str, fmt, 1));
      fmt.clear();
      str.clear();
    }
    if (!str.empty() && fmt.empty()) {
      vec.push_back(std::make_tuple(str, std::string(), 1));
      fmt.clear();
      str.clear();
    }
  }
  static std::map<std::string, std::function<FormatItem::FormatItemPtr(
                                   const std::string &str)>>
      formatterItems = {
#define XX(str, c)                                                             \
  {                                                                            \
#str, [](const std::string                                                 \
                 &str) { return FormatItem::FormatItemPtr(new c(str)); }       \
  }
          XX(m, MessageFormatItem),    // m:消息
          XX(p, LevelFormatItem),      // p:日志级别
          XX(r, ElapseFormatItem),     // r:累计毫秒数
          XX(c, NameFormatItem),       // c:日志名称
          XX(t, ThreadIdFormatItem),   // t:线程id
          XX(n, NewLineFormatItem),    // n:换行
          XX(d, DateTimeFormatItem),   // d:时间
          XX(f, FilenameFormatItem),   // f:文件名
          XX(l, LineFormatItem),       // l:行号
          XX(T, TabFormatItem),        // T:Tab
          XX(F, FiberIdFormatItem),    // F:协程id
          XX(N, ThreadNameFormatItem), // N:线程名称
#undef XX

      };
  for (auto &i : vec) {
    if (std::get<2>(i) == 0) {
      m_item.push_back(
          FormatItem::FormatItemPtr(new StringFormatItem(std::get<0>(i))));
    } else {
      auto it = formatterItems.find(std::get<0>(i));
      if (it == formatterItems.end()) {
        m_item.push_back(FormatItem::FormatItemPtr(
            new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
        m_error = true;
      } else {
        m_item.push_back(it->second(std::get<1>(i)));
      }
    }
  }
}

void LogAppender::setFormatter(LogFormatter::LogFormatterPtr val) {
  m_formatter = val; // *
}

LogFormatter::LogFormatterPtr LogAppender::getFormatter() {
  return m_formatter;
}

void StdoutLogAppender::log(LogEvent::LogEventPtr event) {
  if (event->getLevel() >= m_level) {
    m_formatter->format(std::cout, event);
  }
}

FileLogAppender::FileLogAppender(const std::string &filename)
    : m_filename(filename) {
  reopen();
}
void FileLogAppender::log(LogEvent::LogEventPtr event) {
  if (event->getLevel() >= m_level) {
    uint64_t now = event->getTime();
    if (now >= (m_lastTime + 3)) {
      reopen();
      m_lastTime = now;
    }
    // if(!(m_filestream << m_formatter->format(logger, level, event))) {
    if (!m_formatter->format(m_filestream, event)) {
      std::cout << "error" << std::endl;
    }
  }
}
bool FileLogAppender::reopen() {
  if (m_filestream.is_open()) {
    m_filestream.close();
  }
  m_filestream.open(m_filename, std::ios::out | std::ios::app);
  //* !!是为了将m_filesttream转换为布尔值
  return !!m_filestream;
}

Logger::Logger(const std::string &name)
    : m_name(name), m_level(LogLevel::DEBUG) {

  m_formatter.reset(new LogFormatter(
      "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
  m_appenders.push_back(LogAppender::LogAppenderPtr(new StdoutLogAppender()));
  m_appenders.push_back(
      LogAppender::LogAppenderPtr(new FileLogAppender("./Log.txt")));
  this->setFormatter(m_formatter);
}
void Logger::log( LogEvent::LogEventPtr event) {
  auto self = shared_from_this();
  if (m_level <= event->getLevel()) {
    if (!m_appenders.empty()) {
      for (auto &i : m_appenders) {
        i->log(event);
      }
    } else if (m_root) {
      m_root->log(event);
    }
  }
}
void Logger::addAppender(LogAppender::LogAppenderPtr appender) {
  m_appenders.emplace_back(appender);
}

void Logger::delAppender(LogAppender::LogAppenderPtr appender) {
  for (auto it = m_appenders.begin(); it != m_appenders.end(); it++) {
    //*解引用之后才是appender类型的指针
    if (*it == appender) {
      m_appenders.erase(it);
      break;
    }
  }
}

void Logger::clearAppenders() { m_appenders.clear(); }

void Logger::setFormatter(LogFormatter::LogFormatterPtr val) {
  m_formatter = val;
  for (auto &i : m_appenders) {
    if (!i->hasFormatter()) {
      i->setFormatter(m_formatter);
    }
  }
}

void Logger::setFormatter(const std::string &val) {
  LogFormatter::LogFormatterPtr new_val(new LogFormatter(val));
  if (new_val->isError()) {
    std::cout << "Logger setFormatter name=" << m_name << " value=" << val
              << " invalid formatter" << std::endl;
    return;
  }
  // m_formatter = new_val;
  setFormatter(new_val);
}



LogEventWrap::LogEventWrap(LogEvent::LogEventPtr e) :m_event(e){
  
  }

LogEventWrap::~LogEventWrap() {
  m_event->getLogger()->log(m_event);
  }



// std::stringstream &LogEventWrap::getSS() {
  
//   }

