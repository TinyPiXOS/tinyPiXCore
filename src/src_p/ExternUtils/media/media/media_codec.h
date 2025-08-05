#ifndef _MEDIA_CODEC_H_
#define _MEDIA_CODEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <alsa/asoundlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include "media_timer.h"
#include "media.h"
#include "utils/variable_array.h"
#include "audio_play.h"
#include "media_play_temp.h"
#include "media_config.h"



typedef enum AVMediaType  MediaType;
typedef VariableArray	MediaStreamArray;

//待解码队列状态
typedef enum {
	MEDIA_PACK_QUEUE_FULL,		//队列已满或达到最大允许的队列长度
	MEDIA_PACK_QUEUE_EMPTY,		//队列为空
	MEDIA_PACK_QUEUE_OTHER,		//其他状态
}MediaPacketQueueState;


//视频播放的硬件相关参数
/*struct VideoStreamParams{
	struct MediaRect *rect_src;
	struct MediaRect *rect_dst;
	uint32_t format;			//格式，RGB，YUV等，当启用本地显示的时候就是带鸟sdl窗口的格式，如果没有启用就代表用户设置的格式(当前用户默认使用RGB888)
	VideoScalingType fill;
};*/

//视频播放的句柄（包含硬件信息，流解码前后详细信息）
struct MediaVideoHandle{
#ifdef MEDIA_SDL_ENABLE
	SDL_Window *window;			//窗口
	SDL_Renderer *renderer;		//渲染器
	SDL_Texture *texture;		//纹理
#endif
	bool is_sdl;				//是否启用本地显示(如果不启用需要上层绘制图像)
};

//原PIAudioConf
//音频的硬件参数和句柄
/*struct MediaAudioHandle{
	char *device;					//声卡设备名
	snd_pcm_t *handle;             	//设备打开后的句柄
	snd_pcm_hw_params_t *hwparams;  //设备配置信息的结构体(结构体内部隐藏)，配置信息保存在该结构体
	uint8_t file_type;          	//音频文件类型(AudioFileType类型)
	uint8_t thread_num;				//线程编号，暂时未使用
	struct AudioStreamParams *adparams;	//解码后可以用于播放的音频流的参数
	struct PcmHardParams *ahparams;	//设置后的一些关键硬件参数(其实是从snd_pcm_hw_params_t里面拿出来的几个常用的参数)
	struct SwrContext *swr_ctr;	//音频重采样和转换句柄
};*/


struct MediaStreamCodecParams{
	AVFormatContext *format_ctx;	//输入输出相关信息，贯穿ffmpeg
	AVCodecContext *codec_ctx;		//编码器上下文，源文件中的音频参数,位宽，声道等，视频的帧率分辨率等
	int stream_index;				//流索引号
	MediaType type;					//流类型
	bool enable;					//是否启用此流的处理
};


//媒体流通用参数
struct MediaStreamParams{
	AVFormatContext *format_ctx;	//输入输出相关信息，贯穿ffmpeg
//	AVCodecParameters *codec_params;
	AVCodecContext *codec_ctx;		//编码器上下文，源文件中的音频参数,位宽，声道等，视频的帧率分辨率等
	int stream_index;				//流索引号
	MediaType type;					//流类型
	bool enable;					//是否启用此流的处理

	union		//每个流独特的硬件相关参数
	{
		struct{
			struct MediaAudioHandle *handle;	//音频硬件的采样参数
			struct SwrContext *swr_ctx;
		}audio;
		struct{
			struct MediaVideoHandle *handle;
			//struct VideoStreamParams *params_s;	
			//struct VideoStreamParams *params_d;	
			uint32_t format;			//FFMPEG的格式，RGB，YUV等，(当前用户默认使用RGB888)
		}video;
		
	};
	struct {
		union{
			int (*callback_play_audio) (uint8_t *buf,uint32_t frames,int offset,void *param);		//音频播放的回调函数

		};
		void *callback_param;        //回调函数的参数（不需要则置NULL）
	};
	
	void *codec_thread;		//struct MediaThread *
};



struct MediaPlayerHandle{
	char *url;
	AVFormatContext *format_ctx;	//输入输出相关信息，贯穿ffmpeg
	int sysnc_clock_index;			//主同步时钟的流索引号
	struct TimerHandle *clock;		//同步时钟
	MediaStreamArray *stream_array;	//所有的流


	int (*player_start)(MediaStreamArray *stream_array);
	int (*player_wait)(MediaStreamArray *stream_array);
	int (*player_pause)(MediaStreamArray *stream_array);
	int (*player_resume)(MediaStreamArray *stream_array);
	int (*set_state)(MediaStreamArray *stream_array, AudioPlayState state);

	int (*flush_list)(MediaStreamArray *stream_array);			//删除全部流队列中所有元素
	int (*packet_exit)(MediaStreamArray *stream_array);			//
	MediaPacketQueueState (*list_state)(MediaStreamArray *stream_array);
};



MediaFormatContext *Media_Get_File_Info(const char *filename,MediaStreamArray *media_array);
int Media_Free_File(MediaStreamArray *media_array);
int Mediao_File_Codec(struct MediaPlayerHandle *player,struct MediaParams *user);

struct MediaPlayerHandle *media_player_handle_creat();
int media_player_handle_delete(struct MediaPlayerHandle *player);

#ifdef __cplusplus
}
#endif

#endif
