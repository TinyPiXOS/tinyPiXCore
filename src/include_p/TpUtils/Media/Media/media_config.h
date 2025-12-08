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
#define DEBUG_AUDIO_PLAY
#define DEBUG_VIDEO_PLAY

#define VIDEO_RENDERING_SDL		//使用SDL渲染视频画面
#define VIDEO_RENDERING_PIX		//使用PIX渲染视频画面

#define VIDEO_FRAME_LAG_LOSS_TIME	10000	//视频播放最大允许滞后时间，当解码速度偏慢的情况下需要丢帧来达到同步
#define AUDIO_CODEC_CHANNEL_DEF		16		//音频解码默认使用的声道数(其他参数由解码器自行获取)

#define USER_CONF_VOLUME_MAX	100	//音量最大值
#define USER_CONF_VOLUME_MIN	0	//音量最小值
#define USER_CONF_SPEED_MAX		8.0	
#define USER_CONF_SPEED_MIN		0.5	//播放速度最小值

#define AUDIO_MAX_QUEUE_SIZE	500		//音频缓存区最大长度
#define VIDEO_MAX_QUEUE_SIZE	100		//视频缓存区最大长度

typedef enum VideoScalingType_{
		MEDIA_VIDEO_SCALING_STRETCH		= 0X01,	//拉伸显示，图像可能变形
		MEDIA_VIDEO_SCALING_FILL		= 0X02,	//填充显示，可能会裁剪
		MEDIA_VIDEO_SCALING_FIT			= 0X03, //保持原始比例并适应屏幕，可能添加黑边
		MEDIA_VIDEO_SCALING_ZOOM		= 0X04,	//放大画面以填充屏幕，可能会裁剪边缘。
		MEDIA_VIDEO_SCALING_CROP		= 0X05,	//裁剪画面以填充屏幕
		MEDIA_VIDEO_SCALING_LETTERBOX	= 0X06	//保持原始比例，上下左右添加黑边
}VideoScalingType;

typedef enum AudioPlayType_{
	AUDIO_PLAY_NONE		= 0X00,
	AUDIO_PLAY_LIST		= 0X01,
	AUDIO_PLAY_FILE		= 0X02,
	AUDIO_PLAY_STREAM	= 0X03
}AudioPlayType;



typedef enum MediaFileType_{
	MEDIA_FILE_TYPE_MP3		=0x01,
	MEDIA_FILE_TYPE_M4A		=0X02,
	MEDIA_FILE_TYPE_WAV		=0x03
}MediaFileType;

typedef enum MediaPlayState_{
	MEDIA_STATE_NONE 	= 0X00,		//初始状态
	MEDIA_STATE_STOP	= 0x01,		//停止状态
	MEDIA_STATE_START 	= 0X02,		//已经开始，用于防止线程还没启动，用户已经发送开始命令
	MEDIA_STATE_PLAYING = 0X03,		//播放中
	MEDIA_STATE_RECORD 	= 0X04,		//录制中
	MEDIA_STATE_PAUSEING= 0X05,		//暂停中
	MEDIA_STATE_JUMP	= 0X07,		//正在切换
	MEDIA_STATE_EXIT 	= 0x09,		//退出,此状态不允许设置，设置为MEDIA_STATE_STOP后会自动更新到此状态
	MEDIA_THREAD_WAITING		//等待中，通常是解码完成或线程启动准备完成可以解码
}MediaPlayState;

typedef enum MediaPlayCommand_{
	MEDIA_PLCMD_NONE 	= 0X00,		//初始
	MEDIA_PLCMD_STOP	= 0x01,		//停止
	MEDIA_PLCMD_START 	= 0X02,		//播放
	MEDIA_PLCMD_SUSPEND	= 0X04,		//挂起(等待)
	MEDIA_PLCMD_CONTINUE= 0X05,		//继续
	MEDIA_PLCMD_NEXT	= 0X07,		//切换到下一个
	MEDIA_PLCMD_LAST	= 0X08,		//切换到上一个
	MEDIA_PLCMD_EXIT 	= 0x09,		//退出
}MediaPlayCommand;

#ifdef __cplusplus
}
#endif

#endif
