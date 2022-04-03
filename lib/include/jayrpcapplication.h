#pragma once

#include <iostream>
#include <unistd.h>
#include "jayrpcconfig.h"
#include "logger.h"

namespace JayRPC
{
    // JayRPC框架的基础类，负责框架的一些初始化操作
    class JayRpcApplication
    {
    public:
        static void Init(int argc, char *argv[]);
        static JayRpcApplication &GetInstance(); // 单例模式
        static JayRpcConfig &GetConfig();

    private:
        static JayRpcConfig __config;

        JayRpcApplication() {}
        JayRpcApplication(const JayRpcApplication &) = delete;
        JayRpcApplication(JayRpcApplication &&) = delete;
    };

} // namespace JayRPC