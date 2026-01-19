#ifndef _TP_BLUETOOTH_SERVICE_DISCOVERY_H_
#define _TP_BLUETOOTH_SERVICE_DISCOVERY_H_

#include <TpCore.h>
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"
#include "TpBluetoothService.h" 
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ITpBluetoothServiceDiscoveryData);


class TpBluetoothServiceDiscovery{
public:
	TpBluetoothServiceDiscovery(const TpBluetoothAddress &addr);
	~TpBluetoothServiceDiscovery();

public:
	/// @brief 扫描目标主机支持的服务(SDP通信获取)，成功获取到服务后自动结束
	/// @return 
	int discoveryServices();

	/// @brief 设置目标主机地址
	/// @param addr 
	/// @return 
	int setRemoteAddress(const TpBluetoothAddress &addr);

	/// @brief 获取目标主机地址
	/// @return 
	TpBluetoothAddress getRemoteAddress() const;

	/// @brief 设置UUID过滤器(暂不支持)
	/// @param uuid 
	/// @return 
	int setUuidFilter(const TpBluetoothUuid &uuid);

	/// @brief 设置UUID过滤器列表(暂不支持)
	/// @param uuid 
	/// @return 
	int setUuidFilter(const TpList<TpBluetoothUuid> &uuid);

	/// @brief 获取UUID过滤器列表
	/// @return 
	TpList<TpBluetoothUuid> getUuidFilter() const;

	/// @brief 是否正在扫描中
	/// @return 
	tpBool isDiscovering() const;

	/// @brief 获取扫描结果
	/// @return 
	TpList<TpBluetoothService> getDiscoveredServices()const ;

public
signals:
    declare_signal(finished,TpList<TpBluetoothService>&);

private:
	void discoveryOnce();
private:
	ITpBluetoothServiceDiscoveryData *data_;
};




#endif