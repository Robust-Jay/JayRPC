#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace JayRPC
{
    class JayRpcController : public google::protobuf::RpcController
    {
    public:
        JayRpcController();
        void Reset();
        bool Failed() const;
        std::string ErrorText() const;
        void SetFailed(const std::string &reason);

        // 目前未实现的功能
        void StartCancel();
        bool IsCanceled() const;
        void NotifyOnCancel(google::protobuf::Closure *callback);

    private:
        bool __failed;         // rpc方法执行过程中的状态
        std::string __errText; // rpc方法执行过程中的错误信息
    };

} // namespace JayRPC