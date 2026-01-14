#ifndef _VIDEO_FILTERS_H_
#define _VIDEO_FILTERS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <alsa/asoundlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>


typedef enum VideoFilterType{
	VIDEO_FILTER_NONE = 0,
	VIDEO_FILTER_FFMPEG,	// 使用ffmpeg软处理
	VIDEO_FILTER_RGA,		// 使用RGA处理
	VIDEO_FILTER_HARDWARE	// 使用硬件处理
}VideoFilterType;


struct RgaSwsContext{
	int src_width;
	int src_height;
	uint32_t src_format;
	int dst_width;
	int dst_height;
	uint32_t dst_format;
};

struct VideoFilterContext{
	VideoFilterType type;


	union {
		struct SwsContext *swsContext;
		struct RgaSwsContext *rgaSwsContext;

	}filter_ctx;		//滤镜上下文

	enum AVPixelFormat src_format;	//源格式
	AVFrame *frame_s;				//源数据帧
	
	TpVideoFormat dst_format;	//目标格式

};



#ifdef __cplusplus
}
#endif

#endif // _VIDEO_FILTERS_H_