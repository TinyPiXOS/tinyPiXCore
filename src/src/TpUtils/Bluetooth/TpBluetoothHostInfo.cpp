//        本地蓝牙信息
#include "TpBluetoothHostInfo.h"
#include "TpBluetoothAddress.h"
#include "TpDbusConnectManage.h"
#include "blt_hard.h"
#include "blt_device.h"
#include "blt_agent.h"
#include "blt_sdp.h"

struct TpBluetoothHostInfoData
{
    TpBluetoothAddress address;
    TpString name;

    TpBluetoothHostInfoData()
    {
    };
    ~TpBluetoothHostInfoData() {
        // delete(address);	//address为指针时需要调用
        // if(adapter)

    };
};



TpBluetoothHostInfo::TpBluetoothHostInfo(int id, const char *address, const char *name)
{
    data_ = new TpBluetoothHostInfoData();
    TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);

    data->address = TpBluetoothAddress(TpString(address));
    data->name = TpString(name);

}

TpBluetoothHostInfo::TpBluetoothHostInfo(const TpString &name) : TpBluetoothHostInfo(name.c_str())
{
}

TpBluetoothHostInfo::TpBluetoothHostInfo(const char *name)
{
    data_ = new TpBluetoothHostInfoData();
    TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);

    data->name = TpString(name);
}


TpBluetoothHostInfo::TpBluetoothHostInfo(const TpBluetoothHostInfo &other)
{
	data_ = new TpBluetoothHostInfoData();
	TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);
	TpBluetoothHostInfoData *otherData = static_cast<TpBluetoothHostInfoData *>(other.data_);

	data->address = otherData->address;
	data->name = otherData->name;
}

// 拷贝赋值运算符声明
TpBluetoothHostInfo& TpBluetoothHostInfo::operator=(const TpBluetoothHostInfo &other)
{
	if (this == &other)
		return *this;

	TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);
	TpBluetoothHostInfoData *otherData = static_cast<TpBluetoothHostInfoData *>(other.data_);

	data->address = otherData->address;
	data->name = otherData->name;

	return *this;
}

// 移动赋值运算符声明
TpBluetoothHostInfo& TpBluetoothHostInfo::operator=(TpBluetoothHostInfo&& other) noexcept
{
	if (this == &other)
		return *this;

	TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);
	TpBluetoothHostInfoData *otherData = static_cast<TpBluetoothHostInfoData *>(other.data_);

	data->address = otherData->address;
	data->name = otherData->name;

	return *this;
}

bool TpBluetoothHostInfo::operator!=(const TpBluetoothHostInfo &other) const
{
	return !(*this == other);
}

bool TpBluetoothHostInfo::operator==(const TpBluetoothHostInfo &other) const
{
	if (this == &other)
		return true;

	TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);
	TpBluetoothHostInfoData *otherData = static_cast<TpBluetoothHostInfoData *>(other.data_);

	return data->address == otherData->address && data->name == otherData->name;
}



TpBluetoothHostInfo::~TpBluetoothHostInfo()
{
    TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);
    if (!data)
        return;

    delete (data);
}


TpString TpBluetoothHostInfo::name() const
{
    TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);
    return data->name;
}

void TpBluetoothHostInfo::setName(const TpString &name)
{
    TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);
    data->name = name;
}


TpBluetoothAddress TpBluetoothHostInfo::address() const
{
    TpBluetoothHostInfoData *data = static_cast<TpBluetoothHostInfoData *>(data_);
    return data->address;
}

