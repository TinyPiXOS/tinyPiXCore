/*///------------------------------------------------------------------------------------------------------------------------//
		音频编解码
说 明 : 如果是非pcm流的原始文件需要解码，
日 期 : 2024.11.27

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif
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
#include "../media/media.h"
#include "audio_play.h"
#include "audio_codec.h"
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

//调整解码的位置(可以用于设置编码文件的播放位置)
static void seek_to_position(struct MediaCodecParam *codec, double target_seconds) 
{
	// 获取时间基
	int streamIndex = codec->stream_index;
	AVStream* stream = codec->format_ctx->streams[streamIndex];	//流参数
	AVRational time_base = stream->time_base;

	// 计算目标时间戳
	int64_t target_ts = target_seconds / av_q2d(time_base);

	// 跳转到目标时间戳
	if (av_seek_frame(codec->format_ctx, streamIndex, target_ts, AVSEEK_FLAG_BACKWARD) < 0) {
		fprintf(stderr, "Failed to seek to %.2f seconds\n", target_seconds);
	} 
	else {
		avcodec_flush_buffers(codec->codec_ctx); // 清空解码器缓冲区
		debug_printf("Seeked to %.2f seconds successfully\n", target_seconds);
	}
}




//根据URL打开合适的编解码器
//url: 待播放的数据的文件路径或者网络URL
//audio: 返回的解码器参数
int find_codec(const char *url,struct MediaCodecParam *audio)
{
	int ret;
	AVFormatContext *format_ctx = NULL;
	AVCodecContext *codec_ctx;
	AVCodec *codec;
	
	media_init(1);	//初始化并使能网络流
	if(media_get_file_info(url,&format_ctx)<0)
		return -1;

    // 查找最佳匹配音频流索引号
    int audio_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream_index < 0) {
        fprintf(stderr, "Could not find audio stream\n");
        return -1;
    }

    // 查找解码器
	AVCodecParameters *codec_params = format_ctx->streams[audio_stream_index]->codecpar;
	codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        fprintf(stderr, "Could not find codec\n");
        return -1;
    }

    // 打开解码器
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        fprintf(stderr, "Could not allocate codec context\n");
        return -1;
    }
    if ((ret = avcodec_parameters_to_context(codec_ctx, codec_params)) < 0) {
        fprintf(stderr, "Could not copy codec parameters to context\n");
		//fprintf(stderr, "Could not copy codec parameters to context: %s\n", av_err2str(ret));
        return -1;
    }
    if ((ret = avcodec_open2(codec_ctx, codec, NULL)) < 0) {
        fprintf(stderr, "Could not open codec\n");
		//fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
        return -1;
    }

    // 确保通道布局被设置
    if (!codec_ctx->channel_layout) {
        codec_ctx->channel_layout = av_get_default_channel_layout(codec_ctx->channels);
    }

	audio->stream_index=audio_stream_index;
	audio->codec_ctx=codec_ctx;
	audio->format_ctx=format_ctx;
	//audio->bytes_per_sample=bytes_per_sample;
	return 0;
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

//解码并播放
int codec_play(struct MediaCodecParam *audio,struct MediaParams *conf)
{
	int ret;
	AVCodecContext *codec_ctx=audio->codec_ctx;
	struct AudioStreamParams *hard_param=audio->hard_param;

	struct SwrContext *swr_ctx = swr_set_with_hard_param(codec_ctx,hard_param);
    if (!swr_ctx || swr_init(swr_ctx) < 0) {
        fprintf(stderr, "Could not initialize resampler\n");
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&audio->format_ctx);
        return -1;
    }

	// 分配缓冲区
//	int max_samples = 4096; // 设置一个初始值，可根据需要调整
//	uint8_t **converted_data = NULL;
//	av_samples_alloc_array_and_samples(&converted_data, NULL, codec_ctx->channels, max_samples, AV_SAMPLE_FMT_S16, 1);//codec_ctx->frame_size
//	AVFrame *convert_frame;
/*	AVFrame *convert_frame = alloc_avframe_frames_hard(max_samples,hard_param);
	if(!convert_frame)
	{
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&audio->format_ctx);
		return -1;
	}*/
    AVPacket *packet = av_packet_alloc();
	 if (!packet) {
        fprintf(stderr, "Could not allocate packet\n");
		//free_avframe(&convert_frame);
		swr_free(&swr_ctx);
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&audio->format_ctx);
        return -1;
    }
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
		av_packet_free(&packet);
		//free_avframe(&convert_frame);
		swr_free(&swr_ctx);
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&audio->format_ctx);
        return -1;
    }

	Audio_Set_State(conf,AUDIO_STATE_PLAYING);
	while (av_read_frame(audio->format_ctx, packet) >= 0) 
	{
		AudioPlayCommand cmd=(AudioPlayCommand )(conf->command_get(conf));
		switch(cmd)
		{
			case AUDIO_PLCMD_SUSPEND:	//暂停时等待重新播放
				Audio_Set_State(conf,AUDIO_STATE_PAUSEING);
				conf->cond->wait(conf->cond);
				Audio_Set_State(conf,AUDIO_STATE_PLAYING);
				break;
			case AUDIO_PLCMD_NEXT:
			case AUDIO_PLCMD_LAST:
				debug_printf("上一首/下一首命令\n");
				Audio_Set_State(conf,AUDIO_STATE_JUMP);
				goto EXIT;
				break;
			case AUDIO_PLCMD_STOP:
			case AUDIO_PLCMD_EXIT:
				debug_printf("退出命令\n");
				Audio_Set_State(conf,AUDIO_STATE_STOP);
				goto EXIT;
				break;
			default:
				break;
		}
		//调整播放位置
		if((ret=Audio_Get_Position_S(conf))>=0)
		{
			seek_to_position(audio,ret);
			Audio_Set_BytePosition(conf,ret*hard_param->byteFrams);
			printf("Audio_Set_BytePosition:%d\n",ret*hard_param->byteFrams);
			avcodec_flush_buffers(codec_ctx); // 清空解码器缓冲区
		}

		if (packet->stream_index == audio->stream_index) 
		{
			int ret = avcodec_send_packet(codec_ctx, packet);
			if (ret < 0) {
				perror("解码失败: ");
				continue;
			}

			while ((ret = avcodec_receive_frame(codec_ctx, frame))>=0) 
			{
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
					break;
				if (ret < 0) {
					perror("解码错误: ");
					break;
				}

				/*if (frame->nb_samples > max_samples) 
				{		
					max_samples = frame->nb_samples;
					debug_printf("重新分配,侦数：%d\n",max_samples);
					//av_freep(&converted_data[0]);
					//av_samples_alloc_array_and_samples(&converted_data, NULL, codec_ctx->channels, max_samples, AV_SAMPLE_FMT_S16, 1);
					//free_avframe(&convert_frame);
					//convert_frame = alloc_avframe_frames_hard(4096,hard_param);
					//if(!convert_frame)
					//{
					//	max_samples=0;
					//	continue;
					//}
				}*/
				AVFrame *convert_frame = alloc_avframe_frames_hard(frame->nb_samples,hard_param);
				if(!convert_frame)
				{
					continue;
				}

				int samples_converted = swr_convert(swr_ctx, convert_frame->data, frame->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
				if(samples_converted<=0)
				{
					free_avframe(&convert_frame);
					continue;
				}

				audio->callback_play((uint8_t *)convert_frame, samples_converted,1,audio->callback_param );		//callback_codec_play
				
				free_avframe(&convert_frame);
			}
		}
		av_packet_unref(packet);
	}
EXIT:
	av_packet_free(&packet);
	av_frame_free(&frame);
	swr_free(&swr_ctx);
	avcodec_free_context(&codec_ctx);
    avformat_close_input(&audio->format_ctx);
	return 0;
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


AudioFileType Audio_Get_File_Type(FILE *fp)
{
	AudioWavHeader wav_header;
	int nread;
	nread=fread(&wav_header,1,sizeof(AudioWavHeader),fp);
	if(strncmp(wav_header.rld,"RIFF",4)==0)
		return AUDIO_FILE_TYPE_WAV;
	return AUDIO_FILE_TYPE_NONE;
}

/// @brief 获取音频解码器以及各种信息
/// @return 
int Audio_Get_Codec_Info(const char *filename,struct MediaCodecParam *codec)
{
	if(find_codec(filename,codec)<0)
	{
		fprintf(stderr,"Find codec error\n");
		return -1;
	}
	debug_printf("获取音频文件信息成功\n");
	return 0;
}

int Audio_File_Codec(struct MediaCodecParam *audio,struct MediaParams *conf)
{
	return codec_play(audio, conf);
}

//获取采样参数
int Audio_Get_Sample_Parame()
{
	return 0;
}



#ifdef __cplusplus
}
#endif
