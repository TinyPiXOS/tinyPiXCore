#ifndef _TP_VIDEO_DEVICE_H_
#define _TP_VIDEO_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VIDEO_RENDERING_SDL		//使用SDL渲染视频画面
#define VIDEO_RENDERING_PIX		//使用PIX渲染视频画面

#define VIDEO_FRAME_LAG_LOSS_TIME	1000		//视频播放最大允许滞后时间，当解码速度偏慢的情况下需要丢帧来达到同步

typedef enum VideoScalingType_{
		MEDIA_VIDEO_SCALING_STRETCH		= 0X01,	//拉伸显示，图像可能变形
		MEDIA_VIDEO_SCALING_FILL		= 0X02,	//填充显示，可能会裁剪
		MEDIA_VIDEO_SCALING_FIT			= 0X03, //保持原始比例并适应屏幕，可能添加黑边
		MEDIA_VIDEO_SCALING_ZOOM		= 0X04,	//放大画面以填充屏幕，可能会裁剪边缘。
		MEDIA_VIDEO_SCALING_CROP		= 0X05,	//裁剪画面以填充屏幕
		MEDIA_VIDEO_SCALING_LETTERBOX	= 0X06	//保持原始比例，上下左右添加黑边
}VideoScalingType;

typedef struct MediaConf PIAudioConf __attribute__((used));

int Video_Play_Main(struct MediaParams *user,const char *audio_card) __attribute__((used));
int Video_Get_Position(struct MediaParams *conf,PIAudioConf *pcm_play) __attribute__((used));

int Video_Set_Width_Height(struct MediaParams *conf,uint16_t width,uint16_t height);
int Video_Get_Width_Height(struct MediaParams *conf,uint16_t *width,uint16_t *height);
int Video_Set_Coordinates(struct MediaParams *conf,int16_t x,int16_t y);
int Video_Set_Fill_Mode(struct MediaParams *conf,VideoScalingType mode);

#ifdef __cplusplus
}
#endif

#endif
