#include <iostream>
#include "jayrpcapplication.h"
#include "jayrpcchannel.h"
#include "user.pb.h"
#include <string>

using namespace JayRPC;

int main(int argc, char *argv[])
{
    JayRpcApplication::Init(argc, argv); // 整个程序启动以后，想使用JayRPC框架来享受rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次）
    
    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new JayRpcChannel());

    fixbug::LoginRequest request; // rpc方法的请求
    request.set_name("zhang san");
    request.set_pwd("123456");
    fixbug::LoginResponse response; // rpc方法的响应
    stub.Login(nullptr, &request, &response, nullptr); // JayRpcChannel->CallMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    // 一次rpc调用完成，读响应的结果
    if (0 == response.result().errcode())
    {
        std::cout << "rpc login response success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }

    // 演示调用远程发布的rpc方法Register
    fixbug::RegisterRequest req;
    req.set_id(2000);
    req.set_name("JayRPC");
    req.set_pwd("666666");
    fixbug::RegisterResponse rsp;
    stub.Register(nullptr, &req, &rsp, nullptr);
    // 一次rpc调用完成，读响应的结果
    if (0 == rsp.result().errcode())
    {
        std::cout << "rpc register response success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc register response error: " << response.result().errmsg() << std::endl;
    }


    return 0;
}