#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

namespace JayRPC
{
    class ZkClient
    {
    public:
        ZkClient();
        ~ZkClient();

        void Start(); // zkclient启动连接zkserver
        void Create(const char *path, const char *data, int datelen, int state = 0); // 在zkserver上根据指定的path创建znode节点
        std::string GetData(const char *path); // 根据参数指定的znode节点路径，获取znode节点的值

    private:
        zhandle_t *__zhandle; // zookeeper的客户端句柄
    };

} // namespace JayRPC