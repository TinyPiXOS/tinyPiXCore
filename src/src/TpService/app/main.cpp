#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include "TpGatewayServerPUBSUB.h"
#include "TpRPCSystemApi.hpp"
#include "erpc_arbitrated_client_setup.h"
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "erpc_tcp_transport.hpp"
#include "erpc_server_setup.h"
#include "erpc_transport_setup.h"

std::atomic<bool> grunning{true};
void signalHandler(int signal)
{
    grunning = false;
}

// 自定义传输类，支持现有socket
class CustomTCPTransport : public erpc::TCPTransport
{
public:
    CustomTCPTransport(int socket_fd) : erpc::TCPTransport(true)
    {
        m_socket = socket_fd; // m_socket是基类的protected成员
    }

    virtual erpc_status_t open(void) override
    {
        // Socket已经连接，直接返回成功
        return kErpcStatus_Success;
    }

    virtual erpc_status_t close(bool stopServer = true) override
    {
        // 重写close方法，不调用基类的close
        return kErpcStatus_Success;
    }
};

#if 1
// RPC服务端线程函数
erpc_transport_t rpcTransport = nullptr;
erpc_server_t rpcServer = nullptr;
erpc_mbf_t message_buffer_factory = nullptr;
erpc_service_t service = nullptr;
erpc_transport_t g_arbitrator;

// 客户端处理函数
void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;

    // 创建自定义传输实例
    CustomTCPTransport *transport = new CustomTCPTransport(client_socket);

    // 为新客户端创建独立的transport和server
    erpc_mbf_t message_buffer_factory = erpc_mbf_dynamic_init();
    erpc_server_t client_server = erpc_server_init(reinterpret_cast<erpc_transport_t>(transport), message_buffer_factory);

    // 添加服务
    erpc_service_t service = create_SystemApiService_service();
    erpc_add_service_to_server(client_server, service);

    // 处理客户端请求（非阻塞方式）
    while (1)
    {
        erpc_status_t err = erpc_server_poll(client_server);
        if (err != kErpcStatus_Success)
        {
            std::cout << "客户端终止连接，socket: " << client_socket << std::endl;
            break;
        }
        usleep(1000); // 避免CPU占用过高
    }

    // 清理资源
    // erpc_server_deinit(client_server);
    // erpc_mbf_dynamic_deinit(message_buffer_factory);
    // erpc_transport_tcp_deinit(reinterpret_cast<erpc_transport_t>(transport));
    close(client_socket);

    return nullptr;
}

void dealRPCFunc()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Failed to create socket");
        return;
    }

    // 设置socket选项，允许地址重用
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("Failed to set socket options");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12581);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Failed to bind socket");
        return;
    }

    if (listen(server_socket, 10) < 0)
    {
        perror("Failed to listen");
        return;
    }

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

        if (client_socket > 0)
        {
            printf("New client connected from %s:%d (socket: %d)\n",
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port),
                   client_socket);

            // 为socket描述符分配内存，避免指针问题
            int *socket_ptr = (int *)malloc(sizeof(int));
            *socket_ptr = client_socket;

            // 为每个客户端创建独立线程
            pthread_t client_thread;
            if (pthread_create(&client_thread, NULL, handle_client, socket_ptr) == 0)
            {
                pthread_detach(client_thread); // 分离线程，自动回收资源
            }
            else
            {
                printf("Failed to create thread for client %d\n", client_socket);
                free(socket_ptr);
                close(client_socket);
            }
        }
    }

    close(server_socket);

    // // 创建传输层对象(TCP),0.0.0.0:12581
    // TpString listenIp = "127.0.0.1";
    // uint16_t listenPort = 12581;

    // // 创建TCP传输层
    // rpcTransport = erpc_transport_tcp_init(listenIp.c_str(), listenPort, true);

    // // 初始化消息缓冲工厂
    // message_buffer_factory = erpc_mbf_dynamic_init();

    // // 初始化仲裁客户端（用于多客户端支持）
    // // erpc_client_t client = erpc_arbitrated_client_init(rpcTransport,
    // //                                                    message_buffer_factory,
    // //                                                    &g_arbitrator);
    // /* eRPC 服务端初始化 */
    // rpcServer = erpc_server_init(rpcTransport, message_buffer_factory);
    // service = create_SystemApiService_service();

    // /** 将生成的接口服务DEMO添加到server, 参见生成的源文件 erpcdemo_server.h */
    // erpc_add_service_to_server(rpcServer, service);
    // std::cout << "RPC 服务端启动成功；监听: " << listenIp << " , " << listenPort << std::endl;

    // while (grunning)
    // {
    //     std::cout << "开启RPC等待接收 " << std::endl;

    //     erpc_status_t err = erpc_server_poll(rpcServer);

    //     if (err != kErpcStatus_Success)
    //     {
    //         std::cout << "RPC 服务端错误，错误码: " << err << std::endl;
    //     }

    //     /* or erpc_server_poll(); */
    //     // erpc_server_poll(server);
    //     // erpc_server_run(server);
    //     // std::cout << "收到RPC请求 " << std::endl;
    // }
}
#endif

int main()
{
    // 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // 创建发布订阅网关服务器
    auto server = createGatewayServer();

    // 启动发布订阅服务器
    const uint16_t port = 5555;
    if (!server->start(port))
    {
        std::cerr << "发布订阅网关服务创建失败!" << std::endl;
        return 1;
    }

    std::cout << "发布订阅网关服务启动成功!" << std::endl;
    std::cout << "发布订阅监听 端口：订阅端口： " << port << " and 发布端口" << (port + 1) << std::endl;

    std::thread REPWorker(&dealRPCFunc);
    REPWorker.detach();

    // 主循环
    while (grunning)
    {
        // 每秒打印一次状态
        static auto last_time = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_time).count();

        if (elapsed >= 1)
        {
            size_t msg_rate = server->getMessageRate(); // 每秒消息数

            std::cout << "Server status: " << msg_rate
                      << " msgs/s" << std::endl;

            last_time = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 停止服务器
    server->stop();
    std::cout << "Gateway server stopped gracefully." << std::endl;

    // std::cout << "RPC 服务端 反初始化！ " << std::endl;

    destroy_SystemApiService_service(service);
    erpc_server_deinit(rpcServer);
    erpc_mbf_dynamic_deinit(message_buffer_factory);
    erpc_transport_tcp_close(rpcTransport);
    erpc_transport_tcp_deinit(rpcTransport);

    // erpc_server_deinit(server);
    // erpc_server_stop(server);

    /* 关闭socket */
    // erpc_transport_tcp_deinit(rpcTransport);

    // tinyPiX_sys_free(globalAgent);

    return 0;
}