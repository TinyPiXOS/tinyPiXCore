/*///------------------------------------------------------------------------------------------------------------------------//
		扫描到的无限网络的信息
说 明 :
日 期 : 2024.12.25

/*///------------------------------------------------------------------------------------------------------------------------//

#include "TpWirelessInfo.h"

struct TpWirelessInfoData
{
	TpString ssid;
	TpString mac;
	double freq;
	tpUInt16 channel;
	tpInt8 level;
	TpWirelessInfoData(const TpString& mac_) :mac(mac_)  {}
	TpWirelessInfoData(){}
};


TpWirelessInfo::TpWirelessInfo(const TpString& mac)
{
	data_=new TpWirelessInfoData(mac);
}

TpWirelessInfo::~TpWirelessInfo()
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	if(wlData)
		delete(wlData);
}

// 拷贝构造（深拷贝）
TpWirelessInfo::TpWirelessInfo(const TpWirelessInfo& other) {
    TpWirelessInfoData* otherData = static_cast<TpWirelessInfoData*>(other.data_);
    data_ = new TpWirelessInfoData(); 
    TpWirelessInfoData* data = static_cast<TpWirelessInfoData*>(data_);
   
    // 深拷贝数据缓冲区
	data->ssid=otherData->ssid;
	data->mac=otherData->mac;
	data->freq=otherData->freq;
	data->channel=otherData->channel;
	data->level=otherData->level;
}

// 赋值运算符（深拷贝）
TpWirelessInfo& TpWirelessInfo::operator=(const TpWirelessInfo& other) {
    if (this == &other) return *this;

	TpWirelessInfoData* otherData = static_cast<TpWirelessInfoData*>(other.data_);
	TpWirelessInfoData* data = static_cast<TpWirelessInfoData*>(data_);

    // 深拷贝新数据
	data->ssid=otherData->ssid;
	data->mac=otherData->mac;
	data->freq=otherData->freq;
	data->channel=otherData->channel;
	data->level=otherData->level;
    return *this;
}


TpString TpWirelessInfo::getSsid() const
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	return wlData->ssid;
}

TpString TpWirelessInfo::getMacAddr() const
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	return wlData->mac;
}

double TpWirelessInfo::getFreq() const
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	return wlData->freq;
}

tpUInt16 TpWirelessInfo::getChannel() const
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	return wlData->channel;
}

tpInt8 TpWirelessInfo::getLevel() const
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	return wlData->level;
}

tpInt32 TpWirelessInfo::setSsid(TpString& ssid)
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	wlData->ssid=ssid;
	return 0;
}

tpInt32 TpWirelessInfo::setFreq(double freq)
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	wlData->freq=freq;
	return 0;
}

tpInt32 TpWirelessInfo::setChannel(tpUInt16 channel)
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	wlData->channel=channel;
	return 0;
}

tpInt32 TpWirelessInfo::setLevel(tpInt8 level)
{
	TpWirelessInfoData *wlData=static_cast<TpWirelessInfoData *>(data_);
	wlData->level=level;
	return 0;
}
