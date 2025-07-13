#ifndef __TP_SOUND_H
#define __TP_SOUND_H

#include "tpUtils.h"
#include "tpAudioInterface.h"

TP_DEF_VOID_TYPE_VAR(ItpSoundData);

class tpSound
{	
public:
	tpSound(const tpString& name);
	tpSound(tpAudioInterface *audio);
	~tpSound();
public:
	/// @brief 获取本机声卡设备列表，可直接使用返回的名字创建音频类（内部直接使用tpAudioInterface）
	/// @return 
	static tpList<tpString> getDevices();

	/// @brief 获取使用中的设备
	/// @return 
	static tpString getUsedDevice();

	/// @brief 设置使用的声卡设备
	/// @param name 设备名，建议使用getDevices获取
	/// @return 
	static tpBool setUsedDevice(const tpString& name);

	/// @brief 设置绑定的应用，如果社设置应用音量需要绑定应用或者在构造的时候绑定
	/// @param audio 
	/// @return 
	int setAudio(tpAudioInterface *audio);

	/// @brief 获取已经绑定的应用
	/// @return 
	tpAudioInterface *getAudio();

	/// @brief 设置应用音量
	/// @param volume 
	/// @return 
	int setVolume(tpUInt8 volume);

	/// @brief 获取应用音量
	/// @return 
	int getVolume();

	/// @brief 设置系统音量
	/// @param volume 音量，0～100
	/// @return 
	int setSystemVolume(tpUInt8 volume);

	/// @brief 获取系统音量
	/// @return 
	int getSystemVolume();
	
private:
	ItpSoundData *data_;
};




#endif