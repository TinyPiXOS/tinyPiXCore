//临时文件(重构使用，后续移走)
#include "utils/variable_array.h"
#include "media_codec.h"
#include "audio_play.h"
#include "media_play_temp.h"
#include "video_play.h"
#include "media_file_list.h"

//音频播放回调函数用户参数
struct codePlayCallbackParam{
	PIAudioConf *pcm;
	struct MediaParams *conf;
	struct AudioStreamParams *audio_param;
	int delay;
};


/// @brief 音频播放回调，如果要自行设置offset，offset传入-1；
/// @param buff 缓存区
/// @param frames 侦数
/// @param offset 截至当前为止写入的字节数量(会根据字节数量和硬件配置自动设置进度)
/// @return 
static int callback_codec_play(uint8_t *buff,uint32_t frames,int offset,void *param)
{
	struct codePlayCallbackParam *p=(struct codePlayCallbackParam *)param;
	return audio_stream_write(p->pcm, p->conf, buff, frames, -1, offset, p->delay);
}

/// @brief 写入硬件的音频流接口写入硬件的音频流接口
/// @param pcm_play 
/// @param conf 
/// @param audio_param 
/// @param buffer 缓存区
/// @param frames 原始的帧数
/// @param volume 音量，设置为-1的时候自己从conf中读取
/// @param offset 位置偏移，设置为-1的时候需要上层自己写入位置，设置为正值的时候自动写入(当前只作为标志位使用)
/// @param delay 阻塞时长，
/// @return 
int audio_stream_write(PIAudioConf *pcm_play,struct MediaParams *conf,
							uint8_t *buffer,uint32_t frames,
							float volume,
							int offset,int delay)
{

	int ret=0;
	float volume_set;
	struct AudioStreamParams *audio_param=pcm_play->adparams;
	static struct MediaFilterParam *filter=NULL;
	AVFrame *frame_flt=NULL;
	int64_t position=Audio_Get_BytePosition(conf);		//获取当前播放位置
	uint8_t *data=((AVFrame *)buffer)->data[0];		//输出的数据
	uint32_t data_frames=frames;
	static float speed_l=0.0;
	static uint16_t wChannels_l=0,wBitsPerSample_l=0;
	static uint32_t nSamplesPersec_l=0;

	float speed=Audio_Get_Speed(conf);
	if((speed_l!=speed || 
			audio_param->wChannels!=wChannels_l || 
			audio_param->wBitsPerSample!=wBitsPerSample_l ||
			audio_param->nSamplesPersec!=nSamplesPersec_l) 
			&& speed!=1)		//位置为0认为是新的一首开始播放，或者两次速度不相等，都需要重新设置过滤器
	{
		
		if(filter)
			audio_filter_delete(filter);
		filter=audio_filter_creat_init(speed,audio_param->nSamplesPersec,audio_param->wChannels,code_get_channel_layout(audio_param->wChannels),code_get_format(audio_param->wBitsPerSample));
		if(!filter)
		{
			ret = -1;
			goto WRITE_POS;
		}
		conf->filter=filter;
		speed_l=speed;
		wChannels_l=audio_param->wChannels;
		wBitsPerSample_l=audio_param->wBitsPerSample;
		nSamplesPersec_l=audio_param->nSamplesPersec;
	}
	if(speed!=1)
	{
		AVFrame *convert_frame=(AVFrame *)buffer;
		frame_flt = av_frame_alloc();		//提前申请
		
		if(!frame_flt)
		{
			ret =-1;
			goto WRITE_POS;
		}
		media_filte_get_data(filter,convert_frame,frame_flt);
		data=frame_flt->data[0];
		data_frames=frame_flt->nb_samples;
	}

	if(volume<0)
	{
		volume_set=Audio_Get_Volume(conf);
		volume_set*=0.01;
	}
	else
		volume_set=volume;
	
	pcm_data_adjust_volume(data,data_frames,audio_param->wChannels,volume_set,audio_param->wBitsPerSample);
	ret= pcm_write_data(pcm_play,data,data_frames,delay);

FREE_FLT:
	if(frame_flt)
	{
		av_frame_free(&frame_flt);
	}
WRITE_POS:
//	printf("offset:%d,audio_param->nAvgBitsPerSample:%d,进度：%d\n",offset,audio_param->nAvgBitsPerSample,position);
	if(offset >= 0 && audio_param->nAvgBitsPerSample!= 0)		//如果要自行设置offset直接传入-1即可
	{
		position += (frames*audio_param->byteFrams);
		Audio_Set_BytePosition(conf,(int64_t)position);
	}
	
	return ret;
}


//SDL初始化(显示)
static int sdl_display_init(struct MediaVideoHandle *display,uint32_t format,int x, int y, int w, int h)
{
#ifdef MEDIA_SDL_ENABLE	
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
	display->texture = sdl_creat_texture_near(display->renderer, &format,w,h);
	if(display->texture==NULL)
	{
		fprintf(stderr, "Creat Texture! SDL_Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        SDL_Quit();
	}
	
	debug_printf("debug:sdl init ok, display on(%d,%d %d*%d)\n",x,y,w,h);
#endif
	return 0;
}

static int sdl_display_deinit(struct VideoHardParam *display)
{
#ifdef MEDIA_SDL_ENABLE
	if(display->texture);
		SDL_DestroyTexture(display->texture);
	if(display->renderer)
		SDL_DestroyRenderer(display->renderer);
	if(display->window)
		SDL_DestroyWindow(display->window);
	SDL_Quit();
#endif
	return 0;
}


//声卡硬件初始化(使用解码器的参数自动设置)
static int Audio_Hard_Auto_Init(PIAudioConf *pcm_play,struct MediaParams *conf,struct MediaStreamParams *audio)
{
	debug_printf("初始化声卡硬件\n");
	struct AudioStreamParams *stream_params=(struct AudioStreamParams *)malloc(sizeof(struct AudioStreamParams));
	audioStreamParams_init(audio->codec_ctx->channels,			//使用流的参数来初始化声卡
								audio->codec_ctx->sample_rate,
								AUDIO_CODEC_CHANNEL_DEF,		//使用16位宽，(本值是解码时候自己指定的，不需要动态设置)
								stream_params);
	if(!pcm_play || !pcm_play->handle)
	{
		audio->callback_play_audio=NULL;
		audio->callback_param=NULL;
		audio->audio.handle->adparams=stream_params;
		return -1;
	}
	if(pcm_hwparams_set(pcm_play,stream_params)<0)
		return -1;
	if(pcm_start_play(pcm_play)<0)
		return -1;
	struct codePlayCallbackParam *cb_param=(struct codePlayCallbackParam *)malloc(sizeof(struct codePlayCallbackParam));
	if(cb_param==NULL)
		return -1;
	cb_param->conf=conf;
	cb_param->audio_param=stream_params;
	cb_param->delay=100;
	cb_param->pcm=pcm_play;
	audio->callback_play_audio=callback_codec_play;
	audio->callback_param=cb_param;
	audio->audio.handle->adparams=stream_params;
	return 0;
}


//声卡初始化
static int alsa_hard_init(const char *name,struct MediaStreamParams *audio,struct MediaParams *user)
{
	AVCodecContext *codec_ctx=audio->codec_ctx;
	PIAudioConf *pcm_play=Audio_Play_Open(name);
	if(pcm_play==NULL){
		fprintf(stderr, "Audio pcm open error\n");
		return -1;
	}
	if(Audio_Hard_Auto_Init(pcm_play,user,audio)<0)		//声卡初始化
		return -1;
	struct SwrContext *swrContext=swr_set_with_hard_param(codec_ctx,pcm_play->adparams);
    if (!swrContext || swr_init(swrContext) < 0) {
		perror("Could not initialize resampler");
		fprintf(stderr, "Could not initialize resampler\n");
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&audio->format_ctx);
        return -1;
    }

	debug_printf("Resampling:%d\n",codec_ctx->sample_rate);
	debug_printf("init sdl audio ok\n");
	audio->audio.swr_ctx=swrContext;
	audio->audio.handle=pcm_play;

	//display->audio_data=audioData;
	return 0;	
}

//声卡取消初始化
static int alsa_hard_deinit(struct MediaStreamParams *audio)
{
	if(!audio)
		return 0;
	
	if(!audio->codec_ctx)
		avcodec_free_context(&audio->codec_ctx);
	if(!audio->format_ctx)
		avformat_close_input(&audio->format_ctx);
	if(!audio->audio.swr_ctx)
		swr_free(&audio->audio.swr_ctx);
	Audio_Hard_Deinit(audio);		//取消硬件的设置
	Audio_Device_Close(audio->audio.handle);			//关闭设备
	return 0;
}


//根据用户设置参数计算画面真实显示尺寸
//user_params：用户设置参数
//rect_s:原始图像中的提取矩形
//rect_d:显示的位置需要的矩形
int count_rect_size_from_user(struct VideoStreamParams *user_params,AVCodecContext *codec_ctx,struct MediaRect *rect_s,struct MediaRect *rect_d)
{
	struct VideoStreamParams *user_=user_params;
//	struct MediaRect *rect_d=(struct MediaRect *)malloc(sizeof(struct MediaRect));
//	struct MediaRect *rect_s=(struct MediaRect *)malloc(sizeof(struct MediaRect));
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
}

static int get_display_params_user_codec(struct MediaParams *user,AVCodecContext *codec_ctx,struct VideoStreamParams *video_params)
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

//使用流信息获取文件时长
static double media_get_stream_duration()
{
	
}



//视频流的硬件初始化获取视频类流的
static int media_stream_video_init_handle(struct MediaStreamParams *stream,struct MediaParams *user)
{
	struct MediaVideoHandle *handle=(struct MediaVideoHandle *)malloc(sizeof(struct MediaVideoHandle));
	if(!handle)
		return -1;
	
	if(!user->get_callback_video(user))		//用户没设置回调就启用本地显示
	{
		printf("=============启用本地显示===========\n");
		handle->is_sdl=true;
	}

	struct VideoStreamParams user_params;	//用户设置的显示位置和宽高以及亮度填充
	get_display_params_user_codec(user,stream->codec_ctx,&user_params);

	struct VideoStreamParams *params_d=(struct VideoStreamParams *)malloc(sizeof(struct VideoStreamParams));
	if(!params_d)
	{
		goto ERROR_RETURN;
	}
	struct VideoStreamParams *params_s=(struct VideoStreamParams *)malloc(sizeof(struct VideoStreamParams));
	if(!params_d)
	{
		goto ERROR_RETURN;
	}
	count_rect_size_from_user(&user_params,stream->codec_ctx,&params_s->rect,&params_d->rect);		//根据用户设置来调整需要提取的位置和输出位置
	uint32_t sdl_format;
	if(handle->is_sdl)
	{
		sdl_format=(uint32_t)get_sdl_pixel_format(user->format_video);

		//视频播放的SDL初始化
		if(sdl_display_init(handle,sdl_format,user_params.rect.x, user_params.rect.y, user_params.rect.w, user_params.rect.h )<0)
		{
			fprintf(stderr,"init sdl error\n");
			goto ERROR_RETURN;
		}
	}
	else	
		sdl_format=user->format_video;

	stream->video.format=sdl_format;
	stream->video.params_d=params_d;
	stream->video.params_s=params_s;
	stream->video.handle=handle;
	return 0;

ERROR_RETURN:
	if(params_s)
		free(params_s);
	if(params_d)
		free(params_d);
	if(handle)
		free(handle);

	return -1;
}

static int media_stream_audio_init_handle(struct MediaStreamParams *stream,struct MediaParams *user)
{
	if(alsa_hard_init(stream->audio.handle->device,stream,user)<0)
	{
		fprintf(stderr,"[Error]:Init audio error,The video will play silently\n");
		return -1;
	}
	return 0;
}

//根据每个流的参数信息来初始化对应的硬件
static int media_stream_all_init_handle(MediaStreamArray *array, struct MediaParams *user)
{
	int size_array=array->get_size(array);
	for(int i=0 ;i<size_array;i++)
	{
		struct MediaStreamParams *stream =array->get(array,i);
		switch(stream->type)
		{
			case AVMEDIA_TYPE_VIDEO:   		// 视频流
				media_stream_video_init_handle(stream,user);
				break;
        	case AVMEDIA_TYPE_AUDIO:   		// 音频流
				media_stream_audio_init_handle(stream,user);
				break;
			case AVMEDIA_TYPE_SUBTITLE: 	// 字幕
				break;
			default:
				break;
		}
	}
}



static int media_stream_video_deinit_handle(struct MediaStreamParams *stream)
{
	if(stream->video.handle->is_sdl)
		sdl_display_deinit(stream->video.handle);
}

static int media_stream_audio_deinit_handle(struct MediaStreamParams *stream)
{
	if(alsa_hard_deinit(stream)<0)
	{
		fprintf(stderr,"alsa hard deinit error\n");
	}
	return 0;
}

static int media_stream_all_deinit_handle(MediaStreamArray *array)
{
	int size_array=array->get_size(array);
	for(int i=0 ;i<size_array;i++)
	{
		struct MediaStreamParams *stream =array->get(array,i);
		switch(stream->type)
		{
			case AVMEDIA_TYPE_VIDEO:   		// 视频流
				media_stream_video_deinit_handle(stream);
				break;
        	case AVMEDIA_TYPE_AUDIO:   		// 音频流
				media_stream_audio_deinit_handle(stream);
				break;
			case AVMEDIA_TYPE_SUBTITLE: 	// 字幕
				break;
			default:

				break;
		}
	}
}





//播放解码文件
//display:硬件参数
//uaer:
//filename:
int media_player_codec_file(struct MediaParams *user,const char *filename)
{
	struct MediaPlayerHandle *player=media_player_handle_creat();
	if(!player)
		return -1;

	//获取文件信息和每个流解码器
	MediaStreamArray *array=creat_variable_array(sizeof(struct MediaStreamParams),2);
	if(!array)
		return -1;
	MediaFormatContext *mediaFormat=Media_Get_File_Info(filename,array);

	//设置时长
	Audio_Set_Length(user,media_get_url_duration_sec(mediaFormat));
		
	
	player->stream_array=array;
	media_stream_all_init_handle(array,user);		//初始化对应硬件



	media_stream_all_deinit_handle(array);

	Media_Free_File_Info(array);
	media_player_handle_delete(player);
	return 0;
}



int Media_Play_Main(struct MediaParams *user)
{
	struct MediaFileList *list=user->list;

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
		media_player_codec_file(user,name);
		Audio_Set_Is_Playing(user,false);
	}

	printf("播放结束\n");
	return 0;
}
