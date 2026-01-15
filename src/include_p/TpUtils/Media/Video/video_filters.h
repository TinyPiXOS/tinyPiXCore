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

#define DMA_HEAP_DMA32_UNCACHED_PATH    "/dev/dma_heap/system-uncached-dma32"

struct rga_buffer_t;
typedef enum VideoFilterType{
	VIDEO_FILTER_NONE = 0,
	VIDEO_FILTER_FFMPEG,	// 使用ffmpeg软处理
	VIDEO_FILTER_RGA,		// 使用RGA处理
	VIDEO_FILTER_HARDWARE	// 使用硬件处理
}VideoFilterType;

//视频内存类型
typedef enum VideoMemoryType{
	VIDEO_MEMORY_TYPE_UNKNOWN = 0,
	VIDEO_MEMORY_TYPE_DMA,	// DMA内存
	VIDEO_MEMORY_TYPE_DRM,	// DRM内存
	VIDEO_MEMORY_TYPE_CMA,	// CMA内存
	VIDEO_MEMORY_TYPE_NORMAL // 普通内存
}VideoMemoryType;

//视频连续内存缓冲区结构体
struct VideoMemoryBuffer{
	int fd;			// dma/drm/cma
	void *ptr;		// 虚拟地址
	size_t size;	// 大小
	VideoMemoryType type;	// 内存类型
};

struct RgaSwsContext{
	double fw;	// (double) dst.width / src.width
	double fh;	// (double) dst.height / src.height
};

struct VideoFilterContext{
	VideoFilterType type;


	union {
		struct{
			struct SwsContext *swsContext;
			AVFrame *frame_d;
			uint8_t *buffer;
		};
		struct{
			struct RgaSwsContext *rgaSwsContext;
			rga_buffer_t *rga_frame_d;
		};
	}filter_ctx;		//滤镜上下文

	enum AVPixelFormat src_format;	//源格式
	TpVideoFormat dst_format;	//目标格式

};



#ifdef __cplusplus
}
#endif

#endif // _VIDEO_FILTERS_H_