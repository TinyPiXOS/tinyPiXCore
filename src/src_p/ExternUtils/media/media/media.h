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

uint8_t media_is_network_file(const char *path);
int media_get_file_info(const char *url,MediaFormatContext **format_ctx);
int media_delete_file_info(MediaFormatContext *format_ctx);
int media_init(uint8_t en_net);
int media_deinit(uint8_t en_net);

#ifdef __cplusplus
}
#endif

#endif