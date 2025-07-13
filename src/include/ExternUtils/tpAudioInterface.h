#ifndef __TP_AUDIO_INTERFACE_H
#define __TP_AUDIO_INTERFACE_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpAudioInfData);

class tpAudioInterface
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
	tpAudioInterface(const tpString& name="default");
	~tpAudioInterface();
public:
	/// @brief 获取本机声卡设备列表，可直接使用返回的名字创建音频类
	/// @return 
	static tpList<tpString> getDevices();
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
	/// @brief 设置播放速度
	/// @param speed 播放速度，0.5～8.0
	/// @return 
	int setSpeed(float speed);
	/// @brief 获取播放速度
	/// @return 
	int getSpeed();
	/// @brief 设置当前文件的播放位置
	/// @param position 播放位置，单位为秒
	/// @return 
	int setPosition(tpUInt32 position);
	/// @brief 获取当前播放位置
	/// @return 播放位置，单位为秒
	int getPosition();
	/// @brief 获取文件总时长
	/// @return 文件时长，单位为秒
	tpUInt32 getDuration();
	/// @brief 添加要播放的文件
	/// @param file 文件
	/// @return 
	int addFile(const tpString& file);
	int addFile(const char *file);
	/// @brief 删除列表中的文件
	/// @param file 文件
	/// @return 
	int deleteFile(const tpString& file);
	int deleteFile(const char *file);
	/// @brief 设置播放文件
	/// @param file 文件
	/// @return 
	int setFile(const tpString& file);
	int setFile(const char *file);
	/// @brief 开始播放
	/// @return 
	int playStart();
	/// @brief 播放继续
	/// @return 
	int playContinue();
	/// @brief 播放暂停
	/// @return 
	int playPause();
	/// @brief 播放停止
	/// @return 
	int playStop();
	/// @brief 播放下一个
	/// @return 
	int playNext();
	/// @brief 播放上一个
	/// @return 
	int playLast();
	/// @brief 是否播放结束
	/// @return 
	tpBool isPlayEnd();
	/// @brief 根据文件的信息自动解码并设置硬件采样参数(暂时无使用需求)
	/// @param file 
	/// @return 
	//virtual int setSampleParame(tpString &file) = 0;
	//virtual int setSampleParame(const char *file) = 0;
	/// @brief 手动设置硬件采样参数
	/// @param rate 
	/// @param channel 
	/// @param bits 
	/// @return 
	int setSampleParame(SampleRate rate, SampleChannel channel, SampleBits bits);
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
	int getSampleParame(const char *file, SampleRate &rate, SampleChannel &channel, SampleBits &bits);

	/// @brief 获取音量允许的最大值
	/// @return 
	int getMaxVolume();
	/// @brief 获取音量允许的最小值
	/// @return 
	int getMinVolume();
	/// @brief 获取速度允许的最大值
	/// @return 
	float getMaxSpeed();
	/// @brief 获取速度允许的最小值
	/// @return 
	float getMinSpeed();

private:
	int threadAudio();
private:
	ItpAudioInfData *data_;
};


#endif
