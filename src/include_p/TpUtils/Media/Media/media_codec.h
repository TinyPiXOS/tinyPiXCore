#ifndef _MEDIA_CODEC_H_
#define _MEDIA_CODEC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <alsa/asoundlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include "media_timer.h"
#include "media.h"
#include "tools/variable_array.h"
#include "Media/media_play.h"
#include "Media/media_config.h"
#include "Audio/audio_play.h"


struct MediaUserParams;

typedef enum AVMediaType MediaType;
typedef VariableArray MediaStreamArray;

extern uint8_t media_exit_flag;

// 待解码队列状态
typedef enum
{
    MEDIA_PACK_QUEUE_FULL,  // 队列已满或达到最大允许的队列长度
    MEDIA_PACK_QUEUE_EMPTY, // 队列为空
    MEDIA_PACK_QUEUE_OTHER, // 其他状态
} MediaPacketQueueState;

// 视频播放的硬件相关参数
/*struct VideoStreamParams{
    struct MediaRect *rect_src;
    struct MediaRect *rect_dst;
    uint32_t format;			//格式，RGB，YUV等，当启用本地显示的时候就是带鸟sdl窗口的格式，如果没有启用就代表用户设置的格式(当前用户默认使用RGB888)
    VideoScalingType fill;
};*/

// 视频播放的句柄（包含硬件信息，流解码前后详细信息）
struct MediaVideoHandle
{
#ifdef MEDIA_SDL_ENABLE
    SDL_Window *window;     // 窗口
    SDL_Renderer *renderer; // 渲染器
    SDL_Texture *texture;   // 纹理
#endif
    bool is_sdl; // 是否启用本地显示(如果不启用需要上层绘制图像)
};



// 媒体流通用参数
struct MediaStreamParams
{
    AVFormatContext *format_ctx; // 输入输出相关信息，贯穿ffmpeg
    AVCodecContext *codec_ctx; // 编码器上下文，源文件中的音频参数,位宽，声道等，视频的帧率分辨率等
    int stream_index;          // 流索引号
    MediaType type;            // 流类型
    bool enable;               // 是否启用此流的处理

    union // 每个流独特的硬件相关参数
    {
        struct//音频流
        {
            struct MediaAudioHandle *handle; // 音频硬件的采样参数
            struct SwrContext *swr_ctx;
        } audio;
        struct//视频
        {
            struct MediaVideoHandle *handle;
            // struct VideoStreamParams *params_s;
            // struct VideoStreamParams *params_d;
            uint32_t format; // FFMPEG的格式，RGB，YUV等，(当前用户默认使用RGB888)
        } video;
        /*struct{//字幕

        }subtitle;*/
    };

    void *codec_thread; // struct MediaThread *
};

struct MediaPlayerHandle{
    char *url;
    AVFormatContext *format_ctx;    // 输入输出相关信息，贯穿ffmpeg
    
    struct{
        struct TimerHandle *clock;      // 同步时钟
        int sync_clk_stream_index;         // 主同步时钟的流索引号
        int sync_clk_array_index;          // 主同步时钟在流数组中的位置
    }; 
    MediaStreamArray *stream_array;     // 所有的流（内部数据类型为struct MediaStreamParams）

    int (*player_start)(MediaStreamArray *stream_array);
    int (*player_wait)(MediaStreamArray *stream_array);
    int (*player_pause)(struct MediaPlayerHandle *handle);
    int (*player_resume)(struct MediaPlayerHandle *handle);
    int (*set_state)(MediaStreamArray *stream_array, MediaPlayState state);

    int (*flush_list)(MediaStreamArray *stream_array);  // 删除全部流队列中所有元素
    int (*packet_exit)(MediaStreamArray *stream_array); //退出阻塞，并退出队列
    MediaPacketQueueState (*list_state)(MediaStreamArray *stream_array);

    int (*flush_codec_buffers)(MediaStreamArray *stream_array) ;//清空所有解码器缓存
};

MediaFormatContext *Media_Get_File_All_Info(const char *filename, MediaStreamArray *media_array);
int Media_Free_File(MediaStreamArray *media_array);
int Mediao_File_Codec_Play(struct MediaPlayerHandle *player, struct MediaUserParams *user);

struct MediaPlayerHandle *media_player_handle_creat();
void media_player_handle_delete(struct MediaPlayerHandle *player);

struct MediaStreamParams *media_stream_params_creat();
void media_stream_params_delete(struct MediaStreamParams *);

#ifdef __cplusplus
}
#endif

#endif
