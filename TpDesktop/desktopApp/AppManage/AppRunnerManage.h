#ifndef __TP_APP_MANAGER_H
#define __TP_APP_MANAGER_H

#include <TpCore.h>
#include "TpSize.h"
#include "TpVector.h"
#include "AppManage/TpAppConfigIO.h"

class TpImage;

TP_DEF_VOID_TYPE_VAR(IAppRunnerManageData);
/// @brief 桌面应用管理相关接口
class AppRunnerManage
{
public:
    /// @brief 正在运行的应用信息
    struct RunAppInfo
    {
        TpAppConfigIO appInfo;
        int32_t pid;

        RunAppInfo() : pid(0)
        {
        }

        bool isEmpty() { return appInfo.appUuid().empty(); }
    };

public:
    /// @brief 获取唯一单例
    /// @return 实例指针
    static AppRunnerManage *Instance();

public:
    /// @brief 指定应用UUID获取该应用缩略图；若应用未运行则返回空对象
    /// @param uuid 应用UUID
    /// @return 应用缩略图；若应用未运行则返回空对象
    TpImage appImage(const TpString &uuid);

    /// @brief 指定应用UUID启动该应用；若应用已启动则将应用重新置顶显示
    /// @param uuid 应用UUID
    /// @param argList 应用启动参数
    /// @return 启动应用结果
    bool startApp(const TpString &uuid, const TpVector<TpString> &argList = TpVector<TpString>());

    /// @brief 指定应用UUID停止应用进程
    /// @param uuid 应用UUID
    /// @return 停止结果；应用进程未运行同样返回false
    bool killApp(const TpString &uuid);

    /// @brief 停止当前运行的所有应用进程
    /// @return 停止结果
    bool killAllApp();

    /// @brief 获取当前运行应用信息列表
    /// @return 应用信息列表
    TpVector<RunAppInfo> runAppInfoList();

    /// @brief 指定应用UUID获取应用运行信息
    /// @param uuid 应用UUID
    /// @return 该应用的进程信息；若应用未运行则返回空
    RunAppInfo runAppInfo(const TpString &uuid);

public:
    /// @brief 禁用拷贝构造
    AppRunnerManage(const AppRunnerManage &) = delete;
    /// @brief 禁用赋值构造
    AppRunnerManage &operator=(const AppRunnerManage &) = delete;

private:
    AppRunnerManage();
    virtual ~AppRunnerManage();

private:
    IAppRunnerManageData *data_;
};

#endif
