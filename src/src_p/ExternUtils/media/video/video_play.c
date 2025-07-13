/*///------------------------------------------------------------------------------------------------------------------------//
		视频画面显示播放
说 明 : 
日 期 : 2025.1.8

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>		//audio部分
#include "video_play.h"
#include "audio_codec.h"
#include "audio_play.h"
#include "video_codec.h"

int callback_codec_play_audio(uint8_t *buf,uint32_t frames,int offset,void *param);
int callback_codec_play_video(uint8_t *buf,uint32_t frames,void *param);
int get_display_params_user_codec(struct MediaParams *user,AVCodecContext *codec_ctx,struct MediaVideoParams *video_params);

#ifdef DEBUG_VIDEO_INIT
    #define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define debug_printf(fmt, ...)  // 如果不定义DEBUG，什么也不做
#endif

int video_hard_param_init(struct VideoHardParam *video, const char *audio_card)
{
	if(!video)
		return -1;
	video->window=NULL;
	video->renderer=NULL;
	video->texture=NULL;
	video->rect_dst=NULL;
	video->rect_src=NULL;
	video->format=0;
	if(audio_card)
		video->audio_card=strdup(audio_card);
	video->audio_data=NULL;
//	video->pcm_play=NULL;
	video->swr_ctr=NULL;
	video->is_sdl=false;
}

int video_hard_param_deinit(struct VideoHardParam *video)
{
	if(video->audio_card)
		free(video->audio_card);
	return 0;
}


//创建纹理（设置format，如果不支持会设置为其他格式）
//每次调整播放窗口大小都要重新创建纹理
SDL_Texture *sdl_creat_texture_near(SDL_Renderer *renderer,uint32_t *format,int w,int h)
{
	SDL_Texture *texture = SDL_CreateTexture(renderer, *format, SDL_TEXTUREACCESS_STREAMING,w, h);		//SDL_PIXELFORMAT_RGB24
	if(texture!=NULL)
	{
		return texture;
	}
	fprintf(stderr, "Creat Texture! SDL_Error: %s\n", SDL_GetError());

	int num_formats = get_sizeof_format_mapping();

	for (int i = 0; i < num_formats; i++) 
	{
		if((texture = SDL_CreateTexture(renderer, get_format_mapping_with_num(i), SDL_TEXTUREACCESS_STREAMING,w, h))!=NULL)
		{
			*format = get_format_mapping_with_num(i);
			break;
		}
	}
	return texture;
}

//音频播放回调
void sdl_audio_callback(void *userdata, Uint8 *stream, int len) 
{
	struct AudioData *audio = (struct AudioData *)userdata;
	int remaining = audio->buffer_size - audio->buffer_pos;

	if (remaining == 0) {
		memset(stream, 0, len);  // 没有数据时填充静音
		return;
	}

	int to_copy = len > remaining ? remaining : len;
	memcpy(stream, audio->buffer + audio->buffer_pos, to_copy);
	audio->buffer_pos += to_copy;

	if (to_copy < len) {
		memset(stream + to_copy, 0, len - to_copy);  // 填充剩余部分为静音
	}
}

//sdl音频初始化(SDL对alsa进行了封装，也可以直接使用alsa库)
int sdl_audio_init(struct VideoHardParam *display,struct MediaCodecParam *audio)
{
	AVCodecContext *codec_ctx=audio->codec_ctx;

	SDL_AudioSpec audioSpec;
    audioSpec.freq = codec_ctx->sample_rate;
    audioSpec.format = AUDIO_S16SYS;
    audioSpec.channels = codec_ctx->channels;
    audioSpec.silence = 0;
    audioSpec.samples = 1024;
    audioSpec.callback = sdl_audio_callback;

    struct AudioData *audioData = (struct AudioData *)malloc(sizeof(struct AudioData));
	audioData->buffer=NULL;
	audioData->buffer_pos=0;
	audioData->buffer_size=0;
    audioSpec.userdata = audioData;

    if (SDL_OpenAudio(&audioSpec, NULL) < 0) {
        fprintf(stderr, "SDL_OpenAudio error: %s\n", SDL_GetError());
        return -1;
    }

	SDL_PauseAudio(0);
	// 音频重采样
	
	/*struct SwrContext *swrContext = swr_alloc();
	if(swrContext==NULL)
		return -1;
	av_opt_set_int(swrContext, "in_channel_layout", codec_ctx->channel_layout, 0);
	av_opt_set_int(swrContext, "out_channel_layout", codec_ctx->channel_layout, 0);
	av_opt_set_int(swrContext, "in_sample_rate", codec_ctx->sample_rate, 0);
	av_opt_set_int(swrContext, "out_sample_rate", codec_ctx->sample_rate, 0);
	av_opt_set_sample_fmt(swrContext, "in_sample_fmt", codec_ctx->sample_fmt, 0);
	av_opt_set_sample_fmt(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);*/
	struct SwrContext *swrContext = swr_alloc_set_opts(NULL,
                                 codec_ctx->channel_layout,		//codec_ctx->channel_layout = av_get_default_channel_layout(codec_ctx->channels);
                                 AV_SAMPLE_FMT_S16,
                                 codec_ctx->sample_rate,
                                 codec_ctx->channel_layout,
                                 codec_ctx->sample_fmt,
                                 codec_ctx->sample_rate,
                                 0,
                                 NULL);	
	
    if (!swrContext || swr_init(swrContext) < 0) {
        fprintf(stderr, "Could not initialize resampler\n");
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&audio->format_ctx);
        return -1;
    }

	debug_printf("init sdl audio ok\n");
	display->swr_ctr=swrContext;
	display->audio_data=audioData;
	audio->callback_play=callback_codec_play_audio;
	return 0;
}

//声卡初始化
int alsa_hard_init(const char *name,struct VideoHardParam *display,struct MediaCodecParam *audio,struct MediaParams *conf)
{
	AVCodecContext *codec_ctx=audio->codec_ctx;
	PIAudioConf *pcm_play=Audio_Play_Open(name);
	if(pcm_play==NULL){
		fprintf(stderr, "Audio pcm open error\n");
		display->pcm_play=NULL;
		return -1;
	}
	if(Audio_Hard_Auto_Init(pcm_play,conf,audio)<0)
		return -1;
	struct SwrContext *swrContext=swr_set_with_hard_param(codec_ctx,audio->hard_param);
    if (!swrContext || swr_init(swrContext) < 0) {
		perror("Could not initialize resampler");
		fprintf(stderr, "Could not initialize resampler\n");
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&audio->format_ctx);
        return -1;
    }

	debug_printf("Resampling:%d\n",codec_ctx->sample_rate);
	debug_printf("init sdl audio ok\n");
	display->swr_ctr=swrContext;
	display->pcm_play=pcm_play;
	//display->audio_data=audioData;
	return 0;	
}
//声卡取消初始化
int alsa_hard_deinit(struct VideoHardParam *display,struct MediaCodecParam *audio)
{
	if(!audio->codec_ctx)
		avcodec_free_context(&audio->codec_ctx);
	if(!audio->format_ctx)
		avformat_close_input(&audio->format_ctx);
	if(!display->swr_ctr)
		swr_free(&display->swr_ctr);
	Audio_Hard_Deinit(audio);		//取消硬件的设置
	Audio_Device_Close(display->pcm_play);			//关闭设备
	return 0;
}


//SDL初始化(显示)
int sdl_display_init(struct VideoHardParam *display, struct MediaCodecParam *codec_v,uint32_t format,int x, int y, int w, int h)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	//创建SDL窗口
	display->window = SDL_CreateWindow("tinyPiX Video", x, y, w, h, SDL_WINDOW_SHOWN);
	if (!display->window) {
		fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}
	//创建渲染器
	display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
	if(display->renderer==NULL)
	{
		fprintf(stderr, "Creat Renderer!,SDL_Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(display->window);
		SDL_Quit();
		return -1;
	}
	//创建纹理
	display->texture = sdl_creat_texture_near(display->renderer, &format,w,h);		//codec_v->codec_ctx->width,codec_v->codec_ctx->height SDL_PIXELFORMAT_RGB24
	if(display->texture==NULL)
	{
		fprintf(stderr, "Creat Texture! SDL_Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        SDL_Quit();
	}
	
	
	debug_printf("debug:sdl init ok, display on(%d,%d %d*%d)\n",x,y,w,h);
	display->format=format;
	return 0;
}

int sdl_display_deinit(struct VideoHardParam *display)
{
	if(display->texture);
		SDL_DestroyTexture(display->texture);
	if(display->renderer)
		SDL_DestroyRenderer(display->renderer);
	if(display->window)
		SDL_DestroyWindow(display->window);
	SDL_Quit();
	return 0;
}


int video_display_init()
{
	return 0;
}

int callback_codec_play_audio(uint8_t *buf,uint32_t frames,int offset,void *param)
{
	//debug_printf("debug:sdl play audio\n");
	struct AudioData *audio_data=(struct AudioData *)param;
	audio_data->buffer = buf;
	audio_data->buffer_size = frames * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
	audio_data->buffer_pos = 0;
	return 0;
}

int callback_codec_play_video(uint8_t *buf,uint32_t frames,void *param)
{
	return 0;
}



//计算缩放比
static double count_scaling(double src_w,double src_h,double dst_w,double dst_h,uint8_t re)
{
	double scale;
	double scale_x = dst_w / src_w;
	double scale_y = dst_h / src_h;
	// 选择较小的缩放比来保持图片的纵横比例
	if(re==0)
		scale = (scale_x < scale_y) ? scale_x : scale_y;
	else
		scale = (scale_x > scale_y) ? scale_x : scale_y;
	return scale;
}
static double count_scaling_smaller(double src_w,double src_h,double dst_w,double dst_h)
{
	return count_scaling(src_w,src_h,dst_w,dst_h,0);
}
static double count_scaling_larger(double src_w,double src_h,double dst_w,double dst_h)
{
	return count_scaling(src_w,src_h,dst_w,dst_h,1);
}

//相对于中点的坐标变换
static int count_coordinate_displsy(int value1,int value2,int min,int max)
{
	int val=((int)value1-(int)value2)/2;
	if(val<min)	val=min;
	if(val>max)	val=max;
	return val;
}
static int get_smaller_value(int value1,int value2)
{
	return (value1<value2 ? value1:value2);
}
//根据用户设置参数计算画面真实显示尺寸
int count_rect_size_from_user(struct VideoHardParam *display,struct MediaVideoParams *video_params,AVCodecContext *codec_ctx)
{
	struct MediaVideoParams *user_=video_params;
	struct MediaRect *rect_d=(struct MediaRect *)malloc(sizeof(struct MediaRect));
	struct MediaRect *rect_s=(struct MediaRect *)malloc(sizeof(struct MediaRect));
	//获取显示参数
	switch(user_->fill)
	{
		case MEDIA_VIDEO_SCALING_STRETCH:	//拉伸显示，图像可能变形，通过渲染窗口拉伸实现
			rect_d->x=0;
			rect_d->y=0;
			rect_d->w=user_->rect.w;
			rect_d->h=user_->rect.h;
			rect_s->x=0;
			rect_s->y=0;
			rect_s->w=codec_ctx->width;
			rect_s->h=codec_ctx->height;
			break;
		case MEDIA_VIDEO_SCALING_FILL:		//保持原始比例并填充显示，可能会裁剪，通过纹理大小实现
		{
			double scale=count_scaling_larger(codec_ctx->width,codec_ctx->height,user_->rect.w,user_->rect.h);
			rect_d->w=(int16_t)((double)codec_ctx->width*scale);
			rect_d->h=(int16_t)((double)codec_ctx->height*scale);
			rect_d->x=count_coordinate_displsy(user_->rect.w,rect_d->w,0,INT_MAX);
			rect_d->y=count_coordinate_displsy(user_->rect.h,rect_d->h,0,INT_MAX);
			rect_s->x=0;
			rect_s->y=0;
			rect_s->w=codec_ctx->width;
			rect_s->h=codec_ctx->height;
			break;
		}
		case MEDIA_VIDEO_SCALING_FIT:		//保持原始比例并适应屏幕，可能添加黑边,
		{
			double scale=count_scaling_smaller(codec_ctx->width,codec_ctx->height,user_->rect.w,user_->rect.h);
			rect_d->w=(int16_t)((double)codec_ctx->width*scale);
			rect_d->h=(int16_t)((double)codec_ctx->height*scale);
			rect_d->x=count_coordinate_displsy(user_->rect.w,rect_d->w,0,INT_MAX);
			rect_d->y=count_coordinate_displsy(user_->rect.h,rect_d->h,0,INT_MAX);
			rect_s->x=0;
			rect_s->y=0;
			rect_s->w=codec_ctx->width;
			rect_s->h=codec_ctx->height;
			break;
		}
		case MEDIA_VIDEO_SCALING_ZOOM:		//放大画面以填充屏幕，可能会裁剪边缘。
			
			break;
		case MEDIA_VIDEO_SCALING_CROP:		//裁剪画面（画面尺寸达不到则不用裁减）以填充屏幕
			rect_d->x=count_coordinate_displsy(user_->rect.w,codec_ctx->width,0,INT_MAX);
			rect_d->y=count_coordinate_displsy(user_->rect.h,codec_ctx->height,0,INT_MAX);
			rect_d->w=get_smaller_value(user_->rect.w,codec_ctx->width);
			rect_d->h=get_smaller_value(user_->rect.h,codec_ctx->height);
			rect_s->x=count_coordinate_displsy(codec_ctx->width,user_->rect.w,0,INT_MAX);
			rect_s->y=count_coordinate_displsy(codec_ctx->height,user_->rect.h,0,INT_MAX);
			rect_s->w=get_smaller_value(user_->rect.w,codec_ctx->width);
			rect_s->h=get_smaller_value(user_->rect.h,codec_ctx->height);
			break;
		case MEDIA_VIDEO_SCALING_LETTERBOX:	//保持原始比例，上下左右添加黑边

			break;
		default:

			break;
	}
	display->rect_dst=rect_d;
	display->rect_src=rect_s;
}

//播放解码文件
//display:硬件参数
//uaer:
//filename:
int video_play_codec_file(struct VideoHardParam *display,struct MediaParams *user,const char *filename)
{
	uint32_t format=SDL_PIXELFORMAT_RGB24;	//暂时使用固定RGB888格式（）
	if(!user->get_callback_video(user))		//用户没设置回调就启用本地显示
	{
		printf("=============启用本地显示===========\n");
		display->is_sdl=true;
	}

	struct MediaCodecParam codec_v,codec_a;
	if(Video_Get_File_Info(filename,&codec_v,&codec_a)<0)
	{
		fprintf(stderr,"Open file(URL) error\n");
		return -1;
	}
	double duration=codec_v.format_ctx->duration / (double)AV_TIME_BASE;
	Audio_Set_Length(user,duration);
		
	struct MediaVideoParams video_params;
	get_display_params_user_codec(user,codec_v.codec_ctx,&video_params);
	count_rect_size_from_user(display,&video_params,codec_v.codec_ctx);
//	if(sdl_display_init(display, format,0, 0, codec_v.codec_ctx->width, codec_v.codec_ctx->height )<0)
	if(display->is_sdl)
	{
		//format=(uint32_t)get_sdl_pixel_format(codec_v.codec_ctx->pix_fmt);
		printf("codec_v.codec_ctx->pix_fmt:%d\n",codec_v.codec_ctx->pix_fmt);

		//视频播放的SDL初始化
		if(sdl_display_init(display, &codec_v,format,video_params.rect.x, video_params.rect.y, video_params.rect.w, video_params.rect.h )<0)
		{
			fprintf(stderr,"init sdl error\n");
			Video_Free_File(&codec_v,&codec_a);
			return -1;
		}
	}
	else
	{
		display->format=format;
	}
	//音频硬件初始化设置
	if(alsa_hard_init(display->audio_card,display,&codec_a,user)<0)
	{
		fprintf(stderr,"[Error]:Init audio error,The video will play silently\n");
		codec_a.codec_ctx=NULL;
		codec_a.hard_param=NULL;
		codec_a.audio_stream=NULL;
		codec_a.callback_param=NULL;
		codec_a.callback_play=NULL;
		Video_File_Codec(display,&codec_v,NULL,user);		//解码并播放
	}
	else
		Video_File_Codec(display,&codec_v,&codec_a,user);		//解码并播放

	if(display->is_sdl)	sdl_display_deinit(display);
	if(alsa_hard_deinit(display,&codec_a)<0)
	{
		fprintf(stderr,"alsa hard deinit error\n");
	}
	Video_Free_File(&codec_v,&codec_a);
	return 0;
}

//显示YUV
int video_display_yuv()
{
	return 0;
}

//显示RGB
int viodeo_display_rgb()
{
	return 0;
}

//设置视频画面的填充方式
int Video_Set_Fill_Mode(struct MediaParams *conf,VideoScalingType mode)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->video->fill,mode);
	return 0;
}

//显示位置设置
int Video_Set_Coordinates(struct MediaParams *conf,int16_t x,int16_t y)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->video->rect.x,x);
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->video->rect.y,y);
	return 0;
}

//显示位置获取
int Video_Get_Coordinates(struct MediaParams *conf,int16_t *x,int16_t *y)
{
	if(!conf)
		return -1;
	THREAD_READ_USERCONF(conf->rw_mut,conf->video->rect.x,*x);
	THREAD_READ_USERCONF(conf->rw_mut,conf->video->rect.y,*y);
	return 0;
}

//获取位置(视频使用位置)
int Video_Get_Position(struct MediaParams *conf,PIAudioConf *pcm_play)
{
	return (int32_t)Audio_Get_DPosition(conf,pcm_play);
}

//显示宽高
int Video_Get_Width_Height(struct MediaParams *conf,uint16_t *width,uint16_t *height)
{
	if(!conf)
		return -1;
	THREAD_READ_USERCONF(conf->rw_mut,conf->video->rect.w,*width);
	THREAD_READ_USERCONF(conf->rw_mut,conf->video->rect.h,*height);
	return 0;
}

//设置宽高
int Video_Set_Width_Height(struct MediaParams *conf,uint16_t width,uint16_t height)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->video->rect.w,width);
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->video->rect.h,height);
	return 0;
}

//获取亮度
int Video_Get_Light(struct MediaParams *conf)
{
	if(!conf)
		return -1;
	int light;
	THREAD_READ_USERCONF(conf->rw_mut,conf->video->light,light);
	return light;
}

//设置亮度
int Video_Set_Light(struct MediaParams *conf,uint16_t light)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->video->light,light);
	return 0;
}

//获取所有显示参数
int video_params_get_all(struct MediaParams *user,struct MediaVideoParams *video_params)
{
	//使用memcpy有问题，原因未知
	pthread_rwlock_rdlock(&user->rw_mut);
	video_params->rect.w=user->video->rect.w;
	video_params->rect.h=user->video->rect.h;
	video_params->rect.x=user->video->rect.x;
	video_params->rect.y=user->video->rect.y;
	video_params->fill=user->video->fill;
//	memcpy(&video_params,user->video,sizeof(struct MediaVideoParams));
	pthread_rwlock_unlock(&user->rw_mut);
	return 0;
}

//根据用户设置和解码器获取的视频格式调整显示参数
int get_display_params_user_codec(struct MediaParams *user,AVCodecContext *codec_ctx,struct MediaVideoParams *video_params)
{
	if(!video_params)
		return -1;
	video_params_get_all(user,video_params);
	if(video_params->rect.w==0 || video_params->rect.h==0)		//宽高不符合则使用视频默认参数
	{
		video_params->rect.w=codec_ctx->width;
		video_params->rect.h=codec_ctx->height;
	}
	return 0;
}

int Video_Play_Open()
{
	return 0;
}

int Video_Play_Main(struct MediaParams *user,const char *audio_card)
{
	struct MediaFileList *list=user->list;
	struct VideoHardParam display;
	video_hard_param_init(&display,audio_card);
	while(1)
	{
		char *name;
		int cmd=user->command_get(user);
		switch(cmd)
		{
			case AUDIO_PLCMD_NEXT:
				//media_pcm_drop(pcm_play);
				name=list->read_saft(list);
				Audio_Set_Command(user,AUDIO_PLCMD_NONE);
				break;
			case AUDIO_PLCMD_LAST:
				//media_pcm_drop(pcm_play);
				name=list->read_last_saft(list);
				Audio_Set_Command(user,AUDIO_PLCMD_NONE);
				break;
			case AUDIO_PLCMD_STOP:
				if(Audio_Get_State(user)!=AUDIO_STATE_START)
					user->cond->wait(user->cond);		//等待开始信号
				Audio_Set_Command(user,AUDIO_PLCMD_NONE);
				break;
			case AUDIO_PLCMD_EXIT:
				Audio_Set_State(user,AUDIO_STATE_EXIT);
				return 0;
				break;
			default: 
				name=list->read_saft(list);
				break;
		}
		Audio_Set_State(user,AUDIO_STATE_PLAYING);
		if(name==NULL)
		{
			usleep(5000);
			continue;
		}
		Audio_Set_Is_Playing(user,true);
		debug_printf("play file %s\n",name);
		video_play_codec_file(&display,user,name);
		Audio_Set_Is_Playing(user,false);
	}
	video_hard_param_deinit(&display);

	printf("播放结束\n");
	return 0;
}

//临时测试使用的文件播放
int Video_Play_File(struct VideoHardParam *display, struct MediaCodecParam *codec_v,struct MediaCodecParam *codec_a)
{
	struct MediaParams *conf=media_user_config_creat();
	uint32_t format=(uint32_t)get_sdl_pixel_format(codec_v->codec_ctx->pix_fmt);
	if(sdl_display_init(display, codec_v,format,VIDEO_WINDOWPOS_UNDEFINED, VIDEO_WINDOWPOS_UNDEFINED, codec_v->codec_ctx->width, codec_v->codec_ctx->height )<0)
		debug_printf("init sdl  error\n");
	//if(sdl_audio_init(display,codec_a)<0)
	//	debug_printf("init sdl audio error\n");
	if(alsa_hard_init(NULL,display,codec_a,conf)<0)
		debug_printf("init sdl audio error\n");
	struct MediaParams *user=media_user_config_creat();
	Video_File_Codec(display,codec_v,codec_a,user);
	sdl_display_deinit(display);
	if(alsa_hard_deinit(display,codec_a)<0)
		debug_printf("init sdl audio error\n");
	media_user_config_free(conf);
	return 0;
}

#ifdef __cplusplus
}
#endif
