#ifndef __TP_AUDIO_INTERFACE_H
#define __TP_AUDIO_INTERFACE_H

#include <TpCore.h>
#include "TpAudioFormat.h"
#include "TpMediaInterface.h"

TP_DEF_VOID_TYPE_VAR(ITpAudioInfData);

class TpAudioInterface 
{
public:
	TpAudioInterface(const TpString& name="default");
	~TpAudioInterface();
public:
    /// @brief 获取本机声卡设备列表，可直接使用返回的名字创建音频类
	/// @return 
	static TpList<TpString> getDevices();

	/// @brief 设置音频播放音量
	/// @param volume 音量(0~100)
	/// @return 
	int setVolume(tpUInt8 volume);
	/// @brief 获取音频播放音量
	/// @return 
	int getVolume();
    /// @brief 根据文件的信息自动解码并设置硬件采样参数(暂时无使用需求)
	/// @param file 
	/// @return 
	//virtual int setSampleParame(TpString &file) = 0;
	//virtual int setSampleParame(const char *file) = 0;
	/// @brief 手动设置硬件采样参数，(仅在播放PCM流会生效，播放普通文件数据会根据实际情况设置)
	/// @param rate 
	/// @param channel 
	/// @param bits 
	/// @return 
	int setSampleParame(TpAudioFormat::SampleRate rate, TpAudioFormat::SampleChannel channel, TpAudioFormat::SampleBits bits);
	/// @brief 设置非阻塞(用于播放实时性高的音频流数据，播放文件时候设置不会生效)
	/// @param nonblock 设置为true为非阻塞模式 
	/// @return 
	int setNonblock(tpBool nonblock);
	/// @brief 播放音频流
	/// @param data 音频流数据(当前只允许小端数据)
	/// @param frames 帧数(一帧包含一个采样点，每个采样点需要采样的字节数=通道数*位数/8)
	/// @param offset 相对于第一次播放的偏移，如果不需要设置为-1即可，但是内部无法自行记录
	/// @param delay 阻塞时长，不需要阻塞直接设置为0即可
	/// @return 
	int playStream(tpUInt8 *data,tpUInt32 frames,tpInt64 offset,tpInt32 delay);
	/// @brief 从文件获取硬件采样参数
	/// @param rate 
	/// @param channel 
	/// @param bits 
	/// @return 
	int getSampleParame(const char *file, TpAudioFormat::SampleRate &rate, TpAudioFormat::SampleChannel &channel, TpAudioFormat::SampleBits &bits);

	/// @brief 获取音量允许的最大值
	/// @return 
	int getMaxVolume();
	/// @brief 获取音量允许的最小值
	/// @return 
	int getMinVolume();
private:
	friend class TpMediaInterface;
	void *getAudioInfo();
private:
	ITpAudioInfData *aData_;
};


#endif
