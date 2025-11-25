#include "TpGateway.h"
#include <thread>
#include <atomic>
#include <csignal>
#include "TpGatewayClientREQ.h"

std::atomic<bool> grunning{true};
void signalHandler(int signal)
{
    grunning = false;
}

int32_t main(int32_t argc, char *argv[])
{
    // 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // 创建客户端
    std::shared_ptr<TpGatewayClientREQ> client = createGatewayREQClient();

    // 连接到服务器
    if (client->connect())
    {
        std::cout << "连接成功!" << std::endl;
    }
    else
    {
        std::cout << "连接失败!" << std::endl;
        return 0;
    }

    while (grunning)
    {
        // 发送测试请求
        std::string request = "Hello, Server!";
        std::string response = client->sendRequest(request);

        if (!response.empty())
        {
            std::cout << "收到响应: " << response << std::endl;
        }

        // 显示统计信息
        std::cout << "发送请求数: " << client->getRequestCount()
                  << ", 接收响应数: " << client->getResponseCount() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    // 断开连接
    client->disconnect();

    return 0;
}
