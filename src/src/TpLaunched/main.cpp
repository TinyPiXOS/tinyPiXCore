#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include "TpPipe.h"
#include "TpProcess.h"
#include "TpLaunchedDataDef.h"

std::atomic<bool> grunning{true};
void signalHandler(int signal)
{
    grunning = false;
}

int main()
{
    // 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    TpPipe writePipe("/tmp/TinyPiXLaunchedResultPipe", TpPipe::Write, true);
    TpPipe readPipe("/tmp/TinyPiXLaunchedPipe", TpPipe::Read, true);

    std::cout << "进程启动器已启动，等待启动命令..." << std::endl;

    // 主循环
    while (grunning)
    {
        TpPipe::PipeData recvLaunchCmd = readPipe.recv(); // 阻塞等待消息

        TpLaunchedProcessAck launchAck;
        launchAck.launchSucc = false;
        launchAck.pid = -1;

        TpStructPackager ackPackage;

        TpLaunchedProcessData launchData;
        launchData.StructDeserialize(recvLaunchCmd.data.data(), static_cast<uint32_t>(recvLaunchCmd.data.size()));

        if (launchData.processPath.empty())
        {
            std::cout << "接收到空的启动命令，忽略!" << std::endl;

            launchAck.StructSerialize(ackPackage);
            writePipe.send(launchAck.dataHead_.type_.c_str(), (const char *)ackPackage.data(), ackPackage.size());

            continue;
        }

        TpVector<TpString> argList;
        for (const auto &arg : launchData.argList)
            argList.push_back(arg);

        TpProcess exeProcess;
        exeProcess.start(launchData.processPath, argList);
        int32_t processPID = exeProcess.launchProcessID();

        std::cout << "processPID " << processPID << std::endl;

        launchAck.launchSucc = true;
        launchAck.pid = processPID;

        launchAck.StructSerialize(ackPackage);
        writePipe.send(launchAck.dataHead_.type_.c_str(), (const char *)ackPackage.data(), ackPackage.size());

        // 100MS检测一次数据
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}