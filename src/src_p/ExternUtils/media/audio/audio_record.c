/*///------------------------------------------------------------------------------------------------------------------------//
		音频录制
说 明 : 
日 期 : 2025.1.3

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <signal.h>		//用于signal函数，测试使用
#include "audio_record.h"
#include "audio_codec.h"
#include "audio_play.h"


uint8_t run_flag=0;

void exit_sighandler(int sig)
{
	run_flag=1;
}


struct MediaParams *record_config_creat()
{
	struct MediaParams *conf=(struct MediaParams *)malloc(sizeof(struct MediaParams));
	conf->volume=USER_CONF_VOLUME;
	conf->position_s = 0;
	conf->position_p = 0;
	conf->list=NULL;
	
	struct PthreadCond *pthread_cond=pthread_cond_creat_struct();
	if(pthread_cond==NULL)
	{
		free(conf);
		return NULL;
	}
	conf->cond=pthread_cond;
	pthread_rwlock_init(&conf->rw_mut, NULL);
	return conf;
}

void record_config_free(struct MediaParams *conf)
{
	if(!conf)
		return ;
	pthread_rwlock_destroy(&conf->rw_mut);
	pthread_cond_free_struct(conf->cond);
}

//生成WAV文件头,默认按照16位深度采样
//返回长度，直接写入文件头。
int creat_wav_header(AudioWavHeader *wav_record,struct AudioStreamParams *params)
{
	strcpy(wav_record->rld,"RIFF");
//	wav_record.rLen=0;        //文件大小
	strcpy(wav_record->wld,"WAVE");     //格式类型（wave）
	strcpy(wav_record->fld,"fmt ");       //"fmt"
	wav_record->fLen=16;   //sizeof(wave format matex)   
	wav_record->wFormatTag=1;   //编码格式
	wav_record->wChannels=params->wChannels;    //声道数
	wav_record->nSamplesPersec=params->nSamplesPersec;          //采样频率
	wav_record->nAvgBitsPerSample=params->nSamplesPersec*params->wChannels*2;             //WAVE文件采样大小
	wav_record->wBlockAlign=params->wChannels*params->wBitsPerSample/8;                  //块对齐
	wav_record->wBitsPerSample=params->wBitsPerSample;   // 样本数据位数  
	strcpy(wav_record->dld,"data");        //"data"
	return 0;
}

//调整文件头跟实际值一致
/*int auto_adjust_wav_header(AudioWavHeader *wav_record,PIAudioConf *pcm)
{
	wav_record->nSamplesPersec=pcm->adparams->nSamplesPersec;
	wav_record->wChannels=pcm->adparams->wChannels;
	return 0;
}*/

//ffmpeg编码器设置
static int set_ffmpeg_codec(struct MediaCodecParam *mediacodec,const char *filename,MediaFileType type,struct AudioStreamParams *adparams)
{
	int ret=0;
	AVFormatContext *format_ctx = NULL;
	AVCodecContext *codec_ctx=NULL;
	AVCodec *codec;
	AVStream *audio_stream=NULL;
	// 初始化 FFmpeg

#if LIBAVFORMAT_VERSION_MAJOR < 58		//高版本的ffmpeg的av_register_all在内部自动执行
    av_register_all();		//解码
	avcodec_register_all();	//编码
#endif
	
	// 打开输出文件
	if ((ret = avformat_alloc_output_context2(&format_ctx, NULL, NULL, filename)) < 0) {
		fprintf(stderr, "Error opening output context\n");
		return -1;
	}

	// 查找 AAC 编码器
	codec = avcodec_find_encoder(get_codeid_from_file_type(type));
	if (!codec) {
		fprintf(stderr, "AAC encoder not found!\n");
		return -1;
	}

	// 创建音频流
	audio_stream = avformat_new_stream(format_ctx, codec);
	if (!audio_stream) {
		fprintf(stderr, "Error creating stream!\n");
		return -1;
	}
	// 创建编码上下文
    codec_ctx = avcodec_alloc_context3(codec);
    codec_ctx->sample_fmt = code_get_format(adparams->wBitsPerSample);
    codec_ctx->sample_rate = adparams->nSamplesPersec;
    codec_ctx->channel_layout = code_get_channel_layout(adparams->wChannels);
    codec_ctx->channels = adparams->wChannels;
    codec_ctx->bit_rate = adparams->bitRate;

	// 打开编码器
	if ((ret = avcodec_open2(codec_ctx, codec, NULL)) < 0) {
		fprintf(stderr,"Error opening codec\n");
		return -1;
	}

	// 将编码器上下文参数复制到流中
	if (avcodec_parameters_from_context(audio_stream->codecpar, codec_ctx) < 0) {
		fprintf(stderr, "无法设置流的编码器参数\n");
		return -1;
	}

	// 打开输出文件
	if (!(format_ctx->oformat->flags & AVFMT_NOFILE)) {
		if (avio_open(&format_ctx->pb, filename, AVIO_FLAG_WRITE) < 0) {
			fprintf(stderr, "无法打开文件\n");
			return -1;
		}
	}

	// 写入文件头
	if (avformat_write_header(format_ctx, NULL) < 0) {
		fprintf(stderr, "无法写入文件头\n");
		return -1;
	}

	mediacodec->codec_ctx=codec_ctx;
	mediacodec->format_ctx=format_ctx;
	mediacodec->audio_stream=audio_stream;
	return 0;
}

//根据硬件参数和用户需要的输出参数重新设置重采样参数
static struct SwrContext *swr_set_with_user_param(AVCodecContext *codec_ctx,struct AudioStreamParams *hard_param)
{
	struct SwrContext *swr_ctx = swr_alloc_set_opts(NULL,
									code_get_channel_layout(hard_param->wChannels),
									AV_SAMPLE_FMT_FLTP,
									hard_param->nSamplesPersec,
									code_get_channel_layout(hard_param->wChannels),
									code_get_format(hard_param->wBitsPerSample),
									hard_param->nSamplesPersec,
									0,
									NULL);
	return swr_ctx;
}


//pcm硬件音频数据读取
//frames:buffer的长度，单位为帧数
int pcm_read_data(PIAudioConf *pcm,uint8_t *buffer,unsigned long frames,int delay)
{
	int time=0;
	snd_pcm_sframes_t avail;
	snd_pcm_sframes_t delay_p;
	int err;
	delay/=5;
	// 获取播放设备当前的延迟（缓冲区剩余的帧数）
	while (1) 
	{
		err = snd_pcm_readi(pcm->handle, buffer, frames);
        if (err == -EPIPE) {
            fprintf(stderr, "Overrun occurred!\n");
            snd_pcm_prepare(pcm->handle);  // 重置设备
        } 
		else if (err < 0) {
            fprintf(stderr, "Error reading from PCM device: %s\n", snd_strerror(err));
        } 
		else {
           //fwrite(buffer, 1, frames*4, stdout);  // 输出到标准输出
		   break;
        }
		time++;
		if(time>delay)
			return -1;	
		usleep(5000);  // 等待 5 毫秒后再检查
	}
	return err;
}

static int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}


//申请编码所需的AVFrame空间并初始化
static AVFrame *avframe_creat(AVCodecContext *codec_ctx,uint32_t frames)
{
	AVFrame *frame = av_frame_alloc();
	if(!frame)
	{
		return NULL;
	}
	frame->nb_samples = frames;
	frame->format = codec_ctx->sample_fmt;
	frame->channel_layout = codec_ctx->channel_layout;
	frame->channels = codec_ctx->channels;
	if(av_frame_get_buffer(frame, 0)<0)		//为AVFrame分配缓存区
	{
		return NULL;
	}
	return frame;
}

//写入到文件
static void avframe_delete(AVFrame *frame)
{
	av_frame_free(&frame);
}

static int stream_write_file(AVFormatContext *format_ctx,AVCodecContext *codec_ctx,AVStream *audio_stream,struct SwrContext *swr_ctx,
							uint8_t *buffer,uint32_t frames,uint32_t frame_size,
							AVFrame *frame,AVPacket *packet)
{
	int ret=0;

	memcpy(frame->data[0], buffer, frames*frame_size);

	//重采样
	// 将 ALSA 数据从 S16 转换为 FLTP 格式
	uint8_t *out_buffer = NULL;
	ret = swr_convert(swr_ctx, &out_buffer, frames, (const uint8_t **)&buffer, frames);
	if (ret < 0) {
		fprintf(stderr, "Error converting audio format\n");
		return -1;
	}

	// 设置音频帧数据
	frame->data[0] = out_buffer;

	ret = avcodec_send_frame(codec_ctx, frame);
	// 编码音频帧
	ret = avcodec_receive_packet(codec_ctx, packet);
	if (ret == 0) {
		//av_packet_rescale_ts(&packet, codec_ctx->time_base, audio_stream->time_base);		//缩放/转换媒体流中的时间戳
		//ret = av_interleaved_write_frame(format_ctx, &packet);		//写入文件（如果有多个流必须使用这个）
		ret = av_write_frame(format_ctx, packet);					//写入文件
		av_packet_unref(packet);
	} 
	else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
		// 等待更多的音频帧
	} 
	else {
		fprintf(stderr, "Error encoding audio\n");
		return -1;
	}
}

//打开文件并写入文件头
int audio_stream_write_file_head(const char *filename,struct MediaCodecParam *audio)
{
	if ((avio_open(&audio->format_ctx->pb, filename, AVIO_FLAG_WRITE)) < 0) {
		fprintf(stderr,"Error opening file\n");
		return -1;
	}

	if ((avformat_write_header(audio->format_ctx, NULL)) < 0) {
		fprintf(stderr,"Error writing header\n");
		return -1;
	}
	return 0;
}

//录制文件
int audio_record_file(PIAudioConf *pcm,struct MediaParams *conf,const char *file)
{
	uint32_t total_size=0;
	int cmd;
	struct MediaCodecParam codec;

	AudioWavHeader wav_record;
	struct AudioStreamParams audio_param;		//采样的格式
	audio_param.wChannels=2;
	audio_param.nSamplesPersec=44100;
	audio_param.wBitsPerSample=16;
	audio_param.bitRate=128000;
	pcm_hwparams_set(pcm,&audio_param);  		//设置硬件参数

	uint8_t frame_size=PCM_BUFFER_SIZE*audio_param.wChannels*audio_param.wBitsPerSample/8;		//每个侦的字节数
	uint32_t cycle_frames=PCM_BUFFER_FRAMES;
	uint8_t *buffer=(uint8_t *)malloc(cycle_frames*frame_size);
	
	
	snd_pcm_state_t pcm_state = snd_pcm_state(pcm->handle);
	if (pcm_state != SND_PCM_STATE_RUNNING) 
	{
		if (snd_pcm_start(pcm->handle) < 0) 
		{
			fprintf(stderr, "Failed to start PCM device.\n");
			return -1;
		}
	}

	set_ffmpeg_codec(&codec,file,MEDIA_FILE_TYPE_MP3,&audio_param);
	struct SwrContext *swr_ctr=swr_set_with_user_param(codec.codec_ctx,&audio_param);
	AVFrame *frame = avframe_creat(codec.codec_ctx,cycle_frames);
	AVPacket *packet = av_packet_alloc();
		if (!packet) {
		fprintf(stderr, "Could not allocate packet\n");
		return -1;
	}

	Audio_Set_State(conf,AUDIO_STATE_RECORD);
	printf("开始读取pcm并写入文件\n");
	while(1)
	{
		//从声卡设备读取一帧音频数据:字节
		int frames;
		if((frames=pcm_read_data(pcm,buffer,cycle_frames,100))<0) 
		{
			printf("从音频接口读取失败(%s)\n",snd_strerror(frames));
			continue;
		}

		//如果状态是停止则只读取不写入
		cmd = conf->command_get(conf);
		switch(cmd)
		{
			case AUDIO_PLCMD_SUSPEND:
				Audio_Set_State(conf,AUDIO_STATE_PAUSEING);
				conf->cond->wait(conf->cond);
				Audio_Set_State(conf,AUDIO_STATE_RECORD);
				break;
			case AUDIO_PLCMD_STOP:
			case AUDIO_PLCMD_EXIT:
				goto STOP;
				break;
			default:
				break;
		}

		stream_write_file(codec.format_ctx,codec.codec_ctx,codec.audio_stream,swr_ctr,buffer, frames, frame_size, frame,packet);

		//av_packet_unref(&packet);
		total_size+=(frame_size*frames);

		if(run_flag)			//来自于键盘的进程退出信号，作为后台服务时不使用
		{
			printf("停止采集.\n");
			break;
		}
	}
STOP:

	avframe_delete(frame);
	Audio_Set_State(conf,AUDIO_STATE_STOP);
	return 0;
}

//录制wav文件
int audio_record_wav_file(PIAudioConf *pcm,struct MediaParams *conf,const char *file)
{
	uint32_t total_size=0;
	int cmd;
	FILE *fp;
	if((fp = fopen(file, "wb")) == NULL)
	{
		printf("无法创建音频文件.\n");
		return -1;
	}

	AudioWavHeader wav_record;
	struct AudioStreamParams audio_param;
	audio_param.wChannels=2;
	audio_param.nSamplesPersec=44100;
	audio_param.wBitsPerSample=16;
	audio_param.bitRate=128000;
	
	pcm_hwparams_set(pcm,&audio_param);  //设置硬件参数

	uint8_t frame_size=PCM_BUFFER_SIZE*audio_param.wChannels*audio_param.wBitsPerSample/8;		//每个侦的字节数
	uint32_t cycle_frames=PCM_BUFFER_FRAMES;
	uint8_t *buffer=(uint8_t *)malloc(frame_size*cycle_frames);
	printf("sizeof read buffer%d\n",frame_size*cycle_frames);

	snd_pcm_state_t pcm_state = snd_pcm_state(pcm->handle);
	if (pcm_state != SND_PCM_STATE_RUNNING) 
	{
		if (snd_pcm_start(pcm->handle) < 0) 
		{
			fprintf(stderr, "Failed to start PCM device.\n");
			fclose(fp);
			remove(file);
			return -1;
		}
	}
	
	creat_wav_header(&wav_record,&audio_param);
	fwrite(&wav_record,1,sizeof(wav_record),fp);		//写入占位

	Audio_Set_State(conf,AUDIO_STATE_RECORD);
	while(1)
	{
		//从声卡设备读取一帧音频数据:字节
		int frames;
		if((frames=pcm_read_data(pcm,buffer,cycle_frames,100))<0) 
		{
			printf("从音频接口读取失败(%s)\n",snd_strerror(frames));
			continue;
		}
		printf("从音频接口读取长度%d\n",frames);
		for(int i=0;i<20;i++)
		{
			printf("%02x ",buffer[i]);
		}
		//如果状态是停止则只读取不写入
		cmd = conf->command_get(conf);
		switch(cmd)
		{
			case AUDIO_PLCMD_SUSPEND:
				Audio_Set_State(conf,AUDIO_STATE_PAUSEING);
				conf->cond->wait(conf->cond);
				Audio_Set_State(conf,AUDIO_STATE_RECORD);
				break;
			case AUDIO_PLCMD_STOP:
			case AUDIO_PLCMD_EXIT:
				goto STOP;
				break;
			default:
				break;
		}

		fwrite(buffer,1,frame_size*frames,fp);
		total_size+=(frame_size*frames);
		usleep(1000);
		if(run_flag)			//来自于键盘的进程退出信号，作为后台服务时不使用
		{
			printf("停止采集.\n");
			break;
		}
	}
STOP:
	fseek(fp,0,SEEK_SET);
//	auto_adjust_wav_header(&wav_record,pcm);
	wav_record.rLen=36+total_size;
	wav_record.wSampleLength=total_size;
	fwrite(&wav_record,1,sizeof(wav_record),fp);
	get_wav_header_info(fp,&wav_record);
	fclose(fp);
	free(buffer);	
	Audio_Set_State(conf,AUDIO_STATE_STOP);
	return 0;
}


//录制并输出音频流
int audio_record_stream()
{
	return 0;
}

/// @brief 打开Audio设备
/// @param pcm_play 声卡硬件参数，这里仅仅是基础的硬件配置
/// @return 
PIAudioConf *Audio_Record_Open(const char *device)
{
	PIAudioConf *pcm_play=(PIAudioConf *)malloc(sizeof(PIAudioConf));
	if(pcm_play==NULL)
		return NULL;

	if(Audio_Device_Init(pcm_play,device,AUDIO_STREAM_CAPTURE)<0)
	{
		free(pcm_play);
		return NULL;
	}
	return pcm_play;
}


int Record_Set_Start(struct MediaParams *conf,const char *file)
{
	if(!file)
	{
		fprintf(stderr,"录音模式文件名不能为空\n");
		return -1;
	}

	return Audio_Set_Start(conf,file);
}	

int Record_Set_Stop(struct MediaParams *conf)
{
	return Audio_Set_Stop(conf);
}	


//线程或进程的主程序
int Audio_Record_Main(PIAudioConf *pcm,struct MediaParams *conf)
{
	int cmd;
	while(1)
	{
		cmd=conf->command_get(conf);
		switch(cmd)
		{
			case AUDIO_PLCMD_STOP:
				printf("等待开始信号\n");
				conf->cond->wait(conf->cond);		//等待开始信号
				Audio_Set_Command(conf,AUDIO_PLCMD_NONE);
				break;
			case AUDIO_PLCMD_EXIT:
				Audio_Set_State(conf,AUDIO_STATE_EXIT);
				return 0;
				break;
			case AUDIO_PLCMD_NONE:
				conf->cond->wait(conf->cond);		//等待开始信号
				break;
			default:
				break;
		}

		char *file=conf->list->read_saft(conf->list);
		if(file==NULL)
			break;
		printf("start record ,save to %s\n",file);
		Audio_Set_State(conf,AUDIO_STATE_RECORD);
		//audio_record_file(pcm,conf,file);
		audio_record_wav_file(pcm,conf,file);
		printf("save \n");
		conf->list->delete_all_saft(conf->list);
		conf->list=NULL;
	}
	return 0;
}

int Audio_Record_Test(PIAudioConf *pcm,const char *file)
{

	signal(SIGINT, exit_sighandler);
	pcm->file_type=AUDIO_FILE_TYPE_WAV;
	audio_record_wav_file(pcm,NULL,file);

	return 0;
}

//获取录音的音频流


#ifdef __cplusplus
}
#endif

