#ifndef _AUDIO_FILTER_H_
#define _AUDIO_FILTER_H_

#include <libavfilter/avfilter.h>


//过滤器参数
struct MediaFilterParam{
	AVFilterGraph *filter_graph;
	
	AVFilterContext *buffersrc_ctx;
	AVFilterContext *buffersink_ctx;
	AVFilterContext *atempo_ctx; 
	AVFilterContext *aformat_ctx;
};


struct MediaFilterParam *audio_filter_creat_init(float speed,uint32_t rate,int16_t channels,int64_t channel_layout,enum AVSampleFormat fmt);
int audio_filter_delete(struct MediaFilterParam *filter);

int media_filte_get_data(struct MediaFilterParam *filter,AVFrame *frame_src,AVFrame *frame_flt);



#endif
