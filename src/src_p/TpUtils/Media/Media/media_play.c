//临时文件(重构使用，后续移走)
#include "tools/variable_array.h"
#include "Media/media_codec.h"
#include "Media/media_play.h"
#include "Media/media_file_list.h"
#include "Video/video_play.h"
#include "Video/video_display.h"
#include "Audio/audio_play.h"
#include "Audio/audio_play.h"
#include "TpMediaDevice.h"

#ifdef DEBUG_MEDIA_PLAY
    #define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define debug_printf(fmt, ...)  // 如果不定义DEBUG，什么也不做
#endif


//音频播放回调函数用户参数
struct codePlayCallbackParam{
	struct MediaAudioHandle *pcm;
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


//SDL初始化(显示)
//当设置的format无法成功生效时会自动修改可以成功设置的format
static int sdl_display_init(struct MediaVideoHandle *display,uint32_t *format,int x, int y, int w, int h)
{
#ifdef MEDIA_SDL_ENABLE	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	//创建SDL窗口
	display->window = SDL_CreateWindow("TinyPiX Video", x, y, w, h, SDL_WINDOW_HIDDEN);//隐藏：SDL_WINDOW_HIDDEN,显示：SDL_WINDOW_SHOWN
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
	display->texture=NULL;
	/*display->texture = sdl_creat_texture_near(display->renderer, format,w,h);		//codec_v->codec_ctx->width,codec_v->codec_ctx->height SDL_PIXELFORMAT_RGB24
	if(display->texture==NULL)
	{
		fprintf(stderr, "Creat Texture! SDL_Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        SDL_Quit();
	}*/

	debug_printf("debug:sdl init ok, display on(%d,%d %d*%d)\n",x,y,w,h);
#endif
	return 0;
}

static int sdl_display_deinit(struct MediaVideoHandle *display)
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


static int pcm_start_play(struct MediaAudioHandle *pcm)
{
	if(!pcm || !pcm->handle)
		return -1;
	int rc=0;
	//准备播放
	if ((rc = snd_pcm_prepare(pcm->handle)) < 0) {		//在第一次设置时可以不需要准备播放，播放后重新设置需要准备播放
		perror("无法准备播放:");
		media_pcm_close(pcm);
		return -1;
	}
	snd_pcm_start(pcm->handle);
//	media_pcm_drop(pcm);
	debug_printf("PCM handle name = '%s'\n", snd_pcm_name(pcm->handle));
	return 0;
}

/// @brief 暂停播放
/// @param pcm 
/// @return 
static int pcm_play_stop(struct MediaAudioHandle *pcm)
{
	if(!pcm || !pcm->handle)
		return -1;
	int err;

	if (pcm->ahparams->can_pause) {
		if ((err = snd_pcm_pause(pcm->handle, 1)) < 0) {
//		    mp_msg(MSGT_AO,MSGL_ERR,MSGTR_AO_ALSA_PcmPauseError, snd_strerror(err));
        	return -1;
		}
	} 
	else {
		if ((err = media_pcm_drop(pcm)) < 0){
//			mp_msg(MSGT_AO,MSGL_ERR,MSGTR_AO_ALSA_PcmDropError, snd_strerror(err));
			return -1;
		}
	}
	return 0;
}


/// @brief audioStreamParams结构体初始化
/// @param wChannels 通道
/// @param nSamplesPersec 采样频率
/// @param wBitsPerSample 数据位数
/// @return 
static int audio_stream_params_init(int wChannels,int nSamplesPersec,int wBitsPerSample,struct AudioStreamParams *header)
{
	header->wChannels=wChannels;    //声道数
	header->nSamplesPersec=nSamplesPersec;          //采样频率
	header->wBitsPerSample=wBitsPerSample;   		// 样本数据位数  
	header->byteFrams=wChannels*wBitsPerSample/8;
	header->nAvgBitsPerSample=nSamplesPersec*header->byteFrams;             //每秒播放字节数
	return 0;
}

//声卡硬件初始化(使用解码器的参数自动设置)
static int media_audio_hard_auto_init(struct MediaAudioHandle *pcm_play,struct MediaParams *user,struct MediaStreamParams *audio)
{
	debug_printf("初始化声卡硬件\n");
	struct AudioStreamParams *stream_params=(struct AudioStreamParams *)malloc(sizeof(struct AudioStreamParams));
	audio_stream_params_init(audio->codec_ctx->channels,			//使用流的参数来初始化声卡
								audio->codec_ctx->sample_rate,
								AUDIO_CODEC_CHANNEL_DEF,		//使用16位宽，(本值是解码时候自己指定的，不需要动态设置)
								stream_params);
	if(!pcm_play || !pcm_play->handle)
	{
		user->audio_params->callback_audio=NULL;
		user->audio_params->userdata=NULL;

		audio->audio.handle=pcm_play;
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
	printf("[Debug]: set codePlayCallbackParam,%d,%d,%d\n",
		pcm_play->adparams->wBitsPerSample,
		pcm_play->adparams->wChannels,
		pcm_play->adparams->byteFrams);
	cb_param->conf=user;
	cb_param->audio_param=stream_params;
	cb_param->delay=100;
	cb_param->pcm=pcm_play;

	printf("[Debug]: set codePlayCallbackParam callback_audio\n");
	user->audio_params->callback_audio=callback_codec_play;
	user->audio_params->userdata=cb_param;
	audio->audio.handle=pcm_play;
	audio->audio.handle->adparams=stream_params;
	printf("[Debug]: set codePlayCallbackParam callback_audio\n");
	return 0;
}

static int media_audio_hard_deinit(struct MediaStreamParams *audio)
{
	if(!audio)
		return 0;

    return 0;
}

//声卡初始化
static int alsa_hard_init(const char *name,struct MediaStreamParams *audio,struct MediaParams *user)
{
	AVCodecContext *codec_ctx=audio->codec_ctx;
	struct MediaAudioHandle *pcm_play=Audio_Play_Open(name);
	if(pcm_play==NULL){
		fprintf(stderr, "Audio pcm open error\n");
		return -1;
	}

	audio->audio.handle=pcm_play;
	if(media_audio_hard_auto_init(pcm_play,user,audio)<0)		//声卡初始化
		return -1;
	debug_printf("[Debug]: media_audio_hard_auto_init ok\n");
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
	user->nAvgBitsPerSample=pcm_play->adparams->nAvgBitsPerSample;	//此处先设置每秒播放字节数，如果存在视频流，会对其清空
	//display->audio_data=audioData;
	return 0;	
}

//声卡取消初始化
static int alsa_hard_deinit(struct MediaStreamParams *audio)
{
	if(!audio)
		return 0;
	printf("[Debug]: alsa_hard_deinit\n");
	if(!audio->codec_ctx)
		avcodec_free_context(&audio->codec_ctx);
	if(!audio->format_ctx)
		avformat_close_input(&audio->format_ctx);
	if(!audio->audio.swr_ctx)
		swr_free(&audio->audio.swr_ctx);
	media_audio_hard_deinit(audio);		//取消硬件的设置
	printf("[Debug]: Audio_Device_Close\n");
	Audio_Device_Close(audio->audio.handle);			//关闭设备
	return 0;
}


//根据用户设置参数计算画面真实显示尺寸
//user_params：用户设置参数
//rect_s:原始图像中的提取矩形
//rect_d:显示的位置需要的矩形
/*int count_rect_size_from_user(struct VideoStreamParams *user_params,AVCodecContext *codec_ctx,struct MediaRect *rect_s,struct MediaRect *rect_d)
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
			rect_d->w=user_->rect.width();
			rect_d->h=user_->rect.height();
			rect_s->x=0;
			rect_s->y=0;
			rect_s->w=codec_ctx->width;
			rect_s->h=codec_ctx->height;
			break;
		case MEDIA_VIDEO_SCALING_FILL:		//保持原始比例并填充显示，可能会裁剪，通过纹理大小实现
		{
			double scale=count_scaling_larger(codec_ctx->width,codec_ctx->height,user_->rect.width(),user_->rect.height());
			rect_d->w=(int16_t)((double)codec_ctx->width*scale);
			rect_d->h=(int16_t)((double)codec_ctx->height*scale);
			rect_d->x=count_coordinate_displsy(user_->rect.width(),rect_d->w,0,INT_MAX);
			rect_d->y=count_coordinate_displsy(user_->rect.height(),rect_d->h,0,INT_MAX);
			rect_s->x=0;
			rect_s->y=0;
			rect_s->w=codec_ctx->width;
			rect_s->h=codec_ctx->height;
			break;
		}
		case MEDIA_VIDEO_SCALING_FIT:		//保持原始比例并适应屏幕，可能添加黑边,
		{
			double scale=count_scaling_smaller(codec_ctx->width,codec_ctx->height,user_->rect.width(),user_->rect.height());
			rect_d->w=(int16_t)((double)codec_ctx->width*scale);
			rect_d->h=(int16_t)((double)codec_ctx->height*scale);
			rect_d->x=count_coordinate_displsy(user_->rect.width(),rect_d->w,0,INT_MAX);
			rect_d->y=count_coordinate_displsy(user_->rect.height(),rect_d->h,0,INT_MAX);
			rect_s->x=0;
			rect_s->y=0;
			rect_s->w=codec_ctx->width;
			rect_s->h=codec_ctx->height;
			break;
		}
		case MEDIA_VIDEO_SCALING_ZOOM:		//放大画面以填充屏幕，可能会裁剪边缘。
			
			break;
		case MEDIA_VIDEO_SCALING_CROP:		//裁剪画面（画面尺寸达不到则不用裁减）以填充屏幕
			rect_d->x=count_coordinate_displsy(user_->rect.width(),codec_ctx->width,0,INT_MAX);
			rect_d->y=count_coordinate_displsy(user_->rect.height(),codec_ctx->height,0,INT_MAX);
			rect_d->w=get_smaller_value(user_->rect.width(),codec_ctx->width);
			rect_d->h=get_smaller_value(user_->rect.height(),codec_ctx->height);
			rect_s->x=count_coordinate_displsy(codec_ctx->width,user_->rect.width(),0,INT_MAX);
			rect_s->y=count_coordinate_displsy(codec_ctx->height,user_->rect.height(),0,INT_MAX);
			rect_s->w=get_smaller_value(user_->rect.width(),codec_ctx->width);
			rect_s->h=get_smaller_value(user_->rect.height(),codec_ctx->height);
			break;
		case MEDIA_VIDEO_SCALING_LETTERBOX:	//保持原始比例，上下左右添加黑边

			break;
		default:

			break;
	}
}*/

/*static int get_display_params_user_codec(struct MediaParams *user,AVCodecContext *codec_ctx,struct VideoStreamParams *video_params)
{
	if(!video_params)
		return -1;
	video_params_get_all(user,video_params);
	if(video_params->rect.width()==0 || video_params->rect.height()==0)		//宽高不符合则使用视频默认参数
	{
		video_params->rect.width()=codec_ctx->width;
		video_params->rect.height()=codec_ctx->height;
	}
	return 0;
}*/

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

	if(!user->video_params->get_callback_video(user->video_params))		//用户没设置回调就启用本地显示
	{
		printf("=============启用本地显示===========\n");
		handle->is_sdl=true;
	}

	struct VideoStreamParams user_params;	//用户设置的显示位置和宽高以及亮度填充
	get_display_params_user_codec(user,stream->codec_ctx,&user_params);

/*	struct VideoStreamParams *params_d=(struct VideoStreamParams *)malloc(sizeof(struct VideoStreamParams));
	if(!params_d)
	{
		goto ERROR_RETURN;
	}
	struct VideoStreamParams *params_s=(struct VideoStreamParams *)malloc(sizeof(struct VideoStreamParams));
	if(!params_d)
	{
		goto ERROR_RETURN;
	}
*/
#ifdef MEDIA_SDL_ENABLE
	if(handle->is_sdl)
	{
		uint32_t sdl_format=(uint32_t)get_sdl_pixel_format(user->format_video);	//此处的sdl_format已无实际意义，真正格式会在codec中使用，此处为了兼容旧版程序
		//视频播放的SDL初始化
		if(sdl_display_init(handle,&sdl_format,0,0,0,0 )<0)		//只初始化，窗口不显示
		{
			fprintf(stderr,"init sdl error\n");
			goto ERROR_RETURN;
		}
		stream->video.format=get_format_pixel_sdl(sdl_format);	//根据新的sdlformat获取av_format
	}
	else	
		;
#endif
	stream->video.handle=handle;
	stream->video.format=user->video_params->format_video;

	return 0;

ERROR_RETURN:
/*	if(params_s)
		free(params_s);
	if(params_d)
		free(params_d);*/
	free(handle);
	return -1;
}

static int media_stream_audio_init_handle(struct MediaStreamParams *stream,struct MediaParams *user)
{
	if(alsa_hard_init(user->audio_params->device,stream,user)<0)
	{
		fprintf(stderr,"[Error]:Init audio error,The video will play silently\n");
		return -1;
	}
	return 0;
}

//根据每个流的参数信息来初始化对应的硬件
static int media_stream_all_init_handle(struct MediaPlayerHandle *player, struct MediaParams *user)
{
	MediaStreamArray *array=player->stream_array;
	int size_array=array->get_size(array);
	MediaType sync_clk_type = AVMEDIA_TYPE_UNKNOWN;

	for(int i=0 ;i<size_array;i++)
	{
		struct MediaStreamParams *stream = array->get(array,i);

		switch(stream->type)
		{
			case AVMEDIA_TYPE_VIDEO:   		// 视频流
				if(player->sync_clk_stream_index<0 || sync_clk_type != AVMEDIA_TYPE_VIDEO)
				{
					sync_clk_type=stream->type;
					player->sync_clk_stream_index=stream->stream_index;
					player->sync_clk_array_index=i;
				}
				media_stream_video_init_handle(stream,user);
				break;
        	case AVMEDIA_TYPE_AUDIO:   		// 音频流
				if(player->sync_clk_stream_index<0)
				{
					sync_clk_type=stream->type;
					player->sync_clk_stream_index=stream->stream_index;
					player->sync_clk_array_index=i;
				}
				media_stream_audio_init_handle(stream,user);
				break;
			case AVMEDIA_TYPE_SUBTITLE: 	// 字幕
				break;
			default:
				break;
		}
	}
	if(sync_clk_type==AVMEDIA_TYPE_VIDEO)	//
	{
		user->nAvgBitsPerSample=0;
	}
}


static int media_stream_video_deinit_handle(struct MediaStreamParams *stream)
{
	if(!stream->video.handle)
		return -1;
	if(stream->video.handle->is_sdl)
		sdl_display_deinit(stream->video.handle);
	if(stream->video.handle)
		free(stream->video.handle);
	stream->video.handle=NULL;
	return 0;
}

static int media_stream_audio_deinit_handle(struct MediaStreamParams *stream)
{
	if(!stream->audio.handle)
		return -1;
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

	MediaFormatContext *mediaFormat=Media_Get_File_All_Info(filename,player->stream_array);
	if(!mediaFormat)
	{
		fprintf(stderr, "Open file/URL(%s) error\n",filename);
        return -1;
	}

	struct MediaStreamParams *stream0=player->stream_array->get(player->stream_array,0);
	player->format_ctx = stream0->format_ctx;
	
	//设置时长
	Audio_Set_Length(user,media_get_url_duration_sec(mediaFormat));
		
	media_stream_all_init_handle(player,user);		//初始化对应硬件

	Mediao_File_Codec_Play(player,user);
	media_stream_all_deinit_handle(player->stream_array);
	Media_Free_File(player->stream_array);
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
				Media_Set_Command(user,AUDIO_PLCMD_NONE);
				break;
			case AUDIO_PLCMD_LAST:
				//media_pcm_drop(pcm_play);
				name=list->read_last_saft(list);
				Media_Set_Command(user,AUDIO_PLCMD_NONE);
				break;
			case AUDIO_PLCMD_STOP:
				if(Audio_Get_State(user)!=AUDIO_STATE_START)
					user->cond->wait(user->cond);		//等待开始信号
				Media_Set_Command(user,AUDIO_PLCMD_NONE);
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
