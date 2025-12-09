#ifndef _VIDEO_PLAY_H_
#define _VIDEO_PLAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include "Media/media_config.h"

#define SCALE_HANDLE_USE_SDL // 使用SDL处理缩放
// #define SCALE_HANDLE_USE_FFMPEG		//使用FFMPEG处理缩放

typedef int (*CodecSdlPlayAudio)(uint8_t *buf, uint32_t frames, void *param);
typedef int (*CodecSdlPlayVideo)(uint8_t *buf, uint32_t frames, void *param);

typedef int(*CallbackVideoDisplay)(uint8_t **data, int *linesize, uint32_t format ,void *user_data);

struct MediaAudioHandle;
struct VideoStreamParams;
struct MediaVideoInfo;
struct MediaUserParams;
struct MediaStreamParams;

// 视频播放的硬件相关参数
/*struct VideoHardParam
{
#ifdef MEDIA_SDL_ENABLE
    SDL_Window *window;     // 窗口
    SDL_Renderer *renderer; // 渲染器
    SDL_Texture *texture;   // 纹理
#endif
    struct MediaRect *rect_src;
    struct MediaRect *rect_dst;
    uint32_t format; // 格式，RGB，YUV等，当启用本地显示的时候就是带鸟sdl窗口的格式，如果没有启用就代表用户设置的格式(当前用户默认使用RGB888)
    char *audio_card;
    void *audio_data; // 音频数据
    struct MediaAudioHandle *pcm_play;
    struct SwrContext *swr_ctr; // 音频重采样和转换句柄
    bool is_sdl;                // 是否启用本地显示(如果不启用需要上层绘制图像)
};*/

struct MediaVideoInfo{
	pthread_rwlock_t rw_mut;	//数据交互读写锁
	//视频显示回调函数以及解码格式
	uint32_t format_video;		//解码格式，仅在用户自己处理时候才会生效

	struct{
		CallbackVideoDisplay callback_video;
		void *userdata;
		CallbackVideoDisplay (*get_callback_video)(struct MediaVideoInfo *conf);
		void (*set_callback_video)(struct MediaVideoInfo *conf, CallbackVideoDisplay callback,void *userdata);
	};
	
	//以下参数暂时无用
	struct VideoStreamParams *video;		//
};

// 视频播放的句柄（包含硬件信息，流解码前后详细信息）
struct MediaVideoHandle
{
#ifdef MEDIA_SDL_ENABLE
    SDL_Window *window;     // 窗口
    SDL_Renderer *renderer; // 渲染器
    SDL_Texture *texture;   // 纹理
#endif
    bool is_sdl; // 是否启用本地显示(如果不启用需要上层绘制图像)
};

// 视频播放的硬件相关参数
/*struct VideoStreamParams{
    struct MediaRect *rect_src;
    struct MediaRect *rect_dst;
    uint32_t format;			//格式，RGB，YUV等，当启用本地显示的时候就是带鸟sdl窗口的格式，如果没有启用就代表用户设置的格式(当前用户默认使用RGB888)
    VideoScalingType fill;
};*/

struct MediaRect{
	int16_t x;		//显示位置x
	int16_t y;		//显示位置y
	uint16_t w;		//显示宽度
	uint16_t h;		//显示高度
};
struct VideoStreamParams{
	struct MediaRect rect;
	uint16_t light;	//显示亮度
	VideoScalingType fill;
};



#ifdef MEDIA_SDL_ENABLE
SDL_Texture *sdl_creat_texture_near(SDL_Renderer *renderer, uint32_t *format, int w, int h); // 创建纹理(
#endif

int get_display_params_user_codec(struct MediaUserParams *user, AVCodecContext *codec_ctx, struct VideoStreamParams *video_params);
int count_rect_size_from_user(struct VideoStreamParams *user_params, AVCodecContext *codec_ctx, struct MediaRect *rect_s, struct MediaRect *rect_d);

int Video_Get_Position(struct MediaUserParams *conf);
// 获取显示参数
int Video_Get_All_Params(struct MediaUserParams *conf, struct VideoStreamParams *video_params);


int Video_Set_Decode_Format(struct MediaVideoInfo *conf_v, uint32_t format);
int Video_Set_Width_Height(struct MediaVideoInfo *conf,uint16_t width,uint16_t height);
int Video_Get_Width_Height(struct MediaVideoInfo *conf,uint16_t *width,uint16_t *height);
int Video_Set_Coordinates(struct MediaVideoInfo *conf,int16_t x,int16_t y);
int Video_Set_Fill_Mode(struct MediaVideoInfo *conf,VideoScalingType mode);

CallbackVideoDisplay Media_Get_Video_Callback(struct MediaVideoInfo *conf);
void Media_Set_Video_Callback(struct MediaVideoInfo *conf,CallbackVideoDisplay cb, void *userdata);

int media_stream_video_init_handle(struct MediaStreamParams *stream,struct MediaUserParams *user);
int media_stream_video_deinit_handle(struct MediaStreamParams *stream);

#ifdef __cplusplus
}
#endif

#endif
