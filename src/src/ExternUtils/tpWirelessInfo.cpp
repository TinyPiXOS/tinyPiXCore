/*///------------------------------------------------------------------------------------------------------------------------//
		扫描到的无限网络的信息
说 明 :
日 期 : 2024.12.25

/*///------------------------------------------------------------------------------------------------------------------------//

#include "tpWirelessInfo.h"

struct tpWirelessInfoData
{
	tpString ssid;
	tpString mac;
	double freq;
	tpUInt16 channel;
	tpInt8 level;
	tpWirelessInfoData(const tpString& mac_) :mac(mac_)  {}
	tpWirelessInfoData(){}
};


tpWirelessInfo::tpWirelessInfo(const tpString& mac)
{
	data_=new tpWirelessInfoData(mac);
}

tpWirelessInfo::~tpWirelessInfo()
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	if(wlData)
		delete(wlData);
}

// 拷贝构造（深拷贝）
tpWirelessInfo::tpWirelessInfo(const tpWirelessInfo& other) {
    tpWirelessInfoData* otherData = static_cast<tpWirelessInfoData*>(other.data_);
    data_ = new tpWirelessInfoData(); 
    tpWirelessInfoData* data = static_cast<tpWirelessInfoData*>(data_);
   
    // 深拷贝数据缓冲区
	data->ssid=otherData->ssid;
	data->mac=otherData->mac;
	data->freq=otherData->freq;
	data->channel=otherData->channel;
	data->level=otherData->level;
}

// 赋值运算符（深拷贝）
tpWirelessInfo& tpWirelessInfo::operator=(const tpWirelessInfo& other) {
    if (this == &other) return *this;

	tpWirelessInfoData* otherData = static_cast<tpWirelessInfoData*>(other.data_);
	tpWirelessInfoData* data = static_cast<tpWirelessInfoData*>(data_);

    // 深拷贝新数据
	data->ssid=otherData->ssid;
	data->mac=otherData->mac;
	data->freq=otherData->freq;
	data->channel=otherData->channel;
	data->level=otherData->level;
    return *this;
}


tpString tpWirelessInfo::getSsid() const
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	return wlData->ssid;
}

tpString tpWirelessInfo::getMacAddr() const
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	return wlData->mac;
}

double tpWirelessInfo::getFreq() const
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	return wlData->freq;
}

tpUInt16 tpWirelessInfo::getChannel() const
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	return wlData->channel;
}

tpInt8 tpWirelessInfo::getLevel() const
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	return wlData->level;
}

tpInt32 tpWirelessInfo::setSsid(tpString& ssid)
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	wlData->ssid=ssid;
	return 0;
}

tpInt32 tpWirelessInfo::setFreq(double freq)
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	wlData->freq=freq;
	return 0;
}

tpInt32 tpWirelessInfo::setChannel(tpUInt16 channel)
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	wlData->channel=channel;
	return 0;
}

tpInt32 tpWirelessInfo::setLevel(tpInt8 level)
{
	tpWirelessInfoData *wlData=static_cast<tpWirelessInfoData *>(data_);
	wlData->level=level;
	return 0;
}
