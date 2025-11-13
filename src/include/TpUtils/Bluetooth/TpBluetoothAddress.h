#ifndef _TP_BLUETOOTH_ADDRESS_H_
#define _TP_BLUETOOTH_ADDRESS_H_

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ItpBluetoothAddressData);

/// @brief 为蓝牙分配地址
class TpBluetoothAddress{
public:
	TpBluetoothAddress(const TpBluetoothAddress &other);
	TpBluetoothAddress(const TpString &address);
	TpBluetoothAddress();
	~TpBluetoothAddress();

public:
	// 拷贝赋值运算符声明
	TpBluetoothAddress& operator=(const TpBluetoothAddress& other);
	// 移动赋值运算符声明
	TpBluetoothAddress& operator=(TpBluetoothAddress&& other) noexcept;
	// 直接接受字符串的赋值运算符（可选优化）
	TpBluetoothAddress& operator=(const char* address);
	
	bool operator==(const TpBluetoothAddress &other);
    bool operator!=(const TpBluetoothAddress &other);


public:
	static TpBluetoothAddress any();
	tpBool isNull();
	TpString toString();
	tpUInt64 toUInt64();
private:
	void parseString(const TpString &str);

private:
	ItpBluetoothAddressData *data_;
};



#endif