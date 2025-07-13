#ifndef _TP_BLUETOOTH_LOCAL_H_
#define _TP_BLUETOOTH_LOCAL_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothLocalData);

/// @brief 可以对本地蓝牙设备进行管理
class tpBluetoothLocal
{
public:
	enum tpLocalPair
	{
		TP_LOCAL_PAIRED,		   // 已配对但未授权(未信任)
		TP_LOCAL_UNPAIRED,		   // 未配对
		TP_LOCAL_AUTHORIZED_PAIRED // 已配对且已授权(配对+信任)
	};

public:
	tpBluetoothLocal(int id, const char *address, const char *name);
	tpBluetoothLocal(const char *name);
	tpBluetoothLocal(tpBluetoothLocal &other);	// 拷贝构造
	tpBluetoothLocal(tpBluetoothLocal &&other); // 移动构造
	~tpBluetoothLocal();

public:
	/// @brief 获取所有适配器列表
	/// @return
	static tpList<tpBluetoothLocal> getAllDevice();

public:
	/// @brief 获取蓝牙名字
	/// @return
	tpString getName();
	int setName(const tpString &name);

	/// @brief 获取蓝牙地址
	/// @return
	tpBluetoothAddress getAddress();

	/// @brief 获取以配对的设备列表
	/// @return
	tpList<tpBluetoothAddress> getPairedDevices();

	/// @brief 获取已连接的设备列表
	/// @return
	tpList<tpBluetoothAddress> getConnectedDevices();

	/// @brief 设置远程蓝牙设备的信任状态
	/// @param address 远程设备地址
	/// @param trusted 信任/不信任
	/// @return
	int setTrusted(tpBluetoothAddress &address, tpBool trusted);

	/// @brief 设置蓝牙配对请求
	/// @param address 远程蓝牙的地址
	/// @param pair 配对状态(手动授权配对/取消配对/自动授权配对)
	/// @return
	int requestPairing(tpBluetoothAddress &address, tpBluetoothLocal::tpLocalPair pair);

	/// @brief 获取配对状态
	/// @param address 远程蓝牙的地址
	/// @return
	tpBluetoothLocal::tpLocalPair getPairStatus(tpBluetoothAddress &address);

	int removeDevice(tpBluetoothAddress &address);

	/// @brief 设置蓝牙可见性
	/// @param discoverable 可见/不可见
	/// @return
	int setDiscoverable(tpBool discoverable);

	/// @brief 设置蓝牙可见的超时时间
	/// @param timeout 超时时间
	/// @return
	int setDiscoverableTimeout(tpUInt32 timeout);

	/// @brief 本机蓝牙是否打开
	/// @return
	tpBool isPowerOn();

	/// @brief 打开蓝牙
	/// @return
	int powerOn();

	/// @brief 关闭蓝牙
	/// @return
	int powerOff();
	
	int getHostMode();
	int setHostMode();

private:
	void *getAdapter();

private:
	ItpBluetoothLocalData *data_;
};

#endif
