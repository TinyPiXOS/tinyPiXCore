#ifndef _AUDIO_PLAY_H_
#define _AUDIO_PLAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include "filter.h"
#include "Media/media_file_list.h"
#include "Media/Media/media_config.h"

struct MediaAudioInfo;
struct MediaUserParams;


#define NONE_AUDIO_CARD_PLAY	1	//允许无声卡播放
#define DEBUG_AUUDIO		//调试打印接口
//全局配置
#define PCM_BUFFER_FRAMES	1024		//pcm的缓存区帧数，每次可以写入这么多侦的数量
#define PCM_BUFFER_SIZE 	4			//pcm缓存区大小由这个值计算：=	PCM_BUFFER_FRAMES * PCM_BUFFER_SIZE	*侦字节数

#define PCM_CHANNELS_DEFAULT		2		//默认通道树
#define PCM_SAMPLE_PERSEC_DEFAULT	44100	//默认采样频率
#define PCM_Bits_PER_SAMPLE_DEFAULT	16		//默认采样位数


#define USER_CONF_VOLUME 		100	//默认音量

//前置声明
struct MediaUserParams;
struct MediaCodecParam;
struct MediaStreamParams;


typedef int(*CallbackAudioPlay)(uint8_t *buf,uint32_t frames,int offset,void *param);



typedef enum {
    AUDIO_STREAM_PLAYBACK = SND_PCM_STREAM_PLAYBACK,
    AUDIO_STREAM_CAPTURE = SND_PCM_STREAM_CAPTURE
}AudioStreamType;



struct MediaUserParams;


typedef int(*CodecPlayPcm) (uint8_t *buf,uint32_t frames,void *param);


//WAV文件头的信息，44个字节
typedef struct AudioWavHeader{
	char rld[4];    //riff 标志符号
	uint32_t rLen;   
	char wld[4];    //格式类型（wave）
	char fld[4];    //"fmt"

	uint32_t fLen;   //sizeof(wave format matex)

	uint16_t wFormatTag;   //编码格式
	uint16_t wChannels;    //声道数
	uint32_t nSamplesPersec ;  //采样频率
	uint32_t nAvgBitsPerSample;//每秒播放字节数
	uint16_t wBlockAlign; //每个采样点byte数
	uint16_t wBitsPerSample;   //数据位数

	char dld[4];        //”data“
	uint32_t wSampleLength;  //音频数据的大小
}AudioWavHeader;

//声卡和音频相关的采样参数
struct AudioStreamParams{
	uint16_t wChannels;    		//声道数
	uint32_t nSamplesPersec;	//采样频率
	uint16_t wBitsPerSample; 	//数据位数
	uint16_t byteFrams;			//每个帧的字节数
	uint32_t nAvgBitsPerSample;//每秒播放字节数	（=nSamplesPersec*wChannels*wBitsPerSample/8）
	uint32_t rLen; 		//音频数据长度(不同于head中的rLen，这个是去掉头的)
	uint32_t bitRate;
};

//媒体编/解码器参数
struct MediaCodecParam{
	AVFormatContext *format_ctx;	//输入输出相关信息，贯穿ffmpeg
	AVCodecContext *codec_ctx;		//编码器上下文，源文件中的音频参数,位宽，声道等，视频的帧率分辨率等
	int stream_index;				//需要使用的编解码器index	
	AVStream *audio_stream;

	struct AudioStreamParams *hard_param;	//音频硬件的采样参数

	int (*callback_play) (uint8_t *buf,uint32_t frames,int offset,void *param);		//播放的回调函数
	void *callback_param;        //回调函数的参数（不需要则置NULL）
};



//声卡的硬件的部分信息
struct PcmHardParams{
	uint8_t can_pause;			//PCM是否支持暂停
	uint8_t can_resume;			//PCM是否支持恢复播放
	unsigned long cycle_frames;		///每个周期处理的侦的数量，写入的时候可以每次写入这个数量
	unsigned long buff_size;		//缓存区大小
};


//内部使用，声卡配置信息
//音频的硬件参数和句柄
struct MediaAudioHandle{
	snd_pcm_t *handle;             	//设备句柄（当设备句柄为空的时候说明无法使用硬件播放）
	snd_pcm_hw_params_t *hwparams;  //设备配置信息的结构体(结构体内部隐藏)，配置信息保存在该结构体
//	uint8_t file_type;          	//音频文件类型(AudioFileType类型)
	struct AudioStreamParams *adparams;	//解码后可以用于播放的音频流的参数
	struct PcmHardParams *ahparams;		//设置后的一些关键硬件参数(其实是从snd_pcm_hw_params_t里面拿出来的几个常用的参数)
};



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

struct MediaAudioInfo{
	pthread_rwlock_t rw_mut;	//数据交互读写锁
	uint8_t volume;			//声音(0-100)
	char *device;			//声卡

	struct{
		CallbackAudioPlay callback_audio;
		void *userdata;	//struct codePlayCallbackParam
		CallbackAudioPlay (*get_callback_audio)(struct MediaAudioInfo *conf);
		void (*set_callback_audio)(struct MediaAudioInfo *conf, CallbackAudioPlay callback,void *userdata);
	};
};


enum AVSampleFormat code_get_format(uint16_t wBitsPerSample);
int64_t code_get_channel_layout(int channels);
enum AVCodecID get_codeid_from_file_type(MediaFileType type);

struct SwrContext *swr_set_with_hard_param(AVCodecContext *codec_ctx,struct AudioStreamParams *hard_param);
int get_audio_params_wav(FILE *fp,struct AudioStreamParams *params);
void get_wav_header_info(FILE *fp,AudioWavHeader *wav_header);


AVFrame *alloc_avframe_frames_hard(int frames,struct AudioStreamParams *hard_param);
int free_avframe(AVFrame **converted_frame);

int pcm_hwparams_set(struct MediaAudioHandle *pcm,struct AudioStreamParams *audio);		//设置硬件参数
int audio_stream_write(struct MediaAudioHandle *pcm_play,struct MediaUserParams *conf,
							uint8_t *buffer,uint32_t frames,
							float volume,
							int offset,int delay);

int audio_pcm_drain(struct MediaAudioHandle *pcm);
int audio_pcm_drop(struct MediaAudioHandle *pcm);
int audio_pcm_close(struct MediaAudioHandle *pcm);

struct MediaAudioHandle *Audio_Play_Open(const char *device);
int Audio_Device_Init(struct MediaAudioHandle *pcm_play,const char *device,AudioStreamType type);
int Audio_Device_Close(struct MediaAudioHandle *pcm_play);


//设置音量
int Audio_Set_Volume(struct MediaAudioInfo *conf_a,int16_t volume);

//获取音量
int Audio_Get_Volume(struct MediaAudioInfo *conf_a);




//设置硬件
int Audio_Set_Hard_Params(struct MediaAudioHandle *pcm_play,struct MediaUserParams *conf,uint32_t rate,uint16_t channel,uint16_t bits);
//取消硬件设置
int Audio_Set_Nonblock(struct MediaAudioHandle *pcm_play,struct MediaUserParams *conf,uint8_t nonblock);
int Audio_Write_Stream(struct MediaAudioHandle *pcm,struct MediaUserParams *conf,struct AudioStreamParams *hard_params,
							uint8_t *buffer,uint32_t frames,int offset,int delay);


int media_stream_audio_init_handle(struct MediaStreamParams *audio,struct MediaUserParams *user);
int media_stream_audio_deinit_handle(struct MediaStreamParams *audio);

#ifdef __cplusplus
}
#endif
#endif
