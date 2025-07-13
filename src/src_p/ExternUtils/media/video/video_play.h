#ifndef _VIDEO_PLAY_H_
#define _VIDEO_PLAY_H_

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <libswresample/swresample.h>
#include "audio_codec.h"

#define SCALE_HANDLE_USE_SDL		//使用SDL处理缩放
//#define SCALE_HANDLE_USE_FFMPEG		//使用FFMPEG处理缩放

enum VideoWindowPosition{
	VIDEO_WINDOWPOS_UNDEFINED=SDL_WINDOWPOS_UNDEFINED,		//系统确定
	VIDEO_WINDOWPOS_CENTERED=SDL_WINDOWPOS_CENTERED			//中心位置
};


//音频播放回调函数的参数
struct AudioData{
	uint8_t *buffer;
	int buffer_size;
	int buffer_pos;
	double pts;  // 当前音频的时间戳
};



//视频播放的硬件相关参数
struct VideoHardParam{
	SDL_Window *window;			//窗口
	SDL_Renderer *renderer;		//渲染器
	SDL_Texture *texture;		//纹理
	struct MediaRect *rect_src;
	struct MediaRect *rect_dst;
	uint32_t format;			//格式，RGB，YUV等，当启用本地显示的时候就是带鸟sdl窗口的格式，如果没有启用就代表用户设置的格式(当前用户默认使用RGB888)
	char *audio_card;
	void *audio_data;			//音频数据
	PIAudioConf *pcm_play;
	struct SwrContext *swr_ctr;	//音频重采样和转换句柄
	bool is_sdl;				//是否启用本地显示(如果不启用需要上层绘制图像)
};

int video_hard_param_init(struct VideoHardParam *video,const char *audio_card);
int video_params_get_all(struct MediaParams *user,struct MediaVideoParams *video_params);

SDL_Texture *sdl_creat_texture_near(SDL_Renderer *renderer,uint32_t *format,int w,int h);		//创建纹理(
int count_rect_size_from_user(struct VideoHardParam *display,struct MediaVideoParams *video_params,AVCodecContext *codec_ctx);

int Video_Play_File(struct VideoHardParam *display, struct MediaCodecParam *codec_v,struct MediaCodecParam *codec_a);
int Video_Play_Main(struct MediaParams *user,const char *audio_card);
int Video_Get_Position(struct MediaParams *conf,PIAudioConf *pcm_play);
//获取显示参数
int Video_Get_All_Params(struct MediaParams *conf,struct MediaVideoParams *video_params);

#endif
