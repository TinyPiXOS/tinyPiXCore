#ifndef _TP_BLUETOOTH_ADDRESS_H_
#define _TP_BLUETOOTH_ADDRESS_H_

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothAddressData);

/// @brief 为蓝牙分配地址
class tpBluetoothAddress{
public:
	tpBluetoothAddress(const tpBluetoothAddress &other);
	tpBluetoothAddress(const tpString &address);
	tpBluetoothAddress();
	~tpBluetoothAddress();

public:
	// 拷贝赋值运算符声明
	tpBluetoothAddress& operator=(const tpBluetoothAddress& other);
	// 移动赋值运算符声明
	tpBluetoothAddress& operator=(tpBluetoothAddress&& other) noexcept;
	// 直接接受字符串的赋值运算符（可选优化）
	tpBluetoothAddress& operator=(const char* address);
	
	bool operator==(const tpBluetoothAddress &other);
    bool operator!=(const tpBluetoothAddress &other);


public:
	tpBool isNull();
	tpString toString();
	tpUInt64 toUInt64();
private:
	void parseString(const tpString &str);

private:
	ItpBluetoothAddressData *data_;
};



#endif