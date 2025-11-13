#ifndef _TP_BLUETOOTH_DISCOVER_H_
#define _TP_BLUETOOTH_DISCOVER_H_

#include <TpCore.h>
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothDiscoveryData);

/// @brief 蓝牙扫描，可以用于扫描周围的蓝牙
class TpBluetoothDiscovery{
public:
	//蓝牙扫描过滤器
	enum TpDiscoveryInquiryType{
		TP_DISCOVER_INQU_AUTO, 			//自动
		TP_DISCOVER_INQU_CLASSIC,		//经典蓝牙
		TP_DISCOVER_INQU_LOW_ENERGY		//低功耗蓝牙
	};

public:
	TpBluetoothDiscovery(const char *local);
	TpBluetoothDiscovery(const TpString& local);
	~TpBluetoothDiscovery();
public:
	/// @brief 开始扫描
	void start();
	/// @brief 停止扫描
	void stop();
public:
	/// @brief 是否处于扫描中
	/// @return 
	tpBool isDiscovering();
	/// @brief 设置扫描蓝牙的类型
	/// @param type 类型
	/// @return 
	int setInquiryType(TpDiscoveryInquiryType type);

	/// @brief 设置蓝牙扫描的rssi阈值(暂不支持)
	/// @param value 
	/// @return 
	int setRssiThreshold(int value);

	/// @brief 获取蓝牙扫描的设备列表(改为信号的方式后此接口暂不支持)
	/// @return 
	/// TpList<TpBluetoothDevice *> getDeviceList();
	
	/// @brief 设置扫描超时时间(应用于低功耗)(暂不支持)
	/// @param ms 
	/// @return 
	int setTimeout(uint32_t ms);

public signals:
	declare_signal(bluetoothDeviceRemove, TpBluetoothAddress&);
	declare_signal(bluetoothDeviceAdd, TpBluetoothDevice&);

public:
	void onDeviceAdd(const void *remote);    // 真正处理新增设备
    void onDeviceRemove(const void *remote); // 真正处理移除设备

private:
	void discovery();

	ItpBluetoothDiscoveryData *data_;
};











#endif
