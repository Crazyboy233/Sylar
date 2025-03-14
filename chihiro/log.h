#ifndef __CHIHIRO_LOG_H__
#define __CHIHIRO_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <fstream>
#include <sstream>

namespace chihiro {

// 日志事件
class LogEvent{
public:
    using ptr = std::shared_ptr<LogEvent>;

    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, 
            const char * file, int32_t line, 
            uint32_t elapse, uint32_t threadId, 
            uint32_t fiberId, uint64_t time,
            const std::string & content);

    // 获取日志信息
    const char * getFile() const;
    int32_t getLine() const;
    uint32_t getElapse() const;
    uint32_t getThreadId() const;
    uint32_t getFiberId() const;
    uint64_t getTime() const;
    std::string getContent() const;
    std::shared_ptr<Logger> getLogger() const;
    LogLevel::Level getLevel() const;
    std::string getLevelStr() const;

private:
    const char * m_file = nullptr;      // 文件名
    int32_t m_line = 0;                 // 文件行号
    uint32_t m_elapse = 0;              // 程序启动到现在的毫秒数
    uint32_t m_threadId = 0;            // 线程ID
    uint32_t m_fiberID = 0;             // 协程ID
    uint64_t m_time = 0;                // 时间
    std::string m_content;              // 消息
    std::shared_ptr<Logger> m_logger;   // 日志器
    LogLevel::Level m_level;            // 日志级别
};

// 日志级别
// TODO:可以考虑 enum class 
class LogLevel{
public:
    enum Level{
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
    static std::string toString(LogLevel::Level level);
};

// 日志格式器
class LogFormatter{
public:
    using ptr = std::shared_ptr<LogFormatter>;

    std::string format(LogEvent::ptr event);
private:
};

// 日志输出地
class LogAppender{
public:
    using ptr = std::shared_ptr<LogAppender>;

    virtual ~LogAppender();  // 可能有很多输出地，所以设置为虚析构函数
    
    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr format);
    LogFormatter::ptr getFormatter() const;

// 保护权限，保证派生类可以访问
protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};



// 日志器
class Logger{
public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string & name = "root");

    void log(LogLevel::Level level, LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);

    LogLevel::Level setLevel(LogLevel::Level level);
    LogLevel::Level getLevel() const;

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

private:
    std::string m_name;                         // 日志名称
    LogLevel::Level m_level;                    // 日志级别
    std::list<LogAppender::ptr> m_appenders;    // Appender集合
};

// 输出到控制台
class StdoutLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;

    void log(LogLevel::Level level, LogEvent::ptr event) override;
private:
};

// 输出到文件
class FileLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string & filename);

    void log(LogLevel::Level level, LogEvent::ptr event) override;

    /// @brief 重新打开文件
    /// @return 成功返回True，失败返回False
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}

#endif __CHIHIRO_LOG_H__