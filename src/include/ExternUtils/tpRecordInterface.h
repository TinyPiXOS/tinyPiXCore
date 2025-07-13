#ifndef __TP_RECORD_INTERFACE_H
#define __TP_RECORD_INTERFACE_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpRecordInfData);

class tpRecordInterface
{
	enum AudioType{
		TP_RECORD_TYPE_WAV	= 0x01,		//wav
		TP_RECORD_TYPE_M4A	= 0x02		//m4a
	};
	enum AudioBitRate{
		TP_AUDIO_BITRATE_32K	= 32000U,
		TP_AUDIO_BITRATE_40K	= 40000U,
		TP_AUDIO_BITRATE_48K	= 48000U,
		TP_AUDIO_BITRATE_56K	= 56000U,
		TP_AUDIO_BITRATE_64K	= 64000U,
		TP_AUDIO_BITRATE_80K	= 80000U,
		TP_AUDIO_BITRATE_96K	= 96000U,
		TP_AUDIO_BITRATE_112K	= 112000U,
		TP_AUDIO_BITRATE_128K	= 128000U,		//推荐码率
		TP_AUDIO_BITRATE_160K	= 162000U,
		TP_AUDIO_BITRATE_192K	= 192000U,
		TP_AUDIO_BITRATE_224K	= 224000U,
		TP_AUDIO_BITRATE_256K	= 256000U,
		TP_AUDIO_BITRATE_320K	= 320000U,
	};
public:
	tpRecordInterface(const tpString& device="default");
	~tpRecordInterface();
public:
	/// @brief 打开录音设备
	/// @return 
	int openDevice();
	/// @brief 关闭录音设备
	/// @return 
	int closeDevice();
	/// @brief 判断设备是否打开
	/// @return 
	tpBool isOpen();
	/// @brief 录音开始
	/// @param file 录音文件名及路径
	/// @param type 要录制保存的文件类型
	/// @param rate 码率/比特率
	/// @return 
	int recordStart(tpString& file,AudioType type=TP_RECORD_TYPE_WAV,AudioBitRate rate=TP_AUDIO_BITRATE_128K);
	int recordStart(const char *file,AudioType type=TP_RECORD_TYPE_WAV,AudioBitRate rate=TP_AUDIO_BITRATE_128K);
	int getStream(tpUInt8 *data);
	/// @brief 继续录音
	/// @return 
	int recordContinue();
	/// @brief 暂停录音
	/// @return 
	int recordPause();
	/// @brief 录音结束
	/// @return 
	int recordStop();
private:
	int threadRecord();
private:
	ItpRecordInfData *data_;
};


#endif
