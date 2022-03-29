#pragma once

#include "google/protobuf/service.h"

namespace JayRPC
{
    // 框架提供的专门发布rpc服务的网络对象类
    class JayRpcProvider
    {
    public:
        void NotifyService(google::protobuf::Service *service); // 框架提供给外部使用的，可以发布rpc方法的函数接口
        void Run();                                             // 启动rpc服务节点，开始提供rpc远程网络调用服务
    };

} // namespace JayRPC