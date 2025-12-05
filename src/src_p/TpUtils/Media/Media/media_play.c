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



//根据每个流的参数信息来初始化对应的硬件
static int media_stream_all_init_handle(struct MediaPlayerHandle *player, struct MediaUserParams *user)
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
	//if(sync_clk_type==AVMEDIA_TYPE_VIDEO)	//
	{
		user->nAvgBitsPerSample=0;
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
int media_player_codec_file(struct MediaUserParams *user,const char *filename)
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
	debug_printf("[Debug]: Media_Set_Length\n");
	Media_Set_Length(user,media_get_url_duration_sec(mediaFormat));
	debug_printf("[Debug]: 初始化对应硬件\n");
	media_stream_all_init_handle(player,user);		//初始化对应硬件
	debug_printf("[Debug]: 解码并播放\n");
	Mediao_File_Codec_Play(player,user);
	media_stream_all_deinit_handle(player->stream_array);
	Media_Free_File(player->stream_array);
	media_player_handle_delete(player);
	return 0;
}



int Media_Play_Main(struct MediaUserParams *user)
{
	av_log_set_level(AV_LOG_TRACE); // 或者使用数字 AV_LOG_DEBUG = 48
	struct MediaFileList *list=user->list;
	while(1)
	{
		char *name;
		int cmd=user->command_get(user);
		switch(cmd)
		{
			case MEDIA_PLCMD_NEXT:
				//audio_pcm_drop(pcm_play);
				name=list->read_saft(list);
				Media_Set_Command(user,MEDIA_PLCMD_NONE);
				break;
			case MEDIA_PLCMD_LAST:
				//audio_pcm_drop(pcm_play);
				name=list->read_last_saft(list);
				Media_Set_Command(user,MEDIA_PLCMD_NONE);
				break;
			case MEDIA_PLCMD_STOP:
				if(Media_Get_State(user)!=MEDIA_STATE_START)
					user->cond->wait(user->cond);		//等待开始信号
				Media_Set_Command(user,MEDIA_PLCMD_NONE);
				break;
			case MEDIA_PLCMD_EXIT:
				Media_Set_State(user,MEDIA_STATE_EXIT);
				return 0;
				break;
			default: 
				name=list->read_saft(list);
				break;
		}
		Media_Set_State(user,MEDIA_STATE_PLAYING);
		if(name==NULL)
		{
			usleep(5000);
			continue;
		}
		Media_Set_Is_Playing(user,true);
		debug_printf("play file %s\n",name);
		media_player_codec_file(user,name);
		if(media_exit_flag)
			return 0;
		Media_Set_Is_Playing(user,false);
	}

	printf("播放结束\n");
	return 0;
}
