#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include "TpGatewayServerPUBSUB.h"
#include "TpRPCSystemApi.hpp"

std::atomic<bool> grunning{true};
void signalHandler(int signal)
{
    grunning = false;
}

#if 1
// RPC服务端线程函数
erpc_transport_t rpcTransport = nullptr;
void dealRPCFunc()
{
    // 创建传输层对象(TCP),0.0.0.0:12581
    TpString listenIp = "127.0.0.1";
    uint16_t listenPort = 12581;

    rpcTransport = erpc_transport_tcp_init(listenIp.c_str(), listenPort, true);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory = erpc_mbf_dynamic_init();

    /* eRPC 服务端初始化 */
    auto server = erpc_server_init(rpcTransport, message_buffer_factory);

    /** 将生成的接口服务DEMO添加到server, 参见生成的源文件 erpcdemo_server.h */
    erpc_add_service_to_server(server, create_SystemApiService_service());
    std::cout << "RPC 服务端启动成功；监听: " << listenIp << " , " << listenPort << std::endl;

    while (grunning)
    {
        std::cout << "开启RPC等待接收 " << std::endl;
        /* or erpc_server_poll(); */
        // erpc_server_poll(server);
        erpc_server_run(server);
        std::cout << "收到RPC请求 " << std::endl;
    }
}
#endif

int main()
{
    // 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // 创建网关服务器
    auto server = createGatewayServer();

    // 启动服务器
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
    
    // erpc_server_deinit(server);
    // erpc_server_stop(server);

    /* 关闭socket */
    // erpc_transport_tcp_deinit(rpcTransport);

    // tinyPiX_sys_free(globalAgent);

    return 0;
}