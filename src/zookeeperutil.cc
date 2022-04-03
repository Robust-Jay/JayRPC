#include "zookeeperutil.h"

namespace JayRPC
{
    void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx)
    {
        if (type == ZOO_SESSION_EVENT)
        {
            if (state == ZOO_CONNECTED_STATE)
            {
                sem_t *sem = (sem_t *)zoo_get_context(zh);
                sem_post(sem);
            }
        }
    }

    ZkClient::ZkClient() : __zhandle(nullptr)
    {
    }

    ZkClient::~ZkClient()
    {
        if (__zhandle != nullptr)
        {
            zookeeper_close(__zhandle); // 关闭句柄，释放资源
        }
    }

    void ZkClient::Start()
    {
        std::string host = JayRpcApplication::GetInstance().GetConfig().Load("zookeeperip");
        std::string port = JayRpcApplication::GetInstance().GetConfig().Load("zookeeperport");
        std::string connstr = host + ":" + port;

        /*
            zookeeper_mt：多线程版本
            zookeeper的API客户端程序提供了三个线程：API调用线程、网络I/O线程、wather回调线程
        */

        __zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
        if (nullptr == __zhandle)
        {
            LOG_ERROR("zookeeper_init error!");
            exit(EXIT_FAILURE);
        }

        sem_t sem;
        sem_init(&sem, 0, 0);
        zoo_set_context(__zhandle, &sem);

        sem_wait(&sem);
        std:: cout << "zookeeper_init success!";
    }

    void ZkClient::Create(const char *path, const char *data, int datalen, int state)
    {
        char path_buffer[128];
        int bufferlen = sizeof(path_buffer);
        int flag;
        // 先判断path表示的znode节点是否存在，如果存在，就不再重复创建了
        flag = zoo_exists(__zhandle, path, 0, nullptr);
        if (ZNONODE == flag)
        {
            flag = zoo_create(__zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
            if (ZOK == flag)
            {
                LOG_INFO("znode create success... path: %s", path);
            }
            else
            {
                LOG_ERROR("flag: %d\tznode create error... path: %s", flag, path);
                exit(EXIT_FAILURE);
            }
        }
    }

    std::string ZkClient::GetData(const char *path)
    {
        char buffer[64];
        int bufferlen = sizeof(buffer);
        int flag = zoo_get(__zhandle, path, 0, buffer, &bufferlen, nullptr);
        if (flag != ZOK)
        {
            LOG_ERROR("get znode error... path: %s", path);
            return "";
        }
        else
        {
            return buffer;
        }
    }

} // namespace JayRPC