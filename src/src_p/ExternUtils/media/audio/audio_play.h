#ifndef _AUDIO_PLAY_H_
#define _AUDIO_PLAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include "filter.h"
#include "tpAudioDevice.h"
#include "tpVideoDevice.h"

struct MediaParams;
#include <stdbool.h>
#include "audio_codec.h"

#define DEBUG_AUUDIO		//调试打印接口
//全局配置
#define PCM_BUFFER_FRAMES	1024		//pcm的缓存区帧数，每次可以写入这么多侦的数量
#define PCM_BUFFER_SIZE 	4			//pcm缓存区大小由这个值计算：=	PCM_BUFFER_FRAMES * PCM_BUFFER_SIZE	*侦字节数

#define PCM_CHANNELS_DEFAULT		2		//默认通道树
#define PCM_SAMPLE_PERSEC_DEFAULT	44100	//默认采样频率
#define PCM_Bits_PER_SAMPLE_DEFAULT	16		//默认采样位数


#define USER_CONF_VOLUME 		100	//默认音量

typedef struct MediaConf PIAudioConf;
//前置声明
struct MediaCodecParam;

//线程安全读，读取参数param到value
#define THREAD_READ_USERCONF(mut, param, value)		\
	{	\
		pthread_rwlock_rdlock(&mut);\
		value = param;\
		pthread_rwlock_unlock(&mut);\
	}

//线程安全写，把value写入到param
#define THREAD_WRITE_USERCONF(mut, param, value)		\
	{	\
		pthread_rwlock_wrlock(&mut);\
		param = value;\
		pthread_rwlock_unlock(&mut);\
	}


typedef enum AudioPlayCommand_{
	AUDIO_PLCMD_NONE 	= 0X00,		//初始
	AUDIO_PLCMD_STOP	= 0x01,		//停止
	AUDIO_PLCMD_START 	= 0X02,		//播放
	AUDIO_PLCMD_SUSPEND	= 0X04,		//挂起(等待)
	AUDIO_PLCMD_CONTINUE= 0X05,		//继续
	AUDIO_PLCMD_NEXT	= 0X07,		//切换到下一个
	AUDIO_PLCMD_LAST	= 0X08,		//切换到上一个
	AUDIO_PLCMD_EXIT 	= 0x09,		//退出
}AudioPlayCommand;

typedef enum {
    AUDIO_STREAM_PLAYBACK = SND_PCM_STREAM_PLAYBACK,
    AUDIO_STREAM_CAPTURE = SND_PCM_STREAM_CAPTURE
}AudioStreamType;

typedef enum AudioPlayType_{
	AUDIO_PLAY_NONE		= 0X00,
	AUDIO_PLAY_LIST		= 0X01,
	AUDIO_PLAY_FILE		= 0X02,
	AUDIO_PLAY_STREAM	= 0X03
}AudioPlayType;


struct MediaFileNode{
	char *file;
	struct MediaFileNode *next;
	struct MediaFileNode *last;
};

//尾插，头删
//头为固定的空节点，头的下一个是链表第一个节点，end和pos是具体的节点
struct MediaFileList{
	struct MediaFileNode *head;
	struct MediaFileNode *end;
	struct MediaFileNode *pos;	//当前读取的位置
	pthread_rwlock_t mut;
	char *(*read)(struct MediaFileList * list);		//默认读取下一个
	char *(*read_now)(struct MediaFileList * list);
	char *(*read_last)(struct MediaFileList * list);
	char *(*read_saft)(struct MediaFileList * list);		//默认读取下一个
	char *(*read_now_saft)(struct MediaFileList * list);
	char *(*read_last_saft)(struct MediaFileList * list);
	int (*insert_end)(struct MediaFileList * list,char *file);
	int (*insert_pos)(struct MediaFileList * list,char *file);
	int (*insert_end_saft)(struct MediaFileList * list,char *file);
	int (*insert_pos_saft)(struct MediaFileList * list,char *file);
	int (*delete_file)(struct MediaFileList * list,char *file);
	int (*delete_file_saft)(struct MediaFileList * list,char *file);
	int (*delete_all)(struct MediaFileList * list);
	int (*delete_all_saft)(struct MediaFileList * list);
	int (*remove)(struct MediaFileList * list);
	size_t size;
};



//声卡的硬件的部分信息
struct PcmHardParams{
	uint8_t can_pause;			//PCM是否支持暂停
	uint8_t can_resume;			//PCM是否支持恢复播放
	unsigned long cycle_frames;		///每个周期处理的侦的数量，写入的时候可以每次写入这个数量
	unsigned long buff_size;		//缓存区大小
};

//内部播放的操作
struct AudioHostOperate{
	int (*write_data)(PIAudioConf *pcm,uint8_t *buffer,unsigned long frames,int delay);
	int (*adjust_volume)(uint8_t *buffer, size_t frames, int channels, float volume, uint16_t bit);
	int (*adjust_postion)();

};

//内部使用，声卡配置信息
typedef struct MediaConf{
	char *device;
	snd_pcm_t *handle;             	//设备句柄
	snd_pcm_hw_params_t *hwparams;  //设备配置信息的结构体(结构体内部隐藏)，配置信息保存在该结构体
	uint8_t file_type;          	//音频文件类型(AudioFileType类型)
	uint8_t thread_num;				//线程编号，暂时未使用
	struct AudioStreamParams *adparams;	//解码后可以用于播放的音频流的参数
	struct PcmHardParams *ahparams;		//设置后的一些关键硬件参数(其实是从snd_pcm_hw_params_t里面拿出来的几个常用的参数)
}PIAudioConf;


struct PthreadCond{
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int (*send)(struct PthreadCond *cond);
	int (*wait)(struct PthreadCond *cond);
};


struct MediaRect{
	int16_t x;		//显示位置x
	int16_t y;		//显示位置y
	uint16_t w;		//显示宽度
	uint16_t h;		//显示高度
};
struct MediaVideoParams{
	struct MediaRect rect;
	uint16_t light;	//显示亮度
	VideoScalingType fill;
};
//内部使用，用户交互信息
struct MediaParams{        //公共区用户设置
	bool is_playing;
	struct MediaFileList *list;//文件列表
	void *memory;
	char *file;				//正在播放的文件
	AudioPlayType type;		//文件/列表/流	，暂时未使用
	uint8_t volume;			//声音(0-100)
	struct{
		float speed;		//播放速度，0.5～100
		struct MediaFilterParam *filter;	//过滤器，用于速度控制，此处只是一个地址用于防止退出的时候内存泄漏
	};
	struct MediaVideoParams *video;
	struct{   //当前进度和用户设置进度(0-*s)                           初始化-1
		int32_t position_s;
		union{
			double position_p;
			int64_t position_bytes;		//已播放的字节数
		};
	};
	double length;			//正在播放的文件的时长(单位：s)
	AudioPlayState state;	//播放状态 ,
	struct{
		AudioPlayCommand cmd;	//控制命令
		int (*command_get)(struct MediaParams *conf);		//安全获取当前的命令
	};
	struct{		//视频显示回调函数以及解码格式
		uint32_t format_video;		//解码格式，仅在用户自己处理时候才会生效
		CallbackVideoDisplay callback_video;
		void *userdata;
		CallbackVideoDisplay (*get_callback_video)(struct MediaParams *conf);
		void (*set_callback_video)(struct MediaParams *conf, CallbackVideoDisplay callback,void *userdata);
	};
	pthread_rwlock_t rw_mut;	//数据交互读写锁
	struct PthreadCond *cond;
};

int media_pcm_hwparams_init(PIAudioConf *pcm);
int pcm_hwparams_set(PIAudioConf *pcm,struct AudioStreamParams *audio);		//设置硬件参数


int media_pcm_drain(PIAudioConf *pcm);
int media_pcm_drop(PIAudioConf *pcm);
int media_pcm_close(PIAudioConf *pcm);
struct MediaParams *media_user_config_creat();
struct PthreadCond *pthread_cond_creat_struct();
int pthread_cond_free_struct(struct PthreadCond *cond);
void media_user_config_free(struct MediaParams *conf);
int Audio_Hard_Auto_Init(PIAudioConf *pcm_play,struct MediaParams *conf,struct MediaCodecParam *codec);
int Audio_Hard_Deinit(struct MediaCodecParam *codec);
PIAudioConf *Audio_Play_Open(const char *device);
int Audio_Device_Init(PIAudioConf *pcm_play,const char *device,AudioStreamType type);
int Audio_Device_Close(PIAudioConf *pcm_play);
int Audio_Play_Main(PIAudioConf *pcm_play,struct MediaParams *conf);
int Audio_Play_Test(PIAudioConf *pcm_play,const char *name);

int64_t Audio_Get_BytePosition(struct MediaParams *conf);
int64_t Audio_Set_BytePosition(struct MediaParams *conf,int64_t position);

//开始
int Audio_Set_Start(struct MediaParams *conf, const char *file);

//获取命令(内部使用)
int Audio_Get_Command(struct MediaParams *conf);

//设置命令(内部使用)
int Audio_Set_Command(struct MediaParams *conf,AudioPlayCommand cmd);

//设置状态
int Audio_Set_State(struct MediaParams *conf, AudioPlayState state);

//设置音量
int Audio_Set_Volume(struct MediaParams *conf,int16_t volume);

//获取音量
int Audio_Get_Volume(struct MediaParams *conf);

//内部获取用户设置的播放位置
int Audio_Get_Position_S(struct MediaParams *conf);

//内部设置实时播放位置
int Audio_Set_Position_N(struct MediaParams *conf,int32_t position);

//设置位置
int Audio_Set_Position(struct MediaParams *conf,int32_t position);

//获取位置
int Audio_Get_Position(struct MediaParams *conf,PIAudioConf *pcm_play);
double Audio_Get_DPosition(struct MediaParams *conf,PIAudioConf *pcm_play);

int64_t Audio_Get_BytePosition(struct MediaParams *conf);
int64_t Audio_Set_BytePosition(struct MediaParams *conf,int64_t position);

//暂停播放
int Audio_Set_Suspend(struct MediaParams *conf);

//继续播放
int Audio_Set_Continue(struct MediaParams *conf);

int Audio_Get_State(struct MediaParams *conf);

//停止播放,只是停止不会关闭声卡，不同于暂停，停止会清空大多数播放信息
int Audio_Set_Stop(struct MediaParams *conf);

//关闭声卡
int Audio_Set_Close(struct MediaParams *conf);

//播放新文件
int Audio_Set_Play(struct MediaParams *conf,const char *file);

int Audio_Play_Next(struct MediaParams *conf);
int Audio_Play_Last(struct MediaParams *conf);
//内部设置媒体文件时长
int Audio_Set_Length(struct MediaParams *conf,double length);
//获取音频时长
double Audio_Get_Length(struct MediaParams *conf);
int Audio_Set_Is_Playing(struct MediaParams *conf,bool is_playing);
//添加播放的文件
int Audio_Add_File(struct MediaParams *conf, const char *file);

//删除播放文件
int Audio_Del_File(struct MediaParams *conf, const char *file);

//设置硬件
int Audio_Set_Hard_Params(PIAudioConf *pcm_play,struct MediaParams *conf,uint32_t rate,uint16_t channel,uint16_t bits);
//取消硬件设置
int Audio_Set_Nonblock(PIAudioConf *pcm_play,struct MediaParams *conf,uint8_t nonblock);
int Audio_Write_Stream(PIAudioConf *pcm,struct MediaParams *conf,struct AudioStreamParams *hard_params,
							uint8_t *buffer,uint32_t frames,int offset,int delay);
CallbackVideoDisplay Audio_Get_Video_Callback(struct MediaParams *conf);
void Audio_Set_Video_Callback(struct MediaParams *conf,CallbackVideoDisplay cb, void *userdata);
#endif
