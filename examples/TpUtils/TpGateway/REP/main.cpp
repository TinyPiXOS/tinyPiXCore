#include "TpGateway.h"
#include <thread>
#include <atomic>
#include <csignal>

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

    TpGatewayClientREQ client;
    client.openConnect("ipc:///tmp/gateway_rep.ipc");

    while (grunning)
    {
        std::cout << "SendReq " << std::endl;
        std::string recvString = client.sendRequest("TestReqKey");
        std::cout << "接收到服务端响应：" << recvString << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    client.closeConnect();

    return 0;
}
