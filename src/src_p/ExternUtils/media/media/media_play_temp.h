#ifndef _MEDIA_PLAYER_H_
#define _MEDIA_PLAYER_H_

#ifdef __cplusplus
extern "C" {
#endif



struct MediaAudioUserParams{

};




struct MediaUserParams{

};



struct MediaPlayerHandle{
	char *url;
	AVFormatContext *format_ctx;	//输入输出相关信息，贯穿ffmpeg
	int sysnc_clock_index;			//主同步时钟的流索引号
	struct TimerHandle *clock;		//同步时钟
	MediaStreamArray *stream_array;	//所有的流


	int (*player_start)(MediaStreamArray *stream_array);
	int (*player_wait)(MediaStreamArray *stream_array);
	int (*player_pause)(MediaStreamArray *stream_array);
	int (*player_resume)(MediaStreamArray *stream_array);
	int (*set_state)(MediaStreamArray *stream_array, AudioPlayState state);

	int (*flush_list)(MediaStreamArray *stream_array);			//删除全部流队列中所有元素
	int (*packet_exit)(MediaStreamArray *stream_array);			//
	MediaPacketQueueState (*list_state)(MediaStreamArray *stream_array)
};


#ifdef __cplusplus
extern "C" {
#endif

#endif
