#ifndef __TP_APP_CONFIG_IO_H
#define __TP_APP_CONFIG_IO_H

#include "tpUtils.h"
#include "tpString.h"
#include "tpVector.h"

TP_DEF_VOID_TYPE_VAR(ItpAppConfigIOData);
/// @brief 应用配置信息访问IO
class tpAppConfigIO
{
public:
    tpAppConfigIO();
    tpAppConfigIO(const tpString &appUuid);
    virtual ~tpAppConfigIO();

    /// @brief 获取所有已安装应用的UUID列表
    /// @return 已安装应用的UUID列表
    static tpVector<tpString> installAppUuidList();

    /// @brief 手动设置应用UUID；每次调用后会刷新缓存
    /// @param appUuid 应用UUID
    bool setAppUuid(const tpString &appUuid);

    /// @brief 刷新缓存；刷新后会重新解析所有配置文件
    void refreshCache();

    /// @brief 获取图标绝对路径
    /// @return 应用icon绝对路径
    tpString iconPath();

    /// @brief 获取应用可执行文件绝对路径
    /// @return 文件绝对路径
    tpString runnerPath();

    /// @brief 获取应用名称
    /// @return 应用名称
    tpString appName();

private:
    ItpAppConfigIOData *data_;
};

#endif
