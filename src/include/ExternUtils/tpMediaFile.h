#ifndef __TP_MEDIA_FILE_H
#define __TP_MEDIA_FILE_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpMediaFileData);

class tpMediaFile
{
public:
	enum tpMediaFileType
	{
		TP_MEDIA_FILE_NONE,
		TP_MEDIA_FILE_AUDIO,
		TP_MEDIA_FILE_VIDEO
	};

public:
	/// @brief
	/// @param file 媒体文件路径或URL
	tpMediaFile(const tpString &file);
	~tpMediaFile();

public:
	/// @brief 获取文件时长
	/// @return 秒
	tpInt64 getDuration();

	/// @brief 获取比特率(音频)
	/// @return kbps
	tpInt64 getBitRate();

	/// @brief 获取文件是音频还ushi视频
	/// @return
	tpMediaFile::tpMediaFileType getType();

	/// @brief 获取文件格式
	/// @return (MP3,WAV,MP4....)
	int getFormat();

	/// @brief 获取视频帧宽度
	/// @return
	tpUInt32 getWidth();

	/// @brief 获取视频帧高度
	/// @return
	tpUInt32 getHeight();

	/// @brief 获取视频分辨率
	/// @param width 宽
	/// @param height 高
	/// @return 
	int getResolution(tpUInt32 *width, tpUInt32 *height);

	/// @brief 获取视频码率
	/// @return
	tpUInt32 getFrameRate();

private:
	ItpMediaFileData *data_;
};

#endif