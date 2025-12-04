/*///------------------------------------------------------------------------------------------------------------------------//
        视频画面显示播放
说 明 :
日 期 : 2025.1.8

/*/
//------------------------------------------------------------------------------------------------------------------------//


#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h> //audio部分
#include "Video/video_play.h"
#include "Video/video_display.h"
#include "Audio/audio_codec.h"
#include "Audio/audio_play.h"
#include "Media/media_file_list.h"


int get_display_params_user_codec(struct MediaUserParams *user, AVCodecContext *codec_ctx, struct VideoStreamParams *video_params);

#ifdef DEBUG_VIDEO_INIT
#define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...) // 如果不定义DEBUG，什么也不做
#endif



// 创建纹理（设置format，如果不支持会设置为其他格式）
// 每次调整播放窗口大小都要重新创建纹理
#ifdef MEDIA_SDL_ENABLE
SDL_Texture *sdl_creat_texture_near(SDL_Renderer *renderer, uint32_t *format, int w, int h)
{
    SDL_Texture *texture = SDL_CreateTexture(renderer, *format, SDL_TEXTUREACCESS_STREAMING, w, h); // SDL_PIXELFORMAT_RGB24
    if (texture != NULL)
    {
        return texture;
    }
    fprintf(stderr, "Creat Texture! SDL_Error: %s\n", SDL_GetError());

    int num_formats = get_sizeof_format_mapping();

    for (int i = 0; i < num_formats; i++)
    {
        if ((texture = SDL_CreateTexture(renderer, get_format_mapping_with_num(i), SDL_TEXTUREACCESS_STREAMING, w, h)) != NULL)
        {
            *format = get_format_mapping_with_num(i);
            break;
        }
    }
    return texture;
}
#endif



// SDL初始化(显示)
#ifdef MEDIA_SDL_ENABLE
static int sdl_display_init(struct VideoHardParam *display, struct MediaCodecParam *codec_v, uint32_t format, int x, int y, int w, int h)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // 创建SDL窗口
    display->window = SDL_CreateWindow("TinyPiX Video", x, y, w, h, SDL_WINDOW_HIDDEN); // 隐藏：SDL_WINDOW_HIDDEN,显示：SDL_WINDOW_SHOWN
    if (!display->window)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    // 创建渲染器
    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    if (display->renderer == NULL)
    {
        fprintf(stderr, "Creat Renderer!,SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        return -1;
    }
    // 创建纹理
    display->texture = NULL;
    /*display->texture = sdl_creat_texture_near(display->renderer, &format,w,h);		//codec_v->codec_ctx->width,codec_v->codec_ctx->height SDL_PIXELFORMAT_RGB24
    if(display->texture==NULL)
    {
        fprintf(stderr, "Creat Texture! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        SDL_Quit();
    }*/

    debug_printf("debug:sdl init ok, display on(%d,%d %d*%d)\n", x, y, w, h);
    display->format = format;
    return 0;
}
#endif

#ifdef MEDIA_SDL_ENABLE
static int sdl_display_deinit(struct VideoHardParam *display)
{
    if (display->texture)
        ;
    SDL_DestroyTexture(display->texture);
    if (display->renderer)
        SDL_DestroyRenderer(display->renderer);
    if (display->window)
        SDL_DestroyWindow(display->window);
    SDL_Quit();
    return 0;
}
#endif


// 计算缩放比
static double count_scaling(double src_w, double src_h, double dst_w, double dst_h, uint8_t re)
{
    double scale;
    double scale_x = dst_w / src_w;
    double scale_y = dst_h / src_h;
    // 选择较小的缩放比来保持图片的纵横比例
    if (re == 0)
        scale = (scale_x < scale_y) ? scale_x : scale_y;
    else
        scale = (scale_x > scale_y) ? scale_x : scale_y;
    return scale;
}
static double count_scaling_smaller(double src_w, double src_h, double dst_w, double dst_h)
{
    return count_scaling(src_w, src_h, dst_w, dst_h, 0);
}
static double count_scaling_larger(double src_w, double src_h, double dst_w, double dst_h)
{
    return count_scaling(src_w, src_h, dst_w, dst_h, 1);
}

// 相对于中点的坐标变换
static int count_coordinate_displsy(int value1, int value2, int min, int max)
{
    int val = ((int)value1 - (int)value2) / 2;
    if (val < min)
        val = min;
    if (val > max)
        val = max;
    return val;
}
static int get_smaller_value(int value1, int value2)
{
    return (value1 < value2 ? value1 : value2);
}
// 根据用户设置参数计算画面真实显示尺寸
int count_rect_size_from_user(struct VideoStreamParams *user_params, AVCodecContext *codec_ctx, struct MediaRect *rect_s, struct MediaRect *rect_d)
{
    struct VideoStreamParams *user_ = user_params;
    //	struct MediaRect *rect_d=(struct MediaRect *)malloc(sizeof(struct MediaRect));
    //	struct MediaRect *rect_s=(struct MediaRect *)malloc(sizeof(struct MediaRect));
    // 获取显示参数
    switch (user_->fill)
    {
    case MEDIA_VIDEO_SCALING_STRETCH: // 拉伸显示，图像可能变形，通过渲染窗口拉伸实现
        rect_d->x = 0;
        rect_d->y = 0;
        rect_d->w = user_->rect.w;
        rect_d->h = user_->rect.h;
        rect_s->x = 0;
        rect_s->y = 0;
        rect_s->w = codec_ctx->width;
        rect_s->h = codec_ctx->height;
        break;
    case MEDIA_VIDEO_SCALING_FILL: // 保持原始比例并填充显示，可能会裁剪，通过纹理大小实现
    {
        double scale = count_scaling_larger(codec_ctx->width, codec_ctx->height, user_->rect.w, user_->rect.h);
        rect_d->w = (int16_t)((double)codec_ctx->width * scale);
        rect_d->h = (int16_t)((double)codec_ctx->height * scale);
        rect_d->x = count_coordinate_displsy(user_->rect.w, rect_d->w, 0, INT_MAX);
        rect_d->y = count_coordinate_displsy(user_->rect.h, rect_d->h, 0, INT_MAX);
        rect_s->x = 0;
        rect_s->y = 0;
        rect_s->w = codec_ctx->width;
        rect_s->h = codec_ctx->height;
        break;
    }
    case MEDIA_VIDEO_SCALING_FIT: // 保持原始比例并适应屏幕，可能添加黑边,
    {
        double scale = count_scaling_smaller(codec_ctx->width, codec_ctx->height, user_->rect.w, user_->rect.h);
        rect_d->w = (int16_t)((double)codec_ctx->width * scale);
        rect_d->h = (int16_t)((double)codec_ctx->height * scale);
        rect_d->x = count_coordinate_displsy(user_->rect.w, rect_d->w, 0, INT_MAX);
        rect_d->y = count_coordinate_displsy(user_->rect.h, rect_d->h, 0, INT_MAX);
        rect_s->x = 0;
        rect_s->y = 0;
        rect_s->w = codec_ctx->width;
        rect_s->h = codec_ctx->height;
        break;
    }
    case MEDIA_VIDEO_SCALING_ZOOM: // 放大画面以填充屏幕，可能会裁剪边缘。

        break;
    case MEDIA_VIDEO_SCALING_CROP: // 裁剪画面（画面尺寸达不到则不用裁减）以填充屏幕
        rect_d->x = count_coordinate_displsy(user_->rect.w, codec_ctx->width, 0, INT_MAX);
        rect_d->y = count_coordinate_displsy(user_->rect.h, codec_ctx->height, 0, INT_MAX);
        rect_d->w = get_smaller_value(user_->rect.w, codec_ctx->width);
        rect_d->h = get_smaller_value(user_->rect.h, codec_ctx->height);
        rect_s->x = count_coordinate_displsy(codec_ctx->width, user_->rect.w, 0, INT_MAX);
        rect_s->y = count_coordinate_displsy(codec_ctx->height, user_->rect.h, 0, INT_MAX);
        rect_s->w = get_smaller_value(user_->rect.w, codec_ctx->width);
        rect_s->h = get_smaller_value(user_->rect.h, codec_ctx->height);
        break;
    case MEDIA_VIDEO_SCALING_LETTERBOX: // 保持原始比例，上下左右添加黑边

        break;
    default:

        break;
    }
    return 0;
}

// 显示YUV
int video_display_yuv()
{
    return 0;
}

// 显示RGB
int viodeo_display_rgb()
{
    return 0;
}

// 设置视频画面的填充方式
int Video_Set_Fill_Mode(struct MediaVideoInfo *conf_v, VideoScalingType mode)
{
    if (!conf_v)
        return -1;
    THREAD_WRITE_USERCONF(conf_v->rw_mut, conf_v->video->fill, mode);
    return 0;
}

// 显示位置设置
int Video_Set_Coordinates(struct MediaVideoInfo *conf_v, int16_t x, int16_t y)
{
    if (!conf_v)
        return -1;
    THREAD_WRITE_USERCONF(conf_v->rw_mut, conf_v->video->rect.x, x);
    THREAD_WRITE_USERCONF(conf_v->rw_mut, conf_v->video->rect.y, y);
    return 0;
}

// 显示位置获取
int Video_Get_Coordinates(struct MediaVideoInfo *conf_v, int16_t *x, int16_t *y)
{
    if (!conf_v)
        return -1;
    THREAD_READ_USERCONF(conf_v->rw_mut, conf_v->video->rect.x, *x);
    THREAD_READ_USERCONF(conf_v->rw_mut, conf_v->video->rect.y, *y);
    return 0;
}

// 获取位置(视频使用位置)
int Video_Get_Position(struct MediaUserParams *conf)
{
    return (int32_t)Media_Get_DPosition(conf);
}

// 显示宽高
int Video_Get_Width_Height(struct MediaVideoInfo *conf_v, uint16_t *width, uint16_t *height)
{
    if (!conf_v)
        return -1;
    THREAD_READ_USERCONF(conf_v->rw_mut, conf_v->video->rect.w, *width);
    THREAD_READ_USERCONF(conf_v->rw_mut, conf_v->video->rect.h, *height);
    return 0;
}

// 设置宽高
int Video_Set_Width_Height(struct MediaVideoInfo *conf_v, uint16_t width, uint16_t height)
{
    if (!conf_v)
        return -1;
    THREAD_WRITE_USERCONF(conf_v->rw_mut, conf_v->video->rect.w, width);
    THREAD_WRITE_USERCONF(conf_v->rw_mut, conf_v->video->rect.h, height);
    return 0;
}

// 获取亮度
int Video_Get_Light(struct MediaVideoInfo *conf_v)
{
    if (!conf_v)
        return -1;
    int light;
    THREAD_READ_USERCONF(conf_v->rw_mut, conf_v->video->light, light);
    return light;
}

// 设置亮度
int Video_Set_Light(struct MediaVideoInfo *conf_v, uint16_t light)
{
    if (!conf_v)
        return -1;
    THREAD_WRITE_USERCONF(conf_v->rw_mut, conf_v->video->light, light);
    return 0;
}

//设置解码格式
int Video_Set_Decode_Format(struct MediaVideoInfo *conf_v, uint32_t format)
{
    if (!conf_v)
        return -1;
    THREAD_WRITE_USERCONF(conf_v->rw_mut, conf_v->format_video, format);
    return 0;
}

// 获取所有显示参数
static int video_params_get_all(struct MediaVideoInfo *conf_v, struct VideoStreamParams *video_params)
{
    // 使用memcpy有问题，原因未知
    pthread_rwlock_rdlock(&conf_v->rw_mut);
    video_params->rect.w = conf_v->video->rect.w;
    video_params->rect.h = conf_v->video->rect.h;
    video_params->rect.x = conf_v->video->rect.x;
    video_params->rect.y = conf_v->video->rect.y;
    video_params->fill = conf_v->video->fill;
    //	memcpy(&video_params,user->video,sizeof(struct VideoStreamParams));
    pthread_rwlock_unlock(&conf_v->rw_mut);
    return 0;
}

// 根据用户设置和解码器获取的视频格式调整显示参数
// user：用户设置的参数
// codec_ctx：编解码上下文
// video_params：返回的实际
int get_display_params_user_codec(struct MediaUserParams *user, AVCodecContext *codec_ctx, struct VideoStreamParams *video_params)
{
    if (!video_params)
        return -1;
    video_params_get_all(user->video_params, video_params);
    if ((video_params->rect.w == 0 || video_params->rect.h == 0) && codec_ctx != NULL) // 宽高不符合则使用视频默认参数,若没有传默认参数则直接返回
    {
        video_params->rect.w = codec_ctx->width;
        video_params->rect.h = codec_ctx->height;
    }
    return 0;
}


