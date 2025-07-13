#ifndef _TP_AUDIO_DEVICE_H_
#define _TP_AUDIO_DEVICE_H_

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "../audio/hard.h"

#define DEBUG_VIDEO
#define DEBUG_AUDIO

#define USER_CONF_VOLUME_MAX	100	//音量最大值
#define USER_CONF_VOLUME_MIN	0	//音量最小值
#define USER_CONF_SPEED_MAX		8.0	
#define USER_CONF_SPEED_MIN		0.5	//播放速度最小值

typedef int(*CallbackVideoDisplay)(uint8_t **data, int *linesize, uint32_t format ,void *user_data);

typedef enum AudioPlayState_{
	AUDIO_STATE_NONE 	= 0X00,		//初始状态
	AUDIO_STATE_STOP	= 0x01,		//停止状态
	AUDIO_STATE_START 	= 0X02,		//已经开始，用于防止线程还没启动，用户已经发送开始命令
	AUDIO_STATE_PLAYING = 0X03,		//播放中
	AUDIO_STATE_RECORD 	= 0X04,		//录制中
	AUDIO_STATE_PAUSEING= 0X05,		//暂停中
	AUDIO_STATE_JUMP	= 0X07,		//正在切换
	AUDIO_STATE_EXIT 	= 0x09,		//退出,此状态不允许设置，设置为AUDIO_STATE_STOP后会自动更新到此状态
	MEDIA_THREAD_WAITING		//等待中，通常是解码完成或线程启动准备完成可以解码
}AudioPlayState;

typedef enum MediaFileType_{
	MEDIA_FILE_TYPE_MP3		=0x01,
	MEDIA_FILE_TYPE_M4A		=0X02,
	MEDIA_FILE_TYPE_WAV		=0x03
}MediaFileType;

typedef struct MediaConf PIAudioConf;
struct MediaParams;
struct MediaCodecParam;
struct AudioStreamParams;

struct MediaParams *media_user_config_creat() __attribute__((used));
void media_user_config_free(struct MediaParams *conf) __attribute__((used));

int Audio_Hard_Auto_Init(PIAudioConf *pcm_play,struct MediaParams *conf,struct MediaCodecParam *codec);
int Audio_Hard_Deinit(struct MediaCodecParam *codec);
//打开音频播放设备并初始化
PIAudioConf *Audio_Play_Open(const char *name)  __attribute__((used));
int Audio_Play_Test(PIAudioConf *pcm_play,const char *name) __attribute__((used));

//音频播放是否退出
int Audio_State_Is_Exit(struct MediaParams *conf) __attribute__((used));

//音频播放设备关闭
int Audio_Device_Close(PIAudioConf *pcm_play)  __attribute__((used));

//循环播放列表
int Audio_Play_Main(PIAudioConf *pcm_play,struct MediaParams *conf)  __attribute__((used));

//设置音量
int Audio_Set_Volume(struct MediaParams *conf,int16_t volume)  __attribute__((used));

//获取音量
int Audio_Get_Volume(struct MediaParams *conf)  __attribute__((used));

//获取播放速度
float Audio_Get_Speed(struct MediaParams *conf)  __attribute__((used));

//设置播放速度
int Audio_Set_Speed(struct MediaParams *conf,float speed)  __attribute__((used));

//内部获取用户设置的播放位置
int Audio_Get_Position_S(struct MediaParams *conf)  __attribute__((used));

//内部设置实时播放位置
int Audio_Set_Position_N(struct MediaParams *conf,int32_t position)  __attribute__((used));

//设置位置
int Audio_Set_Position(struct MediaParams *conf,int32_t position)  __attribute__((used));

//获取位置，还没有写
int Audio_Get_Position(struct MediaParams *conf,PIAudioConf *pcm_play)  __attribute__((used));

//暂停播放
int Audio_Set_Suspend(struct MediaParams *conf) __attribute__((used));

//继续播放
int Audio_Set_Continue(struct MediaParams *conf) __attribute__((used));

int Audio_Get_State(struct MediaParams *conf) __attribute__((used));

//开始播放
int Audio_Set_Start(struct MediaParams *conf,const char *file) __attribute__((used));

//停止播放,只是停止不会关闭声卡，不同于暂停，停止会清空大多数播放信息
int Audio_Set_Stop(struct MediaParams *conf) __attribute__((used));

//关闭声卡
int Audio_Set_Close(struct MediaParams *conf) __attribute__((used));

//播放新文件
int Audio_Set_Play(struct MediaParams *conf,const char *file) __attribute__((used));

//播放下一个文件(针对列表)
int Audio_Play_Next(struct MediaParams *conf) __attribute__((used));

//播放上一个文件(针对列表)
int Audio_Play_Last(struct MediaParams *conf) __attribute__((used));

//获取正在播放
bool Audio_Get_Is_Playing(struct MediaParams *conf);



//获取音频时长
double Audio_Get_Length(struct MediaParams *conf);

//添加播放的文件
int Audio_Add_File(struct MediaParams *conf, const char *file) __attribute__((used));

//删除播放文件
int Audio_Del_File(struct MediaParams *conf, const char *file) __attribute__((used));

int Audio_Set_Hard_Params(PIAudioConf *pcm_play,struct MediaParams *conf,uint32_t rate,uint16_t channel,uint16_t bits) __attribute__((used));
//设置非阻塞(只允许在初始状态/停止状态/暂停状态可以设置)
int Audio_Set_Nonblock(PIAudioConf *pcm_play,struct MediaParams *conf,uint8_t nonblock);
int Audio_Write_Stream(PIAudioConf *pcm,struct MediaParams *conf,struct AudioStreamParams *hard_params,
							uint8_t *buffer,uint32_t frames,int offset,int delay);

int Audio_Set_System_Volume(uint8_t volume,const char *name);
int Audio_Get_System_Volume(const char *name);
void Audio_Set_Video_Callback(struct MediaParams *conf,CallbackVideoDisplay cb,void *userdata);
int Audio_Set_Video_Decode_Format(struct MediaParams *conf, uint32_t format_video);
#ifdef __cplusplus
}
#endif

#endif
