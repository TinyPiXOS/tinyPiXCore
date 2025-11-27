#ifndef __TP_AUDIO_INTERFACE__H
#define __TP_AUDIO_INTERFACE__H

#include <TpCore.h>
#include "TpMediaInterface.h"

TP_DEF_VOID_TYPE_VAR(ItpAudioInfData);

class TpAudioInterface_ : public TpMediaInterface
{
	enum SampleRate{
		TP_AUDIO_RATE_11025 = 11025,
		TP_AUDIO_RATE_22050 = 22050,
		TP_AUDIO_RATE_44100 = 44100,
		TP_AUDIO_RATE_47250 = 47250,

		TP_AUDIO_RATE_8000 = 8000,
		TP_AUDIO_RATE_24000 = 24000,
		TP_AUDIO_RATE_32000 = 32000,
		TP_AUDIO_RATE_48000 = 48000,
		TP_AUDIO_RATE_96000 = 96000,
		TP_AUDIO_RATE_192000 = 192000,
	};

	enum SampleChannel{
		TP_AUDIO_CHANNEL_1	= 8,
		TP_AUDIO_CHANNEL_2  = 16,
		TP_AUDIO_CHANNEL_2_1 = 24,
		TP_AUDIO_CHANNEL_5_1 = 48,
		TP_AUDIO_CHANNEL_7_1 = 64,
	};

	enum SampleBits{
		TP_AUDIO_BITS_8	= 8,
		TP_AUDIO_BITS_16 = 16,
		TP_AUDIO_BITS_24 = 24,
		TP_AUDIO_BITS_32 = 32,
	};

public:
	TpAudioInterface_(const TpString& name="default");
	~TpAudioInterface_();
public:
    /// @brief 获取本机声卡设备列表，可直接使用返回的名字创建音频类
	/// @return 
	static TpList<TpString> getDevices();
	/// @brief 打开音频播放设备
	/// @return 
	int openDevice();
	/// @brief 关闭音频播放设备
	/// @return 
	int closeDevice();
	/// @brief 音频播放设备是否打开
	/// @return 
	tpBool isOpen();
	/// @brief 设置音频播放音量
	/// @param volume 音量(0~100)
	/// @return 
	int setVolume(tpUInt8 volume);
	/// @brief 获取音频播放音量
	/// @return 
	int getVolume();
};


#endif
