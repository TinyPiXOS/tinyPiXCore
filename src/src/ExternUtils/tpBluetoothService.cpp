/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙协议相关
说 明 : 
日 期 : 2025.8.7

/*///------------------------------------------------------------------------------------------------------------------------//

#include "tpBluetoothService.h"
#include "bluetooth/include/blt_device.h"

struct tpBluetoothServiceData{
	tpBluetoothAddress addr;	//服务产生的设备
	tpString uuid;				//服务的uuid
	tpString name;				//服务的名字
	tpString desc;				//服务的描述
	tpUInt16 channel;			//通道
	tpBluetoothServiceData(){
		channel=0;
	};
};


tpBluetoothService::tpBluetoothService()
{
	data_ = new tpBluetoothServiceData();
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	if(!data)
	{
		fprintf(stderr,"[Error]: tpBluetoothServiceData\n");
		return ;
	}


}

// 拷贝赋值
tpBluetoothService& tpBluetoothService::operator=(const tpBluetoothService& other) {
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
    if (this != &other) {
        delete[] data;
        data_ = new tpBluetoothServiceData();
		memcpy(data_,other.data_,sizeof(tpBluetoothServiceData));
    }
    return *this;
}




tpBluetoothService::~tpBluetoothService()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	if(!data)
		return ;

	delete(data);
}



/// @brief 获取服务的UUID
/// @return 
tpString tpBluetoothService::getServiceUuid() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	return data->uuid;
}

/// @brief 设置服务的UUID
/// @param uuid 
/// @return 
int tpBluetoothService::setServiceUuid(const tpString& uuid)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	return 0;
}

/// @brief 获取服务的名字
/// @return 
tpString tpBluetoothService::getServiceName() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	return data->name;
}

/// @brief 设置服务的名字
/// @param name 
/// @return 
int tpBluetoothService::setServiceName(const tpString& name)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->name=name;
	return 0;
}

/// @brief 获取产生服务的设备地址
/// @return 
tpBluetoothAddress tpBluetoothService::getDeviceAddress() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	return data->addr;
}

/// @brief 设置产生服务的设备地址
/// @param addr 
/// @return 
int tpBluetoothService::setDeviceAddress(tpBluetoothAddress& addr)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->addr=addr;
	return 0;
}

/// @brief 获取服务的描述
/// @return 
tpString tpBluetoothService::getServiceDescription() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	return data->desc;
}

/// @brief 设置服务的描述
/// @param desc 
/// @return 
int tpBluetoothService::setServiceDescription(tpString& desc)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->desc=desc;
	return 0;
}

/// @brief 获取通道号
/// @return 
tpUInt16 tpBluetoothService::getServerChannel()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	return data->channel;
}
