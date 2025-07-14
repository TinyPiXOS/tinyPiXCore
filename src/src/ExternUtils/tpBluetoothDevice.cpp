/*///------------------------------------------------------------------------------------------------------------------------//
		远程蓝牙设备信息
说 明 : 
日 期 : 2025.4.23

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <string.h>
#include "tpBluetoothDevice.h"


struct tpBluetoothDeviceData{
	tpBluetoothAddress address;
	tpString name;
//	BltUUID uuid;	//uuid
	tpInt16 rssi;	//信号质量
	tpUInt32 class_type;	//设备类型
	tpBool paired;
	tpBool legacy_pairing;	//是否支持传统配对方式，0表示仅支持
	char *alias;
	char *icon;
	tpBluetoothDeviceData(): address(), name(), rssi(0), class_type(0), paired(TP_FALSE), legacy_pairing(TP_FALSE), alias(NULL), icon(NULL) {}

    // 深拷贝
    tpBluetoothDeviceData(const tpBluetoothDeviceData &other)
        : address(other.address),
          name(other.name),
          rssi(other.rssi),
          class_type(other.class_type),
          paired(other.paired),
          legacy_pairing(other.legacy_pairing),
          alias(nullptr),
          icon(nullptr) {
        if (other.alias) alias = strdup(other.alias);
        if (other.icon)  icon  = strdup(other.icon);
    }
	~tpBluetoothDeviceData()
	{
		if(alias)
			free(alias);
		if(icon)
			free(icon);
		alias=NULL;
		icon=NULL;
	};
};




tpBluetoothDevice::tpBluetoothDevice(const char *name,
									const char *address,
									uint16_t rssi,
									uint32_t class_type,
									uint8_t paired,
									uint8_t legacy_pairing,
									char *alias,
									char *icon)
{
	data_ = new tpBluetoothDeviceData();
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	data->address=tpBluetoothAddress(tpString(address));
	data->name=tpString(name);
	data->rssi=rssi;
	data->class_type=class_type;
	data->legacy_pairing=(legacy_pairing == 0) ? TP_FALSE : TP_TRUE;
	data->paired=(paired == 0) ? TP_FALSE : TP_TRUE;
	if(alias)
		data->alias=strdup(alias);
	if(icon)
		data->icon=strdup(icon);
}


tpBluetoothDevice::~tpBluetoothDevice()
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	if(!data)
		return ;
	delete(data);
	data=NULL;
}




// 深拷贝构造
tpBluetoothDevice::tpBluetoothDevice(const tpBluetoothDevice &other) : data_(nullptr) {
    auto od = static_cast<tpBluetoothDeviceData*>(other.data_);
    data_ = new tpBluetoothDeviceData(*od);
}

// 深拷贝赋值

// 深拷贝赋值：先清理自己，再像拷贝构造那样复制一份新的
tpBluetoothDevice &tpBluetoothDevice::operator=(const tpBluetoothDevice &other) {
    if (this != &other) {
        // delete existing data
        delete static_cast<tpBluetoothDeviceData*>(data_);
        // allocate new copy
        auto od = static_cast<tpBluetoothDeviceData*>(other.data_);
        data_ = new tpBluetoothDeviceData(*od);
    }
    return *this;
}


tpString tpBluetoothDevice::getName()
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return data->name;
}

tpBluetoothAddress tpBluetoothDevice::getAddress() const
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return data->address;
}

tpInt16 tpBluetoothDevice::getRssi()
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return data->rssi;
}

tpBool tpBluetoothDevice::getPaired()
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return data->paired;
}

tpBool tpBluetoothDevice::getLegacyPairing()	//是否支持传统配对方式，0表示仅支持
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return data->legacy_pairing;
}

char *tpBluetoothDevice::getAlias()
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return data->alias;
}

char *tpBluetoothDevice::getIcon()
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return data->icon;
}

tpUInt32 tpBluetoothDevice::getDeviceType()
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return data->class_type;
}

/* 蓝牙SIG（负责蓝牙标准的组织）在 ​​Device Class​​ 字段中定义了一个3字节的值，其中
//
​​// 第1字节的高3位​​：表示​​主要设备类​​（Major Device Class），例如“电话”、“计算机”或“音频设备”等。
​​// 第1字节的低5位​​：表示​​次要设备类​​（Minor Device Class），用于在主要类别下进一步细分设备类型。
​​// 后2字节​​：保留用于服务类别（Service Class）。
*/
tpUInt8 tpBluetoothDevice::getMajorDeviceClass()
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return 0;
}

tpUInt8 tpBluetoothDevice::getMinorDeviceClass()
{
	tpBluetoothDeviceData *data = static_cast<tpBluetoothDeviceData *>(data_);
	return 0;
}

