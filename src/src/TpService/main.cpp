#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include "TpGatewayServerPUBSUB.h"

std::atomic<bool> grunning{true};
void signalHandler(int signal)
{
    grunning = false;
}

#if 1
// 请求问答服务端线程函数
void dealFuncREP()
{
    auto repServer = createGatewayREPServer();
    if (!repServer->start())
    {
        std::cerr << "请求问答网关服务创建失败!" << std::endl;
        return;
    }
    std::cout << "请求问答网关服务启动成功!" << std::endl;

    // 主循环
    while (grunning)
    {
        // 每秒打印一次状态
        static auto last_time = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_time).count();

        if (elapsed >= 1)
        {
            int32_t reqCount = repServer->getRequestCount(); // 每秒消息数

            std::cout << "请求应答消息数: " << reqCount
                      << " msgs/s" << std::endl;

            last_time = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 停止服务器
    repServer->stop();
}
#endif

int main()
{
    TpString testString = "123123213";
    std::cout << testString << std::endl;

    // TpString* testPtr = new TpString();

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

    std::thread REPWorker(&dealFuncREP);

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

    return 0;
}