#ifndef __TP_APP_CONFIG_IO_H
#define __TP_APP_CONFIG_IO_H

#include "TpUtils.h"
#include "TpString.h"
#include "TpVector.h"

TP_DEF_VOID_TYPE_VAR(ItpAppConfigIOData);
/// @brief 应用配置信息访问IO
class TpAppConfigIO
{
public:
    /// @brief 应用组件信息
    struct AppWidgetInfo
    {
        TpString appUuid;
        TpString linkAppUuid;
        TpString widgetUuid;
        TpString name;
        TpString path;
        AppWidgetInfo() : appUuid(""), linkAppUuid(""), widgetUuid(""), name(""), path("")
        {
        }
    };

public:
    TpAppConfigIO();
    TpAppConfigIO(const TpString &appUuid);
    virtual ~TpAppConfigIO();

    /// @brief 获取所有已安装应用的UUID列表
    /// @return 已安装应用的UUID列表
    static TpVector<TpString> installAppUuidList();

    /// @brief 手动设置应用UUID；每次调用后会刷新缓存
    /// @param appUuid 应用UUID
    bool setAppUuid(const TpString &appUuid);

    /// @brief 刷新缓存；刷新后会重新解析所有配置文件
    void refreshCache();

    /// @brief 获取图标绝对路径
    /// @return 应用icon绝对路径
    TpString iconPath();

    /// @brief 获取应用可执行文件绝对路径
    /// @return 文件绝对路径
    TpString runnerPath();

    /// @brief 获取应用名称
    /// @return 应用名称
    TpString appName();

    /// @brief 获取应用所有小组件信息列表
    /// @return 小组件信息列表
    TpVector<AppWidgetInfo> widgetsInfo();

    /// @brief 指定小组件UUID获取小组件信息
    /// @param widgetUuid 小组件UUID
    /// @return 小组件信息;未找到则返回nullptr
    tpShared<AppWidgetInfo> widgetInfo(const TpString &widgetUuid);

    /// @brief 获取默认小组件信息
    /// @return 无默认小组件则返回 nullptr
    tpShared<AppWidgetInfo> defaultWidgetInfo();

private:
    ItpAppConfigIOData *data_;
};

#endif
