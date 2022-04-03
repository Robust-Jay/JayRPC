#pragma once

#include "lockqueue.h"
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>

namespace JayRPC
{
    enum LogLevel
    {
        INFO,  // 普通信息
        ERROR, // 错误信息
    };

    // JayRPC框架提供的日志系统
    class Logger
    {
    public:
        static Logger &GetInstance(); // 获取单例
        void Log(std::string msg, LogLevel level);    // 写日志

    private:
        LockQueue<std::string> __lockQueue; // 日志缓冲队列

        Logger();
        Logger(const Logger &) = delete;
        Logger(Logger &&) = delete;
    };

#define LOG_INFO(logmsgformat, ...)                     \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::GetInstance();         \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c, INFO);                            \
    } while (0);

#define LOG_ERROR(logmsgformat, ...)                    \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::GetInstance();         \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c, ERROR);                           \
    } while (0);

} // namespace JayRPC