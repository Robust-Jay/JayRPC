#include "jayrpccontroller.h"

namespace JayRPC
{
    JayRpcController::JayRpcController()
    {
        __failed = false;
        __errText = "";

    }

    void JayRpcController::Reset()
    {
        __failed = false;
        __errText = "";
    }

    bool JayRpcController::Failed() const
    {
        return __failed;
    }

    std::string JayRpcController::ErrorText() const
    {
        return __errText;
    }

    void JayRpcController::SetFailed(const std::string &reason)
    {
        __failed = true;
        __errText = reason;
    }

    // 目前未实现的功能
    void JayRpcController::StartCancel() {}
    bool JayRpcController::IsCanceled() const { return false; }
    void JayRpcController::NotifyOnCancel(google::protobuf::Closure *callback) {}

} // namespace JayRPC