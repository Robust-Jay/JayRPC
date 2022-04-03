#include "logger.h"

namespace JayRPC
{
    Logger &Logger::GetInstance()
    {
        static Logger logger;
        return logger;
    }

    Logger::Logger()
    {
        // 启动专门的写日志线程
        std::thread writeLogTask([&]()
            {
                for (; ;)
                {
                    // 获取当前的日期，取日志信息，写入相应的日志文件中 a+
                    time_t now = time(nullptr);
                    tm *nowtm = localtime(&now);

                    std::string file_name = std::to_string(nowtm->tm_year + 1900) + "-" +
                                            std::to_string(nowtm->tm_mon + 1) + "-" +
                                            std::to_string(nowtm->tm_mday) + "-log.txt";

                    std::ofstream pf(file_name, std::ios::app);
                    if (!pf.is_open())
                    {
                        std::cout << "logger file: " << file_name << " open error!" << std::endl;
                        exit(EXIT_FAILURE);
                    }

                    std::string msg = __lockQueue.Pop();
                    char time_buf[128] = {0};
                    sprintf(time_buf, "%d:%d:%d --> ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);
                    msg.insert(0, time_buf);
                    msg.push_back('\n');
                    pf << msg;
                    pf.close();
                } 
            });

        writeLogTask.detach(); // 设置线程分离
    }

    void Logger::Log(std::string msg, LogLevel level)
    {
        msg = (level == INFO ? "[info] " : "[error] ") + msg;
        __lockQueue.Push(msg);
    }

} // namespace JayRPC