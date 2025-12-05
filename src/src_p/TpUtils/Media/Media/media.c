

#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include "tools/utilslib.h"
#include "Media/media.h"
#include "Media/media_file_list.h"
#include "Audio/filter.h"

// 获取文件信息
int media_get_file_info(const char *url, MediaFormatContext **format_ctx)
{
    int ret;
    if ((ret = avformat_open_input(format_ctx, url, NULL, NULL)) < 0)
    {
        fprintf(stderr, "Could not open input file '%s'\n", url);
        // fprintf(stderr, "Could not open input file '%s': %s\n", filename, av_err2str(ret));
        return -1;
    }

    // 获取流信息
    if ((ret = avformat_find_stream_info(*format_ctx, NULL)) < 0)
    {
        fprintf(stderr, "Could not find stream information\n");
        // fprintf(stderr, "Could not find stream information: %s\n", av_err2str(ret));
        avformat_close_input(format_ctx);
        return -1;
    }
    return 0;
}

int media_delete_file_info(MediaFormatContext *format_ctx)
{
    if(!format_ctx)
        return 0;
    avformat_close_input(&format_ctx);
    return 0;
}

uint8_t media_is_network_file(const char *path)
{
    if (!path)
        return 0;

    // 常见网络协议前缀列表（FFmpeg 支持的标准协议）
    const char *protocols[] = {
        "http://", "https://", "rtmp://", "rtsp://",
        "udp://", "tcp://", "srt://", "ftp://", NULL};

    // 检查路径是否以协议前缀开头
    for (int i = 0; protocols[i]; i++)
    {
        if (strncasecmp(path, protocols[i], strlen(protocols[i])) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// 媒体初始化
// en_net:是否初始化ffmpeg网络模块
int media_init(uint8_t en_net)
{
#if LIBAVFORMAT_VERSION_MAJOR < 58 // 高版本的ffmpeg的av_register_all在内部自动执行
    av_register_all();
#endif
    if (en_net)
        avformat_network_init();
    return 0;
}

int media_deinit(uint8_t en_net)
{
    if (en_net)
        avformat_network_deinit();
    return 0;
}

// 获取秒级时长
double media_get_url_duration_sec(MediaFormatContext *format_ctx)
{
    return format_ctx->duration / (double)AV_TIME_BASE;
}







struct MediaUserParams *media_user_config_creat()
{
    struct MediaUserParams *conf = (struct MediaUserParams *)malloc(sizeof(struct MediaUserParams));
    if(!conf)
        return NULL;
    conf->is_playing = false;
    conf->position_s = -1;
    conf->position_p = 0; 
    conf->speed = 1.0;
    conf->filter = NULL;
    conf->position_bytes = 0;
    conf->state = MEDIA_STATE_STOP;
    conf->cmd = MEDIA_PLCMD_STOP;
    conf->audio_params = NULL;
    conf->video_params = NULL;
  
    struct MediaFileList *list = creat_media_file_list();
    if (list == NULL)
    {
        perror("list creat error\n");
        free(conf);
        return NULL;
    }
    conf->list = list;

    struct PthreadCond *pthread_cond = pthread_cond_creat_struct();
    if (pthread_cond == NULL)
    {
        delete_media_file_list(conf->list);
        free(conf);
        return NULL;
    }

    conf->cond = pthread_cond;
    pthread_rwlock_init(&conf->rw_mut, NULL);

    conf->command_get = Media_Get_Command;

    return conf;
}

void media_user_config_delete(struct MediaUserParams *conf)
{
    if (!conf)
        return;
    pthread_rwlock_destroy(&conf->rw_mut);
    pthread_cond_free_struct(conf->cond);
    conf->cond = NULL;
    delete_media_file_list(conf->list);
    if (conf->filter)
        audio_filter_delete(conf->filter);
    conf->filter = NULL;
    free(conf);
    
    conf = NULL;
}



int Audio_Set_BitsPerSample(struct MediaUserParams *conf, uint32_t byte)
{
    if (!conf)
        return -1;
    THREAD_WRITE_USERCONF(conf->rw_mut, conf->nAvgBitsPerSample, byte);
    return 0;
}

int Audio_Get_BitsPerSample(struct MediaUserParams *conf)
{
    if (!conf)
        return -1;
    uint32_t byte=0;
    THREAD_READ_USERCONF(conf->rw_mut, conf->nAvgBitsPerSample, byte);
    return byte;
}

// 内部获取用户设置的播放位置(只允许内部调用)
int Media_Get_Position_S(struct MediaUserParams *conf)
{
    if (!conf)
        return -1;
    int32_t position;
    pthread_rwlock_rdlock(&conf->rw_mut);
    position = conf->position_s;
    if (conf->position_s >= 0)
        conf->position_s = -1;
    pthread_rwlock_unlock(&conf->rw_mut);
    return position;
}
// 内部设置实时播放位置(只允许内部调用)
int Media_Set_Position_N(struct MediaUserParams *conf, int32_t position)
{
    if (!conf)
        return -1;
    pthread_rwlock_wrlock(&conf->rw_mut);
    conf->position_p = position;
    pthread_rwlock_unlock(&conf->rw_mut);
    return 0;
}
// 设置位置（设置值小于0不生效）
int Media_Set_Position(struct MediaUserParams *conf, int32_t position)
{
    if (!conf)
        return -1;
    THREAD_WRITE_USERCONF(conf->rw_mut, conf->position_s, position);
    return 0;
}
// 获取位置（音频使用字节数计算）
int Media_Get_Position(struct MediaUserParams *conf)
{
    int nbyte=0;
    if ((nbyte=Audio_Get_BitsPerSample(conf))==0)
    {
        return ((int)(Media_Get_DPosition(conf)));
    }
    int64_t bytes = Media_Get_BytePosition(conf);
    int position = bytes / nbyte;
    return position;
}

// 获取双精度位置
double Media_Get_DPosition(struct MediaUserParams *conf)
{
    if (!conf)
        return -1;
    double position;
    THREAD_READ_USERCONF(conf->rw_mut, conf->position_p, position);
    return position;
}

int64_t Media_Get_BytePosition(struct MediaUserParams *conf)
{
    if (!conf)
        return -1;
    int64_t position;
    THREAD_READ_USERCONF(conf->rw_mut, conf->position_bytes, position);
    return (int64_t)position;
}

int64_t Media_Set_BytePosition(struct MediaUserParams *conf, int64_t position)
{
    if (!conf)
        return -1;
    THREAD_WRITE_USERCONF(conf->rw_mut, conf->position_bytes, position);
    return 0;
}




// 设置状态为开始，录音的时候必须传file，播放的时候可以不传，如果传的话会自动加入到播放列表末尾
int Media_Set_Start(struct MediaUserParams *conf, const char *file)
{
    int state = Media_Get_State(conf);
    if (state != MEDIA_STATE_STOP) // EXIT后不允许开始
        return -1;
    Media_Set_Command(conf, MEDIA_PLCMD_NEXT);
    if (file != NULL)
    {
        Media_Add_File(conf, file);
    }
    conf->cond->send(conf->cond);
    Media_Set_State(conf, MEDIA_STATE_START);
    return 0;
}

// 设置播放状态-暂停播放
int Media_Set_Suspend(struct MediaUserParams *conf)
{
    printf("Media_Set_Suspend\n");
    return Media_Set_Command(conf, MEDIA_PLCMD_SUSPEND);
}
// 设置播放状态-继续播放
int Media_Set_Continue(struct MediaUserParams *conf)
{
    printf("Media_Set_Continue\n");
    if (Media_Get_State(conf) != MEDIA_STATE_PAUSEING)
        return -1;
    Media_Set_Command(conf, MEDIA_PLCMD_CONTINUE);
    conf->cond->send(conf->cond);
    return 0;
}
// 获取播放状态
int Media_Get_State(struct MediaUserParams *conf)
{
    if (!conf)
        return -1;
    int state = 0;
    THREAD_READ_USERCONF(conf->rw_mut, conf->state, state);
    return state;
}

// 获取命令
int Media_Get_Command(struct MediaUserParams *conf)
{
    int cmd = 0;
    THREAD_READ_USERCONF(conf->rw_mut, conf->cmd, cmd);
    return cmd;
}

// 设置播放状态(只允许内部使用)
int Media_Set_Command(struct MediaUserParams *conf, MediaPlayCommand cmd)
{
    if (!conf)
        return -1;
    THREAD_WRITE_USERCONF(conf->rw_mut, conf->cmd, cmd);
    return 0;
}




// 设置播放状态(只允许内部使用)
int Media_Set_State(struct MediaUserParams *conf, MediaPlayState state)
{
    if (!conf)
        return -1;
    THREAD_WRITE_USERCONF(conf->rw_mut, conf->state, state);
    return 0;
}

// 停止播放,只是停止不会关闭声卡，不同于暂停，停止会清空大多数播放信息
int Media_Set_Stop(struct MediaUserParams *conf)
{
    if (Media_Get_State(conf) == MEDIA_STATE_PAUSEING)
        conf->cond->send(conf->cond);
    return Media_Set_Command(conf, MEDIA_PLCMD_STOP);
}
// 关闭声卡
int Audio_Set_Close(struct MediaUserParams *conf)
{
    int state = Media_Get_State(conf);
    if (state < 0)
        return -1;
    if (state != MEDIA_STATE_STOP)
        Media_Set_Stop(conf);
    Media_Set_Command(conf, MEDIA_PLCMD_EXIT);
    conf->cond->send(conf->cond); // 防止处于等待状态
    return 0;
}

// 是否退出
int Media_State_Is_Exit(struct MediaUserParams *conf)
{
    return (Media_Get_State(conf) == MEDIA_STATE_EXIT) ? 1 : 0;
}

// 播放速度
float Media_Get_Speed(struct MediaUserParams *conf)
{
    if (!conf)
        return -1;
    float speed = 1.0;
    THREAD_READ_USERCONF(conf->rw_mut, conf->speed, speed);
    return speed;
}

int Media_Set_Speed(struct MediaUserParams *conf, float speed)
{
    if (!conf)
        return -1;
    if (speed <= 0)
        return -1;
    speed = limit_min_max(speed, USER_CONF_SPEED_MIN, USER_CONF_SPEED_MAX);
    THREAD_WRITE_USERCONF(conf->rw_mut, conf->speed, speed);
    return 0;
}

// 添加播放文件
int Media_Add_File(struct MediaUserParams *conf, const char *file)
{
    char *file_new = strdup(file);
    if (!file_new)
        return -1;
    //	pthread_rwlock_wrlock(&conf->rw_mut);
    conf->list->insert_end_saft(conf->list, file_new);
    //	pthread_rwlock_unlock(&conf->rw_mut);
    return 0;
}
// 删除播放文件
int Media_Del_File(struct MediaUserParams *conf, const char *file)
{
    char *file_new = strdup(file);
    if (!file_new)
        return -1;
    //	pthread_rwlock_wrlock(&conf->rw_mut);
    conf->list->delete_file_saft(conf->list, file_new);
    //	pthread_rwlock_unlock(&conf->rw_mut);
    return 0;
}

// 播放文件（将需要播放的文件插入到播放列表的正在播放的位置的下一个并且直接取消当前文件的播放）
int Media_Set_Play(struct MediaUserParams *conf, const char *file)
{
    char *file_new = strdup(file);
    if (!file_new)
        return -1;
    //	pthread_rwlock_wrlock(&conf->rw_mut);
    conf->list->insert_pos_saft(conf->list, file_new);
    //	pthread_rwlock_unlock(&conf->rw_mut);
    return Media_Play_Next(conf);
}

// 下一个(仅针对列表)
int Media_Play_Next(struct MediaUserParams *conf)
{
    return Media_Set_Command(conf, MEDIA_PLCMD_NEXT);
}

// 上一个(仅针对列表)
int Media_Play_Last(struct MediaUserParams *conf)
{
    return Media_Set_Command(conf, MEDIA_PLCMD_LAST);
}

int Media_Set_Is_Playing(struct MediaUserParams *conf, bool is_playing)
{
    if (!conf)
        return -1;
    THREAD_WRITE_USERCONF(conf->rw_mut, conf->is_playing, is_playing);
    return 0;
}

bool Media_Get_Is_Playing(struct MediaUserParams *conf)
{
    if (!conf)
        return false;
    bool is_playing;
    THREAD_READ_USERCONF(conf->rw_mut, conf->is_playing, is_playing);
    return is_playing;
}

// 设置正在播放的音频的时长
int Media_Set_Length(struct MediaUserParams *conf, double length)
{
    THREAD_WRITE_USERCONF(conf->rw_mut, conf->length, length);
    return 0;
}

// 获取正坐在播放的音频时长
double Media_Get_Length(struct MediaUserParams *conf)
{
    if (!conf)
        return -1;
    double length;
    THREAD_READ_USERCONF(conf->rw_mut, conf->length, length);
    return length;
}