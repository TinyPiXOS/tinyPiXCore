

#include <stdio.h>
#include <libavutil/imgutils.h>
#include "video_play.h"
#include "video_display.h"



int video_display_image(uint8_t **data, int *linesize, uint32_t format ,void *user_data)
{
	struct VideoHardParam *display=(struct VideoHardParam *)user_data;
	if(!display->renderer)
	{	
		fprintf(stderr,"sdl windows is not create\n");
		return -1;
	}

	SDL_Rect dst_rect = {display->rect_dst->x, display->rect_dst->y, display->rect_dst->w, display->rect_dst->h};  // 设置目标矩形为 960x540
	SDL_Rect src_rect = {display->rect_src->x,display->rect_src->y,display->rect_src->w,display->rect_src->h};

	if(SDL_ISPIXELFORMAT_FOURCC(display->format))		//如果是YUV格式
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
	else
	{
		SDL_UpdateTexture(display->texture, NULL, data[0], linesize[0]);
	}

	dst_rect = (SDL_Rect){display->rect_dst->x, display->rect_dst->y, display->rect_dst->w, display->rect_dst->h};  // 设置目标矩形为 960x540
	src_rect = (SDL_Rect){display->rect_src->x, display->rect_src->y, display->rect_src->w, display->rect_src->h};
	
	//SDL_Rect dst_rect={0,200,480,480*480/854};
	//SDL_Rect src_rect={0,0,854,480};
	SDL_RenderClear(display->renderer);
	SDL_RenderCopy(display->renderer, display->texture, &src_rect, &dst_rect);	//将纹理复制到渲染区域
	SDL_RenderPresent(display->renderer);
	return 0;
}