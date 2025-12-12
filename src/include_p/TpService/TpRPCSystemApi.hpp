#ifndef __GATEWAY_RPC_SERVER_H
#define __GATEWAY_RPC_SERVER_H

#include <erpc_server_setup.h>
#include "c_TpAppManager_server.h"
#include "TpAppManager_server.hpp"
#include "tinyPiXUtils.h"
#include "tinyPiXSys.h"
#include "TpPipe.h"
#include "TpLaunchedDataDef.h"

#include <mutex>
#include <thread>
#include <TpProcess.h>
#include <TpFileInfo.h>
#include <AppManage/TpAppConfigIO.h>
#include <TpHash.h>
#include <TpString.h>
#include <TpNetDataGlobal.h>
#include <TpInteractDataDef/TpDesktopData.h>

static std::mutex globalAppMutex;
static TpHash<TpString, int32_t> appUuidPidMap = TpHash<TpString, int32_t>();
static IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

binary_t *TPR_RunAppProcessInfo(void)
{
    std::cout << "+++++++++++++++++++++++++++" << std::endl;
    std::cout << "查询APP运行信息 " << appUuidPidMap.size() << std::endl;
    std::cout << "+++++++++++++++++++++++++++" << std::endl;

    TpRPCRunAppProcessInfo processInfo;

    {
        std::lock_guard<std::mutex> lockG(globalAppMutex);
        for (auto it = appUuidPidMap.begin(); it != appUuidPidMap.end(); ++it)
        {
            processInfo.uuidList.emplace_back(it->first);
            processInfo.pidList.emplace_back(it->second);

            std::cout << "当前运行进程 PID " << it->second << std::endl;
        }
    }

    TpStructPackager package;
    processInfo.StructSerialize(package);

    // 创建返回缓冲区
    binary_t *rpcRespData = new binary_t();
    rpcRespData->data = new uint8_t[package.size()];
    rpcRespData->dataLength = package.size();
    memcpy(rpcRespData->data, package.data(), package.size());

    std::cout << "+++++++++++++++++++++++++++" << std::endl;
    std::cout << "查询APP运行信息结束 " << std::endl;
    std::cout << "+++++++++++++++++++++++++++" << std::endl;

    return rpcRespData;
}

/*! 指定UUID启动应用 */
bool TPR_StartApp(const binary_t *startParams)
{
    TpRunApp startAppData;
    startAppData.StructDeserialize(startParams->data, startParams->dataLength);

    TpString uuid = startAppData.appUuid;

    std::lock_guard<std::mutex> lockG(globalAppMutex);

    if (appUuidPidMap.contains(uuid))
    {
        // 应用已启动；恢复应用
        int32_t pid = appUuidPidMap.value(uuid);

        std::cout << "恢复应用 pid: " << pid << std::endl;
        tinyPiX_sys_set_visible(globalAgent, pid, true);
        tinyPiX_sys_set_active(globalAgent, pid, true);
    }
    else
    {

        TpAppConfigIO configIO(uuid);
        TpString runnerPath = configIO.runnerPath();

        TpLaunchedProcessData launchData;
        launchData.processPath = runnerPath;
        launchData.argList = startAppData.argList;

        TpStructPackager package;
        launchData.StructSerialize(package);

        TpPipe writePipe("/tmp/TinyPiXLaunchedPipe", TpPipe::Write, true);
        writePipe.send(launchData.dataHead_.type_.c_str(), (const char *)package.data(), package.size());

        // 读取启动进程PID
        TpPipe readPipe("/tmp/TinyPiXLaunchedResultPipe", TpPipe::Read, true);
        TpPipe::PipeData recvData = readPipe.recv();

        TpLaunchedProcessAck launchAck;
        launchAck.StructDeserialize(recvData.data.data(), static_cast<uint32_t>(recvData.data.size()));

        int32_t pid = launchAck.pid;
        appUuidPidMap[uuid] = pid;

        std::cout << "启动应用: pid: " << pid << std::endl;
    }

    return true;
}

/*! 指定应用UUID终止进程 */
bool TPR_KillApp(const char *uuid)
{
    std::lock_guard<std::mutex> lockG(globalAppMutex);

    if (!appUuidPidMap.contains(uuid))
        return false;

    int32_t pid = appUuidPidMap.value(uuid);

    // std::lock_guard<std::mutex> lockG(apiData->readAppMutex);
    appUuidPidMap.erase(uuid);

    std::cout << "结束应用 pid: " << pid << std::endl;
    tinyPiX_sys_kill_process(globalAgent, pid);

    return true;
}

/*! 终止所有应用进程 */
bool TPR_KillAllApp(void)
{
    std::lock_guard<std::mutex> lockG(globalAppMutex);

    // 杀掉所有应用
    for (const auto &appIdIter : appUuidPidMap)
    {
        tinyPiX_sys_kill_process(globalAgent, appIdIter.second);
    }
    // 清理缓存的应用运行信息
    appUuidPidMap.clear();

    return true;
}

#endif
