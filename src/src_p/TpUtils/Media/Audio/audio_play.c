/*///------------------------------------------------------------------------------------------------------------------------//
        音频播放相关
说 明 : 视频播放需要借助于此程序
日 期 : 2024.11.26

/*/
//------------------------------------------------------------------------------------------------------------------------//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include "tools/utilslib.h"
#include "Audio/audio_play.h"
#include "filter.h"
#include "Media/Media/media.h"
#include "Media/Media/media_codec.h"


#ifdef DEBUG_AUDIO_PLAY
#include "Log/elog.h"
#define debug_printf(...) elog_d("MediaPlayAudio", ##__VA_ARGS__)
//#define debug_printf(fmt, ...) elog_d(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...) // 如果不定义DEBUG，什么也不做
#endif

CallbackAudioPlay Media_Get_Audio_Callback(struct MediaAudioInfo *conf_v);
void Media_Set_Audio_Callback(struct MediaAudioInfo *conf_v, CallbackAudioPlay cb, void *userdata);
int Audio_Set_BitsPerSample(struct MediaUserParams *conf, uint32_t byte);

// 发送条件变量
int send_pthread_cond(struct PthreadCond *cond)
{
    pthread_mutex_lock(&cond->lock);
    pthread_cond_signal(&cond->cond); // 通知
    pthread_mutex_unlock(&cond->lock);
    return 0;
}
// 等待条件变量
int wait_pthread_cond(struct PthreadCond *cond)
{
    pthread_mutex_lock(&cond->lock);
    pthread_cond_wait(&cond->cond, &cond->lock); // 等待条件变量
    pthread_mutex_unlock(&cond->lock);
    return 0;
}
// 创建条件变量及其配合的互斥锁
struct PthreadCond *pthread_cond_creat_struct()
{
    struct PthreadCond *cond = (struct PthreadCond *)malloc(sizeof(struct PthreadCond));
    if (cond == NULL)
        return NULL;
    if (pthread_mutex_init(&cond->lock, NULL) != 0)
    {
        free(cond);
        return NULL;
    }

    // 动态初始化条件变量
    if (pthread_cond_init(&cond->cond, NULL) != 0)
    {
        pthread_mutex_destroy(&cond->lock);
        free(cond);
        return NULL;
    }
    cond->send = send_pthread_cond;
    cond->wait = wait_pthread_cond;
    return cond;
}

// 释放条件变量以及其锁
int pthread_cond_free_struct(struct PthreadCond *cond)
{
    if (cond == NULL)
        return -1;
    pthread_mutex_destroy(&cond->lock);
    pthread_cond_destroy(&cond->cond);
    return 0;
}

struct MediaAudioInfo *media_audio_info_creat(const char *device)
{
    struct MediaAudioInfo *conf = (struct MediaAudioInfo *)malloc(sizeof(struct MediaAudioInfo));
    if(!conf)
        return NULL;
    conf->device = strdup(device);
    conf->volume = USER_CONF_VOLUME;
    conf->volume = 100;
    
    conf->get_callback_audio = Media_Get_Audio_Callback;
    conf->set_callback_audio = Media_Set_Audio_Callback;
    
    pthread_rwlock_init(&conf->rw_mut, NULL);

    return conf;
}

void media_audio_info_delete(struct MediaAudioInfo *conf)
{
    if(!conf)
        return ;
    if(conf->device)
        free(conf->device);
    conf->device=NULL;
    conf->get_callback_audio = NULL;
    conf->set_callback_audio = NULL;
    pthread_rwlock_destroy(&conf->rw_mut);
    free(conf);
    conf=NULL;
}



// 阻塞直到缓存区数据播放完成
int audio_pcm_drain(struct MediaAudioHandle *pcm)
{
    return snd_pcm_drain(pcm->handle);
}

// 丢弃音频缓存区中的数据
int audio_pcm_drop(struct MediaAudioHandle *pcm)
{
    if (!pcm || !pcm->handle)
        return -1;
    return snd_pcm_drop(pcm->handle);
}
// 关闭音频硬件
int audio_pcm_close(struct MediaAudioHandle *pcm)
{
    if (!pcm || !pcm->handle)
        return -1;
    return snd_pcm_close(pcm->handle);
}

/// @brief 配置PCM的参数
/// @param pcm
/// @return
static int media_pcm_hwparams_init(struct MediaAudioHandle *pcm)
{
    int rc = 0;
    debug_printf("start to open pcm\n");
    if (!pcm || !pcm->handle)
        return -1;
    rc = snd_pcm_hw_params_any(pcm->handle, pcm->hwparams); // 初始化params(如果已经设置万参数了想要重新设置需要调用此函数)
    if (rc < 0)
    {
        perror("\nsnd_pcm_hw_params_any:");
        return -1;
    }
    /*rc = snd_pcm_hw_params_set_rate_resample(pcm->handle, pcm->hwparams, 1);	//启用重采样,(可以硬件自动重采样以播放和硬件的采样率不一致的音频，但是不是所有设备都支持)
    if (rc < 0) {
        debug_printf("\nResampling setup failed for playback: ");
        return -1;
    }*/
    rc = snd_pcm_hw_params_set_access(pcm->handle, pcm->hwparams, SND_PCM_ACCESS_RW_INTERLEAVED); // 初始化访问权限
    if (rc < 0)
    {
        perror("\nsed_pcm_hw_set_access:");
        return -1;
    }
    // rc=snd_pcm_hw_params_set_channels(pcm->handle, pcm->hwparams, numChannels);  // 设置通道数

    return 0;
}

/// @brief audioStreamParams结构体初始化
/// @param wChannels 通道
/// @param nSamplesPersec 采样频率
/// @param wBitsPerSample 数据位数
/// @return
static int audio_stream_params_init(int wChannels, int nSamplesPersec, int wBitsPerSample, struct AudioStreamParams *header)
{
    header->wChannels = wChannels;           // 声道数
    header->nSamplesPersec = nSamplesPersec; // 采样频率
    header->wBitsPerSample = wBitsPerSample; // 样本数据位数
    header->byteFrams = wChannels * wBitsPerSample / 8;
    header->nAvgBitsPerSample = nSamplesPersec * header->byteFrams; // 每秒播放字节数
    return 0;
}

/// @brief 获取声卡的一些功能
/// @param pcm
/// @return
static int pcm_get_function(snd_pcm_hw_params_t *hwparams, struct PcmHardParams *ahparams)
{
    int dir = 0;
    snd_pcm_uframes_t frames, buff_size;

    ahparams->can_pause = snd_pcm_hw_params_can_pause(hwparams); // 检查硬件是否支持暂停
    debug_printf("PCM can_pause'%d'\n", ahparams->can_pause);
    ahparams->can_resume = snd_pcm_hw_params_can_resume(hwparams); // 检查硬件是否支持恢复
    debug_printf("PCM can_resume'%d'\n", ahparams->can_resume);
    if (snd_pcm_hw_params_get_period_size(hwparams, &frames, &dir) < 0) /*获取周期长度*/
        return -1;
    if (snd_pcm_hw_params_get_buffer_size(hwparams, &buff_size) < 0)
        return -1;
    ahparams->cycle_frames = frames;
    ahparams->buff_size = buff_size;

    debug_printf("cycle_frames=%ld,buff_size=%ld\n", frames, buff_size);
    return 0;
}

/// @brief 根据音配的参数配置声卡，会根据最近的配置来尽量配置完成，即实际配置结果和原来可能不一致，配置结果会保存到pcm的里面
/// @param pcm
/// @param header
/// @return
int pcm_hwparams_set(struct MediaAudioHandle *pcm, struct AudioStreamParams *audio)
{
    if (!pcm || !pcm->handle)
        return -1;

    int rc;
    int dir = 0;
    unsigned int channels = PCM_CHANNELS_DEFAULT;
    unsigned int frequency = PCM_SAMPLE_PERSEC_DEFAULT;
    unsigned int bit = PCM_Bits_PER_SAMPLE_DEFAULT;
    if (audio != NULL)
    {
        channels = audio->wChannels;
        frequency = audio->nSamplesPersec;
        bit = audio->wBitsPerSample;
    }
    else
        fprintf(stderr, "set pcm error,used dwefault param\n");
    debug_printf("debug:bit=%d,frequency=%d,channels=%d\n", bit, frequency, channels);
    switch (bit / 8) // 设置采样位数
    {
    case 1:
        snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_U8);
        break;
    case 2:
        snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_S16_LE);
        break;
    case 3:
        snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_S24_LE);
        break;
    case 4:
        snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_S32_LE);
        break;
    default:
        debug_printf("set SND_PCM_FORMAT_U8\n");
        snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_U8);
        break;
    }
    debug_printf("pcm set format%d\n", bit / 8);

    rc = snd_pcm_hw_params_set_channels_near(pcm->handle, pcm->hwparams, &channels); // 设置声道,1表示单声道，2表示立体声
    if (rc < 0)
    {
        perror("\nsnd_pcm_hw_params_set_channels:");
        return -1;
    }
    debug_printf("pcm set channel:%d\n", channels);

    rc = snd_pcm_hw_params_set_rate_near(pcm->handle, pcm->hwparams, &frequency, &dir); // 设置频率
    if (rc < 0)
    {
        perror("\nsnd_pcm_hw_params_set_rate_near:");
        return -1;
    }
    debug_printf("pcm set rate:%d\n", frequency);

    // 根据获取的缓存区调整周期大小（只有WAV才设置，其他类型文件不需要设置）(理论不用设置，但是在读取缓存区各种帧数的时候貌似会出问题，暂时先全部设置)
    // if(pcm->file_type==AUDIO_FILE_TYPE_WAV)
    {
        snd_pcm_uframes_t frames = PCM_BUFFER_FRAMES;
        snd_pcm_uframes_t buffer_size = PCM_BUFFER_FRAMES * PCM_BUFFER_SIZE * channels * (bit / 8);
        if (snd_pcm_hw_params_set_period_size_near(pcm->handle, pcm->hwparams, &frames, 0) < 0) // 设置周期长度
        {
            perror("snd_pcm_hw_params_set_period_size,error");
            debug_printf("snd_pcm_hw_params_set size=%ld\n", frames);
            // return -1;
        }
        if (snd_pcm_hw_params_set_buffer_size_near(pcm->handle, pcm->hwparams, &buffer_size) < 0)
        {
            perror("snd_pcm_hw_params_set_buffer_size_near error");
            // return -1;
        }
        debug_printf("pcm set period size:%ld  buffer_size%ld\n", frames, buffer_size);
    }

    // 应用pcm参数
    rc = snd_pcm_hw_params(pcm->handle, pcm->hwparams);
    if (rc < 0)
    {
        debug_printf("unable to set hw parameters: %s\n", snd_strerror(rc));
        snd_pcm_state_t state = snd_pcm_state(pcm->handle);
        fprintf(stderr, "current state = %s\n", snd_pcm_state_name(state));
        return -1;
    }

    // 获取设备的一些功能，一般在设置完参数后调用
    pcm_get_function(pcm->hwparams, pcm->ahparams);

    // 设置完后修改最终成功设置的参数
    if (audio != NULL)
    {
        audio_stream_params_init(channels, frequency, bit, audio);
        memcpy(pcm->adparams, audio, sizeof(struct AudioStreamParams));
        /*pcm->adparams->nAvgBitsPerSample=audio->nAvgBitsPerSample;
        pcm->adparams->wBitsPerSample=audio->wBitsPerSample;
        pcm->adparams->nSamplesPersec=audio->nSamplesPersec;
        pcm->adparams->wChannels=audio->wChannels;
        pcm->adparams->bitRate=audio->bitRate;
        pcm->adparams->rLen=audio->rLen;*/
    }
    return 0;
}

static int pcm_start_play(struct MediaAudioHandle *pcm)
{
    if (!pcm || !pcm->handle)
        return -1;
    int rc = 0;
    // 准备播放
    if ((rc = snd_pcm_prepare(pcm->handle)) < 0)
    { // 在第一次设置时可以不需要准备播放，播放后重新设置需要准备播放
        perror("无法准备播放:");
        audio_pcm_close(pcm);
        return -1;
    }
    snd_pcm_start(pcm->handle);
    //	audio_pcm_drop(pcm);
    debug_printf("PCM handle name = '%s'\n", snd_pcm_name(pcm->handle));
    return 0;
}

/// @brief 暂停播放
/// @param pcm
/// @return
static int pcm_play_stop(struct MediaAudioHandle *pcm)
{
    if (!pcm || !pcm->handle)
        return -1;
    int err;

    if (pcm->ahparams->can_pause)
    {
        if ((err = snd_pcm_pause(pcm->handle, 1)) < 0)
        {
            //		    mp_msg(MSGT_AO,MSGL_ERR,MSGTR_AO_ALSA_PcmPauseError, snd_strerror(err));
            return -1;
        }
    }
    else
    {
        if ((err = audio_pcm_drop(pcm)) < 0)
        {
            //			mp_msg(MSGT_AO,MSGL_ERR,MSGTR_AO_ALSA_PcmDropError, snd_strerror(err));
            return -1;
        }
    }
    return 0;
}

/// @brief 继续播放
/// @param pcm
/// @return
static int pcm_play_continue(struct MediaAudioHandle *pcm)
{
    if (!pcm || !pcm->handle)
        return -1;
    int err;
    if (snd_pcm_state(pcm->handle) == SND_PCM_STATE_SUSPENDED)
    {
        //    	mp_msg(MSGT_AO,MSGL_INFO,MSGTR_AO_ALSA_PcmInSuspendModeTryingResume);
        while ((err = snd_pcm_resume(pcm->handle)) == -EAGAIN)
            sleep(1);
    }
    if (pcm->ahparams->can_pause)
    {
        if ((err = snd_pcm_pause(pcm->handle, 0)) < 0)
        {
            //      	mp_msg(MSGT_AO,MSGL_ERR,MSGTR_AO_ALSA_PcmResumeError, snd_strerror(err));
            return -1;
        }
        //	mp_msg(MSGT_AO,MSGL_V,"alsa-resume: resume supported by hardware\n");
    }
    else
    {
        if ((err = snd_pcm_prepare(pcm->handle)) < 0)
        {
            //			mp_msg(MSGT_AO,MSGL_ERR,MSGTR_AO_ALSA_PcmPrepareError, snd_strerror(err));
            return -1;
        }
    }
    return 0;
}

/// @brief 软件调节音量，标准处理方式应该是按照不同的format来处理，但是由于本程序默认按照小端模式来设置声卡了，所以只考虑三种模式(8,16,24位)
/// @param buffer 数据
/// @param frames buffer的侦数
/// @param channels 通道数
/// @param volume 音量
/// @param bit 采样位数，
static void pcm_data_adjust_volume(uint8_t *buffer, size_t frames, int channels, float volume, uint16_t bit)
{
    if (bit == 16)
    { // SND_PCM_FORMAT_S16_LE
        int16_t *data = (int16_t *)buffer;
        for (size_t i = 0; i < frames * channels; i++)
        {
            data[i] = (int16_t)(data[i] * volume);
        }
    }
    else if (bit == 8)
    { // SND_PCM_FORMAT_U8
        uint8_t *data = buffer;
        for (size_t i = 0; i < frames * channels; i++)
        {
            int value = (int)(data[i] - 128) * volume + 128; // U8 数据需考虑偏移量
            data[i] = (uint8_t)(value < 0 ? 0 : (value > 255 ? 255 : value));
        }
    }
    else if (bit == 24)
    { // SND_PCM_FORMAT_S24_LE
        // 每个采样点占用 3 字节（24 位）
        for (size_t i = 0; i < frames * channels; i++)
        {
            // 将 3 字节数据读取为 32 位有符号整数
            int32_t sample = (buffer[i * 3 + 2] << 16) | (buffer[i * 3 + 1] << 8) | buffer[i * 3];
            if (sample & 0x800000)
            { // 检测符号位并扩展到 32 位
                sample |= 0xFF000000;
            }
            // 调节音量
            sample = (int32_t)(sample * volume);
            // 防止溢出，截断到 24 位
            if (sample > 8388607)
                sample = 8388607;
            if (sample < -8388608)
                sample = -8388608;
            // 将结果写回 3 字节
            buffer[i * 3] = sample & 0xFF;
            buffer[i * 3 + 1] = (sample >> 8) & 0xFF;
            buffer[i * 3 + 2] = (sample >> 16) & 0xFF;
        }
    }
    else if (bit == 32)
    { // SND_PCM_FORMAT_S32_LE
        int32_t *data = (int32_t *)buffer;
        for (size_t i = 0; i < frames * channels; i++)
        {
            // 调节音量
            data[i] = (int32_t)(data[i] * volume);
            // 防止溢出，限制在 32 位整数范围内
            if (data[i] > 2147483647)
                data[i] = 2147483647;
            if (data[i] < -2147483648)
                data[i] = -2147483648;
        }
    }
    else
    {
        fprintf(stderr, "Unsupported format for volume adjustment,bit=%d\n", bit);
    }
}

/// @brief 向pcm写入音频数据
/// @param pcm
/// @param buffer
/// @param frames :帧数(即采样点的数量)
/// @param delay :写入错误时的阻塞时长,单位ms
/// @return
int pcm_write_data(struct MediaAudioHandle *pcm, uint8_t *buffer, unsigned long frames, int delay)
{
    int time = 0;
    snd_pcm_sframes_t avail;
    snd_pcm_sframes_t delay_p;
    int err;
    delay = delay / 5; // 每次延时5ms;
    // 获取播放设备当前的延迟（缓冲区剩余的帧数）
    while (1)
    {
        // 获取当前硬件可写入的帧数
        avail = snd_pcm_avail(pcm->handle);
        if (avail > frames)
        { // 如果可用帧数足够
            // 可以继续写入数据
            err = snd_pcm_writei(pcm->handle, buffer, frames);
            if (err == -EPIPE)
            {
                // fprintf(stderr, "Buffer underrun occurred\n");
                snd_pcm_prepare(pcm->handle);
            }
            /*else if (err == -EAGAIN) {
                // 缓冲区满
                ;
            }*/
            else if (err < 0)
            {
                fprintf(stderr, "Error writing to PCM device（%d）: %s\n", err, snd_strerror(err));
                // return -1;		//直接返回还是重写尝试写入？？
            }
            // debug_printf("write ok\n");

            // debug_printf("Delay: %ld frames, Avail: %ld frames,My frams%ld\n", delay_p, avail,frames);
            break; // 跳出内部循环，继续读取数据
        }
        else if (avail == -EAGAIN)
            snd_pcm_prepare(pcm->handle);
        // 如果可用帧数不足，检查设备延迟
        err = snd_pcm_delay(pcm->handle, &delay_p); // 返回内部缓存中尚未播放的音频的侦数
        if (err < 0)
        {
            fprintf(stderr, "snd_pcm_delay failed: %s\n", snd_strerror(err));
            break;
        }

        // 输出当前延迟信息
        // debug_printf("Delay: %ld frames, Avail: %ld frames,My frams%ld\n", delay_p, avail,frames);
        time++;
        if (time > delay)
            return -1;
        // 等待一段时间再继续检查
        usleep(5000); // 等待 5 毫秒后再检查
    }
    return err;
}

/// @brief 写入硬件的音频流接口写入硬件的音频流接口
/// @param pcm_play
/// @param conf
/// @param audio_param
/// @param buffer 缓存区
/// @param frames 原始的帧数
/// @param volume 音量，设置为-1的时候自己从conf中读取
/// @param offset 位置偏移，设置为-1的时候需要上层自己写入位置，设置为正值的时候自动写入(当前只作为标志位使用)
/// @param delay 阻塞时长，
/// @return
int audio_stream_write(struct MediaAudioHandle *pcm_play, struct MediaUserParams *conf,
                        uint8_t *buffer, uint32_t frames,
                        float volume,
                        int offset, int delay)
{

    int ret = 0;
    float volume_set;
    struct AudioStreamParams *audio_param = pcm_play->adparams;
    static struct MediaFilterParam *filter = NULL;
    AVFrame *frame_flt = NULL;
    int64_t position = Media_Get_BytePosition(conf); // 获取当前播放位置
    uint8_t *data = ((AVFrame *)buffer)->data[0];    // 输出的数据
    uint32_t data_frames = frames;
    static float speed_l = 0.0;
    static uint16_t wChannels_l = 0, wBitsPerSample_l = 0;
    static uint32_t nSamplesPersec_l = 0;

    float speed = Media_Get_Speed(conf);
    if ((speed_l != speed ||
            audio_param->wChannels != wChannels_l ||
            audio_param->wBitsPerSample != wBitsPerSample_l ||
            audio_param->nSamplesPersec != nSamplesPersec_l) &&
        speed != 1) // 位置为0认为是新的一首开始播放，或者两次速度不相等，都需要重新设置过滤器
    {

        if (filter)
            audio_filter_delete(filter);
        filter = audio_filter_creat_init(speed, audio_param->nSamplesPersec, audio_param->wChannels, code_get_channel_layout(audio_param->wChannels), code_get_format(audio_param->wBitsPerSample));
        if (!filter)
        {
            ret = -1;
            goto WRITE_POS;
        }
        conf->filter = filter;
        speed_l = speed;
        wChannels_l = audio_param->wChannels;
        wBitsPerSample_l = audio_param->wBitsPerSample;
        nSamplesPersec_l = audio_param->nSamplesPersec;
    }
    if (speed != 1)
    {
        AVFrame *convert_frame = (AVFrame *)buffer;
        frame_flt = av_frame_alloc(); // 提前申请

        if (!frame_flt)
        {
            ret = -1;
            goto WRITE_POS;
        }
        media_filte_get_data(filter, convert_frame, frame_flt);
        data = frame_flt->data[0];
        data_frames = frame_flt->nb_samples;
    }

    if (volume < 0)
    {
        volume_set = Audio_Get_Volume(conf->audio_params);
        volume_set *= 0.01;
    }
    else
        volume_set = volume;

    pcm_data_adjust_volume(data, data_frames, audio_param->wChannels, volume_set, audio_param->wBitsPerSample);
    ret = pcm_write_data(pcm_play, data, data_frames, delay);

FREE_FLT:
    if (frame_flt)
    {
        av_frame_free(&frame_flt);
    }
WRITE_POS:
    //debug_printf("offset:%d,audio_param->nAvgBitsPerSample:%d,进度：%d\n",offset,audio_param->nAvgBitsPerSample,position);
    if (offset >= 0 && conf->nAvgBitsPerSample != 0) // 如果要自行设置offset直接传入-1即可
    {
        position += (frames * audio_param->byteFrams);
        Media_Set_BytePosition(conf, (int64_t)position);
        //printf("[Debug]: Media_Set_BytePosition %d \n",position);
    }

    return ret;
}



// 手动设置
int Audio_Hard_Hand_Init(struct MediaAudioHandle *pcm_play, struct MediaUserParams *conf, struct AudioStreamParams *stream_params)
{
    if (!pcm_play || !pcm_play->handle)
    {
        return -1;
    }
    if (pcm_hwparams_set(pcm_play, stream_params) < 0)
        return -1;
    if (pcm_start_play(pcm_play) < 0)
        return -1;
    return 0;
}


// 设置音量
int Audio_Set_Volume(struct MediaAudioInfo *conf_a, int16_t volume)
{
    if (!conf_a)
        return -1;
    volume = (int16_t)limit_min_max(volume, USER_CONF_VOLUME_MIN, USER_CONF_VOLUME_MAX);
   
    THREAD_WRITE_USERCONF(conf_a->rw_mut, conf_a->volume, volume);
    return 0;
}

// 获取音量
int Audio_Get_Volume(struct MediaAudioInfo *conf_a)
{
    if (!conf_a)
        return -1;
    int16_t volume;
    THREAD_READ_USERCONF(conf_a->rw_mut, conf_a->volume, volume);
    return volume;
}


CallbackAudioPlay Media_Get_Audio_Callback(struct MediaAudioInfo *conf_v)
{
    CallbackAudioPlay cb;
    THREAD_READ_USERCONF(conf_v->rw_mut, conf_v->callback_audio, cb);
    return cb;
}

void Media_Set_Audio_Callback(struct MediaAudioInfo *conf_v, CallbackAudioPlay cb, void *userdata)
{
    if (!conf_v)
        return;
    pthread_rwlock_rdlock(&conf_v->rw_mut);
    conf_v->callback_audio = cb;
    conf_v->userdata = userdata;
    pthread_rwlock_unlock(&conf_v->rw_mut);
}


// 播放音频流
int Audio_Set_Stream(struct MediaUserParams *conf, void *data)
{
    int state = Media_Get_State(conf);
    return 0;
}



//设置声卡
int Audio_Set_Card(struct MediaAudioInfo *conf_a,const char *card)
{
    if (!conf_a)
        return -1;
    pthread_rwlock_rdlock(&conf_a->rw_mut);
    if(conf_a->device)
        free(conf_a->device);
    conf_a->device=strdup(card);
    pthread_rwlock_unlock(&conf_a->rw_mut);
}



/// @brief 打开Audio设备
/// @param device 声卡名字
/// @return
struct MediaAudioHandle *Audio_Play_Open(const char *device)
{
    struct MediaAudioHandle *pcm_play = (struct MediaAudioHandle *)malloc(sizeof(struct MediaAudioHandle));
    if (pcm_play == NULL)
        return NULL;
    if (Audio_Device_Init(pcm_play, device, AUDIO_STREAM_PLAYBACK) < 0)
    {
#ifdef NONE_AUDIO_CARD_PLAY
        return pcm_play;
#endif
        free(pcm_play);
        return NULL;
    }
    return pcm_play;
}

int Audio_Device_Init(struct MediaAudioHandle *pcm_play, const char *device, AudioStreamType type)
{
    int err = 0;
    pcm_play->ahparams = NULL;
    pcm_play->adparams = NULL;
    if (device == NULL)
        device = "default";
    //	int len=strlen(device)+1;
    //	pcm_play->device=(char *)malloc(len);
    //	strncpy(pcm_play->device,device,len-1);
    if (snd_pcm_open(&pcm_play->handle, device, (snd_pcm_stream_t)type, 0) < 0) // SND_PCM_STREAM_CAPTURE
    {
        fprintf(stderr, "[Warning]: unable to open pcm device\n");
        pcm_play->handle = NULL;
        return -1;
    }

    err = snd_pcm_hw_params_malloc(&pcm_play->hwparams); // 分配hwparam,snd_pcm_hw_params_alloca
    if (err < 0)
    {
        fprintf(stderr, "unable to alloca hwparams\n");
        audio_pcm_close(pcm_play);
        return -1;
    }
    if (media_pcm_hwparams_init(pcm_play) < 0) // 初始化hwparams
    {
        debug_printf("media_pcm_hwparams_init error\n");
        snd_pcm_hw_params_free(pcm_play->hwparams);
        audio_pcm_close(pcm_play);
        return -1;
    }
    pcm_play->adparams = (struct AudioStreamParams *)malloc(sizeof(struct AudioStreamParams));
    pcm_play->ahparams = (struct PcmHardParams *)malloc(sizeof(struct PcmHardParams));
    return 0;
}

// 关闭声卡硬件
int Audio_Device_Close(struct MediaAudioHandle *pcm_play)
{
    if (!pcm_play || !pcm_play->handle)
        return -1;
    if (pcm_play->hwparams)
        snd_pcm_hw_params_free(pcm_play->hwparams);
    if (pcm_play->handle)
    {
        audio_pcm_drain(pcm_play);
        audio_pcm_close(pcm_play);
    }
    if (pcm_play->adparams)
        free(pcm_play->adparams);
    if (pcm_play->ahparams)
        free(pcm_play->ahparams);
    //	if(pcm_play->device)
    //		free(pcm_play->device);
    return 0;
}

// 向初始化并配置好的设备写入音频流
int Audio_Write_Stream(struct MediaAudioHandle *pcm, struct MediaUserParams *conf, struct AudioStreamParams *hard_params,
                        uint8_t *buffer, uint32_t frames, int offset, int delay)
{
    if (!pcm || !pcm->handle)
        return -1;
    int state = Media_Get_State(conf);
    if (state != MEDIA_STATE_PAUSEING && state != MEDIA_STATE_STOP)
    {
        fprintf(stderr, "声卡只能在未播放音频的时候才能设置\n");
        return -1;
    }
    return audio_stream_write(pcm, conf, buffer, frames, -1, offset, delay);
}

// 设置非阻塞(只允许在初始状态/停止状态/暂停状态可以设置)
int Audio_Set_Nonblock(struct MediaAudioHandle *pcm_play, struct MediaUserParams *conf, uint8_t nonblock)
{
    if (!pcm_play || !pcm_play->handle)
        return -1;
    int state = Media_Get_State(conf);
    if (state != MEDIA_STATE_PAUSEING && state != MEDIA_STATE_STOP) // 只有这三种状态可以设置非阻塞
        return -1;
    if (snd_pcm_nonblock(pcm_play->handle, nonblock) < 0)
    {
        perror("set nonblock error:");
        return -1;
    }
    return 0;
}

// 硬件设置
int Audio_Set_Hard_Params(struct MediaAudioHandle *pcm_play, struct MediaUserParams *conf, uint32_t rate, uint16_t channel, uint16_t bits)
{
    if (!conf)
        return -1;
    int state = Media_Get_State(conf);
    if (state != MEDIA_STATE_STOP)
        return -1;
    struct AudioStreamParams hard_params;
    hard_params.nSamplesPersec = rate;
    hard_params.wChannels = channel;
    hard_params.wBitsPerSample = bits;
    if (Audio_Hard_Hand_Init(pcm_play, conf, &hard_params) < 0)
    {
        return -1;
    }
    return 0;
}



//根据文件类型获取对应的解码器
enum AVCodecID get_codeid_from_file_type(MediaFileType type)
{
	switch(type)
	{
		case MEDIA_FILE_TYPE_MP3:	return AV_CODEC_ID_MP3;
		case MEDIA_FILE_TYPE_M4A:	return AV_CODEC_ID_AAC;

	}
	return AV_CODEC_ID_MP3;
}



//按照音频参数和帧数申请合适大小的AVFrame
AVFrame *alloc_avframe_frames_hard(int frames,struct AudioStreamParams *hard_param)
{
	AVFrame *converted_frame=av_frame_alloc();
	if(!converted_frame)
	{
		fprintf(stderr, "Alloc for package uint8 to frame error\n");
		return NULL;
	}
	converted_frame->nb_samples = frames;
	converted_frame->format = code_get_format(hard_param->wBitsPerSample); 
	converted_frame->channels = hard_param->wChannels;	
	converted_frame->channel_layout=code_get_channel_layout(hard_param->wChannels);
	converted_frame->sample_rate = hard_param->nSamplesPersec; 

	if (av_frame_get_buffer(converted_frame, 0) < 0) {
		fprintf(stderr, "Could not allocate frame buffer\n");
		av_frame_free(&converted_frame);
		return NULL;
	}
	return converted_frame;
}
//释放AVFrame
int free_avframe(AVFrame **converted_frame)
{
	av_frame_free(converted_frame);
}


//获取解码器的AVSampleFormat
enum AVSampleFormat code_get_format(uint16_t wBitsPerSample)
{
	enum AVSampleFormat fmt=AV_SAMPLE_FMT_NONE;
	switch(wBitsPerSample)
	{
		case 8: fmt=AV_SAMPLE_FMT_U8; break;
		case 16:fmt=AV_SAMPLE_FMT_S16;break;
		case 24:fmt=AV_SAMPLE_FMT_S32;break;
		case 32:fmt=AV_SAMPLE_FMT_S32;break;
		default:fmt=AV_SAMPLE_FMT_U8; break;
	}
	return fmt;
}

//获取解码器的channel_layout
int64_t code_get_channel_layout(int channels)
{
	int64_t channel_layout = av_get_default_channel_layout(channels);
	return channel_layout;
}

//使用硬件配置重新设置重采样参数
struct SwrContext *swr_set_with_hard_param(AVCodecContext *codec_ctx,struct AudioStreamParams *hard_param)
{
	struct SwrContext *swr_ctx = swr_alloc_set_opts(NULL,
									code_get_channel_layout(hard_param->wChannels),
									code_get_format(hard_param->wBitsPerSample),
									hard_param->nSamplesPersec,
									codec_ctx->channel_layout,
									codec_ctx->sample_fmt,
									codec_ctx->sample_rate,
									0,
									NULL);	
	return swr_ctx;
}


int get_audio_params_wav(FILE *fp,struct AudioStreamParams *params)
{
	AudioWavHeader wav_header;
	get_wav_header_info(fp,&wav_header);
	params->nAvgBitsPerSample=wav_header.nAvgBitsPerSample;
	params->nSamplesPersec=wav_header.nSamplesPersec;
	params->wBitsPerSample=wav_header.wBitsPerSample;
	params->wChannels=wav_header.wChannels;
	params->rLen=wav_header.rLen-sizeof(AudioWavHeader);
	return 0;
}

//获取wav文件头信息
void get_wav_header_info(FILE *fp,AudioWavHeader *wav_header)
{
	int nread;
	fseek(fp,0,SEEK_SET); 
	nread=fread(wav_header,1,sizeof(AudioWavHeader),fp);
	debug_printf("nread=%d\n",nread);
	debug_printf("RIFF 标志%s\n",wav_header->rld);
	debug_printf("文件大小rLen：%d\n",wav_header->rLen);
	debug_printf("文件类型%c%c%c%c\n",wav_header->wld[0],wav_header->wld[1],wav_header->wld[2],wav_header->wld[3]);
	debug_printf("格式块标志符：%s\n",wav_header->fld);
	debug_printf("格式块大小：%d\n",wav_header->fLen);
	debug_printf("编码格式:%d\n",wav_header->wFormatTag);
	debug_printf("声道数：%d\n",wav_header->wChannels);
	debug_printf("采样频率：%d\n",wav_header->nSamplesPersec);
	debug_printf("每秒播放字节数：%d\n",wav_header->nAvgBitsPerSample);
	debug_printf("每个采样点byte数：%d\n",wav_header->wBlockAlign);
	debug_printf("数据位数：%d\n",wav_header->wBitsPerSample);
    
	debug_printf("data=%s\n",wav_header->dld);
	debug_printf("wSampleLength=%d\n",wav_header->wSampleLength);    		
}


//音频播放回调函数用户参数
struct codePlayCallbackParam{
	struct MediaAudioHandle *pcm;
	struct MediaUserParams *conf;
	struct AudioStreamParams *audio_param;
	int delay;
};


/// @brief 音频播放回调，如果要自行设置offset，offset传入-1；
/// @param buff 缓存区
/// @param frames 侦数
/// @param offset 截至当前为止写入的字节数量(会根据字节数量和硬件配置自动设置进度)
/// @return 
static int callback_codec_play(uint8_t *buff,uint32_t frames,int offset,void *param)
{
	struct codePlayCallbackParam *p=(struct codePlayCallbackParam *)param;
	return audio_stream_write(p->pcm, p->conf, buff, frames, -1, offset, p->delay);
}



//声卡硬件初始化(使用解码器的参数自动设置)
int media_audio_hard_auto_init(struct MediaAudioHandle *pcm_play,struct MediaUserParams *user,struct MediaStreamParams *audio)
{
	debug_printf("初始化声卡硬件\n");
	struct AudioStreamParams *stream_params=(struct AudioStreamParams *)malloc(sizeof(struct AudioStreamParams));
	audio_stream_params_init(audio->codec_ctx->channels,			//使用流的参数来初始化声卡
								audio->codec_ctx->sample_rate,
								AUDIO_CODEC_CHANNEL_DEF,		//使用16位宽，(本值是解码时候自己指定的，不需要动态设置)
								stream_params);
	if(!pcm_play || !pcm_play->handle)
	{
		user->audio_params->callback_audio=NULL;
		user->audio_params->userdata=NULL;

		audio->audio.handle=pcm_play;
		audio->audio.handle->adparams=stream_params;
		return -1;
	}
	if(pcm_hwparams_set(pcm_play,stream_params)<0)
		return -1;
	if(pcm_start_play(pcm_play)<0)
		return -1;
	struct codePlayCallbackParam *cb_param=(struct codePlayCallbackParam *)malloc(sizeof(struct codePlayCallbackParam));
	if(cb_param==NULL)
		return -1;

	cb_param->conf=user;
	cb_param->audio_param=stream_params;
	cb_param->delay=100;
	cb_param->pcm=pcm_play;

	user->audio_params->callback_audio=callback_codec_play;
	user->audio_params->userdata=cb_param;
	audio->audio.handle=pcm_play;
	audio->audio.handle->adparams=stream_params;

	return 0;
}

static int media_audio_hard_deinit(struct MediaStreamParams *audio)
{
	if(!audio)
		return 0;

    return 0;
}



//声卡初始化
int media_stream_audio_init_handle(struct MediaStreamParams *audio,struct MediaUserParams *user)
{
	const char *name=user->audio_params->device;

	AVCodecContext *codec_ctx=audio->codec_ctx;
	struct MediaAudioHandle *pcm_play=Audio_Play_Open(name);
	if(pcm_play==NULL){
		fprintf(stderr, "[Error]: Audio pcm open error\n");
		return -1;
	}

	audio->audio.handle=pcm_play;
	if(media_audio_hard_auto_init(pcm_play,user,audio)<0)		//声卡初始化
		return -1;
	debug_printf("[Debug]: media_audio_hard_auto_init ok\n");
	struct SwrContext *swrContext=swr_set_with_hard_param(codec_ctx,pcm_play->adparams);
    if (!swrContext || swr_init(swrContext) < 0) {
		perror("Could not initialize resampler");
		fprintf(stderr, "Could not initialize resampler\n");
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&audio->format_ctx);
        return -1;
    }

	debug_printf("Resampling:%d\n",codec_ctx->sample_rate);
	debug_printf("init sdl audio ok\n");
	audio->audio.swr_ctx=swrContext;
	audio->audio.handle=pcm_play;
	user->nAvgBitsPerSample=pcm_play->adparams->nAvgBitsPerSample;	//此处先设置每秒播放字节数，如果存在视频流，会对其清空
	//display->audio_data=audioData;
	return 0;
}

//声卡取消初始化
int media_stream_audio_deinit_handle(struct MediaStreamParams *audio)
{
	if(!audio)
		return 0;

	if(!audio->audio.handle)
		return -1;

	if(!audio->codec_ctx)
		avcodec_free_context(&audio->codec_ctx);
	if(!audio->format_ctx)
		avformat_close_input(&audio->format_ctx);
	if(!audio->audio.swr_ctx)
		swr_free(&audio->audio.swr_ctx);
	media_audio_hard_deinit(audio);		//取消硬件的设置

	Audio_Device_Close(audio->audio.handle);			//关闭设备
	return 0;

}