#ifndef _TP_BLUETOOTH_AUDIO_MANAGER_H_
#define _TP_BLUETOOTH_AUDIO_MANAGER_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothAudioManagerData);

/// @brief 蓝牙音频服务的管理
class tpBluetoothAudioManager{
public:

public:
	tpBluetoothAudioManager();
	~tpBluetoothAudioManager();
public:
	/// @brief 启动蓝牙音频服务
	/// @return 
	int startService();

	/// @brief 停止蓝牙音频服务
	/// @return 
	int stopService();

	/// @brief 重启蓝牙音频服务
	/// @return 
	int restartService();
	
	/// @brief 蓝牙音频服务是否已经在运行
	/// @return 
	tpBool isRuning();
private:
	ItpBluetoothAudioManagerData *data_;
};




#endif