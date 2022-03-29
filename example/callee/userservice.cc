#include <iostream>
#include <string>
#include "user.pb.h"
#include "jayrpcapplication.h"
#include "jayrpcprovider.h"

using namespace JayRPC;

/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFrendLists
*/
class UserService : public fixbug::UserServiceRpc // 使用在rpc服务发布端（rpc服务提供者）
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing login service: Login" << std::endl;
        std::cout << "name: " << name << "\tpassword: " << pwd << std::endl;
        return true;
    }

    // 重写基类UsesrServiceRpc的虚函数 下面这些方法都是框架直接调用的
    // 1. caller ---> Login(LoginRequest) ---> moduo ---> callee
    // 2. callee ---> Login(LoginRequest) ---> 交到下面重写的Login方法上
    void Login(::google::protobuf::RpcController *controller,
               const ::fixbug::LoginRequest *request,
               ::fixbug::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // 1. 框架给业务上报了请求参数LoginRequest，应用获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 2. 做本地业务
        bool login_ret = Login(name, pwd);

        // 3. 写入响应：包括错误码、错误消息、返回值
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("ok");
        response->set_success(login_ret);

        // 4. 执行回调操作，执行响应对象数据的序列化和网络发送（都是由框架来完成的）
        done->Run();
    }
};

int main(int argc, char *argv[])
{
    JayRpcApplication::Init(argc, argv);

    return 0;
}