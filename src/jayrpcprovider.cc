#include "jayrpcprovider.h"

namespace JayRPC
{
    void JayRpcProvider::NotifyService(google::protobuf::Service *service)
    {
        ServiceInfo service_info;

        const google::protobuf::ServiceDescriptor *server_descriptor = service->GetDescriptor(); // 获取服务对象的描述信息
        std::string service_name = server_descriptor->name();                                    // 获取服务的名字
        int methodCnt = server_descriptor->method_count();                                       // 获取服务对象service方法的数量

        std::cout << "service name: " << service_name << std::endl;

        for (int i = 0; i < methodCnt; i++)
        {
            const google::protobuf::MethodDescriptor *method_descriptor = server_descriptor->method(i); // 获取方法的描述信息
            std::string method_name = method_descriptor->name();
            service_info.__methodMap.insert({method_name, method_descriptor});

            std::cout << "method name: " << method_name << std::endl;
        }
        service_info.__service = service;
        __serviceMap.insert({service_name, service_info});
    }

    void JayRpcProvider::Run()
    {
        std::string ip = JayRpcApplication::GetInstance().GetConfig().Load("jayrpcserverip");
        uint16_t port = atoi(JayRpcApplication::GetInstance().GetConfig().Load("jayrpcserverport").c_str());
        muduo::net::InetAddress address(ip, port);

        muduo::net::TcpServer server(&__eventLoop, address, "JayRpcProvider");                                                                       // 创建TcpServer对象
        server.setConnectionCallback(std::bind(&JayRpcProvider::OnConnection, this, std::placeholders::_1));                                         // 绑定连接回调
        server.setMessageCallback(std::bind(&JayRpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); // 绑定读写回调
        server.setThreadNum(4);                                                                                                                      // 设置muduo的线程数量
        std::cout << "JayRpcProvider start service at ip: " << ip << " port: " << port << std::endl;
        server.start(); // 启动网络服务
        __eventLoop.loop();
    }

    void JayRpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
    {
        if (!conn->connected())
        {
            // 和rpc client的连接断开了
            conn->shutdown();
        }
    }

    void JayRpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp)
    {
        std::string recv_buf = buffer->retrieveAllAsString(); // 网络上接收的远程rpc调用请求的字符流：header_size(4 bytes) + header_str(service_name + method_name + args_size) + args_str
        uint32_t header_size = 0;
        recv_buf.copy((char *)&header_size, 4, 0); // 从字符流中读取前4个字节的数据
        std::string rpc_header_str = recv_buf.substr(4, header_size);
        JayRPC::RpcHeader rpcHeader;
        std::string service_name;
        std::string method_name;
        uint32_t args_size;
        if (rpcHeader.ParseFromString(rpc_header_str))
        {
            // 数据头反序列化成功
            service_name = rpcHeader.service_name();
            method_name = rpcHeader.method_name();
            args_size = rpcHeader.args_size();
        }
        else
        {
            // 数据头反序列化失败;
            std::cout << "rpc_header_str: " << rpc_header_str << " parse error!" << std::endl;
        }

        std::string args_str = recv_buf.substr(4 + header_size, args_size); // 获取rpc方法参数的字符流数据

        // 打印调试信息
        std::cout << "=========================================================" << std::endl;
        std::cout << "header_size: " << header_size << std::endl;
        std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
        std::cout << "service_name: " << service_name << std::endl;
        std::cout << "method_name: " << method_name << std::endl;
        std::cout << "args_str: " << args_str << std::endl;
        std::cout << "=========================================================" << std::endl;

        auto s_it = __serviceMap.find(service_name);
        if (s_it == __serviceMap.end())
        {
            std::cout << service_name << " not exist!" << std::endl;
            return;
        }
        google::protobuf::Service *service = s_it->second.__service; // 获取service对象

        auto m_it = s_it->second.__methodMap.find(method_name);
        if (m_it == s_it->second.__methodMap.end())
        {
            std::cout << method_name << " not exist!" << std::endl;
            return;
        }
        const google::protobuf::MethodDescriptor *methodDescriptor = m_it->second; // 获取method对象

        // 生成rpc方法调用的请求request和响应response参数
        google::protobuf::Message *request = service->GetRequestPrototype(methodDescriptor).New();
        if (!request->ParseFromString(args_str))
        {
            std::cout << "request parse error, content: " << args_str << std::endl;
            return;
        }
        google::protobuf::Message *response = service->GetResponsePrototype(methodDescriptor).New();

        // 给下面的method方法的调用，绑定一个Closure的回调函数
        google::protobuf::Closure *done = google::protobuf::NewCallback<JayRpcProvider,
                                                                        const muduo::net::TcpConnectionPtr &,
                                                                        google::protobuf::Message *>(this,
                                                                                                     &JayRpcProvider::SendRpcResponse,
                                                                                                     conn,
                                                                                                     response);

        // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
        service->CallMethod(methodDescriptor, nullptr, request, response, done);
    }

    void JayRpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *msg)
    {
        std::string response_str;
        if (msg->SerializeToString(&response_str)) // response进行序列化
        {
            conn->send(response_str); // 序列化成功后，通过网络把rpc方法执行的结果发送给rpc的调用方
        }
        else
        {
            std::cout << "serialize response_str error!" << std::endl;
        }
        conn->shutdown(); // 模拟http短连接服务，由RpcProvider主动断开连接
    }

} // namespace JayRPC