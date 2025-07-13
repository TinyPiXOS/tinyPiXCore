

#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include "media.h"





//获取文件信息
int media_get_file_info(const char *url,MediaFormatContext **format_ctx)
{
	int ret;
	if ((ret = avformat_open_input(format_ctx, url, NULL, NULL)) < 0) {
        fprintf(stderr, "Could not open input file '%s'\n", url);
		//fprintf(stderr, "Could not open input file '%s': %s\n", filename, av_err2str(ret));
        return -1;
    }

    // 获取流信息
    if ((ret = avformat_find_stream_info(*format_ctx, NULL)) < 0) {
        fprintf(stderr, "Could not find stream information\n");
		//fprintf(stderr, "Could not find stream information: %s\n", av_err2str(ret));
        return -1;
    }
	return 0;
}

int media_delete_file_info(MediaFormatContext *format_ctx)
{
	avformat_close_input(&format_ctx);
}






uint8_t media_is_network_file(const char *path) {
    if (!path) return 0;

    // 常见网络协议前缀列表（FFmpeg 支持的标准协议）
    const char *protocols[] = {
        "http://", "https://", "rtmp://", "rtsp://", 
        "udp://", "tcp://", "srt://", "ftp://", NULL
    };

    // 检查路径是否以协议前缀开头
    for (int i = 0; protocols[i]; i++) {
        if (strncasecmp(path, protocols[i], strlen(protocols[i])) == 0) {
            return 1;
        }
    }
    return 0;
}




//媒体初始化
//en_net:是否初始化ffmpeg网络模块
int media_init(uint8_t en_net)
{
#if LIBAVFORMAT_VERSION_MAJOR < 58		//高版本的ffmpeg的av_register_all在内部自动执行
    av_register_all();
#endif
	if(en_net)
		avformat_network_init();
}


int media_deinit(uint8_t en_net)
{
	if(en_net)
		avformat_network_init();
}


