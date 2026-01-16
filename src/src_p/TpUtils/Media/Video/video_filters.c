/*///------------------------------------------------------------------------------------------------------------------------//
        视频滤镜处理模块
说 明 : 处理视频图像的缩放、裁剪、格式转换等滤镜操作，从ffmpeg中分离出来，三种处理方式，硬件 > RGA > ffmpeg软处理
日 期 : 2026.1.13

/*///------------------------------------------------------------------------------------------------------------------------//


#include "Media/Video/video_filters.h"
#include "allocator/dma_alloc.h"
#include "Media/TpVideoFormat.h"
#if TP_HAVE_RGA
#include <rga/rga.h>
#include <rga/im2d.h>
#include <rga/im2d_buffer.h>
#include <rga/im2d_type.h>
#include <rga/im2d_single.h>
#endif

#ifdef DEBUG_VIDEO_PLAY
#define LOG_TAG "MediaPlayer.Video"
#include "Log/elog.h"
#define debug_printf(fmt, ...) elog_d(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...) // 如果不定义DEBUG，什么也不做
#endif


static int file_exists(const char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (file != NULL) {
        fclose(file);
        return true;
    }

    return false;
}

#if TP_HAVE_RGA
//tinypix媒体格式转换为RGA媒体格式
static uint32_t media_pixfmt_to_rga(TpVideoFormat format)
{
	switch (format) {
	case TP_VIDEO_FORMAT_RGB24:
		return RK_FORMAT_RGB_888;
	case TP_VIDEO_FORMAT_BGR24:
		return RK_FORMAT_BGR_888;
	case TP_VIDEO_FORMAT_RGBA8888:
		return RK_FORMAT_RGBA_8888;
	case TP_VIDEO_FORMAT_IYUV:
		return RK_FORMAT_YUYV_420;
	case TP_VIDEO_FORMAT_YUV2:
		return RK_FORMAT_YUYV_422;
	case TP_VIDEO_FORMAT_UYVY:
		return RK_FORMAT_YCbCr_444_SP;
	default:
		return 0;
	}
}

//RGA媒体格式转换为tinypix媒体格式
static TpVideoFormat rga_pixfmt_to_media(uint32_t rga_format)
{
	switch (rga_format) {
	case RK_FORMAT_RGB_888:
		return TP_VIDEO_FORMAT_RGB24;
	case RK_FORMAT_BGR_888:
		return TP_VIDEO_FORMAT_BGR24;
	case RK_FORMAT_RGBA_8888:
		return TP_VIDEO_FORMAT_RGBA8888;
	case RK_FORMAT_YUYV_420:
		return TP_VIDEO_FORMAT_IYUV;
	case RK_FORMAT_YUYV_422:
		return TP_VIDEO_FORMAT_YUV2;
	case RK_FORMAT_YCbCr_444_SP:
		return TP_VIDEO_FORMAT_UYVY;
	default:
		return TP_VIDEO_FORMAT_UNKNOWN;
	}
}
#endif
//tinypix媒体格式转换为ffmpeg媒体格式
enum AVPixelFormat media_pixfmt_to_ffmpeg(TpVideoFormat format)
{
	switch (format) {
	case TP_VIDEO_FORMAT_RGB24:
		return AV_PIX_FMT_RGB24;
	case TP_VIDEO_FORMAT_BGR24:
		return AV_PIX_FMT_BGR24;
	case TP_VIDEO_FORMAT_RGBA8888:
		return AV_PIX_FMT_RGBA;
	case TP_VIDEO_FORMAT_IYUV:
		return AV_PIX_FMT_YUV420P;
	case TP_VIDEO_FORMAT_YUV2:
		return AV_PIX_FMT_YUYV422;
	case TP_VIDEO_FORMAT_UYVY:
		return AV_PIX_FMT_UYVY422;
	default:
		return AV_PIX_FMT_NONE;
	}
}

//ffmpeg媒体格式转换为tinypix媒体格式
TpVideoFormat ffmpeg_pixfmt_to_media(enum AVPixelFormat format)
{
	switch (format) {
	case AV_PIX_FMT_RGB24:
		return TP_VIDEO_FORMAT_RGB24;
	case AV_PIX_FMT_BGR24:
		return TP_VIDEO_FORMAT_BGR24;
	case AV_PIX_FMT_RGBA:
		return TP_VIDEO_FORMAT_RGBA8888;
	case AV_PIX_FMT_YUV420P:
		return TP_VIDEO_FORMAT_IYUV;
	case AV_PIX_FMT_YUYV422:
		return TP_VIDEO_FORMAT_YUV2;
	case AV_PIX_FMT_UYVY422:
		return TP_VIDEO_FORMAT_UYVY;
	default:
		return TP_VIDEO_FORMAT_UNKNOWN;
	}
}



//智能申请内存
//依次尝试使用DMA->DRM->CMA分配内存，失败后使用普通malloc分配内存
struct VideoMemoryBuffer *video_smart_malloc(int size)
{
	struct VideoMemoryBuffer *video_buf = malloc(sizeof(struct VideoMemoryBuffer));
	video_buf->size = size;
	void *ptr = NULL;
	int dma_fd;
	
	//1. 尝试DMA分配
	dma_fd = dma_alloc(NULL, size, &ptr);
	if (dma_fd >= 0) {	
		video_buf->fd = dma_fd;
		video_buf->ptr = ptr;
		video_buf->type = VIDEO_MEMORY_TYPE_DMA;
		return video_buf;
	}
	
	//2.尝试DRM分配

	//3.尝试CMA分配

	//4.普通malloc分配
	ptr = malloc(size);
	if (ptr) {
		video_buf->fd = -1;
		video_buf->ptr = ptr;
		video_buf->type = VIDEO_MEMORY_TYPE_NORMAL;
		return video_buf;
	}
	free(video_buf);
	return NULL;
}

void video_smart_free(struct VideoMemoryBuffer *video_buf)
{
	if(!video_buf) {
		return;
	}

	switch(video_buf->type) {
		case VIDEO_MEMORY_TYPE_DMA:
			dma_buf_free(video_buf->size, video_buf->fd, video_buf->ptr);
			break;
		case VIDEO_MEMORY_TYPE_DRM:
			break;
		case VIDEO_MEMORY_TYPE_CMA:
			break;
		case VIDEO_MEMORY_TYPE_NORMAL:
			free(video_buf->ptr);
			break;
		default:
			break;
	}

	free(video_buf);
}	


//ffmpeg申请图像缓存
//主要用于为转码后的图像分配缓存
static int video_ffmpeg_malloc_codec_frame(int width,int height, enum AVPixelFormat pix_fmt,uint8_t **buffer, AVFrame **frame_d)
{
	int numBytes;
	numBytes = av_image_get_buffer_size(pix_fmt, width, height, 1);		//计算需要的空间大小
	*buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	if(*buffer == NULL) {
		return -1;
	}
	*frame_d = av_frame_alloc();
	if(*frame_d == NULL) {
		av_free(*buffer);
		return -1;
	}
	if(av_image_fill_arrays((*frame_d)->data, (*frame_d)->linesize, *buffer, pix_fmt,
							width, height, 1)<0)
	{
		av_free(*buffer);
		av_frame_free(&(*frame_d));
		return -1;
	}
	return 0;
}

//RGA申请图像缓存
void video_rga_malloc_codec_frame(int width,int height, uint32_t rga_format, uint8_t **buffer, struct rga_buffer_t *rga_buf)
{

}

//申请图像缓存
void video_malloc_codec_frame(int width,int height, TpVideoFormat pix_fmt,uint8_t **buffer, AVFrame **frame_d)
{

	video_ffmpeg_malloc_codec_frame(width, height, pix_fmt, buffer, frame_d);
}

#if TP_HAVE_RGA
//将RGB格式的AVFrame数据转换为RGA缓冲区
static void avframe_to_rga_buffer(const AVFrame *frame, rga_buffer_t *rga_buf)
{
	int rga_fmt = media_pixfmt_to_rga(ffmpeg_pixfmt_to_media(frame->format));
    if (rga_fmt < 0)
        return -1;

    *rga_buf = wrapbuffer_virtualaddr(
        frame->data[0],
        frame->width,
        frame->height,
        rga_fmt
    );

    return 0;
}
#endif

struct VideoFilterContext * video_filter_context_creat(
								int src_w, int src_h, TpVideoFormat src_format,
								int dst_w, int dst_h, TpVideoFormat dst_format)
{
	struct VideoFilterContext *filter_ctx = malloc(sizeof(struct VideoFilterContext));
	if (!filter_ctx) {
		fprintf(stderr, "Could not allocate VideoFilterContext.\n");
		return NULL;
	}
	filter_ctx->type = VIDEO_FILTER_FFMPEG;
#if TP_HAVE_RGA
	filter_ctx->type = VIDEO_FILTER_RGA;
#endif
	filter_ctx->src_format = src_format;
	filter_ctx->dst_format = dst_format;

	//缩放滤镜创建
	switch (filter_ctx->type) {
		case VIDEO_FILTER_HARDWARE:
			// 硬件加速处理
			break;
#if TP_HAVE_RGA
		case VIDEO_FILTER_RGA:
			rga_buffer_t *rga_buf=malloc(sizeof(rga_buffer_t));
			rga_buffer_handle_t dst_handle;
			int dst_buf_size = dst_w * dst_h * get_bpp_from_format(dst_format);
			struct VideoMemoryBuffer *video_buf = video_smart_malloc(dst_buf_size);
			if(!video_buf) {

				return NULL;
			}

			if(video_buf->type == VIDEO_MEMORY_TYPE_NORMAL) {		//普通malloc
				dst_handle = importbuffer_virtualaddr(video_buf->ptr, &(im_handle_param_t){dst_w, dst_h, dst_format});
			}
			else{
				dst_handle = importbuffer_fd(video_buf->fd, &(im_handle_param_t){dst_w, dst_h, dst_format});
			}
			if (dst_handle == 0) {
				printf("Failed to import CMA buffer fd for dst channel! %s\n", imStrError());
			}
			*rga_buf = wrapbuffer_handle(dst_handle, dst_w, dst_h, dst_format);
			filter_ctx->filter_ctx.rga_frame_d = rga_buf;
			break;
#endif
		case VIDEO_FILTER_FFMPEG:
		default:
			// 使用ffmpeg的sws_scale进行处理
			{
				uint8_t *buffer = NULL;
				AVFrame *frame_d = NULL;
				struct SwsContext *swsContext=NULL;
				swsContext = sws_getContext(src_w, src_h, 		//创建一个swsContext用于处理图像缩放格式转换
							media_pixfmt_to_ffmpeg(src_format),
							dst_w,dst_h,
							media_pixfmt_to_ffmpeg(dst_format),
							SWS_BICUBIC, NULL, NULL, NULL);	//缩放算法及参数
				if (!swsContext) {
					fprintf(stderr, "Could not initialize SwsContext.\n");
					return NULL;
				}
				if(video_ffmpeg_malloc_codec_frame(dst_w, dst_h, media_pixfmt_to_ffmpeg(dst_format), &buffer, &frame_d)<0)
				{
					sws_freeContext(swsContext);
					return NULL;
				}
				filter_ctx->filter_ctx.swsContext = swsContext;
				filter_ctx->filter_ctx.frame_d = frame_d;
				filter_ctx->filter_ctx.buffer = buffer;
				filter_ctx->filter_ctx.srcSliceY = 0;
				filter_ctx->filter_ctx.srcSliceH = src_h;
			}
			break;
	}
	return filter_ctx;
}

void video_filter_context_free(struct VideoFilterContext *filter_ctx)
{
	if (!filter_ctx) {
		return;
	}

	switch (filter_ctx->type) {
		case VIDEO_FILTER_HARDWARE:
			// 硬件加速处理
			break;

		case VIDEO_FILTER_RGA:

			break;
		case VIDEO_FILTER_FFMPEG:
		default:
			if (filter_ctx->filter_ctx.swsContext) {
				sws_freeContext(filter_ctx->filter_ctx.swsContext);
			}
			if (filter_ctx->filter_ctx.frame_d) {
				av_frame_free(&filter_ctx->filter_ctx.frame_d);
			}
			if (filter_ctx->filter_ctx.buffer) {
				av_free(filter_ctx->filter_ctx.buffer);
			}
			break;
	}

	free(filter_ctx);
}


static int rga_sws_scale()
{

}



//滤镜处理(缩放、裁剪、格式转换等)
int video_filter_process(struct VideoFilterContext *filter_ctx, AVFrame *frame_s)	
{
	switch(filter_ctx->type) {
	case VIDEO_FILTER_HARDWARE:
		// 硬件加速处理
		break;
#if TP_HAVE_RGA
	case VIDEO_FILTER_RGA:
		// RGA处理
		rga_buffer_t src_rga_buf;

		avframe_to_rga_buffer(frame_s, &src_rga_buf);

		imresize(src_rga_buf, *filter_ctx->filter_ctx.rga_frame_d);
		break;
#endif
	case VIDEO_FILTER_FFMPEG:
	default:
		// 使用ffmpeg的sws_scale进行处理
		if(filter_ctx->filter_ctx.swsContext)
		{

			sws_scale(filter_ctx->filter_ctx.swsContext, (const uint8_t * const *)frame_s->data, frame_s->linesize,  
					filter_ctx->filter_ctx.srcSliceY, filter_ctx->filter_ctx.srcSliceH,
					filter_ctx->filter_ctx.frame_d->data, filter_ctx->filter_ctx.frame_d->linesize);
		}

		break;
	}

	
	return 0;
}
