/*///------------------------------------------------------------------------------------------------------------------------//
		视频解码
说 明 : libswscale：图片像素处理
日 期 : 2025.1.8

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>		//用于signal函数，测试使用
#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "video_display.h"
#include "video_codec.h"
#include "video_play.h"
#include "audio_codec.h"
#include "audio_play.h"
#include "../media/media.h"

#define AUDIO_MAX_QUEUE_SIZE	500		//音频缓存区最大长度
#define VIDEO_MAX_QUEUE_SIZE	100		//视频缓存区最大长度

#ifdef DEBUG_VIDEO
    #define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define debug_printf(fmt, ...)  // 如果不定义DEBUG，什么也不做
#endif

uint8_t video_flag=0;

void exit_signal(int sig)
{
	video_flag=1;
}

//ffmpeg和sdl映射表
struct PixelFormatMapping{
    enum AVPixelFormat avFmt;
    Uint32 sdlFmt;
};
const struct PixelFormatMapping pixelFormatMap[] = {
    {AV_PIX_FMT_YUV420P, SDL_PIXELFORMAT_IYUV},    // YUV 4:2:0
    {AV_PIX_FMT_YUYV422, SDL_PIXELFORMAT_YUY2},    // YUV 4:2:2
    {AV_PIX_FMT_UYVY422, SDL_PIXELFORMAT_UYVY},    // YUV 4:2:2 (UYVY)
    {AV_PIX_FMT_NV12, SDL_PIXELFORMAT_NV12},       // NV12
    {AV_PIX_FMT_NV21, SDL_PIXELFORMAT_NV21},       // NV21
    {AV_PIX_FMT_RGB24, SDL_PIXELFORMAT_RGB24},     // RGB 24-bit		排列方式为：R0G0B0 R1G1B1 R2G2B2
    {AV_PIX_FMT_BGR24, SDL_PIXELFORMAT_BGR24},     // BGR 24-bit
    {AV_PIX_FMT_ARGB, SDL_PIXELFORMAT_ARGB8888},   // ARGB 32-bit
    {AV_PIX_FMT_RGBA, SDL_PIXELFORMAT_RGBA8888},   // RGBA 32-bit
    {AV_PIX_FMT_ABGR, SDL_PIXELFORMAT_ABGR8888},   // ABGR 32-bit
    {AV_PIX_FMT_BGRA, SDL_PIXELFORMAT_BGRA8888},   // BGRA 32-bit
};

//获取mapping的大小
int get_sizeof_format_mapping()
{
	return sizeof(pixelFormatMap) / sizeof(pixelFormatMap[0]);
}

//根据序号获取SDL格式
uint32_t get_format_mapping_with_num(uint32_t num)
{
	return pixelFormatMap[num].sdlFmt;
}

//根据AVPixelFormat获取sdl的format
uint32_t get_sdl_pixel_format(enum AVPixelFormat pixFmt) 
{
	for (size_t i = 0; i < get_sizeof_format_mapping(); ++i) 
	{
		if (pixelFormatMap[i].avFmt == pixFmt) {
			return pixelFormatMap[i].sdlFmt;
		}
	}
	return SDL_PIXELFORMAT_UNKNOWN; //未找到匹配格式
}

//根据SDL格式获取AVPixelFormat
enum AVPixelFormat get_format_pixel_sdl(uint32_t format)
{
	for (size_t i = 0; i < get_sizeof_format_mapping(); ++i) 
	{
		if (pixelFormatMap[i].sdlFmt == format) {
			return pixelFormatMap[i].avFmt;
		}
	}
	return AV_PIX_FMT_NB; //未找到匹配格式
}

int video_find_codec(const char *url, struct MediaCodecParam *video,struct MediaCodecParam *audio)
{
	int ret=0;
	AVFormatContext *format_ctx = NULL;
	AVCodecContext *videoCodecContext;
	AVCodecContext *audioCodecContext;
	AVCodec *videoCodec;
	AVCodec *audioCodec;
	int videoStreamIndex = -1, audioStreamIndex=-1;

	media_init(1);	//初始化并使能网络流

    // 打开媒体文件
	if(media_get_file_info(url,&format_ctx)<0)
		return -1;
    // 查找流合适的解码器()av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	for (int i = 0; i < format_ctx->nb_streams; i++) {
		if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStreamIndex = i;
		}
		else if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioStreamIndex = i;
		}
	}
	if (videoStreamIndex == -1 || audioStreamIndex==-1) {
		fprintf(stderr, "Could not find video stream.\n");
		goto FREE_FORMAT;
	}

	// Get video stream codec context
	videoCodecContext = avcodec_alloc_context3(NULL);		//AVCodecContext *videoCodecContext
	if (!videoCodecContext) {
		fprintf(stderr, "Could not allocate video codec context.\n");
		goto FREE_FORMAT;
	}
	avcodec_parameters_to_context(videoCodecContext, format_ctx->streams[videoStreamIndex]->codecpar);

	// Find the decoder
	videoCodec = avcodec_find_decoder(videoCodecContext->codec_id);		//AVCodec *videoCodec
	if (!videoCodec) {
		fprintf(stderr, "Codec not found.\n");
		goto FREE_VIDEO_CODEC;
	}

	// Open codec
	if (avcodec_open2(videoCodecContext, videoCodec, NULL) < 0) {
		fprintf(stderr, "Could not open codec.\n");
		goto FREE_VIDEO_CODEC;
	}

	// Get audio stream codec context
	audioCodecContext = avcodec_alloc_context3(NULL);		//AVCodecContext *audioCodecContext
	if (!audioCodecContext) {
		fprintf(stderr, "Could not allocate video codec context.\n");
		goto FREE_VIDEO_CODEC;
	}
	avcodec_parameters_to_context(audioCodecContext, format_ctx->streams[audioStreamIndex]->codecpar);

	// Find the decoder
	audioCodec = avcodec_find_decoder(audioCodecContext->codec_id);		//AVCodec *audioCodec
	if (!audioCodec) {
		fprintf(stderr, "Codec not found.\n");
		goto FREE_AUDIO_CODEC;
	}

	// Open codec
	if (avcodec_open2(audioCodecContext, audioCodec, NULL) < 0) {
		fprintf(stderr, "Could not open codec.\n");
		goto FREE_AUDIO_CODEC;
	}

	video->format_ctx=format_ctx;
	video->codec_ctx=videoCodecContext;
	video->stream_index=videoStreamIndex;

	audio->format_ctx=format_ctx;
	audio->codec_ctx=audioCodecContext;
	audio->stream_index=audioStreamIndex;
	return 0;

FREE_AUDIO_CODEC:
	avcodec_free_context(&audioCodecContext);
FREE_VIDEO_CODEC:
	avcodec_free_context(&videoCodecContext);
FREE_FORMAT:
	avformat_close_input(&format_ctx);
	return -1;
}

struct ThreadData{
	struct MediaThread *thread;
	struct VideoHardParam *display;		//显示信息
	struct MediaCodecParam *codec;
	AVFrame *frame_s ;		//原始的侦数据(直接从文件中解码出来的)
	int8_t err_code;		//错误码
	AVPacket *packet;
	struct MediaParams *user;		//用户交互
};


//初始化包队列
static void packet_queue_init(struct MediaPacketQueue *q) 
{
	memset(q, 0, sizeof(struct MediaPacketQueue));
	q->exit_flag = 0;
	if (pthread_mutex_init(&q->lock, NULL) != 0) {
		return ;
	}

	if (pthread_cond_init(&q->cond, NULL) != 0) {
		pthread_mutex_destroy(&q->lock);
		return ;
	}
//	q->lock = SDL_CreateMutex();
//	q->cond = SDL_CreateCond();
}

//向队列里面新增元素(尾插)
static int packet_queue_put(struct MediaPacketQueue *q, AVPacket *pkt) 
{
	MediaPacketList *pkt_node = (MediaPacketList *)av_malloc(sizeof(MediaPacketList));			//pkt_list是链表的节点，里面的数据部分是AVPacket，(不是指针)
	if (!pkt_node) 
		return -1;
	
	// 创建 pkt 的副本(把pkt的内容深拷贝到pkt_list->pkt)
	AVPacket *packet = (AVPacket *)av_packet_alloc();  // 使用 av_packet_alloc() 分配内存
	if (!packet) {
		fprintf(stderr, "Could not allocate AVPacket\n");
		av_free(pkt_node);
	}
	pkt_node->pkt=packet;

	//av_packet_unref(&pkt_node->pkt);
	if (av_packet_ref(pkt_node->pkt, pkt) < 0) {
		av_packet_free(&packet);
		av_free(pkt_node);
		return -1;
	}
	pkt_node->next = NULL;

	pthread_mutex_lock(&q->lock);
//	SDL_LockMutex(q->lock);
	if (!q->last_pkt)
	{
		//debug_printf("debug:packet list :add %p to first %p\n",&pkt_node->pkt,pkt_node);
		q->first_pkt = pkt_node;
	}
	else
		q->last_pkt->next = pkt_node;

	q->last_pkt = pkt_node;
	q->nb_packets++;
	q->size += pkt->size;
	
	pthread_cond_signal(&q->cond); // 通知
	pthread_mutex_unlock(&q->lock);
//	SDL_CondSignal(q->cond);
//	SDL_UnlockMutex(q->lock);
//	debug_printf("debug: package nb:%d\n",q->nb_packets);	
	return 0;
}

//退出阻塞，并退出队列
static int packet_queue_exit(struct MediaPacketQueue *q)
{
	pthread_mutex_lock(&q->lock);
//	SDL_LockMutex(q->lock);
	q->exit_flag=1;
	pthread_cond_signal(&q->cond); // 通知
	pthread_mutex_unlock(&q->lock);
//	SDL_CondSignal(q->cond);
//	SDL_UnlockMutex(q->lock);
	return 0;
}

//从包队列中取出一个AVpacket，注意必须在使用完的时候释放(头读)
//block:=0非阻塞模式，=1阻塞模式，阻塞模式会一直等待队列中有数据
static AVPacket *packet_queue_get(struct MediaPacketQueue *q, int block) 
{
	AVPacket *pkt=NULL;
	MediaPacketList *pkt_node;
	int ret=0;
	//AVPacket *pkt_temp;
	pthread_mutex_lock(&q->lock);
//	SDL_LockMutex(q->lock);

	while(1) 
	{
		pkt_node = q->first_pkt;
		if (pkt_node) 
		{
			// 从队列中移除头部节点
			q->first_pkt = pkt_node->next;
			if (!q->first_pkt) {
				q->last_pkt = NULL;
			}
			q->nb_packets--;
			q->size -= pkt_node->pkt->size;
			// 将队列中的数据包的内容拷贝到传入的 pkt 中
			pkt = av_packet_alloc();  // 使用 av_packet_alloc() 分配内存
			if (!pkt) {
				fprintf(stderr, "Could not allocate AVPacket\n");
				break;
			}
			if (av_packet_ref(pkt, pkt_node->pkt) < 0) {
				ret = -1;
				fprintf(stderr, "Could not copy AVPacket\n");
				break;
			}
			
			// 释放队列节点
			av_packet_free(&pkt_node->pkt );
			av_free(pkt_node);
			ret = 1;
			break;
		} 
		else if (!block) {
			ret = 0;  // 队列为空且非阻塞模式，立即返回
			break;
		} 
		else {
			// 阻塞模式下等待条件变量
			pthread_cond_wait(&q->cond, &q->lock); // 等待条件变量
//			SDL_CondWait(q->cond, q->lock);
			if(q->exit_flag == 1) 
				break;
		}
	}

	pthread_mutex_unlock(&q->lock);
//	SDL_UnlockMutex(q->lock);
	//debug_printf("package get ptr: %p\n", pkt);
	if(ret<0)
		return NULL;
	return pkt;
}

static int free_packet(AVPacket *packet)
{
	if(packet)
		av_packet_free(&packet);
	return 0;
}

//清空队列
static int packet_queue_flush(struct MediaPacketQueue *q) 
{
	MediaPacketList *pkt_list, *pkt_tmp;
	pthread_mutex_lock(&q->lock);
//	SDL_LockMutex(q->lock);

	// 遍历并释放队列中的所有数据包
	pkt_list = q->first_pkt;
	while (pkt_list) {
		pkt_tmp = pkt_list->next;

		// 释放每个 AVPacket 的资源
		av_packet_free(&pkt_list->pkt );
		av_free(pkt_list);  // 释放节点本身

		pkt_list = pkt_tmp;
	}

	// 重置队列
	q->first_pkt = NULL;
	q->last_pkt = NULL;
	q->nb_packets = 0;
	q->size = 0;

	pthread_mutex_unlock(&q->lock);
//	SDL_UnlockMutex(q->lock);
	return 0;
}
//释放包队列
static void packet_queue_destroy(struct MediaPacketQueue *q) {
    packet_queue_flush(q);  // 清空队列中的所有数据包
	pthread_mutex_destroy(&q->lock);
	pthread_cond_destroy(&q->cond);
//    SDL_DestroyMutex(q->lock);  // 销毁互斥锁
//    SDL_DestroyCond(q->cond);   // 销毁条件变量
//    q->lock = NULL;
//    q->cond = NULL;
}
static uint32_t get_packet_number(struct MediaPacketQueue *q)
{
	uint32_t nb;
	pthread_mutex_lock(&q->lock);
//	SDL_LockMutex(q->lock);
	nb=q->nb_packets;
	pthread_mutex_unlock(&q->lock);
//	SDL_UnlockMutex(q->lock);
	return nb;
}

//开始解码
static int thread_start_codec(struct MediaThread *thread)
{
	pthread_cond_broadcast(&thread->cond.cond);
	pthread_mutex_lock(&thread->cond.lock); 
	thread->cond.flag=1;
	pthread_cond_broadcast(&thread->cond.cond);
	pthread_mutex_unlock(&thread->cond.lock);
	return 0;
}
//等待开始解码
static int thread_wait_codec(struct MediaThread *thread)
{
	pthread_mutex_lock(&thread->cond.lock); 
	while(thread->cond.flag==0)
	{
		pthread_cond_wait(&thread->cond.cond,&thread->cond.lock);	
	}
	thread->cond.flag=0;
	pthread_mutex_unlock(&thread->cond.lock);
	return 0;
}
//获取状态
static AudioPlayState thread_get_state(struct MediaThread *thread)
{
    AudioPlayState state;
    pthread_mutex_lock(&thread->lock);  // 锁定
    state = thread->state;
    pthread_mutex_unlock(&thread->lock);  // 解锁
    return state;
}

//设置状态
static int thread_set_state(struct MediaThread *thread, AudioPlayState state)
{
	if(state == AUDIO_STATE_EXIT)
	{
		debug_printf("Thread exit(%p)\n",thread);
	}

    pthread_mutex_lock(&thread->lock);  // 锁定
	if(thread->state!=AUDIO_STATE_EXIT)		//非退出状态才允许设置状态
    	thread->state=state;
    pthread_mutex_unlock(&thread->lock);  // 解锁
	return 0;
}

static int thread_start_running(struct MediaThread *thread,
								void *(*thread_main)(void *),
								struct VideoHardParam *display,
								struct MediaCodecParam *codec,
								void *packet,
								struct MediaParams *user)
{
	struct ThreadData *data=(struct ThreadData *)malloc(sizeof(struct ThreadData));
	//thread->thread_param=data;
	data->thread=thread;
	data->display=display;
	data->codec=codec;
	data->err_code=0;
	data->packet=(AVPacket *)packet;
	data->user=user;
	if(pthread_create(&thread->thread,NULL, thread_main,(void *)data)<0)
	{
		perror("pthread create failed");
		free(data);
		return -1;
	}
	return 0;
}

static int thread_is_running(struct MediaThread *thread)
{
	AudioPlayState state=thread_get_state(thread);
	if(state==AUDIO_STATE_PLAYING || state==MEDIA_THREAD_WAITING )
		return 1;
	return 0;
}


//为转码后的图像申请帧空间
static int malloc_codec_frame(int dst_width,int dst_height, enum AVPixelFormat pix_fmt,uint8_t **buffer, AVFrame **frame_d)
{
	int numBytes;
	numBytes = av_image_get_buffer_size(pix_fmt, dst_width, dst_height, 1);		//计算需要的空间大小
	*buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	if(*buffer == NULL) {
		return -1;
	}
	*frame_d = av_frame_alloc();
	if(*frame_d == NULL) {
		av_free(*buffer);
		return -1;
	}
	if(av_image_fill_arrays((*frame_d)->data, (*frame_d)->linesize, *buffer, pix_fmt,
							dst_width, dst_height, 1)<0)
	{
		av_free(*buffer);
		av_frame_free(&(*frame_d));
		return -1;
	}
	return 0;
}
static int free_codec_frame(uint8_t *buffer,AVFrame *frame_d)
{
	av_frame_free(&frame_d);
	av_free(buffer);
	return 0;
}

static int re_alloc_codec_context(int srcW, int srcH, enum AVPixelFormat srcFormat,
                                  int dstW, int dstH, enum AVPixelFormat dstFormat,
                                  int flags, SwsFilter *srcFilter,
                                  SwsFilter *dstFilter, const double *param)
{
	return 0;
}

//计算当前时钟需要的延时时间
static double count_media_clock_delay_time(struct MediaParams *user,struct TimerHandle *clock,int64_t pts, AVRational time_base)
{
	float speed=Audio_Get_Speed(user);
	//延时一段时间
	double video_clock = (double)pts * av_q2d(time_base)*1000.0*1000.0/speed;		//time_base为s
	double delay_time=video_clock-clock->get_run_time(clock);
	return delay_time;
}

//视频播放的视频解码线程
static void *thread_video_codec(void *param) 
{
	struct ThreadData *data=(struct ThreadData *) param;
	struct MediaThread *video_t=data->thread;
	struct VideoHardParam *display=data->display;
	struct MediaParams *user=data->user;
	CallbackVideoDisplay callback=user->get_callback_video(user);
	AVFrame *frame_s = av_frame_alloc();	//原始的侦数据(直接从文件中解码出来的)
	if(frame_s == NULL) {
		data->err_code=-1;
		return &data->err_code;
	}
	AVFrame *frame_d ;		//需要写入SDL的格式的数据(/可能和frame_s一致，也可能不一致，不一致的时候需要使用sws_scale转码)
	struct SwsContext *swsContext=NULL;
	struct MediaCodecParam *video=data->codec;
	int videoStreamIndex = video->stream_index;
	AVStream* videoStream = video->format_ctx->streams[	videoStreamIndex];	//流参数

	enum AVPixelFormat pix_fmt_dest = get_format_pixel_sdl(display->format);		//需要的转换后的格式
	enum AVPixelFormat pix_fmt_sour=video->codec_ctx->pix_fmt;				//视频原始的格式
	
	struct MediaVideoParams show_param_l,show_param;		//视频参数(宽高亮度等)		
	memset(&show_param_l,0,sizeof(struct MediaVideoParams));

	SDL_Rect dst_rect = {display->rect_dst->x, display->rect_dst->y, display->rect_dst->w, display->rect_dst->h};  // 设置目标矩形为 960x540
	SDL_Rect src_rect = {display->rect_src->x,display->rect_src->y,display->rect_src->w,display->rect_src->h};

	debug_printf("thread debug:\n");
	int64_t pts=0;	//帧的位置(需要解码才能知道)，可以辅助判断是否丢帧
	int numBytes;
	uint8_t *buffer = NULL;

	data->err_code=1;
	video_t->set_state(video_t,AUDIO_STATE_PLAYING);
	AVPacket *packet;
	int num=0;
	//video_t->clock->start(video_t->clock);
	while(video_t->is_running(video_t))
	{
		int cmd=user->command_get(user);
		switch(cmd)
		{
			case AUDIO_PLCMD_SUSPEND:
				video_t->set_state(video_t,AUDIO_STATE_PAUSEING);
				debug_printf("debug:video thread 暂停\n");
				video_t->wait_codec(video_t);
				debug_printf("debug:video thread 继续\n");
				break;
			default:
				break;
		}
		video_params_get_all(user,&show_param);
		if(show_param.rect.w==0||show_param.rect.h==0)
		{
			continue;
		}
		//重新设置解码器参数
		if(show_param.rect.w!=show_param_l.rect.w || show_param.rect.h!=show_param_l.rect.h)	//宽高不一样就从设大小
		{
			debug_printf("原始尺寸：%d*%d,需要显示成%d*%d\n",video->codec_ctx->width,video->codec_ctx->height,display->rect_dst->w,display->rect_dst->h);
			debug_printf("视频提取：%d,%d %d*%d,需要显示到%d,%d %d*%d\n\n",display->rect_src->x,display->rect_src->y,display->rect_src->w,display->rect_src->h,
																display->rect_dst->x,display->rect_dst->y,display->rect_dst->w,display->rect_dst->h);
			
			count_rect_size_from_user(display,user->video,video->codec_ctx);		//计算新的显示窗口尺寸

			debug_printf("thread debug:pix_fmt_sour != pix_fmt\n");
			swsContext = sws_getContext(video->codec_ctx->width, video->codec_ctx->height, 		//创建一个swsContext用于处理图像缩放格式转换
										pix_fmt_sour,
										//video->codec_ctx->width, video->codec_ctx->height, 
										display->rect_dst->w,display->rect_dst->h,
										pix_fmt_dest,
										SWS_BICUBIC, NULL, NULL, NULL);
			if (!swsContext) {
				fprintf(stderr, "Could not initialize SwsContext.\n");
				data->err_code=-1;
				return &data->err_code;
			}
			// 为转换后的格式申请空间
			if(malloc_codec_frame(display->rect_dst->w,display->rect_dst->h,pix_fmt_dest,&buffer,&frame_d)<0)
			{
				sws_freeContext(swsContext);
				data->err_code=-1;
				return &data->err_code;
			}
			if(display->is_sdl)
			{
				//调整窗口大小
				SDL_SetWindowSize(display->window, show_param.rect.w, show_param.rect.h);
				//更新纹理
				SDL_DestroyTexture(display->texture);		//销毁原来的纹理
				display->texture = sdl_creat_texture_near(display->renderer, &display->format,display->rect_dst->w,display->rect_dst->h);//创建新的纹理
			}
			show_param_l.rect.w=show_param.rect.w;
			show_param_l.rect.h=show_param.rect.h;
		}
		if(show_param.rect.x!=show_param_l.rect.x || show_param.rect.y!=show_param_l.rect.y)	//位置不一样
		{
			if(display->is_sdl)
				SDL_SetWindowPosition(display->window, show_param.rect.x, show_param.rect.y);
			show_param_l.rect.x=show_param.rect.x;
			show_param_l.rect.y=show_param.rect.y;
		}

		packet=video_t->get_packet(&video_t->list,1);
		if(video_t->get_state(video_t)==AUDIO_STATE_EXIT)
			break;
		//debug_printf("开始解码：ptr of frame_s%p, pptr of packet :%p（当前状态%d)\n",frame_s,&packet,video_t->get_state(video_t));
		video_t->set_state(video_t,AUDIO_STATE_PLAYING);
		
		if (avcodec_send_packet(video->codec_ctx, packet) < 0) {		//向解码器发送一个压缩的媒体包
			//fprintf(stderr, "Error sending packet to video codec\n");
			video_t->set_state(video_t,MEDIA_THREAD_WAITING);
			continue;
		}
		
		// Receive the decoded frame
		while (avcodec_receive_frame(video->codec_ctx, frame_s) == 0) 	//从解码器接收解压的媒体包
		{	
			//debug_printf("recv video,ptr of fram:%p\n",frame_s);
			//video_t->start_codec(video_t);
			pts = frame_s->pts;		//侦的位置
			if (pts == AV_NOPTS_VALUE) {
				pts = frame_s->best_effort_timestamp;		//该值无效则使用默认的值
			}
			
			//延时一段时间
			float speed=Audio_Get_Speed(user);
			double video_clock=(double)pts * av_q2d(videoStream->time_base)*1000.0*1000.0/speed;	//time_base为s
			double delay_time=video_clock-video_t->clock->get_run_time(video_t->clock);
			if(delay_time>0)
			{
				//debug_printf("延时%lf\n",delay_time);
				usleep(delay_time);
			}
			else if(delay_time< (-VIDEO_FRAME_LAG_LOSS_TIME))
			{
				//printf("===========舍弃====\n");
				break;
			}

			// Check if we need to convert the pixel format to RGB
			if(swsContext)
			{
				sws_scale(swsContext, (const uint8_t * const *)frame_s->data, frame_s->linesize,  0, video->codec_ctx->height,frame_d->data, frame_d->linesize);
			} 

			// 显示
			if(callback)
			{
				printf("callback\n");
				callback(frame_d->data,frame_d->linesize,pix_fmt_dest,user->userdata);
			}
			else
				video_display_image(frame_d->data,frame_d->linesize,pix_fmt_dest,display);
			
			//写入进度
			Audio_Set_Position_N(user,(int32_t)(video_clock/1000.0/1000.0));
		}
		video_t->free_packet(packet);
		video_t->set_state(video_t,MEDIA_THREAD_WAITING);
	}
	printf("显示宽高：%d*%d\n",display->rect_dst->w,display->rect_dst->h);
	debug_printf("video线程结束\n");
	if(1)
	//if (pix_fmt_sour != pix_fmt) 
	{
		free_codec_frame(buffer,frame_d);
		if (swsContext) {
			sws_freeContext(swsContext);
		}
	}  

	if(frame_s)
		av_frame_free(&frame_s);
	printf("video thread debug:exit ok\n");
	return NULL;
}


static void *thread_audio_codec(void *param)
{
	struct ThreadData *data=(struct ThreadData *) param;
	if(!data->codec)
		return NULL;
	struct MediaThread *audio_t=data->thread;
	struct VideoHardParam *display=data->display;
	struct MediaParams *user=data->user;
	AVFrame *frame_s = av_frame_alloc();	//原始的侦数据(直接从文件中解码出来的)
	if(frame_s == NULL) {
		data->err_code=-1;
		return &data->err_code;
	}

	struct MediaCodecParam *audio=data->codec;
	int audioStreamIndex = audio->stream_index;
	AVStream* audioStream = audio->format_ctx->streams[	audioStreamIndex];	//流参数

	double video_clock = 0.0,audio_clock=0.0; // 用于保持视频的播放时间

	// 分配音频缓冲区
//	int max_samples = 4096; // 设置一个初始值，可根据需要调整
//	uint8_t **converted_data = NULL;
//	av_samples_alloc_array_and_samples(&converted_data, NULL, audio->codec_ctx->channels, max_samples, AV_SAMPLE_FMT_S16, 1);//codec_ctx->frame_size
//	debug_printf("channel:%d\n",audio->codec_ctx->channels);

	data->err_code=1;
	audio_t->set_state(audio_t,AUDIO_STATE_PLAYING);
	AVPacket *packet;
	while(audio_t->is_running(audio_t))
	{
		int cmd=user->command_get(user);
		switch(cmd)
		{
			case AUDIO_PLCMD_SUSPEND:
				audio_t->set_state(audio_t,AUDIO_STATE_PAUSEING);
				debug_printf("debug:audio thread 暂停\n");
				audio_t->wait_codec(audio_t);
				debug_printf("debug:audio thread 继续\n");
				break;
			default:
				break;
		}
		packet=audio_t->get_packet(&audio_t->list,1);		//
		if(audio_t->get_state(audio_t)==AUDIO_STATE_EXIT)
			break;
		audio_t->set_state(audio_t,AUDIO_STATE_PLAYING);
		if (avcodec_send_packet(audio->codec_ctx, packet) < 0) {		//向解码器发送一个压缩的媒体包
			fprintf(stderr, "Error sending packet to audio codec\n");
			audio_t->set_state(audio_t,MEDIA_THREAD_WAITING);
			continue;
		}
		while (avcodec_receive_frame(audio->codec_ctx, frame_s) == 0) 
		{
			//struct AudioData *audioData=(struct AudioData *)display->audio_data;
			//debug_printf("recv audio侦数：%d\n",frame_s->nb_samples);
			/*if (frame_s->nb_samples > max_samples) 
			{		
				max_samples = frame_s->nb_samples;
				av_freep(&converted_data[0]);
				debug_printf("重新分配,侦数：%d\n",max_samples);
				av_samples_alloc_array_and_samples(&converted_data, NULL, audio->codec_ctx->channels, max_samples, AV_SAMPLE_FMT_S16, 1);
			}*/
			int64_t pts = frame_s->pts;		//侦的位置
			if (pts == AV_NOPTS_VALUE) {
				pts = frame_s->best_effort_timestamp;		//该值无效则使用默认的值
			}

			float speed=Audio_Get_Speed(user);
			double audio_clock=(double)pts * av_q2d(audioStream->time_base)*1000.0*1000.0/speed;	//time_base为s
			double delay_time=video_clock-audio_t->clock->get_run_time(audio_t->clock);
			if(delay_time>0)
			{
				//debug_printf("延时%lf\n",delay_time);
				usleep(delay_time);
			}
			else if(delay_time< (-VIDEO_FRAME_LAG_LOSS_TIME))
			{
				//printf("===========舍弃====\n");
				break;
			}
			AVFrame *convert_frame = alloc_avframe_frames_hard(frame_s->nb_samples,audio->hard_param);
			if(!convert_frame)
			{
				continue;
			}

			int samples_converted = swr_convert(display->swr_ctr, convert_frame->data, frame_s->nb_samples, (const uint8_t **)frame_s->data, frame_s->nb_samples);
			if(samples_converted>0)
				audio->callback_play((uint8_t *)convert_frame,samples_converted,-1,audio->callback_param);	//display->audio_data
		}
		audio_t->free_packet(packet);
		audio_t->set_state(audio_t,MEDIA_THREAD_WAITING);
	}
	debug_printf("audio线程结束\n");

	if(frame_s)
		av_frame_free(&frame_s);
	printf("audio thread debug:exit ok\n");
	return NULL;
}


int video_codec_play(struct VideoHardParam *display,struct MediaCodecParam *video,struct MediaCodecParam *audio,struct MediaParams *user)
{
	int err=0;
	int videoStreamIndex,audioStreamIndex;
	if(!video)
		return -1;
	videoStreamIndex = video->stream_index;
	if(!audio)
		audioStreamIndex=-1;
	else
		audioStreamIndex = audio->stream_index;
	AVPacket packet;
	struct MediaThread *video_t=Media_Thread_Creat();
	if(video_t==NULL)
	{
		return -1;
	}
	if(video_t->start_thread(video_t,thread_video_codec,display,video,&packet,user)<0)
	{
		Media_Thread_Free(video_t);
		return -1;
	}
	//video_t->get_state(video_t);
	debug_printf("thread creat ok\n");


	struct MediaThread *audio_t=Media_Thread_Creat();
	if(audio_t==NULL)
	{
		Media_Thread_Free(video_t);
		return -1;
	}
	if(audio_t->start_thread(audio_t,thread_audio_codec,display,audio,&packet,user)<0)
	{
		Media_Thread_Free(video_t);
		Media_Thread_Free(audio_t);
		return -1;
	}
	
	int wait_time=0;
	while(audio && !audio_t->is_running(audio_t)) 	//音频解码器未空就不创建音频解码线程
	{
		debug_printf("wait audio thread init\n");
		usleep(10000);		//10ms
		wait_time++;
		if(wait_time>100) 
		{
			Media_Thread_Free(video_t);
			Media_Thread_Free(audio_t);
			return -1;
		}
	}
	wait_time=0;
	while(!video_t->is_running(video_t)) 
	{
		debug_printf("wait video thread init\n");
		usleep(10000);
		wait_time++;
		if(wait_time>100) 
		{
			Media_Thread_Free(video_t);
			Media_Thread_Free(audio_t);
			return -1;
		}
	}
	debug_printf("ok\n");
	int test=0;
	video_t->clock->start(video_t->clock);
	audio_t->clock->start(audio_t->clock);
	Audio_Set_State(user,AUDIO_STATE_PLAYING);
	while (1) 	
	{
		if(video_t->packet_number(&video_t->list)>VIDEO_MAX_QUEUE_SIZE||
				audio_t->packet_number(&audio_t->list)>AUDIO_MAX_QUEUE_SIZE)
		{
			//debug_printf("队列已满，等待...\n");
			usleep(5000);
		}
		else if(av_read_frame(video->format_ctx, &packet) < 0)	//video和audio的format_ctx是同一个
			break;

#ifdef DEBUG_VIDEO
		if(video_flag==1)
		{
			debug_printf("===========================================================================================\n");
			video_t->start_codec(video_t);
			video_t->set_state(video_t,AUDIO_STATE_EXIT);
			break;
		}
#endif
		if((err=Audio_Get_Position_S(user))>=0)
		{
			AVRational reference_time_base = video->format_ctx->streams[videoStreamIndex]->time_base;
			int64_t target_timestamp = err / av_q2d(reference_time_base);
			av_seek_frame(video->format_ctx,-1,target_timestamp,AVSEEK_FLAG_BACKWARD);	//调整所有流的位置，stream_index设置为-1
			//清空队列
			video_t->flush_list(&video_t->list);
			audio_t->flush_list(&audio_t->list);
			//清空声卡缓存
			//media_pcm_drop(display->pcm_play);

			video_t->clock->adjust_time(video_t->clock,(long)err*1000*1000);
			audio_t->clock->adjust_time(audio_t->clock,(long)err*1000*1000);
			continue;
		}
		int cmd=user->command_get(user);
		switch(cmd)
		{
			case AUDIO_PLCMD_SUSPEND:
				Audio_Set_State(user,AUDIO_STATE_PAUSEING);
				debug_printf("debug:暂停\n");
				video_t->clock->pause(video_t->clock);
				audio_t->clock->pause(audio_t->clock);
				user->cond->wait(user->cond);
				video_t->clock->resume(video_t->clock);
				audio_t->clock->resume(audio_t->clock);
				debug_printf("debug:继续\n");
				audio_t->start_codec(audio_t);
				video_t->start_codec(video_t);
				Audio_Set_State(user,AUDIO_STATE_PLAYING);
				break;
			case AUDIO_PLCMD_NEXT:
			case AUDIO_PLCMD_LAST:
				Audio_Set_State(user,AUDIO_STATE_JUMP);
			case AUDIO_PLCMD_STOP:
			case AUDIO_PLCMD_EXIT:
				video_t->set_state(video_t,AUDIO_STATE_EXIT);
				video_t->packet_exit(&video_t->list);					//防止队列中没有数据，线程阻塞
				audio_t->set_state(audio_t,AUDIO_STATE_EXIT);
				audio_t->packet_exit(&audio_t->list);					//防止队列中没有数据，线程阻塞
				//清空队列
				video_t->flush_list(&video_t->list);
				audio_t->flush_list(&audio_t->list);
				//清空声卡缓存
				media_pcm_drop(display->pcm_play);
				debug_printf("退出\n");
				goto FREE_AUDIO_THREAD;
			default:
				break;
		}

		if (audioStreamIndex>=0 && packet.stream_index == audioStreamIndex) 
		{
			//debug_printf("recv audio\n");
			audio_t->push_packet(&audio_t->list,&packet);
			packet.stream_index=-1;
        }
		else if (videoStreamIndex>=0 && packet.stream_index == videoStreamIndex) 
		{
			// Send the packet to the decoder
			//debug_printf("recv video,%d\n",test);
			video_t->push_packet(&video_t->list,&packet);
			packet.stream_index=-1;
		}
		else{
			packet.stream_index=-1;
		}
			
		av_packet_unref(&packet);
	}
	test=0;
	while(0)
	{
		if(video_t->packet_number(&video_t->list)==0 && audio_t->packet_number(&audio_t->list)==0)
			break;
		usleep(10000);
		test++;
		if(test>1000)
			break;
	}
	video_t->set_state(video_t,AUDIO_STATE_EXIT);
	audio_t->set_state(audio_t,AUDIO_STATE_EXIT);
	video_t->packet_exit(&video_t->list);
	audio_t->packet_exit(&audio_t->list);
	// Clean up

FREE_AUDIO_THREAD:
	Media_Thread_Free(audio_t);
	Media_Thread_Free(video_t);
    return err;
}

//流播放线程结构体创建
struct MediaThread *Media_Thread_Creat()
{
	struct MediaThread *thread=(struct MediaThread *)malloc(sizeof(struct MediaThread));
	if(thread==NULL)
		return NULL;
	if((thread->clock=timer_ofday_handle_creat())==NULL)
	{
		free(thread);
		return NULL;
	}

	pthread_cond_init(&thread->cond.cond,NULL);
    pthread_mutex_init(&thread->cond.lock,NULL);
	thread->cond.flag=0;
	pthread_mutex_init(&thread->lock,NULL);

	thread->wait_codec=thread_wait_codec;		//线程需要挂起时候使用
	thread->start_codec=thread_start_codec;
	thread->get_state=thread_get_state;
	thread->set_state=thread_set_state;
	thread->start_thread=thread_start_running;
	thread->is_running=thread_is_running;
	thread->push_packet=packet_queue_put;
	thread->get_packet=packet_queue_get;
	thread->free_packet=free_packet;
	thread->packet_number=get_packet_number;
	thread->packet_exit=packet_queue_exit;
	thread->flush_list=packet_queue_flush;
	packet_queue_init(&thread->list);			//初始化包队列
	return thread;
}

//
int Media_Thread_Free(struct MediaThread *thread)
{
	thread->set_state(thread,AUDIO_STATE_NONE);
	pthread_join(thread->thread,NULL);
	pthread_cond_destroy(&thread->cond.cond);
    pthread_mutex_destroy(&thread->cond.lock);
	pthread_mutex_destroy(&thread->lock);
	packet_queue_destroy(&thread->list);
	
	timer_ofday_handle_free(thread->clock);

	return 0;
}

/// @brief 获取编解码信息
/// @return 
int Video_Get_File_Info(const char *filename,struct MediaCodecParam *codec_v,struct MediaCodecParam *codec_a)
{
	FILE *fp=fopen(filename,"rb");
	if(fp)
		fclose(fp);
	return video_find_codec(filename,codec_v,codec_a);
}

/// @brief 释放文件
/// @param codec_v 
/// @param codec_a 
/// @return 
int Video_Free_File(struct MediaCodecParam *codec_v,struct MediaCodecParam *codec_a)
{
	avcodec_free_context(&codec_a->codec_ctx);
	avformat_close_input(&codec_a->format_ctx);		//因为是一个文件，video和audio共用一个format_ctx只需释放一次
	avcodec_free_context(&codec_v->codec_ctx);
	return 0;
}

int Video_File_Codec(struct VideoHardParam *display,struct MediaCodecParam *codec_v,struct MediaCodecParam *codec_a,struct MediaParams *user)
{
	signal(SIGINT, exit_signal);
	return video_codec_play(display,codec_v,codec_a,user);
}



#ifdef __cplusplus
}
#endif