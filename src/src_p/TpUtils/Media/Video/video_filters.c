/*///------------------------------------------------------------------------------------------------------------------------//
        视频滤镜处理模块
说 明 : 处理视频图像的缩放、裁剪、格式转换等滤镜操作，从ffmpeg中分离出来，三种处理方式，硬件 > RGA > ffmpeg软处理
日 期 : 2026.1.13

/*///------------------------------------------------------------------------------------------------------------------------//


#include "video_filters.h"
#include "Media/TpVideoFormat.h"
//#if TP_HAVE_RGA
#include <rga/rga.h>
#include <rga/im2d.h>
#include <rga/im2d_buffer.h>
#include <rga/im2d_type.h>
#include <rga/im2d_single.h>
//#endif

#ifdef DEBUG_VIDEO_PLAY
#define LOG_TAG "MediaPlayer.Video"
#include "Log/elog.h"
#define debug_printf(fmt, ...) elog_d(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...) // 如果不定义DEBUG，什么也不做
#endif


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

//tinypix媒体格式转换为ffmpeg媒体格式
static enum AVPixelFormat media_pixfmt_to_ffmpeg(TpVideoFormat format)
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
static TpVideoFormat ffmpeg_pixfmt_to_media(enum AVPixelFormat format)
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



//RGA申请图像缓存
void video_rga_malloc_codec_frame()
{


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

//将AVFrame数据转换为RGA缓冲区
static void avframe_to_rga_buffer(AVFrame *frame, struct rga_buffer_t *rga_buf)
{
	int rga_fmt = ffmpeg_pixfmt_to_media(media_pixfmt_to_rga(frame->format));
    if (rga_fmt < 0)
        return -1;

    *rga_buf = wrapbuffer_virtualaddr_t(
        frame->data[0],
        frame->width,
        frame->height,
        rga_fmt
    );

    return 0;
}

//初始化滤镜上下文（或重新初始化）
int video_filter_init(struct VideoFilterContext *filter_ctx, VideoFilterType type)
{
	


}



void video_filter_resize_crop(struct VideoFilterContext *filter_ctx,
								int src_w, int src_h, TpVideoFormat src_format,
								int dst_w, int dst_h, TpVideoFormat dst_format)
{


	//缩放滤镜创建
	switch (filter_ctx->type) {
		case VIDEO_FILTER_HARDWARE:
			// 硬件加速处理
			break;

		case VIDEO_FILTER_RGA:
			
			break;

		case VIDEO_FILTER_FFMPEG:
		default:
			// 使用ffmpeg的sws_scale进行处理
			{
				struct SwsContext *swsContext=NULL;
				swsContext = sws_getContext(src_w, src_h, 		//创建一个swsContext用于处理图像缩放格式转换
							media_pixfmt_to_ffmpeg(src_format),
							dst_w,dst_h,
							media_pixfmt_to_ffmpeg(dst_format),
							SWS_BICUBIC, NULL, NULL, NULL);
				if (!swsContext) {
					fprintf(stderr, "Could not initialize SwsContext.\n");
					return -1;
				}
				video_ffmpeg_malloc_codec_frame(dst_w, dst_h, media_pixfmt_to_ffmpeg(dst_format), NULL, NULL);
				
			}
			break;
	}

}




//滤镜处理(缩放、裁剪、格式转换等)
int video_filter_process(struct VideoFilterContext *filter_ctx, AVFrame *frame_s)	
{
	switch(filter_ctx->type) {
	case VIDEO_FILTER_HARDWARE:
		// 硬件加速处理
		break;

	case VIDEO_FILTER_RGA:
		// RGA处理
		rga_buffer_t src_rga_buf, dst_rga_buf;
		avframe_to_rga_buffer(frame_s, &src_rga_buf);
		imresize(src_rga_buf,
			dst_rga_buf,
			double fx = 0,
			double fy = 0,
			int interpolation = INTER_LINEAR,
			int sync = 1,
			int *release_fence_fd = NULL);
		break;
	case VIDEO_FILTER_FFMPEG:
	default:
		// 使用ffmpeg的sws_scale进行处理
		if(filter_ctx->filter_ctx.swsContext)
		{
			sws_scale(filter_ctx->filter_ctx.swsContext, (const uint8_t * const *)frame_s->data, frame_s->linesize,  0, stream->codec_ctx->height,frame_d->data, frame_d->linesize);
		}

		break;
	}

	
	return 0;
}