#ifndef _VIDEO_DISPLAY_H_
#define _VIDEO_DISPLAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavutil/imgutils.h>
#include "Media/media_config.h"
#include "Audio/audio_play.h"

    int get_sizeof_format_mapping();
    uint32_t get_format_mapping_with_num(uint32_t num);
    uint32_t get_sdl_pixel_format(enum AVPixelFormat pixFmt);
    enum AVPixelFormat get_format_pixel_sdl(uint32_t format);

#ifdef MEDIA_SDL_ENABLE
    int video_display_image(uint8_t **data, int *linesize, uint32_t format, void *user_data); //(uint8_t *data[AV_NUM_DATA_POINTERS], int linesize[AV_NUM_DATA_POINTERS], uint32_t format ,void *user_data);
    int video_display_image_sdl(uint8_t **data, int *linesize, uint32_t format, SDL_Renderer *renderer, SDL_Texture *texture, struct MediaRect *rect_src, struct MediaRect *rect_dst);
#endif

#ifdef __cplusplus
}
#endif

#endif