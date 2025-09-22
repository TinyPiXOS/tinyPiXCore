#include "TpApp.h"
#include "TpString.h"
#include "TpPipe.h"
#include "TpTimer.h"
#include <thread>

void sendDataFunc()
{
    TpPipe sendPipe("TestPipe", TpPipe::Write, true);

    uint64_t sendValue = 0;
    while (1)
    {
        std::cout << " 发送数据 " << std::endl;
        sendPipe.send("Thread Data", (const char *)&sendValue, sizeof(uint64_t));
        ++sendValue;
        TpTimer::sleep(500);
    }
}

int32_t main(int32_t argc, char *argv[])
{
    std::thread sendThread(sendDataFunc);
    sendThread.detach();

    TpPipe recvPipe("TestPipe", TpPipe::Read, true);

    while (1)
    {
        TpPipe::PipeData recvData = recvPipe.recv();

        if (!recvData.topic.empty() && recvData.data.size() > 0)
        {
            uint64_t sendValue = 0;
            memcpy(&sendValue, recvData.data.data(), sizeof(uint64_t));
            std::cout << "Topic : " << recvData.topic << " Value : " << sendValue << std::endl;
        }
        else
        {
            // std::cout << " 未接收到数据 ！" << std::endl;
        }

        TpTimer::sleep(500);
    }

    return 0;
}
