#include <iostream>
#include <string>
#include "friend.pb.h"
#include "jayrpcapplication.h"
#include "jayrpcprovider.h"
#include <vector>

using namespace JayRPC;

class FriendService : public fixbug::FriendServiceRpc
{
public:
    std::vector<std::string> GetFriendList(uint32_t userid)
    {
        std::cout << "do GetFriendList service! userid: " << userid << std::endl;
        std::vector<std::string> vec;
        vec.emplace_back("gao yang");
        vec.emplace_back("liu hong");
        vec.emplace_back("wang shuo");
        return vec;
    }

    void GetFriendList(google::protobuf::RpcController *controller,
                       const ::fixbug::GetFriendListRequest *request,
                       ::fixbug::GetFriendListResponse *response,
                       ::google::protobuf::Closure *done)
    {
        uint32_t userid = request->userid();
        std::vector<std::string> friendList = GetFriendList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("ok");
        for (std::string &name : friendList)
        {
            std::string *p = response->add_friends();
            *p = name;
        }
        done->Run();
    }
};

int main(int argc, char *argv[])
{
    JayRpcApplication::Init(argc, argv);
    JayRpcProvider provider;
    provider.NotifyService(new FriendService());
    provider.Run();
    return 0;
}