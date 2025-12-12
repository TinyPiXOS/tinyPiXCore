#ifndef __TP_LAUNCHED_DATA_DEF_H
#define __TP_LAUNCHED_DATA_DEF_H

#include <TpString.h>
#include <TpVector.h>
#include <TpNetDataGlobal.h>

/// @brief 启动进程信息；Service发送至Launched进程
static const char *TpLaunchedProcessDataKey = "TinyPiXOS.Service.TpLaunchedProcessDataKey";
struct TpLaunchedProcessData : NetDataBase
{
    std::string processPath;
    std::vector<std::string> argList;

    TpLaunchedProcessData() : NetDataBase(TpLaunchedProcessDataKey), processPath("")
    {
    }

    StructSerializeFunc;
};
TP_STRUCT_PACKAGE(TpLaunchedProcessData, dataHead_, processPath, argList);

/// @brief Launched进程启动进程结果反馈；Launched发送至Service进程
static const char *TpLaunchedProcessAckKey = "TinyPiXOS.Service.TpLaunchedProcessAckKey";
struct TpLaunchedProcessAck : NetDataBase
{
    bool launchSucc;
    int32_t pid;

    TpLaunchedProcessAck() : NetDataBase(TpLaunchedProcessAckKey), launchSucc(false), pid(0)
    {
    }

    StructSerializeFunc;
};
TP_STRUCT_PACKAGE(TpLaunchedProcessAck, dataHead_, launchSucc, pid);

#endif