#ifndef _TP_BLUETOOTH_AUDIO_DEVICE_H_
#define _TP_BLUETOOTH_AUDIO_DEVICE_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothAudioDeviceData);

/// @brief 本机蓝牙连接蓝牙音频设备并播放音频
class tpBluetoothAudioDevice{
public:
	enum tpAudioProfileType{
		TP_PROFILE_AUDIO_SOURCE,	//A2DP
		TP_PROFILE_AUDIO_SINK,		
		TP_PROFILE_AUDIO_HEADSET,	//HSP
	};
public:
	tpBluetoothAudioDevice(const tpString &local,const tpBluetoothDevice &other);
	tpBluetoothAudioDevice(const tpString &local,const tpBluetoothAddress &address);
	~tpBluetoothAudioDevice();

public:
	/// @brief 连接到蓝牙多媒体设备
	/// @return 
	int connectToDevice();

	/// @brief 和蓝牙多媒体设备断开连接
	/// @return 
	int disconnectDevice();

	/// @brief 获取连接的设备名
	/// @return 
	tpString getDevice();

private:
	void eventThread();
private:
	ItpBluetoothAudioDeviceData *data_;
};



#endif