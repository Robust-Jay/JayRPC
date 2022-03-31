#pragma once

#include "google/protobuf/service.h"
#include <google/protobuf/descriptor.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/InetAddress.h>
#include <muduo/base/Timestamp.h>
#include <string>
#include "jayrpcapplication.h"
#include <functional>
#include <unordered_map>
#include "rpcheader.pb.h"

namespace JayRPC
{
    // 框架提供的专门发布rpc服务的网络对象类
    class JayRpcProvider
    {
    public:
        void NotifyService(google::protobuf::Service *service); // 框架提供给外部使用的，可以发布rpc方法的函数接口
        void Run();                                             // 启动rpc服务节点，开始提供rpc远程网络调用服务

    private:
        muduo::net::EventLoop __eventLoop; // 组合EventLoop

        struct ServiceInfo //服务类型信息
        {
            google::protobuf::Service *__service;                                                    // 保存服务对象
            std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> __methodMap; // 保存服务方法
        };
        std::unordered_map<std::string, ServiceInfo> __serviceMap; // 存储注册成功的服务对象和服务方法的所有信息

        void OnConnection(const muduo::net::TcpConnectionPtr &);                                      // socket连接回调
        void OnMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp); // 读写回调
        void SendRpcResponse(const muduo::net::TcpConnectionPtr &, google::protobuf::Message *);      // Closure的回调操作，用于序列化rpc的响应和网络发送
    };

} // namespace JayRPC