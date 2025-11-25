#ifndef __GATEWAY_RPC_SERVER_H
#define __GATEWAY_RPC_SERVER_H

#include <erpc_server_setup.h>
#include "c_TpSystemApi_server.h"
#include "TpSystemApi_server.hpp"
#include "tinyPiXUtils.h"
#include "tinyPiXSys.h"

#include <TpProcess.h>
#include <TpFileInfo.h>
#include <Service/TpAppConfigIO.h>
#include <TpHash.h>
#include <TpString.h>
#include <TpNetDataGlobal.h>

static TpHash<TpString, int32_t> appUuidPidMap = TpHash<TpString, int32_t>();
static IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

binary_t *TPR_RunAppProcessInfo(void)
{
    std::cout << "TPR_RunAppProcessInfo called" << std::endl;

    try
    {
        TpRPCRunAppProcessInfo processInfo;

        for (auto it = appUuidPidMap.begin(); it != appUuidPidMap.end(); ++it)
        {
            processInfo.uuidList.emplace_back(it->first);
            processInfo.pidList.emplace_back(it->second);
        }

        PStructPackager package;
        processInfo.StructSerialize(package);

        // 创建返回缓冲区
        binary_t *rpcRespData = new binary_t();
        rpcRespData->data = new uint8_t[package.size()];
        rpcRespData->dataLength = package.size();
        memcpy(rpcRespData->data, package.data(), package.size());

        return rpcRespData;
    }
    catch (const std::exception &e)
    {
        std::cerr << "TPR_RunAppProcessInfo 异常: " << e.what() << std::endl;
        return nullptr;
    }
}

/*! 指定UUID启动应用 */
bool TPR_StartApp(const char *uuid, const list_string_1_t *args)
{
    if (appUuidPidMap.contains(uuid))
    {
        // 应用已启动；恢复应用
        int32_t pid = appUuidPidMap.value(uuid);

        // 根据pid查询winid
        PiShmBytes *appIdList = nullptr;
        int appSize = 0;
        tinyPiX_sys_find_win_ids(globalAgent, &appIdList, &appSize, Q_FIXS);

        std::cout << "tinyPiX_sys_find_win_ids :  " << appSize << std::endl;
        int32_t winId = -1;
        for (int i = 0; i < appSize; ++i)
        {
            PiShmBytes appIdInfo = appIdList[i];
            if (appIdInfo.p_id == pid)
            {
                winId = appIdInfo.s_id;
                break;
            }
        }

        std::cout << "恢复应用 pid/winid: " << pid << " , " << winId << std::endl;
        tinyPiX_sys_set_visible(globalAgent, winId, true);
        tinyPiX_sys_set_active(globalAgent, winId, true);
    }
    else
    {
        // 解析应用信息
        TpAppConfigIO configIO(uuid);
        TpString runnerPath = configIO.runnerPath();

        TpFileInfo runnerFileInfo(runnerPath);
        if (!runnerFileInfo.exists())
        {
            std::cout << "应用 " << configIO.appName() << " 可执行程序不存在!" << std::endl;
            return false;
        }

        TpVector<TpString> vectorArgs;
        for (int i = 0; i < args->elementsCount; ++i)
        {
            vectorArgs.emplace_back(args->elements[i]);
        }

        TpProcess exeProcess;
        exeProcess.start(runnerPath, vectorArgs);
        // exeProcess.start(exePathStr);
        int32_t processPID = exeProcess.launchProcessID();

        std::cout << "processPID " << processPID << std::endl;
        // std::lock_guard<std::mutex> lockG(apiData->readAppMutex);
        appUuidPidMap[uuid] = processPID;
    }

    return true;
}

/*! 指定应用UUID终止进程 */
bool TPR_KillApp(const char *uuid)
{
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
    // 获取所有应用列表
    PiShmBytes *appIdList = nullptr;
    int appSize = 0;
    tinyPiX_sys_find_win_ids(globalAgent, &appIdList, &appSize, 1);

    // 杀掉所有应用
    for (int i = 0; i < appSize; ++i)
    {
        PiShmBytes appIdInfo = appIdList[i];
        tinyPiX_sys_kill_process(globalAgent, appIdInfo.p_id);
    }

    // 清理缓存的应用运行信息
    {
        // std::lock_guard<std::mutex> lockG(apiData->readAppMutex);
        appUuidPidMap.clear();
    }

    return true;
}

#endif
