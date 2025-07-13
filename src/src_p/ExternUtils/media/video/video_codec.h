#ifndef _VIDEO_CODEC_H_
#define _VIDEO_CODEC_H_


#include <pthread.h>
#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
#include "video_play.h"
#include "audio_codec.h"
#include "media_timer.h"

#define DEBUG
//#define DEBUG_VIDEO
#define DEBUG_VIDEO_INIT

typedef int(*CodecSdlPlayAudio) (uint8_t *buf,uint32_t frames,void *param);
typedef int(*CodecSdlPlayVideo) (uint8_t *buf,uint32_t frames,void *param);

typedef struct MediaPacketList {
    AVPacket *pkt;
    struct MediaPacketList *next;
} MediaPacketList;

//解码的缓存队列
struct MediaPacketQueue{
	MediaPacketList *first_pkt;		//AVPacket的链表头
	MediaPacketList *last_pkt;		//AVPacket的链表尾
	uint32_t nb_packets;			//链表的节点数量
	uint32_t size;					//链表大小

	uint8_t exit_flag;
	pthread_mutex_t lock;
	pthread_cond_t cond;
//	SDL_mutex *lock;
//	SDL_cond *cond;
};

//线程同步
struct MediaThreadCond{
	uint8_t flag;
	pthread_mutex_t lock;
	pthread_cond_t cond ;
};

//线程时钟
struct MediaThreadClock{	
	long long clock_start;		//理论上以当前设着位置相对的开始时间
};

//编解码播放线程信息
struct MediaThread{
	pthread_t thread;			//线程标号
	AudioPlayState state;		//线程状态
	uint8_t running;
	uint8_t codec;
	pthread_mutex_t lock;			//数据锁
	struct MediaThreadCond cond;	//线程队列使用
	struct MediaPacketQueue list;
	struct TimerHandle *clock;		//

	int (*send_codec_signal)(struct MediaThread *thread);		//开始解码


	int (*wait_codec)(struct MediaThread *thread);		//等待解码
	int (*start_codec)(struct MediaThread *thread);		//开始解码

	int (*start_thread)(struct MediaThread *thread,void *(*thread_main)(void *),struct VideoHardParam *display,struct MediaCodecParam *codec,void *packet,struct MediaParams *user);
	int (*is_running)(struct MediaThread *thread);	//线程是否在运行中

	AudioPlayState (*get_state)(struct MediaThread *thread);				//获取线程状态
	int (*set_state)(struct MediaThread *thread, AudioPlayState state);	//设置线程状态

	//队列操作
	int (*push_packet)(struct MediaPacketQueue *list,AVPacket *packet);				//向缓存队列中写入
	AVPacket *(*get_packet)(struct MediaPacketQueue *list,int block);	//从缓存队列中读取
	int (*free_packet)(AVPacket *packet);		//使用get_packet_list获取packet后调用此函数释放
	int (*flush_list)(struct MediaPacketQueue *list);			//删除队列中所有元素
	uint32_t (*packet_number)(struct MediaPacketQueue *list);	//获取队列中元素数量
	int (*packet_exit)(struct MediaPacketQueue *list);
};




int get_sizeof_format_mapping();
uint32_t get_format_mapping_with_num(uint32_t num);
uint32_t get_sdl_pixel_format(enum AVPixelFormat pixFmt);
enum AVPixelFormat get_format_pixel_sdl(uint32_t format);

struct MediaThread *Media_Thread_Creat();
int Media_Thread_Free(struct MediaThread *thread);
int Video_File_Codec(struct VideoHardParam *display,struct MediaCodecParam *codec_v,struct MediaCodecParam *codec_a,struct MediaParams *user);
int Video_Get_File_Info(const char *filename,struct MediaCodecParam *codec_v,struct MediaCodecParam *codec_a);
int Video_Free_File(struct MediaCodecParam *codec_v,struct MediaCodecParam *codec_a);





#endif