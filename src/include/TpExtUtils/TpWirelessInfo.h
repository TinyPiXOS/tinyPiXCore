#ifndef __TP_WIRELESS_INFO_H
#define __TP_WIRELESS_INFO_H

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ItpWirelessInfoData);

class TpWirelessInfo
{
public:
    TpWirelessInfo(const TpString &mac);
    ~TpWirelessInfo();

    TpWirelessInfo(const TpWirelessInfo &other);
    TpWirelessInfo &operator=(const TpWirelessInfo &other);

public:
    /// @brief 获取SSID
    /// @return
    TpString getSsid() const;
    /// @brief 获取MAC地址
    /// @return
    TpString getMacAddr() const;
    /// @brief 获取频率
    /// @return
    double getFreq() const;
    /// @brief 获取通道号
    /// @return
    tpUInt16 getChannel() const;
    /// @brief 获取信号强度
    /// @return
    tpInt8 getLevel() const;

public:
    friend class TpNetworkInterface;
    tpInt32 setSsid(TpString &ssid);
    tpInt32 setFreq(double freq);
    tpInt32 setChannel(tpUInt16 channel);
    tpInt32 setLevel(tpInt8 level);

private:
    ItpWirelessInfoData *data_;
};

#endif
