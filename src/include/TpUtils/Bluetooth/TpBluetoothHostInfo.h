#ifndef _TP_BLUETOOTH_HOSTINFO_H_
#define _TP_BLUETOOTH_HOSTINFO_H_

#include <TpCore.h>
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"
#include "TpBluetoothUuid.h"

TP_DEF_VOID_TYPE_VAR(ITpBluetoothHostInfoData);

class TpBluetoothDiscovery;

/// @brief 可以对本地蓝牙设备进行管理
class TpBluetoothHostInfo
{

public:
    TpBluetoothHostInfo(int id, const char *address, const char *name);
    TpBluetoothHostInfo(const char *name);
    TpBluetoothHostInfo(const TpString &name);
    TpBluetoothHostInfo(const TpBluetoothHostInfo &other);

	// 拷贝赋值运算符声明
	TpBluetoothHostInfo& operator=(const TpBluetoothHostInfo &other);
	// 移动赋值运算符声明
	TpBluetoothHostInfo& operator=(TpBluetoothHostInfo&& other) noexcept;

	bool operator!=(const TpBluetoothHostInfo &other) const;
	bool operator==(const TpBluetoothHostInfo &other) const;


    ~TpBluetoothHostInfo();

public:
    /// @brief 获取蓝牙名字
    /// @return
    TpString name();
    int setName(const TpString &name);

    /// @brief 获取蓝牙地址
    /// @return
    TpBluetoothAddress address();

private:
    ITpBluetoothHostInfoData *data_;
};

#endif
