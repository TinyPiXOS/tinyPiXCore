#ifndef _TP_BLUETOOTH_SERVICE_H_
#define _TP_BLUETOOTH_SERVICE_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothServiceData);


class tpBluetoothService{
public:
	enum Protocol{
		TP_BLUET_UNKNOWN_PROTOCOL,
		TP_BLUET_L2CAP_PROTOCOL,
		TP_BLUET_RFCOMM_PROTOCOL,
	};

	enum Profile{
		TP_BLUET_UNKNOWN_PROFILE,
		TP_BLUET_SPP_PROFILE,
	};

public:
	tpBluetoothService();
	tpBluetoothService& operator=(const tpBluetoothService &other);
	tpBluetoothService(const tpBluetoothService &other);

	~tpBluetoothService();

public:
	/// @brief 获取服务的UUID
	/// @return 
	tpString getServiceUuid() const;

	/// @brief 设置服务的UUID
	/// @param uuid 
	/// @return 
	int setServiceUuid(const tpString& uuid);

	/// @brief 获取服务的名字
	/// @return 
	tpString getServiceName() const;

	/// @brief 设置服务的名字
	/// @param name 
	/// @return 
	int setServiceName(const tpString& name);

	/// @brief 获取产生服务的设备地址
	/// @return 
	tpBluetoothAddress getDeviceAddress() const;

	/// @brief 设置产生服务的设备地址
	/// @param addr 
	/// @return 
	int setDeviceAddress(tpBluetoothAddress& addr);

	/// @brief 获取服务的描述
	/// @return 
	tpString getServiceDescription() const;

	/// @brief 设置服务的描述
	/// @param desc 
	/// @return 
	int setServiceDescription(tpString& desc);

	/// @brief 获取通道号
	/// @return 
	tpUInt16 getServerChannel();
private:
	ItpBluetoothServiceData *data_;
};




#endif