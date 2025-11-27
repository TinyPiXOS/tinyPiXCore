#ifndef __TP_MEDIA_INTERFACE_H
#define __TP_MEDIA_INTERFACE_H

#include <functional>
#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ItpMediaInfData);

class TpMediaInterface
{
public:
	TpMediaInterface();
	virtual ~TpMediaInterface();
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
	/// @brief 设置播放音量
	/// @param volume 播放音量，0～100
	/// @return 
	int setVolume(tpUInt16 volume);
	/// @brief 获取当前播放音量
	/// @return 播放音量，0～100
	int getVolume();
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
	int getPosition();
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
private:
	int threadMedia();

protected:
	ItpMediaInfData *data_;
};


#endif