#ifndef _TP_BLUETOOTH_DEVICE_H_
#define _TP_BLUETOOTH_DEVICE_H_

#include "tpUtils.h"
#include "tpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothDeviceData);

/// @brief 蓝牙设备型管信息(即周围的蓝牙)
class tpBluetoothDevice{
public:
	enum tpDeviceType{
		
	};
public:
	tpBluetoothDevice(const char *name,
						const char *address,
						uint16_t rssi,
						uint32_t class_type,
						uint8_t paired,
						uint8_t legacy_pairing,
						char *alias,
						char *icon);
	~tpBluetoothDevice();

    /// @brief 拷贝构造(不建议使用)
    /// @param other 
    tpBluetoothDevice(const tpBluetoothDevice &other);
    /// @brief 拷贝赋值(不建议使用)
    /// @param other 
    /// @return 
    tpBluetoothDevice &operator=(const tpBluetoothDevice &other);

public:
	/// @brief 获取蓝牙的名字
	/// @return 
	tpString getName();

	/// @brief 获取蓝牙地址
	/// @return 
	tpBluetoothAddress getAddress() const;

	/// @brief 获取蓝牙信号强度
	/// @return 
	tpInt16 getRssi();

	/// @brief 获取蓝牙的配对状态
	/// @return 
	tpBool getPaired();

	/// @brief 获取设备是否支持传统配对方式，0表示仅支持
	/// @return 
	tpBool getLegacyPairing();

	/// @brief 获取蓝牙别名
	/// @return 
	char *getAlias();

	/// @brief 获取蓝牙的图标
	/// @return 
	char *getIcon();

	/// @brief 获取设备类型
	/// @return 
	tpUInt32 getDeviceType();

	/// @brief 获取主设备类型，暂未实现
	/// @return 
	tpUInt8 getMajorDeviceClass();
	
	/// @brief 获取次设备类型，暂未实现
	/// @return 
	tpUInt8 getMinorDeviceClass();

private:
	ItpBluetoothDeviceData *data_;
};










#endif
