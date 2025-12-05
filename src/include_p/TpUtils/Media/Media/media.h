#ifndef _MEDIA_MEDIA_H_
#define _MEDIA_MEDIA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include "Media/media_config.h"
#include "Media/media_file_list.h"


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

typedef AVFormatContext MediaFormatContext;
typedef AVStream	MediaStream;


//主同步时钟
typedef enum{
	SYNC_AUDIO_MASTER,          // 音频主时钟（默认）
	SYNC_VIDEO_MASTER,          // 视频主时钟
	SYNC_EXTERNAL_CLOCK         // 外部时钟
}MediaSyneClockType;

struct PthreadCond{
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int (*send)(struct PthreadCond *cond);
	int (*wait)(struct PthreadCond *cond);
};

//用户交互信息
struct MediaUserParams{        //公共区用户设置
	//通用设置
	bool is_playing;
	struct MediaFileList *list;//文件列表
	char *file;				//正在播放的文件
	AudioPlayType type;		//文件/列表/流	，暂时未使用
	double length;			//正在播放的文件的时长(单位：s)
	MediaPlayState state;	//播放状态 ,
	struct{
		float speed;		//播放速度，0.5～100
		struct MediaFilterParam *filter;	//过滤器，用于速度控制，此处只是一个地址用于防止退出的时候内存泄漏
	};
	struct{   //当前进度和用户设置进度(0-*s)                           初始化-1
		int32_t position_s;
		union{
			double position_p;			//播放位置
			int64_t position_bytes;		//已播放的字节数
		};
		uint32_t nAvgBitsPerSample;		//每秒播放字节数，仅播放音频时使用
	};
	struct{
		MediaPlayCommand cmd;	//控制命令
		int (*command_get)(struct MediaUserParams *conf);		//安全获取当前的命令
	};

	//音频流相关配置
	struct MediaAudioInfo *audio_params;

	//视频流相关配置
	struct MediaVideoInfo *video_params;

	pthread_rwlock_t rw_mut;	//数据交互读写锁
	struct PthreadCond *cond;
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




//用户配置相关结构体创建/释放
struct MediaAudioInfo *media_audio_info_creat(const char *name);
void media_audio_info_delete(struct MediaAudioInfo *conf);
struct MediaVideoInfo *media_video_info_creat();
void media_video_info_delete(struct MediaVideoInfo *conf);
struct MediaUserParams *media_user_config_creat();
struct PthreadCond *pthread_cond_creat_struct();
int pthread_cond_free_struct(struct PthreadCond *cond);
void media_user_config_delete(struct MediaUserParams *conf);



//开始
int Media_Set_Start(struct MediaUserParams *conf, const char *file);

//获取命令(内部使用)
int Media_Get_Command(struct MediaUserParams *conf);

//设置命令(内部使用)
int Media_Set_Command(struct MediaUserParams *conf,MediaPlayCommand cmd);

//设置状态
int Media_Set_State(struct MediaUserParams *conf, MediaPlayState state);

int Media_Get_State(struct MediaUserParams *conf);

int Media_State_Is_Exit(struct MediaUserParams *conf);

int Audio_Get_BitsPerSample(struct MediaUserParams *conf);

//内部获取用户设置的播放位置
int Media_Get_Position_S(struct MediaUserParams *conf);

//内部设置实时播放位置
int Media_Set_Position_N(struct MediaUserParams *conf,int32_t position);

float Media_Get_Speed(struct MediaUserParams *conf);

int Media_Set_Speed(struct MediaUserParams *conf, float speed);

//设置位置
int Media_Set_Position(struct MediaUserParams *conf,int32_t position);

//获取位置
int Media_Get_Position(struct MediaUserParams *conf);
double Media_Get_DPosition(struct MediaUserParams *conf);

int64_t Media_Get_BytePosition(struct MediaUserParams *conf);
int64_t Media_Set_BytePosition(struct MediaUserParams *conf,int64_t position);

//暂停播放
int Media_Set_Suspend(struct MediaUserParams *conf);

//继续播放
int Media_Set_Continue(struct MediaUserParams *conf);

//停止播放,只是停止不会关闭声卡，不同于暂停，停止会清空大多数播放信息
int Media_Set_Stop(struct MediaUserParams *conf);

//关闭声卡
int Audio_Set_Close(struct MediaUserParams *conf);

//播放新文件
int Media_Set_Play(struct MediaUserParams *conf,const char *file);

int Media_Play_Next(struct MediaUserParams *conf);
int Media_Play_Last(struct MediaUserParams *conf);
//内部设置媒体文件时长
int Media_Set_Length(struct MediaUserParams *conf,double length);
//获取音频时长
double Media_Get_Length(struct MediaUserParams *conf);
int Media_Set_Is_Playing(struct MediaUserParams *conf,bool is_playing);
bool Media_Get_Is_Playing(struct MediaUserParams *conf);
//添加播放的文件
int Media_Add_File(struct MediaUserParams *conf, const char *file);

//删除播放文件
int Media_Del_File(struct MediaUserParams *conf, const char *file);



#ifdef __cplusplus
}
#endif

#endif