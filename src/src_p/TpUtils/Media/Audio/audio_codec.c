/*///------------------------------------------------------------------------------------------------------------------------//
		音频编解码
说 明 : 如果是非pcm流的原始文件需要解码，
日 期 : 2024.11.27

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include "Media/media.h"
#include "Media/media_timer.h"
#include "Audio/audio_play.h"
#include "Audio/audio_codec.h"
#include "filter.h"

#ifdef DEBUG_AUUDIO
    #define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define debug_printf(fmt, ...)  // 如果不定义DEBUG，什么也不做
#endif

//根据文件类型获取对应的解码器
enum AVCodecID get_codeid_from_file_type(MediaFileType type)
{
	switch(type)
	{
		case MEDIA_FILE_TYPE_MP3:	return AV_CODEC_ID_MP3;
		case MEDIA_FILE_TYPE_M4A:	return AV_CODEC_ID_AAC;

	}
	return AV_CODEC_ID_MP3;
}



//按照音频参数和帧数申请合适大小的AVFrame
AVFrame *alloc_avframe_frames_hard(int frames,struct AudioStreamParams *hard_param)
{
	AVFrame *converted_frame=av_frame_alloc();
	if(!converted_frame)
	{
		fprintf(stderr, "Alloc for package uint8 to frame error\n");
		return NULL;
	}
	converted_frame->nb_samples = frames;
	converted_frame->format = code_get_format(hard_param->wBitsPerSample); 
	converted_frame->channels = hard_param->wChannels;	
	converted_frame->channel_layout=code_get_channel_layout(hard_param->wChannels);
	converted_frame->sample_rate = hard_param->nSamplesPersec; 

	if (av_frame_get_buffer(converted_frame, 0) < 0) {
		fprintf(stderr, "Could not allocate frame buffer\n");
		av_frame_free(&converted_frame);
		return NULL;
	}
	return converted_frame;
}
//释放AVFrame
int free_avframe(AVFrame **converted_frame)
{
	av_frame_free(converted_frame);
}


//获取解码器的AVSampleFormat
enum AVSampleFormat code_get_format(uint16_t wBitsPerSample)
{
	enum AVSampleFormat fmt=AV_SAMPLE_FMT_NONE;
	switch(wBitsPerSample)
	{
		case 8: fmt=AV_SAMPLE_FMT_U8; break;
		case 16:fmt=AV_SAMPLE_FMT_S16;break;
		case 24:fmt=AV_SAMPLE_FMT_S32;break;
		case 32:fmt=AV_SAMPLE_FMT_S32;break;
		default:fmt=AV_SAMPLE_FMT_U8; break;
	}
	return fmt;
}

//获取解码器的channel_layout
int64_t code_get_channel_layout(int channels)
{
	int64_t channel_layout = av_get_default_channel_layout(channels);
	return channel_layout;
}

//使用硬件配置重新设置重采样参数
struct SwrContext *swr_set_with_hard_param(AVCodecContext *codec_ctx,struct AudioStreamParams *hard_param)
{
	struct SwrContext *swr_ctx = swr_alloc_set_opts(NULL,
									code_get_channel_layout(hard_param->wChannels),
									code_get_format(hard_param->wBitsPerSample),
									hard_param->nSamplesPersec,
									codec_ctx->channel_layout,
									codec_ctx->sample_fmt,
									codec_ctx->sample_rate,
									0,
									NULL);	
	return swr_ctx;
}


//获取wav文件头信息
void get_wav_header_info(FILE *fp,AudioWavHeader *wav_header)
{
	int nread;
	fseek(fp,0,SEEK_SET); 
	nread=fread(wav_header,1,sizeof(AudioWavHeader),fp);
	debug_printf("nread=%d\n",nread);
	debug_printf("RIFF 标志%s\n",wav_header->rld);
	debug_printf("文件大小rLen：%d\n",wav_header->rLen);
	debug_printf("文件类型%c%c%c%c\n",wav_header->wld[0],wav_header->wld[1],wav_header->wld[2],wav_header->wld[3]);
	debug_printf("格式块标志符：%s\n",wav_header->fld);
	debug_printf("格式块大小：%d\n",wav_header->fLen);
	debug_printf("编码格式:%d\n",wav_header->wFormatTag);
	debug_printf("声道数：%d\n",wav_header->wChannels);
	debug_printf("采样频率：%d\n",wav_header->nSamplesPersec);
	debug_printf("每秒播放字节数：%d\n",wav_header->nAvgBitsPerSample);
	debug_printf("每个采样点byte数：%d\n",wav_header->wBlockAlign);
	debug_printf("数据位数：%d\n",wav_header->wBitsPerSample);
    
	debug_printf("data=%s\n",wav_header->dld);
	debug_printf("wSampleLength=%d\n",wav_header->wSampleLength);    		
}

int get_audio_params_wav(FILE *fp,struct AudioStreamParams *params)
{
	AudioWavHeader wav_header;
	get_wav_header_info(fp,&wav_header);
	params->nAvgBitsPerSample=wav_header.nAvgBitsPerSample;
	params->nSamplesPersec=wav_header.nSamplesPersec;
	params->wBitsPerSample=wav_header.wBitsPerSample;
	params->wChannels=wav_header.wChannels;
	params->rLen=wav_header.rLen-sizeof(AudioWavHeader);
	return 0;
}

