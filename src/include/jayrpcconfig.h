#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <iostream>
#include <fstream>
#include "logger.h"

namespace JayRPC
{
    // 框架读取配置文件类
    // jayrpcserverip jayprcserverport zookeeperip zookeeperport
    class JayRpcConfig
    {
    public:
        void LoadConfigFile(const char *configfile); // 负责解析加载配置文件
        std::string Load(const std::string &key);    // 查询配置项信息
    private:
        std::unordered_map<std::string, std::string> __configMap;
    };

} // namespace JayRPC