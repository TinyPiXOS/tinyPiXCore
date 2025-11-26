#ifndef __TP_DESKTOP_DATA_H
#define __TP_DESKTOP_DATA_H

/*
 *  桌面相关交互数据定义
 */

#include <TpNetDataGlobal.h>

/// @brief 应用上线标识;应用启动时发送该主题；桌面会通知应用状态栏信息
static const char *TpDeskAppStartKey = "TinyPiXOS.Desktop.TpDeskAppStartKey";

/// @brief 桌面发布状态栏信息主题
static const char *TpDeskStatusBarInfoKey = "TinyPiXOS.Desktop.TpDeskStatusBarInfoKey";
/// @brief 桌面通知应用，桌面的状态栏信息
struct TpDeskStatusBarInfo : NetDataBase
{
    /// @brief 状态栏位置；0=上，1=右，2=下，3=左，其它值=上
    int32_t statusBarLocation;
    /// @brief 状态栏宽度值
    int32_t statusBarWidth;
    /// @brief 顶状态栏高度值
    int32_t statusBarHeight;
    /// @brief 状态栏是否显示；true显示，false隐藏
    bool statusBarVislble;

    TpDeskStatusBarInfo() : NetDataBase(TpDeskStatusBarInfoKey),
                            statusBarLocation(0), statusBarWidth(0), statusBarHeight(0), statusBarVislble(false)
    {
    }

    virtual ~TpDeskStatusBarInfo() {}

    bool operator==(const TpDeskStatusBarInfo &others)
    {
        return (statusBarLocation == others.statusBarLocation) &&
               (statusBarWidth == others.statusBarWidth) &&
               (statusBarHeight == others.statusBarHeight) &&
               (statusBarVislble == others.statusBarVislble);
    }

    StructSerializeFunc;
};
TP_STRUCT_PACKAGE(TpDeskStatusBarInfo, dataHead_, statusBarLocation, statusBarWidth, statusBarHeight, statusBarVislble);

/// @brief 应用通知桌面改变状态栏样式
static const char *TpChangeDeskStatusBarStyleKey = "TinyPiXOS.Desktop.TpChangeDeskStatusBarStyleKey";
struct TpChangeDeskStatusBarStyle : NetDataBase
{
    int32_t bgRgba;

    TpChangeDeskStatusBarStyle() : NetDataBase(TpChangeDeskStatusBarStyleKey)
    {
    }

    StructSerializeFunc;
};
TP_STRUCT_PACKAGE(TpChangeDeskStatusBarStyle, dataHead_, bgRgba);

/// @brief 应用通知桌面改变状态栏显隐
static const char *TpChangeDeskStatusBarVisibleKey = "TinyPiXOS.Desktop.TpChangeDeskStatusBarVisibleKey";
struct TpChangeDeskStatusBarVisible : NetDataBase
{
    bool visible;

    TpChangeDeskStatusBarVisible() : NetDataBase(TpChangeDeskStatusBarVisibleKey)
    {
    }

    StructSerializeFunc;
};
TP_STRUCT_PACKAGE(TpChangeDeskStatusBarVisible, dataHead_, visible);

/// @brief 应用通知桌面启动指定应用
static const char *TpRunAppKey = "TinyPiX.Desktop.TpRunAppKey";
struct TpRunApp : NetDataBase
{
    /// @brief 应用UUID
    std::string appUuid;
    /// @brief 启动参数列表
    std::vector<std::string> argList;

    TpRunApp() : NetDataBase(TpRunAppKey)
    {
    }

    StructSerializeFunc;
};
TP_STRUCT_PACKAGE(TpRunApp, dataHead_, appUuid, argList);

/// @brief RPC获取当前运行的应用信息
static const char *TpRPCRunAppProcessInfoKey = "TinyPiX.TpRPCRunAppProcessInfoKey";
struct TpRPCRunAppProcessInfo : NetDataBase
{
    std::vector<std::string> uuidList;
    std::vector<int32_t> pidList;

    TpRPCRunAppProcessInfo() : NetDataBase(TpRPCRunAppProcessInfoKey)
    {
    }

    StructSerializeFunc;
};
TP_STRUCT_PACKAGE(TpRPCRunAppProcessInfo, dataHead_, uuidList, pidList);

#endif