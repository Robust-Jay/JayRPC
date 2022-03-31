#include "jayrpcapplication.h"
#include <iostream>
#include <unistd.h>

namespace JayRPC
{
    void ShowArgsHelp()
    {
        std::cout << "format: command -i <configfile>" << std::endl;
    }

    JayRpcConfig JayRpcApplication::__config;

    void JayRpcApplication::Init(int argc, char *argv[])
    {
        if (argc < 2)
        {
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        }

        int c = 0;
        std::string config_file;
        while ((c = getopt(argc, argv, "i:")) != -1)
        {
            switch (c)
            {
            case 'i':
                config_file = optarg;
                break;
            case '?':
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            case ':':
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            default:
                exit(EXIT_FAILURE);
            }
        }

        // 开始加载配置文件: jayprcserverip jayrpcserverport zookeeperip zookeeperport
        __config.LoadConfigFile(config_file.c_str());

        // std::cout << "jayrpcserverip: " << __config.Load("jayrpcserverip") << std::endl;
        // std::cout << "jayrpcserverport: " << __config.Load("jayrpcserverport") << std::endl;
        // std::cout << "zookeeperip: " << __config.Load("zookeeperip") << std::endl;
        // std::cout << "zookeeperport: " << __config.Load("zookeeperport") << std::endl;
    }

    JayRpcApplication &JayRpcApplication::GetInstance()
    {
        static JayRpcApplication app;
        return app;
    }

    JayRpcConfig &JayRpcApplication::GetConfig()
    {
        return __config;
    }
} // namespace JayRPC