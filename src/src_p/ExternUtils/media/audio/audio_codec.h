#ifndef _AUDIO_CODEC_H_
#define _AUDIO_CODEC_H_


#include <stdio.h>
#include <alsa/asoundlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libswresample/swresample.h>

#include "audio_play.h"


#define AUDIO_CODEC_CHANNEL_DEF	16		//音频解码默认使用的声道数(其他参数由解码器自行获取)

typedef int(*CodecPlayPcm) (uint8_t *buf,uint32_t frames,void *param);


struct MediaParams;


//音频文件类型
typedef enum{
	AUDIO_FILE_TYPE_NONE=0X00,
	AUDIO_FILE_TYPE_WAV

}AudioFileType;


//WAV文件头的信息，44个字节
typedef struct WAV_HEADER{
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
//	AVCodecParameters *codec_params;
//	AVCodec *codec;
	AVCodecContext *codec_ctx;		//编码器上下文，源文件中的音频参数,位宽，声道等，视频的帧率分辨率等
	int stream_index;				//需要使用的编解码器index	
	AVStream *audio_stream;

	struct AudioStreamParams *hard_param;	//音频硬件的采样参数

	int (*callback_play) (uint8_t *buf,uint32_t frames,int offset,void *param);		//播放的回调函数
	void *callback_param;        //回调函数的参数（不需要则置NULL）
};


enum AVSampleFormat code_get_format(uint16_t wBitsPerSample);
int64_t code_get_channel_layout(int channels);
enum AVCodecID get_codeid_from_file_type(MediaFileType type);

struct SwrContext *swr_set_with_hard_param(AVCodecContext *codec_ctx,struct AudioStreamParams *hard_param);
int get_audio_params_wav(FILE *fp,struct AudioStreamParams *params);
void get_wav_header_info(FILE *fp,AudioWavHeader *wav_header);

AudioFileType Audio_Get_File_Type(FILE *fp);
int Audio_Get_Codec_Info(const char *filename,struct MediaCodecParam *codec);
int Audio_File_Codec(struct MediaCodecParam *audio,struct MediaParams *conf);

AVFrame *alloc_avframe_frames_hard(int frames,struct AudioStreamParams *hard_param);
int free_avframe(AVFrame **converted_frame);













#endif
