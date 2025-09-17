#ifndef _TP_BLUETOOTH_AUDIO_DEVICE_H_
#define _TP_BLUETOOTH_AUDIO_DEVICE_H_

#include "TpUtils.h"
#include "TpBluetoothDevice.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothAudioDeviceData);

/// @brief 本机蓝牙连接蓝牙音频设备并播放音频
class TpBluetoothAudioDevice{
public:
	enum TpAudioProfileType{
		TP_PROFILE_AUDIO_SOURCE,	//A2DP
		TP_PROFILE_AUDIO_SINK,		
		TP_PROFILE_AUDIO_HEADSET,	//HSP
	};
public:
	TpBluetoothAudioDevice(const TpString &local,const TpBluetoothDevice &other);
	TpBluetoothAudioDevice(const TpString &local,const TpBluetoothAddress &address);
	~TpBluetoothAudioDevice();

public:
	/// @brief 连接到蓝牙多媒体设备
	/// @return 
	int connectToDevice();

	/// @brief 和蓝牙多媒体设备断开连接
	/// @return 
	int disconnectDevice();

	/// @brief 获取连接的设备名
	/// @return 
	TpString getDevice();

private:
	void eventThread();
private:
	ItpBluetoothAudioDeviceData *data_;
};



#endif