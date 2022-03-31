#include <iostream>
#include "jayrpcapplication.h"
#include "jayrpcchannel.h"
#include "jayrpccontroller.h"
#include "friend.pb.h"

using namespace JayRPC;

int main(int argc, char *argv[])
{
    JayRpcApplication::Init(argc, argv); // 整个程序启动以后，想使用JayRPC框架来享受rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次）
    
    // 演示调用远程发布的rpc方法GetFriendList
    fixbug::FriendServiceRpc_Stub stub(new JayRpcChannel());

    fixbug::GetFriendListRequest request; // rpc方法的请求
    request.set_userid(1000);
    fixbug::GetFriendListResponse response; // rpc方法的响应
    JayRpcController controller;
    stub.GetFriendList(&controller, &request, &response, nullptr); // JayRpcChannel->CallMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    // 一次rpc调用完成，读响应的结果
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if (0 == response.result().errcode())
        {
            std::cout << "rpc login response success!" << std::endl;
            int size = response.friends_size();
            for (int i = 0; i < size; i++)
            {
                std::cout << "index: " << i + 1 << " name: " << response.friends(i) << std::endl;
            }

        }
        else
        {
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
        }
    }
    return 0;
}