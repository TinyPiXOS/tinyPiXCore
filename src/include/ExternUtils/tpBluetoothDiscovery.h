#ifndef _TP_BLUETOOTH_DISCOVER_H_
#define _TP_BLUETOOTH_DISCOVER_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"
#include "tpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothDiscoveryData);

/// @brief 蓝牙扫描，可以用于扫描周围的蓝牙
class tpBluetoothDiscovery{
public:
	//蓝牙扫描过滤器
	enum tpDiscoveryInquiryType{
		TP_DISCOVER_INQU_AUTO, 			//自动
		TP_DISCOVER_INQU_CLASSIC,		//经典蓝牙
		TP_DISCOVER_INQU_LOW_ENERGY		//低功耗蓝牙
	};

public:
	tpBluetoothDiscovery(const char *local);
	tpBluetoothDiscovery(const tpString& local);
	~tpBluetoothDiscovery();
public:
	/// @brief 开始扫描
	void start();
	/// @brief 停止扫描
	void stop();
public:
	/// @brief 设置扫描蓝牙的类型
	/// @param type 类型
	/// @return 
	int setInquiryType(tpDiscoveryInquiryType type);

	/// @brief 设置蓝牙扫描的rssi阈值(暂不支持)
	/// @param value 
	/// @return 
	int setRssiThreshold(int value);

	/// @brief 获取蓝牙扫描的设备列表(改为信号的方式后此接口暂不支持)
	/// @return 
	/// tpList<tpBluetoothDevice *> getDeviceList();
	
	/// @brief 设置扫描超时时间(应用于低功耗)(暂不支持)
	/// @param ms 
	/// @return 
	int setTimeout(uint32_t ms);

public signals:
	declare_signal(bluetoothDeviceRemove, tpBluetoothAddress&);
	declare_signal(bluetoothDeviceAdd, tpBluetoothDevice&);

public:
	void onDeviceAdd(const void *remote);    // 真正处理新增设备
    void onDeviceRemove(const void *remote); // 真正处理移除设备

private:
	void discovery();

	ItpBluetoothDiscoveryData *data_;
};











#endif
