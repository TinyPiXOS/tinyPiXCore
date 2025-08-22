/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙协议相关
说 明 : 
日 期 : 2025.8.7

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include "tpBluetoothService.h"
#include "tpDbusConnectManage.h"
#include "bluetooth/include/blt_device.h"
#include "bluetooth/include/blt_service_reg.h"
#include "bluetooth/include/blt_sdp.h"

struct tpBluetoothServiceData{
	tpBluetoothAddress addr;	//服务产生或注册的设备
	tpBluetoothUuid uuid;		//服务的uuid
	tpString name;				//服务的名字
	tpString desc;				//服务的描述
	tpUInt16 port;			//通道
	tpBool is_registed;			//是否注册
	int servicefd;
	tpBluetoothServiceData(){
		port=0;
		is_registed=TP_FALSE;
	};
};


tpBluetoothService::tpBluetoothService()
{
	data_ = new tpBluetoothServiceData();
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	if(!data)
	{
		fprintf(stderr,"[Error]: tpBluetoothServerData\n");
		return ;
	}
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	service_registry_init();
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
//	unregisterService();
	delete(data);
}



/// @brief 获取服务的UUID
/// @return 
tpBluetoothUuid tpBluetoothService::getServiceUuid() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	return data->uuid;
}

/// @brief 设置服务的UUID
/// @param uuid 
/// @return 
int tpBluetoothService::setServiceUuid(const tpBluetoothUuid& uuid)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->uuid=uuid;
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
int tpBluetoothService::setServiceDescription(const tpString& desc)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->desc=desc;
	return 0;
}


int tpBluetoothService::setServiceChannel(tpUInt16 channel)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->port=channel;
	return 0;
}

/// @brief 获取通道号
/// @return 
tpUInt16 tpBluetoothService::getServerChannel()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	return data->port;
}

tpBool tpBluetoothService::registerService(const tpBluetoothAddress& address)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	if(data->uuid.isNull())
	{
		fprintf(stderr,"[Error]: 未设置uuid\n");
		return TP_FALSE;
	}

	const char* name = bluet_uuid_to_name(data->uuid.toUInt16());
	if (!name) {
		fprintf(stderr,"[Error]: uuid不合法\n");
		return TP_FALSE;
	}
	
	if(data->name.empty())
	{
		data->name=tpString(name);
	}

	data->servicefd=register_bluetooth_service( data->uuid.toString().c_str(),
                                  data->name.c_str(),
                                  data->port, 
                                  "server");		//暂时只支持注册为服务端(客户端暂无必要场景)
	if(data->servicefd<0)
		return TP_FALSE;
	data->is_registed=TP_TRUE;
	return data->is_registed;
}

tpBool tpBluetoothService::isRegisted()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	return data->is_registed;
}

tpBool tpBluetoothService::unregisterService()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	unregister_bluetooth_service(data->servicefd);
	data->is_registed=TP_FALSE;
	return TP_TRUE;
}

