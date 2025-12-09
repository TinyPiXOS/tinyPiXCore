
#include <signal.h>		//用于signal函数，测试使用
#include "Media/Media/media_codec.h"
#include "Media/Media/media_play.h"
#include "Media/Video/video_display.h"
#include "Media/Video/video_play.h"
#include "Media/Audio/audio_play.h"


#ifdef DEBUG_MEDIA_CODEC
#include "Log/elog.h"
#define debug_printf(...) 	elog_d("MediaPlayCodec", ##__VA_ARGS__)
//#define debug_printf(fmt, ...) elog_d(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...)  // 如果不定义DEBUG，什么也不做
#endif


/*struct AudioData
{
    uint8_t *buffer;
    int buffer_size;
    int buffer_pos;
    double pts; // 当前音频的时间戳
};*/
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
};

//线程同步
struct MediaThreadCond{
	uint8_t flag;
	pthread_mutex_t lock;
	pthread_cond_t cond ;
};



//编解码播放线程信息
struct MediaThread{
	pthread_t thread;			//线程标号
	MediaPlayState state;		//线程状态
	uint8_t running;
	uint8_t codec;
	pthread_mutex_t lock;			//数据锁

	int (*start_thread)(struct MediaThread *thread,struct TimerHandle *clock,void *(*thread_main)(void *),struct MediaStreamParams *stream,struct MediaUserParams *user,bool is_sync);
	int (*is_running)(struct MediaThread *thread);	//线程是否在运行中

	MediaPlayState (*get_state)(struct MediaThread *thread);				//获取线程状态
	int (*set_state)(struct MediaThread *thread, MediaPlayState state);		//设置线程状态

	struct{
		struct MediaThreadCond cond;	//线程队列使用
		int (*wait_codec)(struct MediaThread *thread);		//等待解码
		int (*start_codec)(struct MediaThread *thread);		//开始解码

		int (*send_codec_signal)(struct MediaThread *thread);		//开始解码（暂时不使用，准备移除）
	};

	//待解码队列
	struct{
		struct MediaPacketQueue list;
		int list_max;		//队列允许最大成员数量,只允许在线程创建之前修改

		//队列操作
		int (*push_packet)(struct MediaPacketQueue *list,AVPacket *packet);				//向缓存队列中写入
		AVPacket *(*get_packet)(struct MediaPacketQueue *list,int block);	//从缓存队列中读取
		int (*free_packet)(AVPacket *packet);		//使用get_packet_list获取packet后调用此函数释放
		int (*unref_packet)(AVPacket *packet);		//引用计数减一
		int (*flush_list)(struct MediaPacketQueue *list);			//删除队列中所有元素
		uint32_t (*packet_number)(struct MediaPacketQueue *list);	//获取队列中元素数量
		int (*packet_exit)(struct MediaPacketQueue *list);
	};
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
		return -1;
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
//	debug_printf("debug: package nb:%d\n",q->nb_packets);	
	return 0;
}

//退出阻塞，并退出队列
static int packet_queue_exit(struct MediaPacketQueue *q)
{
	pthread_mutex_lock(&q->lock);
	q->exit_flag=1;
	pthread_cond_signal(&q->cond); // 通知
	pthread_mutex_unlock(&q->lock);
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
			/*pkt = av_packet_alloc();  // 使用 av_packet_alloc() 分配内存
			if (!pkt) {
				fprintf(stderr, "Could not allocate AVPacket\n");
				break;
			}
			if (av_packet_ref(pkt, pkt_node->pkt) < 0) {
				ret = -1;
				fprintf(stderr, "Could not copy AVPacket\n");
				break;
			}*/

			pkt = pkt_node->pkt; // 现在pkt_ret指向了节点内的数据包
            pkt_node->pkt = NULL; // 将节点的pkt置为NULL，防止节点被释放时误删AVPacket
			
			// 释放队列节点
			//av_packet_free(&pkt_node->pkt );
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
			if(q->exit_flag == 1) 
				break;
		}
	}

	pthread_mutex_unlock(&q->lock);
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

static int unref_packet(AVPacket *packet)
{
	if(packet)
		av_packet_unref(packet);
}

//清空队列
static int packet_queue_flush(struct MediaPacketQueue *q) 
{
	MediaPacketList *pkt_list, *pkt_tmp;
//	printf("[Debug]: packet_queue_flush\n");
	pthread_mutex_lock(&q->lock);
//	printf("[Debug]: packet_queue_flush lock\n");
	// 遍历并释放队列中的所有数据包
	pkt_list = q->first_pkt;
	while (pkt_list) {
		pkt_tmp = pkt_list->next;

		// 释放每个 AVPacket 的资源
		if (pkt_list->pkt)
			av_packet_free(&pkt_list->pkt );
		av_free(pkt_list);  // 释放节点本身

		pkt_list = pkt_tmp;
	}
//	printf("[Debug]: packet_queue_flush 重置队列\n");
	// 重置队列
	q->first_pkt = NULL;
	q->last_pkt = NULL;
	q->nb_packets = 0;
	q->size = 0;

	pthread_mutex_unlock(&q->lock);
//	printf("[Debug]: packet_queue_flush ok\n");
	return 0;
}
//释放包队列
static void packet_queue_destroy(struct MediaPacketQueue *q) {
    packet_queue_flush(q);  // 清空队列中的所有数据包
	pthread_mutex_destroy(&q->lock);
	pthread_cond_destroy(&q->cond);
}

static uint32_t get_packet_number(struct MediaPacketQueue *q)
{
	uint32_t nb;
	pthread_mutex_lock(&q->lock);
	nb=q->nb_packets;
	pthread_mutex_unlock(&q->lock);
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
static MediaPlayState thread_get_state(struct MediaThread *thread)
{
    MediaPlayState state;
    pthread_mutex_lock(&thread->lock);  // 锁定
    state = thread->state;
    pthread_mutex_unlock(&thread->lock);  // 解锁
    return state;
}

//设置状态
static int thread_set_state(struct MediaThread *thread, MediaPlayState state)
{
	if(state == MEDIA_STATE_EXIT)
	{
		debug_printf("Thread exit(%p)\n",thread);
	}

    pthread_mutex_lock(&thread->lock);  // 锁定
	if(thread->state!=MEDIA_STATE_EXIT)		//非退出状态才允许设置状态
    	thread->state=state;
    pthread_mutex_unlock(&thread->lock);  // 解锁
	return 0;
}

struct ThreadData{
	struct MediaThread *thread;
	struct MediaStreamParams *stream;
	AVFrame *frame_s ;		//原始的侦数据(直接从文件中解码出来的)
	int8_t err_code;		//错误码
	struct TimerHandle *clock;
	struct MediaUserParams *user;		//用户交互
	bool is_sync;		//是否是同步时钟参考流
};


static int thread_start_running(struct MediaThread *thread,
								struct TimerHandle *clock,
								void *(*thread_main)(void *),
								struct MediaStreamParams *stream,
								struct MediaUserParams *user,bool is_sync)
{
	struct ThreadData *data=(struct ThreadData *)malloc(sizeof(struct ThreadData));
	//thread->thread_param=data;
	data->thread=thread;
	data->stream=stream;
	data->err_code=0;
	data->user=user;
	data->clock=clock;
	data->is_sync=is_sync;
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
	MediaPlayState state=thread_get_state(thread);
	if(state==MEDIA_STATE_PLAYING || state==MEDIA_THREAD_WAITING )
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
static double count_media_clock_delay_time(struct MediaUserParams *user,struct TimerHandle *clock,int64_t pts, AVRational time_base)
{
	float speed=Media_Get_Speed(user);
	//延时一段时间
	double video_clock = (double)pts * av_q2d(time_base)*1000.0*1000.0/speed;		//time_base为s
	double delay_time=video_clock-clock->get_run_time(clock);
	return delay_time;
}




uint8_t media_exit_flag=0;
struct MediaThread *Media_Thread_Creat();
static int Media_Thread_Free(struct MediaThread *thread);

static void exit_signal(int sig)
{
	media_exit_flag=1;
}


static AVCodecContext *media_get_decodec_context(AVFormatContext *format_ctx, int index)
{
	AVCodecContext *mediaCodecContext;
	AVCodec *mediaCodec;		//无需显式释放
	// Get video stream codec context
	mediaCodecContext = avcodec_alloc_context3(NULL);		//AVCodecContext *mediaCodecContext
	if (!mediaCodecContext) {
		fprintf(stderr, "Could not allocate video codec context.\n");
		goto FREE;
	}
	avcodec_parameters_to_context(mediaCodecContext, format_ctx->streams[index]->codecpar);

	// Find the decoder
	mediaCodec = avcodec_find_decoder(mediaCodecContext->codec_id);		//AVCodec *videoCodec
	if (!mediaCodec) {
		fprintf(stderr, "Codec not found.\n");
		goto FREE;
	}

	// Open codec
	if (avcodec_open2(mediaCodecContext, mediaCodec, NULL) < 0) {
		fprintf(stderr, "Could not open codec.\n");
		goto FREE;
	}
	return mediaCodecContext;
FREE:
	avcodec_free_context(&mediaCodecContext);
	return NULL;
}


//url:
//media_array:MediaStreamParams类型的动态数组
static MediaFormatContext *media_find_codec(const char *url, MediaStreamArray *media_array)
{
	int ret=0;
	AVFormatContext *format_ctx = NULL;
	int size_array=media_array->get_size(media_array);	//获取当前的成员数量
	media_init(1);	//初始化并使能网络流

    // 打开媒体文件
	if(media_get_file_info(url,&format_ctx)<0)
		return NULL;
    // 查找流合适的解码器()av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	for (int i = 0; i < format_ctx->nb_streams; i++) 
	{
		switch(format_ctx->streams[i]->codecpar->codec_type)
		{
			case AVMEDIA_TYPE_VIDEO:   		// 视频流
        	case AVMEDIA_TYPE_AUDIO:   		// 音频流
			{
				AVCodecContext *codec_ctx=media_get_decodec_context(format_ctx,i);
				if(!codec_ctx)
					break;
				struct MediaStreamParams *stream=media_stream_params_creat();
				if(!stream)
					break;
				stream->type=format_ctx->streams[i]->codecpar->codec_type;
				stream->codec_ctx=codec_ctx;
				stream->format_ctx=format_ctx;
				stream->stream_index=i;
				if(media_array->append_shallow(media_array,stream)<0)
					fprintf(stderr,"append_shallow error\n");
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE: 	// 字幕流 (FFmpeg ID:0x10000)
			case AVMEDIA_TYPE_DATA:    		// 数据流 (e.g., timed metadata)
			case AVMEDIA_TYPE_ATTACHMENT: 	// 附件 (如图片)
    		case AVMEDIA_TYPE_NB:
			default:
				break;
		}

	}

	if(media_array->get_size(media_array)==size_array)		//没有在url中找到任何流
	{
		fprintf(stderr,"[Warning]: 没有在url中找到任何流,%d\n",size_array);
		avformat_close_input(&format_ctx);
		return NULL;
	}

	struct MediaStreamParams *stream=media_array->get(media_array,0);
	return format_ctx;
}

//获取流的AVFormatContext
static AVFormatContext *media_get_format_context(MediaStreamArray *array)
{
	struct MediaStreamParams *stream0=array->get(array,0);
	return stream0->format_ctx;
}

//根据时间设置流跳转到制定位置
static int media_seek_frame_with_time(struct MediaUserParams *user, struct MediaPlayerHandle *player, uint32_t sec)
{
	AVFormatContext *format_ctx=player->format_ctx;
	AVRational reference_time_base = format_ctx->streams[player->sync_clk_stream_index]->time_base;
	int64_t target_timestamp = sec / av_q2d(reference_time_base);
	
	//调整所有流的位置，stream_index设置为同步时钟所在的流，设置为-1为自动选择，但是目前自动选择有问题
	av_seek_frame(format_ctx,player->sync_clk_stream_index,target_timestamp,AVSEEK_FLAG_BACKWARD);	
	uint32_t position_bytes=Audio_Get_BitsPerSample(user);
	if(position_bytes!=0)
		Media_Set_Position_N(user,sec*position_bytes);
	else	
		Media_Set_Position_N(user,sec);

	return 0;	
}

//向解码器发送数据包的封装
static int media_send_packet_to_codecc(struct MediaStreamParams *stream,struct MediaThread *thread,AVPacket *packet)
{
	int ret=0;
	pthread_mutex_lock(&thread->lock);
	ret=avcodec_send_packet(stream->codec_ctx, packet);		//向解码器发送一个压缩的媒体包
	pthread_mutex_unlock(&thread->lock);
	return ret;
}

//从解码器接收数据包的封装
static int media_recv_packet_from_codecc(struct MediaStreamParams *stream,struct MediaThread *thread,AVFrame *frame)
{
	int ret=0;
	pthread_mutex_lock(&thread->lock);
	ret=avcodec_receive_frame(stream->codec_ctx, frame);
	pthread_mutex_unlock(&thread->lock);
	return ret;
}

//视频播放的视频解码线程
static void *thread_video_codec(void *param) 
{
	struct ThreadData *data=(struct ThreadData *) param;
	struct TimerHandle *sys_clock=data->clock;
	struct MediaThread *video_t=data->thread;
	bool is_sync=data->is_sync;
	struct MediaUserParams *user=data->user;
	struct MediaStreamParams *stream=data->stream;

	CallbackVideoDisplay callback=user->video_params->get_callback_video(user->video_params);
	AVFrame *frame_s = av_frame_alloc();	//原始的侦数据(直接从文件中解码出来的)
	if(frame_s == NULL) {
		data->err_code=-1;
		return &data->err_code;
	}
	AVFrame *frame_d ;		//需要写入SDL的格式的数据(/可能和frame_s一致，也可能不一致，不一致的时候需要使用sws_scale转码)
	struct SwsContext *swsContext=NULL;
	int videoStreamIndex = stream->stream_index;		//流索引
	AVStream* videoStream = stream->format_ctx->streams[videoStreamIndex];	//流参数

	enum AVPixelFormat pix_fmt_dest = stream->video.format;					//需要的转换后的格式
	enum AVPixelFormat pix_fmt_sour=stream->codec_ctx->pix_fmt;				//视频原始的格式
	
	struct VideoStreamParams show_param_l,show_param;		//视频参数(宽高亮度等)		
	memset(&show_param_l,0,sizeof(struct VideoStreamParams));

	struct MediaRect rect_src,rect_dst;		//用于解码前后的矩形区域

	int64_t pts=0;	//帧的位置(需要解码才能知道)，可以辅助判断是否丢帧
	int numBytes;
	uint8_t *buffer = NULL;

	data->err_code=1;
	video_t->set_state(video_t,MEDIA_STATE_PLAYING);
	AVPacket *packet;
	int num=0;
	//video_t->clock->start(video_t->clock);
#ifdef MEDIA_SDL_ENABLE
	uint32_t sdl_format;
	if(stream->video.handle->is_sdl)
		SDL_ShowWindow(stream->video.handle->window);
#endif
	while(video_t->is_running(video_t))
	{

		//printf("sizeof list %d,time:%ld\n",video_t->list.size ,sys_clock->get_run_time(sys_clock));
		int cmd=user->command_get(user);
		switch(cmd)
		{
			case MEDIA_PLCMD_SUSPEND:
				video_t->set_state(video_t,MEDIA_STATE_PAUSEING);
				debug_printf("debug:video thread 暂停\n");
				video_t->wait_codec(video_t);
				debug_printf("debug:video thread 继续\n");
				break;
			default:
				break;
		}
		get_display_params_user_codec(user,NULL,&show_param);
		if(show_param.rect.w==0||show_param.rect.h==0)
		{
			continue;
		}
		//重新设置解码器参数
		if(show_param.rect.w!=show_param_l.rect.w || show_param.rect.h!=show_param_l.rect.h)	//宽高不一样就从设大小
		{
			count_rect_size_from_user(user->video_params->video,stream->codec_ctx,&rect_src,&rect_dst);		//计算新的显示窗口尺寸

			debug_printf("原始尺寸：%d*%d,需要显示成%d*%d\n",stream->codec_ctx->width,stream->codec_ctx->height,rect_dst.w,rect_dst.h);
			debug_printf("视频提取：%d,%d %d*%d,需要显示到%d,%d %d*%d\n\n",rect_src.x,rect_src.y,rect_src.w,rect_src.h,
																rect_dst.x,rect_dst.y,rect_dst.w,rect_dst.h);
			

			debug_printf("thread debug:pix_fmt_sour != pix_fmt\n");
			swsContext = sws_getContext(stream->codec_ctx->width, stream->codec_ctx->height, 		//创建一个swsContext用于处理图像缩放格式转换
										pix_fmt_sour,
										//stream->codec_ctx->width, stream->codec_ctx->height, 
										rect_dst.w,rect_dst.h,
										pix_fmt_dest,
										SWS_BICUBIC, NULL, NULL, NULL);
			if (!swsContext) {
				fprintf(stderr, "Could not initialize SwsContext.\n");
				data->err_code=-1;
				return &data->err_code;
			}
			// 为转换后的格式申请空间
			if(malloc_codec_frame(rect_dst.w,rect_dst.h,pix_fmt_dest,&buffer,&frame_d)<0)
			{
				sws_freeContext(swsContext);
				data->err_code=-1;
				return &data->err_code;
			}
#ifdef MEDIA_SDL_ENABLE
			if(stream->video.handle->is_sdl)
			{
				//调整窗口大小
				SDL_SetWindowSize(stream->video.handle->window, show_param.rect.w, show_param.rect.h);
				//更新纹理
				if(stream->video.handle->texture)
					SDL_DestroyTexture(stream->video.handle->texture);		//销毁原来的纹理
				sdl_format=(uint32_t)get_sdl_pixel_format(pix_fmt_dest);	
				stream->video.handle->texture = sdl_creat_texture_near(stream->video.handle->renderer, &sdl_format,rect_dst.w,rect_dst.h);//创建新的纹理
				if(!stream->video.handle->texture)
				{
					continue;
				}
				pix_fmt_dest=get_format_pixel_sdl(sdl_format);
			}
#endif
			show_param_l.rect.w=show_param.rect.w;
			show_param_l.rect.h=show_param.rect.h;
		}
		if(show_param.rect.x!=show_param_l.rect.x || show_param.rect.y!=show_param_l.rect.y)	//位置不一样
		{
#ifdef MEDIA_SDL_ENABLE
			if(stream->video.handle->is_sdl)
				SDL_SetWindowPosition(stream->video.handle->window, show_param.rect.x(), show_param.rect.y);
#endif
			show_param_l.rect.x=show_param.rect.x;
			show_param_l.rect.y=show_param.rect.y;
		}

		packet=video_t->get_packet(&video_t->list,1);
		if(video_t->get_state(video_t)==MEDIA_STATE_EXIT)
			break;
		//debug_printf("开始解码：ptr of frame_s%p, pptr of packet :%p（当前状态%d)\n",frame_s,&packet,video_t->get_state(video_t));
		video_t->set_state(video_t,MEDIA_STATE_PLAYING);
		if(!packet)
		{
			continue;
		}

		if (media_send_packet_to_codecc(stream,video_t, packet) < 0) {		//向解码器发送一个压缩的媒体包
			fprintf(stderr, "Error sending packet to video codec\n");
			video_t->set_state(video_t,MEDIA_THREAD_WAITING);
			continue;
		}

		// Receive the decoded frame
		while (media_recv_packet_from_codecc(stream, video_t,frame_s) == 0) 	//从解码器接收解压的媒体包
		{	
			//debug_printf("recv video,ptr of fram:%p\n",frame_s);
			//video_t->start_codec(video_t);
			pts = frame_s->pts;		//侦的位置
			if (pts == AV_NOPTS_VALUE) {
				pts = frame_s->best_effort_timestamp;		//该值无效则使用默认的值
			}
			
			//延时一段时间
			//float speed=Media_Get_Speed(user);
			float speed=1.0;

			double video_clock=(double)pts * av_q2d(videoStream->time_base)*1000.0*1000.0/speed;	//time_base为s
			double delay_time=video_clock - sys_clock->get_run_time(sys_clock);
			if(delay_time>VIDEO_FRAME_LAG_LOSS_TIME)
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
				sws_scale(swsContext, (const uint8_t * const *)frame_s->data, frame_s->linesize,  0, stream->codec_ctx->height,frame_d->data, frame_d->linesize);
			} 

			// 显示
			if(callback)
			{
				//printf("callback\n");
				callback(frame_d->data,frame_d->linesize,pix_fmt_dest,user->video_params->userdata);
			}
			else
			{
#ifdef MEDIA_SDL_ENABLE
				video_display_image_sdl(frame_d->data,frame_d->linesize,sdl_format,
								stream->video.handle->renderer,stream->video.handle->texture,
								&rect_src,&rect_dst);
#endif
			}
				
			//写入进度
			Media_Set_Position_N(user,(int32_t)((double)pts * av_q2d(videoStream->time_base)));
		}
		video_t->free_packet(packet);
		video_t->set_state(video_t,MEDIA_THREAD_WAITING);
	}
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
	struct TimerHandle *sys_clock=data->clock;
	struct MediaThread *audio_t=data->thread;
	bool is_sync=data->is_sync;
	struct MediaStreamParams *stream=data->stream;
	struct MediaUserParams *user=data->user;
	
	CallbackAudioPlay callback=user->audio_params->get_callback_audio(user->audio_params);
	AVFrame *frame_s = av_frame_alloc();	//原始的侦数据(直接从文件中解码出来的)
	if(frame_s == NULL) {
		data->err_code=-1;
		return &data->err_code;
	}
	
	int audioStreamIndex = stream->stream_index;
	AVStream* audioStream = stream->format_ctx->streams[audioStreamIndex];	//流参数

	// 分配音频缓冲区
//	int max_samples = 4096; // 设置一个初始值，可根据需要调整
//	uint8_t **converted_data = NULL;
//	av_samples_alloc_array_and_samples(&converted_data, NULL, audio->codec_ctx->channels, max_samples, AV_SAMPLE_FMT_S16, 1);//codec_ctx->frame_size
//	debug_printf("channel:%d\n",audio->codec_ctx->channels);

	data->err_code=1;
	audio_t->set_state(audio_t,MEDIA_STATE_PLAYING);
	AVPacket *packet;
	while(audio_t->is_running(audio_t))
	{
		int cmd=user->command_get(user);
		switch(cmd)
		{
			case MEDIA_PLCMD_SUSPEND:
				audio_t->set_state(audio_t,MEDIA_STATE_PAUSEING);
				debug_printf("debug:audio thread 暂停\n");
				audio_t->wait_codec(audio_t);
				debug_printf("debug:audio thread 继续\n");
				break;
			default:
				break;
		}
		packet=audio_t->get_packet(&audio_t->list,1);		//
		if(audio_t->get_state(audio_t)==MEDIA_STATE_EXIT)
			break;
		audio_t->set_state(audio_t,MEDIA_STATE_PLAYING);

		if (media_send_packet_to_codecc(stream,audio_t, packet) < 0) {		//向解码器发送一个压缩的媒体包
			fprintf(stderr, "Error sending packet to audio codec\n");
			audio_t->set_state(audio_t,MEDIA_THREAD_WAITING);
			continue;
		}
		
		while (media_recv_packet_from_codecc(stream, audio_t,frame_s) == 0) 
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
			//float speed=Media_Get_Speed(user);
			float speed=1.0;
			double audio_clock=(double)pts * av_q2d(audioStream->time_base)*1000.0*1000.0/speed;	//time_base为s
			double delay_time=audio_clock - sys_clock->get_run_time(sys_clock);
			if(delay_time>0)
			{
				//debug_printf("延时%lf\n",delay_time);
				//usleep(delay_time);
			}
			else if(delay_time< (-VIDEO_FRAME_LAG_LOSS_TIME))
			{
				//debug_printf("===========舍弃====\n");
				//break;
			}
			AVFrame *convert_frame = alloc_avframe_frames_hard(frame_s->nb_samples,stream->audio.handle->adparams);
			if(!convert_frame)
			{
				continue;
			}
			int samples_converted = swr_convert(stream->audio.swr_ctx, convert_frame->data, frame_s->nb_samples, (const uint8_t **)frame_s->data, frame_s->nb_samples);
			if(samples_converted<=0)
				continue;
			
			if(!is_sync)
				callback((uint8_t *)convert_frame,samples_converted,1,user->audio_params->userdata);
			else
			{
				callback((uint8_t *)convert_frame,samples_converted,-1,user->audio_params->userdata);
				Media_Set_Position_N(user,(int32_t)((double)pts * av_q2d(audioStream->time_base)));
			}
		}
		audio_t->free_packet(packet);
		audio_t->set_state(audio_t,MEDIA_THREAD_WAITING);
	}
	debug_printf("audio线程结束\n");

	if(frame_s)
		av_frame_free(&frame_s);
	printf("audio thread debug:exit ok\n");
	audio_pcm_drop(stream->audio.handle);
	return NULL;
}


//释放每个流的播放线程
static int media_delete_player_thread(MediaStreamArray *stream_array)
{
	if(!stream_array)
		return 0;
	int stream_num=stream_array->get_size(stream_array);
	for(int i=0;i<stream_num;i++)
	{
		struct MediaStreamParams *stream=stream_array->get(stream_array,i);
		Media_Thread_Free(stream->codec_thread);
	}
}

//为每个流创建播放线程
static int media_creat_player_thread(MediaStreamArray *stream_array,struct TimerHandle *clock,struct MediaUserParams *user,int sync_index)
{
	int err=0;
	int stream_num=stream_array->get_size(stream_array);
	bool is_sync=false;		//是否是同步时钟参考流
	for(int i=0; i<stream_num; i++)
	{
		struct MediaThread *thread_codec=Media_Thread_Creat();
		if(thread_codec==NULL)
		{
			err=-1;
			break;
		}
		struct MediaStreamParams *stream=stream_array->get(stream_array,i);
		if(sync_index==stream->stream_index)
			is_sync=true;
		else
			is_sync=false;
		switch(stream->type)
		{
			case AVMEDIA_TYPE_VIDEO:   		// 视频流
			{
				thread_codec->list_max=VIDEO_MAX_QUEUE_SIZE;
				if(thread_codec->start_thread(thread_codec,clock,thread_video_codec,stream,user,is_sync)<0)
				{
					Media_Thread_Free(thread_codec);
					err=-1;
					break;
				}
				break;
			}
        	case AVMEDIA_TYPE_AUDIO:   		// 音频流
			{
				thread_codec->list_max=AUDIO_MAX_QUEUE_SIZE;
				if(thread_codec->start_thread(thread_codec,clock,thread_audio_codec,stream,user,is_sync)<0)
				{
					Media_Thread_Free(thread_codec);
					err=-1;
					printf("[Debug]: Creat audio stream thread error\n");
					break;
				}
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE: 	// 字幕流 (FFmpeg ID:0x10000)
			case AVMEDIA_TYPE_DATA:    		// 数据流 (e.g., timed metadata)
			case AVMEDIA_TYPE_ATTACHMENT: 	// 附件 (如图片)
    		case AVMEDIA_TYPE_NB:
			default:
				break;
		}

		stream->codec_thread=thread_codec;
	}

	if(err<0)
	{
		media_delete_player_thread(stream_array);
		return -1;
	}

	//等待所有线程创建完毕
	for(int i=0; i<stream_num; i++)
	{
		int wait_time=0;
		struct MediaStreamParams *stream=stream_array->get(stream_array,i);
		struct MediaThread *codec_thread=stream->codec_thread;
		while(stream_array->get(stream_array,i) && !codec_thread->is_running(codec_thread)) 	//音频解码器未空就不创建音频解码线程
		{
			debug_printf("wait stream thread init\n");
			usleep(100000);		//10ms
			wait_time++;
			if(wait_time>100) 
			{
				fprintf(stderr,"[Error]:threadm init timeout\n");
				break;
			}

		}
	}

	return 0;
}

//
#define FOREACH_THREAD(array, thread_var) \
    struct MediaStreamParams* _stream; \
    for(int _i = 0, _count = (array)->get_size(array); \
        _i < _count && (_stream = (array)->get(array, _i)) && \
        ((thread_var) = _stream->codec_thread); \
        _i++)

//停止同步时钟
static int media_player_pause(struct MediaPlayerHandle *handle)
{
	handle->clock->pause(handle->clock);
}

//继续同步时钟
static int media_player_resume(struct MediaPlayerHandle *handle)
{
	handle->clock->resume(handle->clock);
}

//开解解码所有流
static int media_player_codec_start(MediaStreamArray *stream_array)
{
	struct MediaThread* t;
	FOREACH_THREAD(stream_array,t){
		t->start_codec(t);
	};
}

static int media_player_codec_stop(MediaStreamArray *stream_array)
{
	struct MediaThread* t;
	FOREACH_THREAD(stream_array,t){

	};
}

//同时设置所有流的状态
static int media_player_set_state(MediaStreamArray *stream_array,MediaPlayState state)
{
	struct MediaThread* t;
	FOREACH_THREAD(stream_array,t){
		t->set_state(t,MEDIA_STATE_EXIT);
	};
}


static int media_player_packet_exit(MediaStreamArray *stream_array)
{
	struct MediaThread* t;
	FOREACH_THREAD(stream_array,t){
		t->packet_exit(&t->list);
	};
}

static int media_player_flush_list(MediaStreamArray *stream_array)
{
	struct MediaThread* t;
	FOREACH_THREAD(stream_array,t){
		t->flush_list(&t->list);
	};
}

static int media_avcodec_flush_buffers(MediaStreamArray *stream_array)
{
	int size_array=stream_array->get_size(stream_array);

	for(int i=0; i<size_array; i++)
	{
		struct MediaStreamParams *stream=stream_array->get(stream_array,i);
		struct MediaThread* t=(struct MediaThread* )stream->codec_thread;
		pthread_mutex_lock(&t->lock);
		avcodec_flush_buffers(stream->codec_ctx);
		pthread_mutex_unlock(&t->lock);
	}
	return 0;
}

//写入数据包到对应的队列
static int media_write_packet_to_queue(MediaStreamArray *stream_array, AVPacket *packet)
{
	int stream_num=stream_array->get_size(stream_array);
	for(int i=0; i<stream_num; i++)
	{
		struct MediaStreamParams *stream=stream_array->get(stream_array,i);

		if(stream->stream_index==packet->stream_index)
		{
			struct MediaThread *thread=stream->codec_thread;
			thread->push_packet(&thread->list,packet);
		}	
	}
	packet->stream_index=-1;
}

//获取所有流的队列状态,
//只要有一个队列满就返回满，所有为空才会返回空
static MediaPacketQueueState media_player_get_queue_state(MediaStreamArray *stream_array)
{
	MediaPacketQueueState state=MEDIA_PACK_QUEUE_EMPTY;
	int stream_num=stream_array->get_size(stream_array);
	for(int i=0; i<stream_num; i++)
	{
		struct MediaStreamParams *stream=stream_array->get(stream_array,i);
		struct MediaThread *thread=stream->codec_thread; 
		int number=thread->packet_number(&thread->list);
		if(number >= thread->list_max)
		{
			state=MEDIA_PACK_QUEUE_FULL;
			break;
		}
		else if(number!=0)
			state=MEDIA_PACK_QUEUE_OTHER;
	}
	return state;
}


int media_codec_play(struct MediaPlayerHandle *player,struct MediaUserParams *user)
{
	double speed=1.0,speed_l=1.0;
	MediaStreamArray *stream_array=player->stream_array;
	AVFormatContext *format_ctx=player->format_ctx;
	struct TimerHandle *clock=player->clock;
	int err=0;
	int stream_num=stream_array->get_size(stream_array);
	if(media_creat_player_thread(stream_array, clock, user,player->sync_clk_stream_index)<0)
	{
		return -1;
	}

	AVPacket packet;
	int test=0;
	//启动所有线程的共用同步时钟
	clock->start(player->clock);
	Media_Set_State(user,MEDIA_STATE_PLAYING);
	while (1) 	
	{

//#ifdef DEBUG_VIDEO
		if(media_exit_flag==1)
		{
			debug_printf("强制退出===========================================================================================\n");
			player->player_start(stream_array);
			player->set_state(stream_array,MEDIA_STATE_EXIT);
			exit(0);
			break;
		}
//#endif
		if((err=Media_Get_Position_S(user))>=0)
		{
			printf("[Debug]: media_seek_frame_with_time\n");
			media_seek_frame_with_time(user,player,err);
			printf("[Debug]: media_seek_frame_with_time ok\n");
			player->flush_codec_buffers(stream_array);	//需要考虑竞争，codec_ctx被ffmpeg使用
			//清空队列
			printf("[Debug]: flush_codec_buffers ok\n");
			player->flush_list(stream_array);
			//清空声卡缓存
			//audio_pcm_drop(display->pcm_play);
			clock->adjust_time(clock,(long)err*1000*1000);
		}
		speed=Media_Get_Speed(user);
		if(speed != speed_l)
		{
			clock->set_speed(clock,speed);
			speed_l=speed;
		}
		int cmd=user->command_get(user);
		switch(cmd)
		{
			case MEDIA_PLCMD_SUSPEND:
				Media_Set_State(user,MEDIA_STATE_PAUSEING);
				debug_printf("debug:暂停\n");
				clock->pause(clock);
				user->cond->wait(user->cond);
				debug_printf("debug:继续\n");
				clock->resume(clock);
				
				player->player_start(stream_array);
				Media_Set_State(user,MEDIA_STATE_PLAYING);
				break;
			case MEDIA_PLCMD_NEXT:
			case MEDIA_PLCMD_LAST:
				Media_Set_State(user,MEDIA_STATE_JUMP);
			case MEDIA_PLCMD_STOP:
			case MEDIA_PLCMD_EXIT:
				player->set_state(stream_array,MEDIA_STATE_EXIT);
				player->packet_exit(stream_array);					//防止队列中没有数据，线程阻塞
				//清空队列
				player->flush_list(stream_array);
				//清空声卡缓存
				//audio_pcm_drop(display->pcm_play);	//改到对应线程中
				debug_printf("退出\n");
				goto FREE_THREAD;
			default:
				break;
		}
		
		if(player->list_state(stream_array)==MEDIA_PACK_QUEUE_FULL)
		{
			//debug_printf("队列已满，等待...\n");
			usleep(5000);
			continue;
		}

		if(av_read_frame(format_ctx, &packet) >= 0)	//video和audio的format_ctx是同一个
		{
			//debug_printf("[Debug]: media_write_packet_to_queue\n");
			media_write_packet_to_queue(stream_array, &packet);	
			av_packet_unref(&packet);
			continue;
		}
		//数据已经读完
		if (player->list_state(stream_array)==MEDIA_PACK_QUEUE_EMPTY)
		{
			if(Media_Get_DPosition(user) > user->length)
                break;
		}
		usleep(10000);
		Media_Set_Position_N(user, (int32_t)(clock->get_run_time(clock) / 1000.0 / 1000.0));
	}

	player->set_state(stream_array,MEDIA_STATE_EXIT);
	player->packet_exit(stream_array);
	// Clean up

FREE_THREAD:
	media_delete_player_thread(stream_array);
    return err;
}



struct MediaStreamParams *media_stream_params_creat()
{
	struct MediaStreamParams *params=(struct MediaStreamParams *)malloc(sizeof(struct MediaStreamParams));
	if(!params)
		return NULL;
	params->enable=true;
	return params;
}

void media_stream_params_delete(struct MediaStreamParams *params)
{
	if(!params)
		return ;
	params->format_ctx=NULL;
	params->codec_ctx=NULL;
	free(params);
}

//流播放线程结构体创建
struct MediaThread *Media_Thread_Creat()
{
	struct MediaThread *thread=(struct MediaThread *)malloc(sizeof(struct MediaThread));
	if(thread==NULL)
		return NULL;
/*	if((thread->clock=timer_ofday_handle_creat())==NULL)
	{
		free(thread);
		return NULL;
	}*/

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
	thread->unref_packet=unref_packet;
	thread->packet_number=get_packet_number;
	thread->packet_exit=packet_queue_exit;
	thread->flush_list=packet_queue_flush;
	packet_queue_init(&thread->list);			//初始化包队列
	return thread;
}

//
static int Media_Thread_Free(struct MediaThread *thread)
{
	if(!thread)
		NULL;
	thread->set_state(thread,MEDIA_STATE_NONE);
	pthread_join(thread->thread,NULL);
	pthread_cond_destroy(&thread->cond.cond);
    pthread_mutex_destroy(&thread->cond.lock);
	pthread_mutex_destroy(&thread->lock);
	packet_queue_destroy(&thread->list);
	
//	timer_ofday_handle_delete(thread->clock);

	return 0;
}


struct MediaPlayerHandle *media_player_handle_creat()
{
	struct MediaPlayerHandle *player=(struct MediaPlayerHandle *)malloc(sizeof(struct MediaPlayerHandle));
	if(!player)
		return NULL;
	
	player->clock=timer_ofday_handle_creat();
	if(!player->clock)
	{	
		free(player);
		return NULL;
	}

	player->stream_array=creat_variable_array(-1,2);	//浅拷贝，初始2个元素
	if(!player->stream_array)
	{
		timer_ofday_handle_delete(player->clock);
		free(player);
		return NULL;
	}

	player->url=NULL;
	player->format_ctx=NULL;
	player->sync_clk_array_index=-1;
	player->sync_clk_stream_index=-1;

	player->player_start=media_player_codec_start;
//	player->player_wait)(MediaStreamArray *stream_array);
	player->player_pause=media_player_pause;
	player->player_resume=media_player_resume;
	player->set_state=media_player_set_state;
					
	player->packet_exit=media_player_packet_exit;					//防止队列中没有数据，线程阻塞
	player->flush_list=media_player_flush_list;
	player->list_state=media_player_get_queue_state;

	player->flush_codec_buffers=media_avcodec_flush_buffers;
	return player;
}

void media_player_handle_delete(struct MediaPlayerHandle *player)
{
	if(!player)
		return ;
	if(player->url)
		free(player->url);
	player->url=NULL;	
	timer_ofday_handle_delete(player->clock);
	int size=player->stream_array->get_size(player->stream_array);
	for(int i=0;i<size;i++)
	{
		struct MediaStreamParams *stream=player->stream_array->get(player->stream_array,i);
		media_stream_params_delete(stream);
	}
	delete_variable_array(player->stream_array);
	free(player);
	player=NULL;
}


/// @brief 获取编解码信息
/// @return 
MediaFormatContext *Media_Get_File_All_Info(const char *filename,MediaStreamArray *media_array)
{
	FILE *fp=fopen(filename,"rb");
	if(fp)
		fclose(fp);
	return media_find_codec(filename,media_array);
}

/// @brief 释放文件
/// @param codec_v 
/// @param codec_a 
/// @return 
int Media_Free_File(MediaStreamArray *media_array)
{
	int size_array=media_array->get_size(media_array);
	struct MediaStreamParams *stream=NULL;
	for(int i=0 ;i<size_array;i++)
	{
		stream =media_array->get(media_array,i);
		avcodec_free_context(&stream->codec_ctx);
	}
	avformat_close_input(&stream->format_ctx);		//因为是一个文件，所有流共用一个format_ctx只需释放一次
	return 0;
}

int Mediao_File_Codec_Play(struct MediaPlayerHandle *player,struct MediaUserParams *user)
{
	signal(SIGINT, exit_signal);

	return media_codec_play(player,user);
}





