#ifndef _TP_BLUETOOTH_LOCAL_H_
#define _TP_BLUETOOTH_LOCAL_H_

#include <TpCore.h>
#include "TpBluetoothDevice.h"
#include "TpBluetoothHostInfo.h"
#include "TpBluetoothAddress.h"
#include "TpBluetoothUuid.h"

TP_DEF_VOID_TYPE_VAR(ITpBluetoothLocalData);

class TpBluetoothDiscovery;

/// @brief 可以对本地蓝牙设备进行管理
class TpBluetoothLocal
{
public:
    enum TpLocalPair
    {
		TP_LOCAL_UNKNOWN,			// 未知状态,通常是未打开蓝牙扫描无法获取状态
        TP_LOCAL_PAIRED,			// 已配对但未授权(未信任)
        TP_LOCAL_UNPAIRED,			// 未配对
        TP_LOCAL_AUTHORIZED_PAIRED 	// 已配对且已授权(配对+信任)
    };

public:
    TpBluetoothLocal(const TpBluetoothHostInfo local);
    TpBluetoothLocal(const char *name);
    TpBluetoothLocal(const TpString &name);
    ~TpBluetoothLocal();

public:
    /// @brief 获取所有适配器列表
    /// @return
    static TpList<tpShared<TpBluetoothHostInfo>> allDevice();

public:
    /// @brief 获取蓝牙名字
    /// @return
    TpString name() const;

    /// @brief 获取蓝牙地址
    /// @return
    TpBluetoothAddress address() const;

    /// @brief 获取以配对的设备列表
    /// @return
    TpList<TpBluetoothAddress> pairedDevices();

    /// @brief 获取已连接的设备列表
    /// @return
    TpList<TpBluetoothAddress> connectedDevices();

    /// @brief 和远程的蓝牙设备建立物理连接
    /// @param addr
    /// @return
    int connectToDevice(const TpBluetoothAddress &addr);

    /// @brief 和远程的蓝牙设备断开物理连接
    /// @param addr
    /// @return
    int disconnectToDevice(const TpBluetoothAddress &addr);

    /// @brief 设置远程蓝牙设备的信任状态
    /// @param address 远程设备地址
    /// @param trusted 信任/不信任
    /// @return
    int setTrusted(TpBluetoothAddress &address, tpBool trusted);

    /// @brief 设置蓝牙配对请求
    /// @param address 远程蓝牙的地址
    /// @param pair 配对状态(手动授权配对/取消配对/自动授权配对)
    /// @return
    int requestPairing(TpBluetoothAddress &address, TpBluetoothLocal::TpLocalPair pair);

    /// @brief 获取配对状态
    /// @param address 远程蓝牙的地址
    /// @return
    TpBluetoothLocal::TpLocalPair getPairStatus(TpBluetoothAddress &address);

    int removeDevice(TpBluetoothAddress &address);

    /// @brief 设置蓝牙可见性
    /// @param discoverable 可见/不可见
    /// @return
    int setDiscoverable(tpBool discoverable);

    /// @brief 设置蓝牙可见的超时时间
    /// @param timeout 超时时间
    /// @return
    int setDiscoverableTimeout(tpUInt32 timeout);

    /// @brief 获取本机蓝牙上所有注册的UUID
    /// @return
    TpList<TpBluetoothUuid> uuidList();

    /// @brief 是否已经有某个服务(若想获取该uuid的具体注册信息需要使用服务扫描)
    /// @param uuid uuid值
    /// @return
    tpBool isHaveUuid(TpBluetoothUuid &uuid);

    /// @brief 本机蓝牙是否打开
    /// @return
    tpBool isPowerOn();

    /// @brief 打开蓝牙
    /// @return
    int powerOn();

    /// @brief 关闭蓝牙
    /// @return
    int powerOff();

    int hostMode();
    int setHostMode();

private:
    friend class TpBluetoothDiscovery;
    void *adapter();

private:
    ITpBluetoothLocalData *data_;
};

#endif
