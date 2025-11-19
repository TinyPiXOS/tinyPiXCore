#include "TpGateway.h"
#include <thread>
#include <atomic>
#include <csignal>

std::atomic<bool> grunning{true};
void signalHandler(int signal)
{
    grunning = false;
}

class TestRecvDataClass : public ITpGatewayHander
{
public:
    virtual void recvData(const char *topic, const void *data, const uint32_t &size)
    {
        int32_t recvData = *(int32_t *)data;
        std::cout << "Topic : " << topic << std::endl;
        std::cout << "RecvData : " << recvData << std::endl;
        std::cout << "DataSize : " << size << std::endl;
    }
};

int32_t main(int32_t argc, char *argv[])
{
    // 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    TestRecvDataClass *recvClass = new TestRecvDataClass();

    initializeGateway();
    subscribeGatewayData("TestTopic", recvClass);

    while (grunning)
    {
        std::cout << "PublishData " << std::endl;
        int32_t pubData = 2;
        publishGatewayData("TestTopic22", &pubData, sizeof(int32_t));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    shutdownGateway();

    return 0;
}
