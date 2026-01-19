#ifndef __GATEWAY_RPC_SERVER_H
#define __GATEWAY_RPC_SERVER_H

#include <erpc_server_setup.h>
#include <erpc_port.h>
#include "c_TpDesktopAPI_server.h"
#include "TpDesktopAPI_server.hpp"
#include "tinyPiXUtils.h"
#include "tinyPiXSys.h"

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

        tinyPiX_sys_set_visible(globalAgent, pid, true);
        tinyPiX_sys_set_active(globalAgent, pid, true);

        std::cout << "恢复应用 pid: " << pid << std::endl;
    }
    else
    {
        TpAppConfigIO configIO(uuid);
        TpString runnerPath = configIO.runnerPath();

        TpVector<TpString> argList;
        for (const auto &arg : startAppData.argList)
            argList.push_back(arg);

        TpProcess exeProcess;
        exeProcess.start(runnerPath, argList);
        int32_t processPID = exeProcess.launchProcessID();

        appUuidPidMap[uuid] = processPID;

        std::cout << "启动应用: pid: " << processPID << std::endl;
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
