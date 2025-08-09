

#include <stdio.h>
#include <libavutil/imgutils.h>
#include "video_play.h"
#include "video_display.h"
#include "../media/media_codec.h"

#ifdef MEDIA_SDL_ENABLE
struct MediaSdlDisplayCbData{
	SDL_Renderer *renderer;		//渲染器
	SDL_Texture *texture;		//纹理
	struct MediaRect *rect_src;
	struct MediaRect *rect_dst;
};

//ffmpeg和sdl映射表
struct PixelFormatMapping{
    enum AVPixelFormat avFmt;
    Uint32 sdlFmt;
};
const struct PixelFormatMapping pixelFormatMap[] = {
    {AV_PIX_FMT_YUV420P, SDL_PIXELFORMAT_IYUV},    // YUV 4:2:0
    {AV_PIX_FMT_YUYV422, SDL_PIXELFORMAT_YUY2},    // YUV 4:2:2
    {AV_PIX_FMT_UYVY422, SDL_PIXELFORMAT_UYVY},    // YUV 4:2:2 (UYVY)
    {AV_PIX_FMT_NV12, SDL_PIXELFORMAT_NV12},       // NV12
    {AV_PIX_FMT_NV21, SDL_PIXELFORMAT_NV21},       // NV21
    {AV_PIX_FMT_RGB24, SDL_PIXELFORMAT_RGB24},     // RGB 24-bit		排列方式为：R0G0B0 R1G1B1 R2G2B2
    {AV_PIX_FMT_BGR24, SDL_PIXELFORMAT_BGR24},     // BGR 24-bit
    {AV_PIX_FMT_ARGB, SDL_PIXELFORMAT_ARGB8888},   // ARGB 32-bit
    {AV_PIX_FMT_RGBA, SDL_PIXELFORMAT_RGBA8888},   // RGBA 32-bit
    {AV_PIX_FMT_ABGR, SDL_PIXELFORMAT_ABGR8888},   // ABGR 32-bit
    {AV_PIX_FMT_BGRA, SDL_PIXELFORMAT_BGRA8888},   // BGRA 32-bit
};

//获取mapping的大小
int get_sizeof_format_mapping()
{
	return sizeof(pixelFormatMap) / sizeof(pixelFormatMap[0]);
}

//根据序号获取SDL格式
uint32_t get_format_mapping_with_num(uint32_t num)
{
	return pixelFormatMap[num].sdlFmt;
}

//根据AVPixelFormat获取sdl的format
uint32_t get_sdl_pixel_format(enum AVPixelFormat pixFmt) 
{
	for (size_t i = 0; i < get_sizeof_format_mapping(); ++i) 
	{
		if (pixelFormatMap[i].avFmt == pixFmt) {
			return pixelFormatMap[i].sdlFmt;
		}
	}
	return SDL_PIXELFORMAT_UNKNOWN; //未找到匹配格式
}

//根据SDL格式获取AVPixelFormat
enum AVPixelFormat get_format_pixel_sdl(uint32_t format)
{
	for (size_t i = 0; i < get_sizeof_format_mapping(); ++i) 
	{
		if (pixelFormatMap[i].sdlFmt == format) {
			return pixelFormatMap[i].avFmt;
		}
	}
	return AV_PIX_FMT_NB; //未找到匹配格式
}



int video_display_image(uint8_t **data, int *linesize, uint32_t format ,void *user_data)
{
	struct MediaSdlDisplayCbData *display=(struct MediaSdlDisplayCbData *)user_data;
	if(!display->renderer)
	{	
		fprintf(stderr,"sdl windows is not create\n");
		return -1;
	}

	uint32_t sdl_format=format;
	if(SDL_ISPIXELFORMAT_FOURCC(sdl_format))		//如果是YUV格式（需要特苏处理）
	{
		const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(format);
		if (desc && desc->nb_components >= 3 && desc->log2_chroma_w > 0 && desc->log2_chroma_h > 0)
		{
			SDL_UpdateYUVTexture(display->texture, NULL,
							data[0], linesize[0],
							data[1], linesize[1],
							data[2], linesize[2]);
		}
		else
			fprintf(stderr, "Unsupported pixel format: %s\n", av_get_pix_fmt_name(format));
	}
	else	//普通RGB类型
	{
		SDL_UpdateTexture(display->texture, NULL, data[0], linesize[0]);
	}

	SDL_Rect dst_rect = (SDL_Rect){display->rect_dst->x, display->rect_dst->y, display->rect_dst->w, display->rect_dst->h};  // 设置目标矩形为 960x540
	SDL_Rect src_rect = (SDL_Rect){display->rect_src->x, display->rect_src->y, display->rect_src->w, display->rect_src->h};
	
	//SDL_Rect dst_rect={0,200,480,480*480/854};
	//SDL_Rect src_rect={0,0,854,480};
	SDL_RenderClear(display->renderer);
	SDL_RenderCopy(display->renderer, display->texture, &src_rect, &dst_rect);	//将纹理复制到渲染区域
	SDL_RenderPresent(display->renderer);
	return 0;
}

int video_display_image_sdl(uint8_t **data, int *linesize, uint32_t format, SDL_Renderer *renderer,		//渲染器
	SDL_Texture *texture,		//纹理
	struct MediaRect *rect_src,
	struct MediaRect *rect_dst)
{
	struct MediaSdlDisplayCbData user_data={renderer,texture,rect_src,rect_dst};
	video_display_image(data,linesize,format,&user_data);
}

#endif

