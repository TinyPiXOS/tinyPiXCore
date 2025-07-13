/*///------------------------------------------------------------------------------------------------------------------------//
		缩放过滤器
说 明 : 倍速播放使用，(ffmpeg自带的过滤器封装使用)
日 期 : 2025.02.19

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
#include "tpAudioDevice.h"
#include "filter.h"



//

//创建过滤器并初始化
struct MediaFilterParam *audio_filter_creat_init(float speed,uint32_t rate,int16_t channels, int64_t channel_layout,enum AVSampleFormat fmt)
{
	AVFilterGraph *filter_graph = avfilter_graph_alloc();
    AVFilterContext *buffersrc_ctx = NULL, *buffersink_ctx = NULL, *atempo_ctx=NULL, *aformat_ctx=NULL;
    const AVFilter *buffersrc = avfilter_get_by_name("abuffer");	//输入过滤器
    const AVFilter *buffersink = avfilter_get_by_name("abuffersink");	//输出过滤器
	const AVFilter *aformat_filter = avfilter_get_by_name("aformat");
	const AVFilter *atempo_filter = avfilter_get_by_name("atempo");	//时间伸缩过滤器
	struct MediaFilterParam *filter=NULL;

	if (!buffersrc || !buffersink || !aformat_filter || !atempo_filter) {
        fprintf(stderr, "Failed to find filters\n");
		goto ERROR;
    }
	char args[256];
	snprintf(args, sizeof(args),"sample_rate=44100:sample_fmt=fltp:channel_layout=0x%llx\n",channel_layout);
	//"sample_rate=44100:channel_layout=3:sample_fmt=fltp"
    // 创建输入过滤器（先随便传进去点空参数，要不然会报错）
    if (avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",args , NULL, filter_graph) < 0) {
		fprintf(stderr, "Failed to create buffersrc filters\n");
		goto ERROR;
    }
	if (av_opt_set_int(buffersrc_ctx, "sample_rate", rate, AV_OPT_SEARCH_CHILDREN) < 0) {
		fprintf(stderr, "Error setting sample_rate\n");
		goto ERROR;
	}
	if (av_opt_set_int(buffersrc_ctx, "channels", channels, AV_OPT_SEARCH_CHILDREN) < 0) {
		printf("channels:%d\n",channels);
		fprintf(stderr, "Error setting channels\n");
		goto ERROR;
	}
	snprintf(args, sizeof(args),"0x%llx\n",channel_layout);
	if (av_opt_set(buffersrc_ctx, "channel_layout", args, AV_OPT_SEARCH_CHILDREN) < 0) {
		printf("channel_layout:%ld\n",channel_layout);
		fprintf(stderr, "Error setting channel_layout\n");
		goto ERROR;
	}
	if (av_opt_set_sample_fmt(buffersrc_ctx, "sample_fmt", fmt, AV_OPT_SEARCH_CHILDREN) < 0) {
		fprintf(stderr, "Error setting sample_fmt\n");
		goto ERROR;
	}

	//创建输出过滤器
	if (avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out", NULL, NULL, filter_graph) < 0) {
		fprintf(stderr, "Failed to create buffersink filters\n");
		goto ERROR;
    }

	//创建channel_layouts过滤器
	if (avfilter_graph_create_filter(&aformat_ctx, aformat_filter, "format", "channel_layouts=stereo", NULL, filter_graph) < 0) {
        fprintf(stderr, "Failed to create aformat filter\n");
		goto ERROR;
    }

    // 添加时间伸缩过滤器（如 atempo 或 scaletempo）
    if (avfilter_graph_create_filter(&atempo_ctx, atempo_filter, "tempo", NULL, NULL, filter_graph) < 0) {		//"tempo=2.0"
        fprintf(stderr, "Failed to create atempo filter\n");
		goto ERROR;
    }
	if (av_opt_set_double(atempo_ctx, "tempo", speed, AV_OPT_SEARCH_CHILDREN) < 0) {
		fprintf(stderr, "Error setting tempo\n");
		goto ERROR;
	}

	// 连接过滤器（buffersrc -> atempo -> buffersink）
    if (avfilter_link(buffersrc_ctx, 0, aformat_ctx, 0) < 0) {
        fprintf(stderr, "Error linking buffersrc to atempo filter\n");
		goto ERROR;
    }

	if (avfilter_link(aformat_ctx, 0, atempo_ctx, 0) < 0) {
        fprintf(stderr, "Error linking buffersrc to aformat filter\n");
		goto ERROR;
    }

    if (avfilter_link(atempo_ctx, 0, buffersink_ctx, 0) < 0) {
        fprintf(stderr, "Error linking atempo filter to buffersink\n");
		goto ERROR;
    }

	//输出过滤图详细信息
	avfilter_graph_dump(filter_graph, NULL);
    // 配置过滤器图
    if (avfilter_graph_config(filter_graph, NULL) < 0) {
        fprintf(stderr, "Error configuring filter graph\n");
        goto ERROR;
    }

	filter=(struct MediaFilterParam *)malloc(sizeof(struct MediaFilterParam));
	if(!filter)
		return NULL;
	filter->buffersink_ctx=buffersink_ctx;
	filter->buffersrc_ctx=buffersrc_ctx;
	filter->atempo_ctx=atempo_ctx;
	filter->aformat_ctx=aformat_ctx;
	filter->filter_graph=filter_graph;
	return filter;
ERROR:
	if(filter->buffersrc_ctx)
		avfilter_free(filter->buffersrc_ctx);
	if(filter->aformat_ctx)
		avfilter_free(filter->aformat_ctx);
	if(filter->atempo_ctx)
		avfilter_free(filter->atempo_ctx);
	if(filter->buffersink_ctx)
		avfilter_free(filter->buffersink_ctx);
	avfilter_graph_free(&filter_graph);
	return NULL;
}

//删除过滤器
int audio_filter_delete(struct MediaFilterParam *filter)
{
	if(!filter)
		return -1;

	if(filter->buffersrc_ctx)
		avfilter_free(filter->buffersrc_ctx);
	if(filter->aformat_ctx)
		avfilter_free(filter->aformat_ctx);
	if(filter->atempo_ctx)
		avfilter_free(filter->atempo_ctx);
	if(filter->buffersink_ctx)
		avfilter_free(filter->buffersink_ctx);

	avfilter_graph_free(&filter->filter_graph);
	filter->filter_graph=NULL;
	free(filter);
	filter=NULL;
}

//使用过滤器处理数据
int media_filte_get_data(struct MediaFilterParam *filter,AVFrame *frame_src,AVFrame *frame_flt)
{
	AVFilterContext *buffersrc_ctx = filter->buffersrc_ctx;
	AVFilterContext *buffersink_ctx = filter->buffersink_ctx;
//	printf("debug:media_filte_get_data\n");
	if (av_buffersrc_add_frame(buffersrc_ctx, frame_src) < 0) {
		fprintf(stderr, "Error adding frame to buffer source\n");
		return -1;
	}
//	printf("debug:media_filte_get_data\n");
//	AVFrame *frame_flt = av_frame_alloc();		//提前申请
	if (av_buffersink_get_frame(buffersink_ctx, frame_flt) < 0) {
		//fprintf(stderr, "Error get filter frame\n");
		return -1;
	}
	// 将处理后的音频数据通过 ALSA 播放
//	snd_pcm_writei(pcm_handle, frame_flt->data[0], frame_flt->nb_samples);
//	av_frame_free(&frame_flt);
	return 0;
}

#ifdef __cplusplus
}
#endif

