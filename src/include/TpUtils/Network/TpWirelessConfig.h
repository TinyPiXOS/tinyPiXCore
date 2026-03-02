#ifndef __TP_WIRELESS_CONFIG_H
#define __TP_WIRELESS_CONFIG_H

#include <TpCore.h>
#include "TpSignalSlot.h"
#include "TpWirelessInfo.h"

TP_DEF_VOID_TYPE_VAR(ITpWirelessConfigData);

class TpWirelessConfig
{
public:
    TpWirelessConfig(const TpString &name);
    ~TpWirelessConfig();

public:
    /// @brief 开始扫描无线网络
    /// @return
    tpInt32 startScan();
    /// @brief 停止无线网络扫描
    /// @return
    tpInt32 stopScan();
    /// @brief 获取扫描结果
    /// @return
    TpList<TpWirelessInfo> scanList();

    /// @brief 连接到加密无线网络
    /// @param ssid wifi名称
    /// @param psk wifi密码
    /// @param timeout 超时时间，毫秒
    /// @return
    tpInt32 connectWireless(const TpString &ssid, const TpString &psk = "", tpUInt32 timeout = 50000);
    /// @brief 断开无线网络的连接
    /// @return
    tpInt32 disconnectWireless();
    /// @brief 获取连接的wifi的SSID
    /// @return
    TpString getWirelessSsid();
    /// @brief 设置本机热点的的SSID
    /// @param ssid
    /// @return
    tpInt32 setHotspotSsid(const TpString &ssid);
    /// @brief 设置本机热点的密码
    /// @param password
    /// @return
    tpInt32 setHotspotPwd(const TpString &password);
    /// @brief 打开本机热点
    /// @return
    tpInt32 openHotspot();

public
signals:
    declare_signal(wirelessRemove, TpString);
    declare_signal(wirelessAdd, TpWirelessInfo);

private:
    int threadScan(tpUInt16 time);

private:
    ITpWirelessConfigData *data_;
};

#endif
