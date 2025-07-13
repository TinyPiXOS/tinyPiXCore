#ifndef __TP_PROCESS_H
#define __TP_PROCESS_H

#include "tpString.h"
#include "tpVector.h"

/// @brief 进程管理类
TP_DEF_VOID_TYPE_VAR(ItpProcessData);
class tpProcess
{
public:
    enum ProcessState
    {
        NotRunning,
        Starting,
        Running
    };

public:
    tpProcess();
    ~tpProcess();

    /// @brief 启动一个进程
    /// @param program 进程文件所在全路径
    /// @param arguments 进程传入参数，可为空
    void start(const tpString &program, const tpVector<tpString> &arguments);

    /// @brief 启动一个进程
    /// @param command 启动进程cmd命令
    void start(const tpString &command);

    /// @brief 获取启动进程当前状态
    /// @return 状态枚举
    ProcessState state() const;

    bool waitForFinished(int msecs = 30000);

    bool waitForStarted(int msecs = 30000);

    /// @brief 获取启动进程ID
    /// @return 进程ID，如果进程为空返回-1
    virtual int32_t launchProcessID();
    /// @brief 获取启动进程PID
    /// @return 进程PID，如果进程为空返回-1
    virtual pid_t lauanchProcessPID();
    /// @brief 获取启动进程名称
    /// @return 进程名称
    virtual tpString launchProcessName();

private:
    ItpProcessData *data_;
};

#endif