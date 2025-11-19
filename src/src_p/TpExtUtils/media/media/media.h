#ifndef _MEDIA_MEDIA_H_
#define _MEDIA_MEDIA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>

typedef AVFormatContext MediaFormatContext;
typedef AVStream	MediaStream;


//主同步时钟
typedef enum{
	SYNC_AUDIO_MASTER,          // 音频主时钟（默认）
	SYNC_VIDEO_MASTER,          // 视频主时钟
	SYNC_EXTERNAL_CLOCK         // 外部时钟
}MediaSyneClockType;

//媒体播放器上下文
struct MediaPlayerContext{
	char *url;
	MediaFormatContext *format_ctx;
	MediaSyneClockType sync_clock;
};



uint8_t media_is_network_file(const char *path);
int media_get_file_info(const char *url,MediaFormatContext **format_ctx);
int media_delete_file_info(MediaFormatContext *format_ctx);
int media_init(uint8_t en_net);
int media_deinit(uint8_t en_net);


/// @brief 获取秒级时长
/// @param format_ctx 
/// @return 
double media_get_url_duration_sec(MediaFormatContext *format_ctx);

#ifdef __cplusplus
}
#endif

#endif