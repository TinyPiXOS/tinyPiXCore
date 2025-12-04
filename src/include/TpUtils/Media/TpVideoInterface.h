#ifndef __TP_VIDEO_INTERFACE_H
#define __TP_VIDEO_INTERFACE_H

#include <functional>
#include <TpCore.h>
#include "TpMediaInterface.h"

TP_DEF_VOID_TYPE_VAR(ItpVideoInfData);

class TpVideoInterface : public TpMediaInterface
{
public:
	/// @brief 视频画面填充方式
	enum TpVideoScalingType{
		TP_VIDEO_SCALING_STRETCH	= 0X01,	//拉伸显示，图像可能变形
		TP_VIDEO_SCALING_FILL		= 0X02,	//填充显示，可能会裁剪
		TP_VIDEO_SCALING_FIT		= 0X03, //保持原始比例并适应屏幕，可能添加黑边
		TP_VIDEO_SCALING_ZOOM		= 0X04,	//放大画面以填充屏幕，可能会裁剪边缘。
		TP_VIDEO_SCALING_CROP		= 0X05,	//裁剪画面以填充屏幕
		TP_VIDEO_SCALING_LETTERBOX	= 0X06	//保持原始比例，上下左右添加黑边
	};
	/// @brief 视频画面解码格式(RGB888,YUV等)
	enum TpVideoDecodeType{
		//【计划】暂时默认RGB24
		TP_VIDEO_DECODE_RGB24,
		TP_VIDEO_DECODE_BGR24,
		TP_VIDEO_DECODE_RGBA8888,
		TP_VIDEO_DECODE_IYUV,
		TP_VIDEO_DECODE_YUV2,
		TP_VIDEO_DECODE_UYVY
	};
public:
	/// @brief 用户播放的回调
	/// @param data 数据，可能有多行
	/// @param linesize 每一行的大小，最多8行，不可超过。
	/// @param format 返回的数据格式，需要根据此格式来决定怎么显示
	/// @param userdata 用户数据
	using UserCallback = std::function<int(uint8_t **, int *, uint32_t , void *)>;

public:
	TpVideoInterface(const TpString& video_name = "default" );
	~TpVideoInterface();
public:
	/// @brief 设置音频播放音量
	/// @param volume 音量(0~100)
	/// @return 
	int setVolume(tpUInt8 volume);
	/// @brief 获取音频播放音量
	/// @return 
	int getVolume();

	/// @brief 
	/// @param callback 
	/// @return 
	int setDisplayFunction(UserCallback cb, void *userdata=nullptr,TpVideoDecodeType format=TP_VIDEO_DECODE_RGB24);

	/// @brief 设置视频播放窗口的的位置，在不设置回调，使用内部SDL播放的时候会生效
	/// @param x 播放窗口x坐标
	/// @param y 播放窗口y坐标
	/// @return 
	int setWindowCoordinates(tpInt16 x,tpInt16 y);
	/// @brief 设置视频播放窗口的的大小，会根据设置的大小返回缓存区或自建SDL窗口播放
	/// @param width 
	/// @param height 
	/// @return 
	int setWindowSize(tpUInt16 width,tpUInt16 height);
	/// @brief 设置视频画面填充方式
	/// @param mode 填充方式
	/// @return 
	int setScalingMode(TpVideoScalingType mode);
private:
	/// @brief 设置视频解码格式【计划中，当前使用的是固定RGB888】
	/// @param format 解码格式
	/// @return 
	int setDecode(TpVideoDecodeType format);

private:
	    struct CallbackContext {
        UserCallback callback;  // 用户回调指针
        void* userdata;
    };
	ItpVideoInfData *vData_;
	static int staticBridge(uint8_t** data, int* linesize, uint32_t format, void* rawCtx);
};


#endif