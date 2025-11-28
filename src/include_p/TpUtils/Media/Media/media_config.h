#ifndef _MEDIA_CONFIG_H_
#define _MEDIA_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MEDIA_SDL_ENABLE
#include <SDL2/SDL.h>
#endif


//#define MEDIA_SDL_ENABLE	1			//是否使用SDL
#define DEBUG_MEDIA_CODEC				//媒体编解码调试


#define VIDEO_RENDERING_SDL		//使用SDL渲染视频画面
#define VIDEO_RENDERING_PIX		//使用PIX渲染视频画面

#define VIDEO_FRAME_LAG_LOSS_TIME	10000		//视频播放最大允许滞后时间，当解码速度偏慢的情况下需要丢帧来达到同步


#define USER_CONF_VOLUME_MAX	100	//音量最大值
#define USER_CONF_VOLUME_MIN	0	//音量最小值
#define USER_CONF_SPEED_MAX		8.0	
#define USER_CONF_SPEED_MIN		0.5	//播放速度最小值

typedef enum VideoScalingType_{
		MEDIA_VIDEO_SCALING_STRETCH		= 0X01,	//拉伸显示，图像可能变形
		MEDIA_VIDEO_SCALING_FILL		= 0X02,	//填充显示，可能会裁剪
		MEDIA_VIDEO_SCALING_FIT			= 0X03, //保持原始比例并适应屏幕，可能添加黑边
		MEDIA_VIDEO_SCALING_ZOOM		= 0X04,	//放大画面以填充屏幕，可能会裁剪边缘。
		MEDIA_VIDEO_SCALING_CROP		= 0X05,	//裁剪画面以填充屏幕
		MEDIA_VIDEO_SCALING_LETTERBOX	= 0X06	//保持原始比例，上下左右添加黑边
}VideoScalingType;

//音频文件类型
typedef enum{
	AUDIO_FILE_TYPE_NONE=0X00,
	AUDIO_FILE_TYPE_WAV

}AudioFileType;

typedef enum MediaFileType_{
	MEDIA_FILE_TYPE_MP3		=0x01,
	MEDIA_FILE_TYPE_M4A		=0X02,
	MEDIA_FILE_TYPE_WAV		=0x03
}MediaFileType;

typedef enum AudioPlayState_{
	AUDIO_STATE_NONE 	= 0X00,		//初始状态
	AUDIO_STATE_STOP	= 0x01,		//停止状态
	AUDIO_STATE_START 	= 0X02,		//已经开始，用于防止线程还没启动，用户已经发送开始命令
	AUDIO_STATE_PLAYING = 0X03,		//播放中
	AUDIO_STATE_RECORD 	= 0X04,		//录制中
	AUDIO_STATE_PAUSEING= 0X05,		//暂停中
	AUDIO_STATE_JUMP	= 0X07,		//正在切换
	AUDIO_STATE_EXIT 	= 0x09,		//退出,此状态不允许设置，设置为AUDIO_STATE_STOP后会自动更新到此状态
	MEDIA_THREAD_WAITING		//等待中，通常是解码完成或线程启动准备完成可以解码
}AudioPlayState;


#ifdef __cplusplus
}
#endif

#endif
