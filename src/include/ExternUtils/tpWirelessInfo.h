#ifndef __TP_WIRELESS_INFO_H
#define __TP_WIRELESS_INFO_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpWirelessInfoData);

class tpWirelessInfo
{
public:
	tpWirelessInfo(const tpString& mac);
	~tpWirelessInfo();

	tpWirelessInfo(const tpWirelessInfo& other);
	tpWirelessInfo &operator=(const tpWirelessInfo &other);

public:
	/// @brief 获取SSID
	/// @return 
	tpString getSsid() const;
	/// @brief 获取MAC地址
	/// @return 
	tpString getMacAddr()const;
	/// @brief 获取频率
	/// @return 
	double getFreq()const;
	/// @brief 获取通道号
	/// @return 
	tpUInt16 getChannel()const;
	/// @brief 获取信号强度
	/// @return 
	tpInt8 getLevel()const;
public:
	friend class tpNetworkInterface;
	tpInt32 setSsid(tpString& ssid);
	tpInt32 setFreq(double freq);
	tpInt32 setChannel(tpUInt16 channel);
	tpInt32 setLevel(tpInt8 level);
private:
	ItpWirelessInfoData *data_;
};



#endif
