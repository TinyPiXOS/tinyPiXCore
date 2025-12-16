#ifndef __TP_MEDIA_INTERFACE_H
#define __TP_MEDIA_INTERFACE_H

#include <functional>
#include <TpCore.h>


TP_DEF_VOID_TYPE_VAR(ITpMediaInfData);

class TpAudioOutput;
class TpVideoOutput;
class TpMediaPlayer
{
public:
	TpMediaPlayer();
	virtual ~TpMediaPlayer();
public:
	/// @brief 打开视频播放设备
	/// @param name 
	/// @return 
	int openDevice();
	/// @brief 关闭视频播放设备
	/// @return 
	int closeDevice();
	/// @brief 设备是否打开
	/// @return 打开返回TP_TRUE
	tpBool isOpen();
	/// @brief 设置音频输出
	/// @param audio 音频接口
	/// @return 
	int setAudioOutput(TpAudioOutput *audio);
	/// @brief 获取音频输出
	/// @return 
	TpAudioOutput *audioOutput();
	/// @brief 设置视频输出
	/// @param video 视频接口
	/// @return 
	int setVideoOutput(TpVideoOutput *video);
	/// @brief 获取视频输出
	/// @return 
	TpVideoOutput *videoOutput();
	/// @brief 设置播放速度
	/// @param speed 播放速度，0.5～8.0
	/// @return 
	int setSpeed(float speed);
	/// @brief 获取播放速度
	/// @return 播放速度
	float getSpeed();
	/// @brief 设置当前文件播放位置
	/// @param position 播放位置
	/// @return 
	int setPosition(tpUInt32 position);
	/// @brief 获取当前文件播放位置
	/// @return 
	virtual int getPosition();
	/// @brief 获取文件总时长
	/// @return 文件时长，秒
	tpUInt32 getDuration();
	/// @brief 向播放列表添加文件
	/// @param file 文件
	/// @return 
	int addFile(const TpString& file);
	int addFile(const char *file);
	/// @brief 从播放列表中删除文件
	/// @param file 文件
	/// @return 
	int deleteFile(const TpString& file);
	int deleteFile(const char *file);
	/// @brief 设置播放的文件
	/// @param file 文件
	/// @return 
	int setFile(const TpString& file);
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
	/// @brief 获取速度允许的最大值
	/// @return 
	float getMaxSpeed();
	/// @brief 获取速度允许的最小值
	/// @return 
	float getMinSpeed();
	
private:
	int threadMedia();

protected:
	ITpMediaInfData *data_;
};


#endif