#include "jayrpcchannel.h"

namespace JayRPC
{
    void JayRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                                   google::protobuf::RpcController *controller,
                                   const google::protobuf::Message *request,
                                   google::protobuf::Message *response,
                                   google::protobuf::Closure *done)
    {
        const google::protobuf::ServiceDescriptor *serviceDescriptor = method->service();
        std::string service_name = serviceDescriptor->name();
        std::string method_name = method->name();

        int args_size = 0;
        std::string args_str;
        if (request->SerializeToString(&args_str))
        {
            args_size = args_str.size();
        }
        else
        {
            controller->SetFailed("serialize request error!");
            return;
        }

        // 定义rpc的请求header
        RpcHeader rpcHeader;
        rpcHeader.set_service_name(service_name);
        rpcHeader.set_method_name(method_name);
        rpcHeader.set_args_size(args_size);

        uint32_t header_size = 0;
        std::string rpc_header_str;
        if (rpcHeader.SerializeToString(&rpc_header_str))
        {
            header_size = rpc_header_str.size();
        }
        else
        {
            controller->SetFailed("serialize rpc header error!");
            return;
        }

        // 组织待发送的rpc请求字符串
        std::string send_rpc_str;
        send_rpc_str.insert(0, std::string((char *)&header_size, 4));
        send_rpc_str += rpc_header_str;
        send_rpc_str += args_str;

        // // 打印调试信息
        // std::cout << "=========================================================" << std::endl;
        // std::cout << "header_size: " << header_size << std::endl;
        // std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
        // std::cout << "service_name: " << service_name << std::endl;
        // std::cout << "method_name: " << method_name << std::endl;
        // std::cout << "args_str: " << args_str << std::endl;
        // std::cout << "=========================================================" << std::endl;

        // 使用tcp编程，完成rpc方法的远程调用
        int clientfd = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == clientfd)
        {
            std::string errtxt = "create socket error! errno: " + errno;
            controller->SetFailed(errtxt);
            return;
        }

        // // 读取配置文件jayrpcserver的信息
        // std::string ip = JayRpcApplication::GetInstance().GetConfig().Load("jayrpcserverip");
        // uint16_t port = atoi(JayRpcApplication::GetInstance().GetConfig().Load("jayrpcserverport").c_str());

        // rpc调用方想调用service_name的method_name服务，需要查询zookeeper上该服务所在的host信息
        ZkClient zkCli;
        zkCli.Start();
        std::string method_path = "/" + service_name + "/" + method_name;
        std::string host_data = zkCli.GetData(method_path.c_str());
        if (host_data == "")
        {
            controller->SetFailed(method_path + " not exist!");
            return;
        }
        int idx = host_data.find(':');
        if (idx == -1)
        {
            controller->SetFailed(method_path + " address is invalid!");
            return;
        }
        std::string ip = host_data.substr(0, idx);
        uint16_t port = atoi(host_data.substr(idx + 1, host_data.size() - idx).c_str());

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

        // 连接rpc服务节点
        if (-1 == connect(clientfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
        {
            std::string errtxt = "connect error! errno: " + errno;
            controller->SetFailed(errtxt);
            close(clientfd);
            return;
        }

        // 发送rpc请求
        if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.length(), 0))
        {
            std::string errtxt = "send error! errno: " + errno;
            controller->SetFailed(errtxt);
            close(clientfd);
            return;
        }

        // 接受rpc请求的响应值
        char recv_buf[1024] = {0};
        int recv_size = 0;
        if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0)))
        {
            std::string errtxt = "recv error! errno: " + errno;
            controller->SetFailed(errtxt);
            close(clientfd);
            return;
        }

        // if (!response->ParseFromArray)
        std::string response_str(recv_buf, 0, recv_size);
        if (!response->ParseFromString(response_str))
        {
            std::string errtxt = "parse error! response_str: " + errno;
            controller->SetFailed(errtxt);
            close(clientfd);
            return;
        }

        close(clientfd);
    }

} // namespace JayRPC