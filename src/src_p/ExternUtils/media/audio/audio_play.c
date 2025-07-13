/*///------------------------------------------------------------------------------------------------------------------------//
		音频播放相关
说 明 : 视频播放需要借助于此程序
日 期 : 2024.11.26

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include "audio_play.h"
#include "filter.h"

//获取采样位数为AV_SAMPLE_FMT_S16的字节数
//av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

#ifdef DEBUG_AUUDIO
    #define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define debug_printf(fmt, ...)  // 如果不定义DEBUG，什么也不做
#endif

int Audio_Set_Length(struct MediaParams *conf,double length);
int Audio_Set_Command(struct MediaParams *conf, AudioPlayCommand cmd);

double limit_min_max(double value,double min,double max)
{
	if(value>max) value=max;
	else if(value<min) value=min;
	return value;
}

static struct MediaFileNode* createHeadNode() 
{
    struct MediaFileNode* head = (struct MediaFileNode*)malloc(sizeof(struct MediaFileNode));
    if (!head) {
        debug_printf("Memory allocation failed!\n");
        exit(1);
    }
    head->file = NULL;   
    head->next = NULL;   // 头结点的 next 初始为 NULL
	head->last = NULL;
    return head;
}

static int insertAtEnd(struct MediaFileList *list,char *file) 
{
	struct MediaFileNode* newNode = (struct MediaFileNode*)malloc(sizeof(struct MediaFileNode));
	if (!newNode) {
		debug_printf("Memory allocation failed!\n");
		return -1;
	}
	//newNode->file=strdup(file);
	//if(newNode->file==NULL){
	//	free(newNode);
	//	return -1;
	//}
	newNode->file = file;
	newNode->next = NULL;
	newNode->last = list->end;
	list->end->next=newNode;
	list->end=newNode;
	debug_printf("插入文件%s\n",newNode->file);
	return 0;
}
//在播放位置插入(pos之后)
static int insertAtPosition(struct MediaFileList *list,char *file)
{
	struct MediaFileNode* newNode = (struct MediaFileNode*)malloc(sizeof(struct MediaFileNode));
	if (!newNode) {
		debug_printf("Memory allocation failed!\n");
		return -1;
	}
	//newNode->file=strdup(file);
	//if(newNode->file==NULL){
	//	free(newNode);
	//	return -1;
	//}
	
	newNode->file=file;
	newNode->next = list->pos->next;  
	newNode->last = list->pos;
	if(list->pos->next)
		list->pos->next->last=newNode;
	else	
	{
		list->pos->next=newNode;
		list->end=newNode;
	}
//	list->pos=newNode;
	debug_printf("插入文件%s\n",newNode->file);
	return 0;
}

static int deleteAtHead(struct MediaFileList *list) 
{
	struct MediaFileNode* p = list->head->next;
	if(p==NULL)
		return 0;
	list->head->next=p->next;
	if(p->next)
	{
		p->next->last=list->head;
	}        
	else
	{
		list->end=list->head;
	}
	if(list->pos==p)
	    list->pos=p->next;
	free(p->file);
	free(p);
	return 0;
}

static int deleteWithFile(struct MediaFileList *list,char *file)
{
	struct MediaFileNode* temp_pos=list->head->next;
	
	while(temp_pos)
	{	
		if (strcmp(temp_pos->file, file) == 0) { // 找到匹配节点
			// 更新前后节点的链接
			if (temp_pos->last) {
				temp_pos->last->next = temp_pos->next;
			}
			if (temp_pos->next) {
				temp_pos->next->last = temp_pos->last;
			}
			// 更新尾节点
			if (temp_pos == list->end) {
				list->end = temp_pos->last;
			}
			// 如果 pos 指向当前节点，更新 pos
			if (list->pos == temp_pos) {
				list->pos = temp_pos->next;
			}
			// 释放节点内存
			temp_pos->next=NULL;
			temp_pos->last=NULL;
			free(temp_pos->file); // 释放文件名字符串
			free(temp_pos);			// 释放节点本身
			debug_printf("Node with file '%s' deleted.\n", file);
			return 0;
		}
		temp_pos = temp_pos->next;
	}
	return 0;
}

static int deleteAllNode(struct MediaFileList *list)
{
	if(!list->head)
		return -1;
	struct MediaFileNode *current = list->head->next;  // 从第一个有效节点开始
	struct MediaFileNode *next_node;

	while (current != NULL) 
	{
		next_node = current->next;  // 保存下一个节点的指针
		if (current->file != NULL) {
			free(current->file);
			current->file=NULL;
		}
		free(current);
		current = next_node;
	}
	return 0;
}

static int deletdList(struct MediaFileList *list)
{
	if(!list)
		return 0;
	deleteAllNode(list);
	free(list->head);
	list->head=NULL;
	return 0;
}

static char *readNextFile(struct MediaFileList *list)
{
	if(list->pos==list->end)		
	{
		//list->pos=list->head->next;		//文件尾,直接定位到第一个文件,不需要循环播放则之间诶返回NULL
		return NULL;
	}
	else
	{
		list->pos=list->pos->next;
	}
	if(list->pos==NULL)
		return NULL;
	return list->pos->file;
}

static char *readNowFile(struct MediaFileList *list)
{
	return list->pos->file;
}

static char *readLastFile(struct MediaFileList *list)
{
	if(list->pos==list->head)		//文件头
		return NULL;
	if(list->pos->last==list->head)
		return NULL;
	list->pos=list->pos->last;
	if(list->pos==NULL)
		return NULL;
	return list->pos->file;
}

static int insertAtEndSaft(struct MediaFileList *list,char *file)
{
	int ret;
	pthread_rwlock_rdlock(&list->mut);
	ret= list->insert_end(list,file);
	pthread_rwlock_unlock(&list->mut);
	return ret;
}

static int insertAtPositionSaft(struct MediaFileList *list,char *file)
{
	int ret;
	pthread_rwlock_rdlock(&list->mut);
	ret= list->insert_pos(list,file);
	pthread_rwlock_unlock(&list->mut);
	return ret;
}

static int deleteWithFileSaft(struct MediaFileList *list,char *file)
{
	int ret;
	pthread_rwlock_rdlock(&list->mut);
	ret= list->delete_file(list,file);
	pthread_rwlock_unlock(&list->mut);
	return ret;
}

static char *readNextFileSaft(struct MediaFileList *list)
{
	char *name=NULL;
	pthread_rwlock_rdlock(&list->mut);
	name=list->read(list);
//	debug_printf("list read:%s\n",name);
	pthread_rwlock_unlock(&list->mut);
	return name;
}

static char *readNowFileSaft(struct MediaFileList *list)
{
	char *name=NULL;
	pthread_rwlock_rdlock(&list->mut);
	name=list->read_now(list);
//	debug_printf("list read:%s\n",name);
	pthread_rwlock_unlock(&list->mut);
	return name;
}

static char *readLastFileSaft(struct MediaFileList *list)
{
	char *name;
	pthread_rwlock_rdlock(&list->mut);
	name=list->read_last(list);
	debug_printf("list read:%s\n",name);
	pthread_rwlock_unlock(&list->mut);
	return name;
}
static int deleteAllNodeSaft(struct MediaFileList *list)
{
	pthread_rwlock_rdlock(&list->mut);
	list->delete_all(list);
	pthread_rwlock_unlock(&list->mut);
	return 0;
}

struct MediaFileList *creat_media_file_list()
{
	struct MediaFileNode* head= createHeadNode();
	if(head==NULL)
		return NULL;
	struct MediaFileList *list=(struct MediaFileList *)malloc(sizeof(struct MediaFileList));
	if(!list){
		free(head);
		return NULL;
	}
	if (pthread_rwlock_init(&list->mut, NULL) != 0) {
		return NULL;
	}
	list->pos = head;
	list->head=head;
	list->end=head;
	list->insert_end=insertAtEnd;
	list->insert_pos=insertAtPosition;
	list->delete_file=deleteWithFile;
	list->read=readNextFile;
	list->read_saft=readNextFileSaft;
	list->read_now=readNowFile;
	list->read_now_saft=readNowFileSaft;
	list->read_last=readLastFile;
	list->read_last_saft=readLastFileSaft;
	list->insert_end_saft=insertAtEndSaft;
	list->insert_pos_saft=insertAtPositionSaft;
	list->delete_file_saft=deleteWithFileSaft;
	list->delete_all=deleteAllNode;
	list->delete_all_saft=deleteAllNodeSaft;
	list->remove=NULL;
	return list;
}

int delete_media_file_list(struct MediaFileList *list)
{
	if(!list)
		return 0;
	deletdList(list);
	pthread_rwlock_destroy(&list->mut);
	free(list);
	list=NULL;
	return 0;
}

//发送条件变量
int send_pthread_cond(struct PthreadCond *cond)
{
	pthread_mutex_lock(&cond->lock);
	pthread_cond_signal(&cond->cond); // 通知
	pthread_mutex_unlock(&cond->lock);
	return 0;
}
//等待条件变量
int wait_pthread_cond(struct PthreadCond *cond)
{
	pthread_mutex_lock(&cond->lock);
	pthread_cond_wait(&cond->cond, &cond->lock); // 等待条件变量
	pthread_mutex_unlock(&cond->lock);
	return 0;
}
//创建条件变量及其配合的互斥锁
struct PthreadCond *pthread_cond_creat_struct()
{
	struct PthreadCond *cond=(struct PthreadCond *)malloc(sizeof(struct PthreadCond));
	if(cond==NULL)
		return NULL;
	if (pthread_mutex_init(&cond->lock, NULL) != 0) {
		free(cond);
		return NULL;
	}

	// 动态初始化条件变量
	if (pthread_cond_init(&cond->cond, NULL) != 0) {
		pthread_mutex_destroy(&cond->lock);
		free(cond);
		return NULL;
	}
	cond->send=send_pthread_cond;
	cond->wait=wait_pthread_cond;
	return cond;
}

//释放条件变量以及其锁
int pthread_cond_free_struct(struct PthreadCond *cond)
{
	if(cond==NULL)
		return -1;
	pthread_mutex_destroy(&cond->lock);
	pthread_cond_destroy(&cond->cond);
	return 0;
}

struct MediaParams *media_user_config_creat()
{
	struct MediaParams *conf=(struct MediaParams *)malloc(sizeof(struct MediaParams));
	conf->is_playing=false;
	conf->volume=USER_CONF_VOLUME;
	conf->position_s = -1;
	conf->position_p = 0;
	conf->volume=100;
	conf->speed=1.0;
	conf->filter=NULL;
	conf->position_bytes=0;
	conf->state=AUDIO_STATE_STOP;
	conf->cmd=AUDIO_PLCMD_STOP;
	conf->callback_video=NULL;
	conf->userdata=NULL;
	struct MediaFileList *list=creat_media_file_list();
	if(list==NULL)
	{
		perror("list creat error\n");
		free(conf);
		return NULL;
	}
	conf->list=list;
	
	struct MediaVideoParams *video=(struct MediaVideoParams *)malloc(sizeof(struct MediaVideoParams));
	if(video==NULL)
	{
		delete_media_file_list(conf->list);
		free(conf);
	}
	video->rect.h=0;
	video->rect.w=0;
	video->rect.x=0;
	video->rect.y=0;
	video->fill=MEDIA_VIDEO_SCALING_FIT;
	conf->video=video;

	struct PthreadCond *pthread_cond=pthread_cond_creat_struct();
	if(pthread_cond==NULL)
	{
		free(video);
		delete_media_file_list(conf->list);
		free(conf);
		return NULL;
	}
	conf->cond=pthread_cond;
	pthread_rwlock_init(&conf->rw_mut, NULL);
	
	conf->command_get=Audio_Get_Command;
	conf->get_callback_video=Audio_Get_Video_Callback;
	conf->set_callback_video=Audio_Set_Video_Callback;
	return conf;
}

void media_user_config_free(struct MediaParams *conf)
{
	if(!conf)
		return ;
	pthread_rwlock_destroy(&conf->rw_mut);
	pthread_cond_free_struct(conf->cond);
	conf->cond=NULL;
	delete_media_file_list(conf->list);
	conf->list=NULL;
	if(conf->filter)
		audio_filter_delete(conf->filter);
	conf->filter=NULL;
	free(conf->video);
	free(conf);
	conf->video=NULL;
	conf=NULL;

}

//阻塞直到缓存区数据播放完成
int media_pcm_drain(PIAudioConf *pcm)
{
	return snd_pcm_drain(pcm->handle);
}

//丢弃音频缓存区中的数据
int media_pcm_drop(PIAudioConf *pcm)
{
	if(!pcm || !pcm->handle)
		return -1;
	return snd_pcm_drop(pcm->handle);
}
//关闭音频硬件
int media_pcm_close(PIAudioConf *pcm)
{
	if(!pcm || !pcm->handle)
		return -1;
	return snd_pcm_close(pcm->handle);
}


/// @brief 配置PCM的参数
/// @param pcm 
/// @return 
int media_pcm_hwparams_init(PIAudioConf *pcm)
{
	int rc=0;
	debug_printf("start to open pcm\n");
	rc=snd_pcm_hw_params_any(pcm->handle, pcm->hwparams);//初始化params(如果已经设置万参数了想要重新设置需要调用此函数)
	if(rc<0){
		perror("\nsnd_pcm_hw_params_any:");
		return -1;
	}
	/*rc = snd_pcm_hw_params_set_rate_resample(pcm->handle, pcm->hwparams, 1);	//启用重采样,(可以硬件自动重采样以播放和硬件的采样率不一致的音频，但是不是所有设备都支持)
	if (rc < 0) {
		debug_printf("\nResampling setup failed for playback: ");
		return -1;
	}*/
	rc=snd_pcm_hw_params_set_access(pcm->handle, pcm->hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);   //初始化访问权限
	if(rc<0){
		perror("\nsed_pcm_hw_set_access:");
		return -1;
	}
	//rc=snd_pcm_hw_params_set_channels(pcm->handle, pcm->hwparams, numChannels);  // 设置通道数

	return 0;
}

/// @brief audioStreamParams结构体初始化
/// @param wChannels 通道
/// @param nSamplesPersec 采样频率
/// @param wBitsPerSample 数据位数
/// @return 
int audioStreamParams_init(int wChannels,int nSamplesPersec,int wBitsPerSample,struct AudioStreamParams *header)
{
	header->wChannels=wChannels;    //声道数
	header->nSamplesPersec=nSamplesPersec;          //采样频率
	header->wBitsPerSample=wBitsPerSample;   		// 样本数据位数  
	header->byteFrams=wChannels*wBitsPerSample/8;
	header->nAvgBitsPerSample=nSamplesPersec*header->byteFrams;             //每秒播放字节数
	return 0;
}

/// @brief 获取声卡的一些功能
/// @param pcm 
/// @return 
int pcm_get_function(snd_pcm_hw_params_t *hwparams,struct PcmHardParams *ahparams)
{
	int dir=0;
	snd_pcm_uframes_t frames,buff_size;

	ahparams->can_pause=snd_pcm_hw_params_can_pause(hwparams);  //检查硬件是否支持暂停
	debug_printf("PCM can_pause'%d'\n",ahparams->can_pause);
	ahparams->can_resume=snd_pcm_hw_params_can_resume(hwparams);  //检查硬件是否支持恢复
	debug_printf("PCM can_resume'%d'\n",ahparams->can_resume);
	if(snd_pcm_hw_params_get_period_size(hwparams, &frames, &dir)<0)  /*获取周期长度*/
		return -1;
	if(snd_pcm_hw_params_get_buffer_size(hwparams, &buff_size)<0)
		return -1;
	ahparams->cycle_frames=frames;
	ahparams->buff_size=buff_size;

	debug_printf("cycle_frames=%ld,buff_size=%ld\n",frames,buff_size);
	return 0;
}

/// @brief 根据音配的参数配置声卡，会根据最近的配置来尽量配置完成，即实际配置结果和原来可能不一致，配置结果会保存到pcm的里面
/// @param pcm 
/// @param header 
/// @return 
int pcm_hwparams_set(PIAudioConf *pcm,struct AudioStreamParams *audio)
{
	int rc;
	int dir=0;
	unsigned int channels=PCM_CHANNELS_DEFAULT;
	unsigned int frequency=PCM_SAMPLE_PERSEC_DEFAULT;
	unsigned int bit=PCM_Bits_PER_SAMPLE_DEFAULT;
	if(audio!=NULL){
		channels=audio->wChannels;
		frequency=audio->nSamplesPersec;
		bit=audio->wBitsPerSample;
	}
	else
		fprintf(stderr,"set pcm error,used dwefault param\n");		
	debug_printf("debug:bit=%d,frequency=%d,channels=%d\n",bit,frequency,channels);											
	switch(bit/8)                                                             //设置采样位数
	{
		case 1:snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_U8);
			break ;
		case 2:snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_S16_LE);
			break ;
		case 3:snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_S24_LE);
			break ;
		case 4:snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_S32_LE);
			break ;
		default:
			debug_printf("set SND_PCM_FORMAT_U8\n");
			snd_pcm_hw_params_set_format(pcm->handle, pcm->hwparams, SND_PCM_FORMAT_U8);
			break;
	}
	debug_printf("pcm set format%d\n",bit/8);

	rc=snd_pcm_hw_params_set_channels_near(pcm->handle, pcm->hwparams, &channels);   //设置声道,1表示单声道，2表示立体声
	if(rc<0){
		perror("\nsnd_pcm_hw_params_set_channels:");
		return -1;
	}
	debug_printf("pcm set channel:%d\n",channels);

	rc=snd_pcm_hw_params_set_rate_near(pcm->handle, pcm->hwparams, &frequency, &dir); //设置频率
	if(rc<0){
		perror("\nsnd_pcm_hw_params_set_rate_near:");
		return -1;
	}
	debug_printf("pcm set rate:%d\n", frequency);

	//根据获取的缓存区调整周期大小（只有WAV才设置，其他类型文件不需要设置）(理论不用设置，但是在读取缓存区各种帧数的时候貌似会出问题，暂时先全部设置)
	//if(pcm->file_type==AUDIO_FILE_TYPE_WAV)
	{
		snd_pcm_uframes_t frames=PCM_BUFFER_FRAMES;
		snd_pcm_uframes_t buffer_size=PCM_BUFFER_FRAMES*PCM_BUFFER_SIZE*channels*(bit/8);
		if(snd_pcm_hw_params_set_period_size_near(pcm->handle,pcm->hwparams, &frames, 0)<0)  //设置周期长度
		{
			perror("snd_pcm_hw_params_set_period_size,error");
			debug_printf("snd_pcm_hw_params_set size=%ld\n",frames);
			//return -1;
		}
		if( snd_pcm_hw_params_set_buffer_size_near(pcm->handle, pcm->hwparams, &buffer_size)<0)
		{
			perror("snd_pcm_hw_params_set_buffer_size_near error");
			//return -1;
		}
		debug_printf("pcm set period size:%ld  buffer_size%ld\n", frames,buffer_size);
	}
	
	//应用pcm参数
	rc = snd_pcm_hw_params(pcm->handle, pcm->hwparams);
	if (rc < 0) {
		debug_printf("unable to set hw parameters: %s\n", snd_strerror(rc));
		snd_pcm_state_t state = snd_pcm_state(pcm->handle);
   		fprintf(stderr, "current state = %s\n", snd_pcm_state_name(state));
		return -1;
	}

	//获取设备的一些功能，一般在设置完参数后调用
	pcm_get_function(pcm->hwparams,pcm->ahparams);	

	//设置完后修改最终成功设置的参数
	if(audio!=NULL){
		audioStreamParams_init(channels,frequency,bit,audio);
		memcpy(pcm->adparams,audio,sizeof(struct AudioStreamParams));
		/*pcm->adparams->nAvgBitsPerSample=audio->nAvgBitsPerSample;
		pcm->adparams->wBitsPerSample=audio->wBitsPerSample;
		pcm->adparams->nSamplesPersec=audio->nSamplesPersec;
		pcm->adparams->wChannels=audio->wChannels;
		pcm->adparams->bitRate=audio->bitRate;
		pcm->adparams->rLen=audio->rLen;*/
	}
	return 0;
}

int pcm_start_play(PIAudioConf *pcm)
{
	int rc=0;
	//准备播放
	if ((rc = snd_pcm_prepare(pcm->handle)) < 0) {		//在第一次设置时可以不需要准备播放，播放后重新设置需要准备播放
		perror("无法准备播放:");
		media_pcm_close(pcm);
		return -1;
	}
	snd_pcm_start(pcm->handle);
//	media_pcm_drop(pcm);
//	debug_printf("PCM handle name = '%s'\n", snd_pcm_name(pcm->handle));
	return 0;
}

/// @brief 暂停播放
/// @param pcm 
/// @return 
int pcm_play_stop(PIAudioConf *pcm)
{
	int err;
	if (pcm->ahparams->can_pause) {
		if ((err = snd_pcm_pause(pcm->handle, 1)) < 0) {
//		    mp_msg(MSGT_AO,MSGL_ERR,MSGTR_AO_ALSA_PcmPauseError, snd_strerror(err));
        	return -1;
		}
	} 
	else {
		if ((err = media_pcm_drop(pcm)) < 0){
//			mp_msg(MSGT_AO,MSGL_ERR,MSGTR_AO_ALSA_PcmDropError, snd_strerror(err));
			return -1;
		}
	}
	return 0;
}

/// @brief 继续播放
/// @param pcm 
/// @return 
int pcm_play_continue(PIAudioConf *pcm)
{
    int err;
	if (snd_pcm_state(pcm->handle) == SND_PCM_STATE_SUSPENDED) {
    //    	mp_msg(MSGT_AO,MSGL_INFO,MSGTR_AO_ALSA_PcmInSuspendModeTryingResume);
	 	while ((err = snd_pcm_resume(pcm->handle)) == -EAGAIN) sleep(1);
	}
	if (pcm->ahparams->can_pause) {
		if ((err = snd_pcm_pause(pcm->handle, 0)) < 0){
//      	mp_msg(MSGT_AO,MSGL_ERR,MSGTR_AO_ALSA_PcmResumeError, snd_strerror(err));
            return -1;
       	}
	//	mp_msg(MSGT_AO,MSGL_V,"alsa-resume: resume supported by hardware\n");
    } 
	else {
		if ((err = snd_pcm_prepare(pcm->handle)) < 0){
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
void pcm_data_adjust_volume(uint8_t *buffer, size_t frames, int channels, float volume, uint16_t bit) 
{
	if (bit == 16) {		//SND_PCM_FORMAT_S16_LE
		int16_t *data = (int16_t *)buffer;
		for (size_t i = 0; i < frames * channels; i++) {
			data[i] = (int16_t)(data[i] * volume);
		}
	} 
	else if (bit == 8) {		//SND_PCM_FORMAT_U8
		uint8_t *data = buffer;
		for (size_t i = 0; i < frames * channels; i++) {
			int value = (int)(data[i] - 128) * volume + 128; // U8 数据需考虑偏移量
			data[i] = (uint8_t)(value < 0 ? 0 : (value > 255 ? 255 : value));
		}
	} 
	else if (bit == 24) {		//SND_PCM_FORMAT_S24_LE
		// 每个采样点占用 3 字节（24 位）
		for (size_t i = 0; i < frames * channels; i++) {
			// 将 3 字节数据读取为 32 位有符号整数
			int32_t sample = (buffer[i * 3 + 2] << 16) | (buffer[i * 3 + 1] << 8) | buffer[i * 3];
			if (sample & 0x800000) { // 检测符号位并扩展到 32 位
				sample |= 0xFF000000;
			}
			// 调节音量
			sample = (int32_t)(sample * volume);
			// 防止溢出，截断到 24 位
			if (sample > 8388607) sample = 8388607;
			if (sample < -8388608) sample = -8388608;
			// 将结果写回 3 字节
			buffer[i * 3]     = sample & 0xFF;
			buffer[i * 3 + 1] = (sample >> 8) & 0xFF;
			buffer[i * 3 + 2] = (sample >> 16) & 0xFF;
		}
	} 
	else if (bit == 32) {		// SND_PCM_FORMAT_S32_LE
		int32_t *data = (int32_t *)buffer;
		for (size_t i = 0; i < frames * channels; i++) {
			// 调节音量
			data[i] = (int32_t)(data[i] * volume);
			// 防止溢出，限制在 32 位整数范围内
			if (data[i] > 2147483647) data[i] = 2147483647;
			if (data[i] < -2147483648) data[i] = -2147483648;
		}
	} 
	else {
		fprintf(stderr, "Unsupported format for volume adjustment,bit=%d\n",bit);
	}
}

//调整播放位置
static void pcm_adjust_position(FILE *fp,int16_t position)
{

}

/// @brief 向pcm写入音频数据
/// @param pcm 
/// @param buffer 
/// @param frames :帧数(即采样点的数量)
/// @param delay :写入错误时的阻塞时长,单位ms
/// @return 
int pcm_write_data(PIAudioConf *pcm,uint8_t *buffer,unsigned long frames,int delay)
{
	int time=0;
	snd_pcm_sframes_t avail;
	snd_pcm_sframes_t delay_p;
	int err;
	delay=delay/5;		//每次延时5ms;
	// 获取播放设备当前的延迟（缓冲区剩余的帧数）
	while (1) 
	{
		// 获取当前硬件可写入的帧数
		avail = snd_pcm_avail(pcm->handle);
		if (avail > frames) 
		{  // 如果可用帧数足够
			// 可以继续写入数据
			err = snd_pcm_writei(pcm->handle, buffer, frames);
			if (err == -EPIPE) {
				//fprintf(stderr, "Buffer underrun occurred\n");
				snd_pcm_prepare(pcm->handle);
			} 
			/*else if (err == -EAGAIN) {
				// 缓冲区满
				;
			}*/
			else if (err < 0) {
				fprintf(stderr, "Error writing to PCM device（%d）: %s\n", err,snd_strerror(err));
				//return -1;		//直接返回还是重写尝试写入？？
			}
			//debug_printf("write ok\n");

			//debug_printf("Delay: %ld frames, Avail: %ld frames,My frams%ld\n", delay_p, avail,frames);
			break;  // 跳出内部循环，继续读取数据
		}
		else if (avail == -EAGAIN)
			snd_pcm_prepare(pcm->handle);
		// 如果可用帧数不足，检查设备延迟
		err = snd_pcm_delay(pcm->handle, &delay_p);//返回内部缓存中尚未播放的音频的侦数
		if (err < 0) {
			fprintf(stderr, "snd_pcm_delay failed: %s\n", snd_strerror(err));
			break;
		}

		// 输出当前延迟信息
		//debug_printf("Delay: %ld frames, Avail: %ld frames,My frams%ld\n", delay_p, avail,frames);
		time++;
		if(time>delay)
			return -1;	
		// 等待一段时间再继续检查
		usleep(5000);  // 等待 5 毫秒后再检查
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
int audio_stream_write(PIAudioConf *pcm_play,struct MediaParams *conf,
							uint8_t *buffer,uint32_t frames,
							float volume,
							int offset,int delay)
{

	int ret=0;
	float volume_set;
	struct AudioStreamParams *audio_param=pcm_play->adparams;
	static struct MediaFilterParam *filter=NULL;
	AVFrame *frame_flt=NULL;
	int64_t position=Audio_Get_BytePosition(conf);		//获取当前播放位置
	uint8_t *data=((AVFrame *)buffer)->data[0];		//输出的数据
	uint32_t data_frames=frames;
	static float speed_l=0.0;
	static uint16_t wChannels_l=0,wBitsPerSample_l=0;
	static uint32_t nSamplesPersec_l=0;

	float speed=Audio_Get_Speed(conf);
	if((speed_l!=speed || 
			audio_param->wChannels!=wChannels_l || 
			audio_param->wBitsPerSample!=wBitsPerSample_l ||
			audio_param->nSamplesPersec!=nSamplesPersec_l) 
			&& speed!=1)		//位置为0认为是新的一首开始播放，或者两次速度不相等，都需要重新设置过滤器
	{
		
		if(filter)
			audio_filter_delete(filter);
		filter=audio_filter_creat_init(speed,audio_param->nSamplesPersec,audio_param->wChannels,code_get_channel_layout(audio_param->wChannels),code_get_format(audio_param->wBitsPerSample));
		if(!filter)
		{
			ret = -1;
			goto WRITE_POS;
		}
		conf->filter=filter;
		speed_l=speed;
		wChannels_l=audio_param->wChannels;
		wBitsPerSample_l=audio_param->wBitsPerSample;
		nSamplesPersec_l=audio_param->nSamplesPersec;
	}
	if(speed!=1)
	{
		AVFrame *convert_frame=(AVFrame *)buffer;
		frame_flt = av_frame_alloc();		//提前申请
		
		if(!frame_flt)
		{
			ret =-1;
			goto WRITE_POS;
		}
		media_filte_get_data(filter,convert_frame,frame_flt);
		data=frame_flt->data[0];
		data_frames=frame_flt->nb_samples;
	}

	if(volume<0)
	{
		volume_set=Audio_Get_Volume(conf);
		volume_set*=0.01;
	}
	else
		volume_set=volume;
	
	pcm_data_adjust_volume(data,data_frames,audio_param->wChannels,volume_set,audio_param->wBitsPerSample);
	ret= pcm_write_data(pcm_play,data,data_frames,delay);

FREE_FLT:
	if(frame_flt)
	{
		av_frame_free(&frame_flt);
	}
WRITE_POS:
//	printf("offset:%d,audio_param->nAvgBitsPerSample:%d,进度：%d\n",offset,audio_param->nAvgBitsPerSample,position);
	if(offset >= 0 && audio_param->nAvgBitsPerSample!= 0)		//如果要自行设置offset直接传入-1即可
	{
		position += (frames*audio_param->byteFrams);
		Audio_Set_BytePosition(conf,(int64_t)position);
	}
	
	return ret;
}

struct codePlayCallbackParam{
	PIAudioConf *pcm;
	struct MediaParams *conf;
	struct AudioStreamParams *audio_param;
	int delay;
};

/// @brief 音频播放回调，如果要自行设置offset，offset传入-1；
/// @param buff 缓存区
/// @param frames 侦数
/// @param offset 截至当前为止写入的字节数量(会根据字节数量和硬件配置自动设置进度)
/// @return 
int callback_codec_play(uint8_t *buff,uint32_t frames,int offset,void *param)
{
	struct codePlayCallbackParam *p=(struct codePlayCallbackParam *)param;
	return audio_stream_write(p->pcm, p->conf, buff, frames, -1, offset, p->delay);
}

//播放解码文件
int audio_play_codec_file(PIAudioConf *pcm_play,struct MediaParams *conf,const char *filename)
{
	struct MediaCodecParam codec;
	debug_printf("播放解码文件:%s\n",filename);
	if(Audio_Get_Codec_Info(filename, &codec)<0)
		return -1;
	double duration=codec.format_ctx->duration / (double)AV_TIME_BASE;
	Audio_Set_Length(conf,duration);
	printf("[debug]：每秒字节%d\n",pcm_play->adparams->nAvgBitsPerSample);
	if(Audio_Hard_Auto_Init(pcm_play,conf,&codec)<0)
		return -1;
	printf("[debug]：每秒字节%d\n",pcm_play->adparams->nAvgBitsPerSample);
	debug_printf("开始播放解码文件:%s\n",filename);
	Audio_Set_BytePosition(conf,0);
	Audio_File_Codec(&codec,conf);
	Audio_Hard_Deinit(&codec);
	return 0;
}

//声卡硬件初始化(使用解码器的参数自动设置)
int Audio_Hard_Auto_Init(PIAudioConf *pcm_play,struct MediaParams *conf,struct MediaCodecParam *codec)
{
	debug_printf("初始化声卡硬件\n");
	struct AudioStreamParams *stream_params=(struct AudioStreamParams *)malloc(sizeof(struct AudioStreamParams));
	audioStreamParams_init(codec->codec_ctx->channels,
								codec->codec_ctx->sample_rate,
								AUDIO_CODEC_CHANNEL_DEF,		//使用16位宽，(本值是解码时候自己指定的，不需要动态设置)
								stream_params);
	if(pcm_hwparams_set(pcm_play,stream_params)<0)
		return -1;
	if(pcm_start_play(pcm_play)<0)
		return -1;
	struct codePlayCallbackParam *cb_param=(struct codePlayCallbackParam *)malloc(sizeof(struct codePlayCallbackParam));
	if(cb_param==NULL)
		return -1;
	cb_param->conf=conf;
	cb_param->audio_param=stream_params;
	cb_param->delay=100;
	cb_param->pcm=pcm_play;
	codec->callback_play=callback_codec_play;
	codec->callback_param=cb_param;
	codec->hard_param=stream_params;
	return 0;
}

//手动设置
int Audio_Hard_Hand_Init(PIAudioConf *pcm_play,struct MediaParams *conf,struct AudioStreamParams *stream_params)
{
	if(pcm_hwparams_set(pcm_play,stream_params)<0)
		return -1;
	if(pcm_start_play(pcm_play)<0)
		return -1;
	return 0;
}

int Audio_Hard_Deinit(struct MediaCodecParam *codec)
{
	struct codePlayCallbackParam *cb_param=(struct codePlayCallbackParam *)codec->callback_param;
	if(cb_param!=NULL)
    {
		if(cb_param->audio_param!=NULL)
		    free(cb_param->audio_param);
		free(cb_param);
	}
    return 0;
}

//设置音量
int Audio_Set_Volume(struct MediaParams *conf,int16_t volume)
{
	if(!conf)
		return -1;
	volume=(int16_t)limit_min_max(volume,USER_CONF_VOLUME_MIN,USER_CONF_VOLUME_MAX);	
	pthread_rwlock_wrlock(&conf->rw_mut);
	conf->volume=volume;
	pthread_rwlock_unlock(&conf->rw_mut);
	return 0;
}
//获取音量
int Audio_Get_Volume(struct MediaParams *conf)
{
	if(!conf)
		return -1;
	int16_t volume;
	pthread_rwlock_rdlock(&conf->rw_mut);
	volume=conf->volume;
	pthread_rwlock_unlock(&conf->rw_mut);
	return volume;
}
//内部获取用户设置的播放位置(只允许内部调用)
int Audio_Get_Position_S(struct MediaParams *conf)
{
	if(!conf)
		return -1;
	int32_t position;
	pthread_rwlock_rdlock(&conf->rw_mut);
	position=conf->position_s;
	if(conf->position_s>=0)
		conf->position_s=-1;
	pthread_rwlock_unlock(&conf->rw_mut);
	return position;
}
//内部设置实时播放位置(只允许内部调用)
int Audio_Set_Position_N(struct MediaParams *conf,int32_t position)
{
	if(!conf)
		return -1;
	pthread_rwlock_wrlock(&conf->rw_mut);
	conf->position_p=position;
	pthread_rwlock_unlock(&conf->rw_mut);
	return 0;
}
//设置位置（设置值小于0不生效）
int Audio_Set_Position(struct MediaParams *conf,int32_t position)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->position_s,position);
//	pthread_rwlock_wrlock(&conf->rw_mut);
//	conf->position_s=position;
//	pthread_rwlock_unlock(&conf->rw_mut);
	return 0;
}
//获取位置（音频使用字节数计算）
int Audio_Get_Position(struct MediaParams *conf,PIAudioConf *pcm_play)
{
	int64_t bytes=Audio_Get_BytePosition(conf);
	pthread_rwlock_wrlock(&conf->rw_mut);
	if(pcm_play->adparams->nAvgBitsPerSample==0)
	{
		fprintf(stderr,"获取音频参数异常，无法返回进度\n");
		pthread_rwlock_unlock(&conf->rw_mut);
		return 0;
	}
	int position=bytes/pcm_play->adparams->nAvgBitsPerSample;
	pthread_rwlock_unlock(&conf->rw_mut);
	return position;
}

//获取双精度位置
double Audio_Get_DPosition(struct MediaParams *conf,PIAudioConf *pcm_play)
{
	if(!conf)
		return -1;
	double position;
	THREAD_READ_USERCONF(conf->rw_mut,conf->position_p,position);
	return position;
}

int64_t Audio_Get_BytePosition(struct MediaParams *conf)
{
	if(!conf)
		return -1;
	int64_t position;
	THREAD_READ_USERCONF(conf->rw_mut,conf->position_bytes,position);
	return (int64_t)position;
}

int64_t Audio_Set_BytePosition(struct MediaParams *conf,int64_t position)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->position_bytes,position);
	return 0;
}

//设置状态为开始，录音的时候必须传file，播放的时候可以不传，如果传的话会自动加入到播放列表末尾
int Audio_Set_Start(struct MediaParams *conf,const char *file)
{
	int state=Audio_Get_State(conf);
   	if(state!=AUDIO_STATE_STOP)		//EXIT后不允许开始
		return -1;
	Audio_Set_Command(conf,AUDIO_PLCMD_NEXT);
	if(file!=NULL)
	{
		Audio_Add_File(conf,file);
	}
	conf->cond->send(conf->cond);
	Audio_Set_State(conf,AUDIO_STATE_START);
	return 0;
}	

//设置播放状态-暂停播放
int Audio_Set_Suspend(struct MediaParams *conf)
{
	return Audio_Set_Command(conf,AUDIO_PLCMD_SUSPEND);
}
//设置播放状态-继续播放
int Audio_Set_Continue(struct MediaParams *conf)
{
	if(Audio_Get_State(conf)!=AUDIO_STATE_PAUSEING)
		return -1;
	Audio_Set_Command(conf,AUDIO_PLCMD_CONTINUE);
	conf->cond->send(conf->cond);
	return 0;
}
//获取播放状态
int Audio_Get_State(struct MediaParams *conf)
{
	if(!conf)
		return -1;
	int state = 0;
	THREAD_READ_USERCONF(conf->rw_mut,conf->state,state);
	return state;
}

//获取命令
int Audio_Get_Command(struct MediaParams *conf)
{
	int cmd = 0;
	THREAD_READ_USERCONF(conf->rw_mut,conf->cmd,cmd);
	return cmd;
}

//设置播放状态(只允许内部使用)
int Audio_Set_Command(struct MediaParams *conf, AudioPlayCommand cmd)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->cmd,cmd);
	return 0;
}

CallbackVideoDisplay Audio_Get_Video_Callback(struct MediaParams *conf)
{
	CallbackVideoDisplay cb;
	THREAD_READ_USERCONF(conf->rw_mut,conf->callback_video,cb);
	return cb;
}

void Audio_Set_Video_Callback(struct MediaParams *conf,CallbackVideoDisplay cb,void *userdata)
{
	if(!conf)
		return ;
	pthread_rwlock_rdlock(&conf->rw_mut);
	conf->callback_video=cb;
	conf->userdata=userdata;
	pthread_rwlock_unlock(&conf->rw_mut);
}

int Audio_Set_Video_Decode_Format(struct MediaParams *conf, uint32_t format_video)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->format_video,format_video);
	return 0;
}

//设置播放状态(只允许内部使用)
int Audio_Set_State(struct MediaParams *conf, AudioPlayState state)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->state,state);
	return 0;
}

//停止播放,只是停止不会关闭声卡，不同于暂停，停止会清空大多数播放信息
int Audio_Set_Stop(struct MediaParams *conf)
{
	if(Audio_Get_State(conf)==AUDIO_STATE_PAUSEING)
		conf->cond->send(conf->cond);
	return Audio_Set_Command(conf,AUDIO_PLCMD_STOP);	
}
//关闭声卡
int Audio_Set_Close(struct MediaParams *conf)
{
	int state=Audio_Get_State(conf);
	if(state<0)
		return -1;
	if(state!=AUDIO_STATE_STOP)
		Audio_Set_Stop(conf);
	Audio_Set_Command(conf,AUDIO_PLCMD_EXIT);
	conf->cond->send(conf->cond);		//防止处于等待状态
	return 0;
}

//是否退出
int Audio_State_Is_Exit(struct MediaParams *conf)
{
	return (Audio_Get_State(conf)==AUDIO_STATE_EXIT)? 1:0;
}

//播放速度
float Audio_Get_Speed(struct MediaParams *conf)
{
	if(!conf)
		return -1;
	float speed = 1.0;
	THREAD_READ_USERCONF(conf->rw_mut,conf->speed,speed);
	return speed;
}

int Audio_Set_Speed(struct MediaParams *conf,float speed)
{
	if(!conf)
		return -1;
	if(speed<=0)
		return -1;
	speed=limit_min_max(speed,USER_CONF_SPEED_MIN,USER_CONF_SPEED_MAX);	
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->speed,speed);
	return 0;
}

//添加播放文件
int Audio_Add_File(struct MediaParams *conf, const char *file)
{
	char *file_new=strdup(file);		
	if(!file_new)
		return -1;
//	pthread_rwlock_wrlock(&conf->rw_mut);
	conf->list->insert_end_saft(conf->list,file_new);
//	pthread_rwlock_unlock(&conf->rw_mut);
	return 0;
}
//删除播放文件
int Audio_Del_File(struct MediaParams *conf, const char *file)
{
	char *file_new=strdup(file);		
	if(!file_new)
		return -1;
//	pthread_rwlock_wrlock(&conf->rw_mut);
	conf->list->delete_file_saft(conf->list,file_new);
//	pthread_rwlock_unlock(&conf->rw_mut);
	return 0;
}

//播放文件（将需要播放的文件插入到播放列表的正在播放的位置的下一个并且直接取消当前文件的播放）
int Audio_Set_Play(struct MediaParams *conf,const char *file)
{
	char *file_new=strdup(file);		
	if(!file_new)
		return -1;
//	pthread_rwlock_wrlock(&conf->rw_mut);
	conf->list->insert_pos_saft(conf->list,file_new);
//	pthread_rwlock_unlock(&conf->rw_mut);
	return Audio_Play_Next(conf);
}

//播放音频流
int Audio_Set_Stream(struct MediaParams *conf,void *data)
{
	int state=Audio_Get_State(conf);
	return 0;
}

//下一个(仅针对列表)
int Audio_Play_Next(struct MediaParams *conf)
{
	return Audio_Set_Command(conf,AUDIO_PLCMD_NEXT);
}

//上一个(仅针对列表)
int Audio_Play_Last(struct MediaParams *conf)
{
	return Audio_Set_Command(conf,AUDIO_PLCMD_LAST);
}

int Audio_Set_Is_Playing(struct MediaParams *conf,bool is_playing)
{
	if(!conf)
		return -1;
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->is_playing,is_playing);
	return 0;
}

bool Audio_Get_Is_Playing(struct MediaParams *conf)
{
	if(!conf)
		return false;
	bool is_playing;
	THREAD_READ_USERCONF(conf->rw_mut,conf->is_playing,is_playing);
	return is_playing;
}

//设置正在播放的音频的时长
int Audio_Set_Length(struct MediaParams *conf,double length)
{
	THREAD_WRITE_USERCONF(conf->rw_mut,conf->length,length);
	return 0;
}

//获取正坐在播放的音频时长
double Audio_Get_Length(struct MediaParams *conf)
{
	if(!conf)
		return -1;
	double length;
	THREAD_READ_USERCONF(conf->rw_mut,conf->length,length);
	return length;	
}

/// @brief 音频播放主程序
/// @param pcm_play 声卡硬件参数，这里仅仅是基础的硬件配置，详细的参数会在播放每个文件的时候再设置
/// @param conf	用户配置
int Audio_Play_Main(PIAudioConf *pcm_play,struct MediaParams *conf)
{
	struct MediaFileList *list=conf->list;
	debug_printf("read_audia_list\n");
	while(1)
	{	
		//读播放列表
		char *name=NULL;
		int cmd=conf->command_get(conf);
		
		switch(cmd)
		{
			case AUDIO_PLCMD_NEXT:
				media_pcm_drop(pcm_play);
				name=list->read_saft(list);
				Audio_Set_Command(conf,AUDIO_PLCMD_NONE);
				break;
			case AUDIO_PLCMD_LAST:
				media_pcm_drop(pcm_play);
				name=list->read_last_saft(list);
				Audio_Set_Command(conf,AUDIO_PLCMD_NONE);
				break;
			case AUDIO_PLCMD_STOP:
				debug_printf("等待开始信号\n");
				if(Audio_Get_State(conf)!=AUDIO_STATE_START)
					conf->cond->wait(conf->cond);		//等待开始信号
				Audio_Set_Command(conf,AUDIO_PLCMD_NONE);
				break;
			case AUDIO_PLCMD_EXIT:
				Audio_Set_State(conf,AUDIO_STATE_EXIT);
				return 0;
				break;
			default: 
				name=list->read_saft(list);
				break;
		}
		if(name==NULL)
		{
			usleep(5000);	
			continue;
		}
		printf("name=%s\n",name);
		Audio_Set_State(conf, AUDIO_STATE_PLAYING);
		FILE *file=fopen(name,"rb");
		Audio_Set_Is_Playing(conf,true);
		{
			
			if(file)
				fclose(file);
			pcm_play->file_type = AUDIO_FILE_TYPE_NONE;
			media_pcm_drop(pcm_play);
			media_pcm_hwparams_init(pcm_play);		//重新初始化，否则一些参数无法设置
			audio_play_codec_file(pcm_play,conf,name);
		}
		Audio_Set_Is_Playing(conf,false);
	}
	return 0;
}

int Audio_Play_Test(PIAudioConf *pcm_play,const char *name)
{
	struct MediaParams conf;

	pcm_play->file_type =AUDIO_FILE_TYPE_NONE;
	audio_play_codec_file(pcm_play,&conf,name);		

	return 0;
}

/// @brief 打开Audio设备
/// @param device 声卡名字
/// @return 
PIAudioConf *Audio_Play_Open(const char *device)
{
	PIAudioConf *pcm_play=(PIAudioConf *)malloc(sizeof(PIAudioConf));
	if(pcm_play==NULL)
		return NULL;
	if(Audio_Device_Init(pcm_play,device,AUDIO_STREAM_PLAYBACK)<0)
	{
		free(pcm_play);
		return NULL;
	}
	return pcm_play;
}

int Audio_Device_Init(PIAudioConf *pcm_play,const char *device,AudioStreamType type)
{
	int err=0;
	pcm_play->ahparams=NULL;
	pcm_play->adparams=NULL;
	if(device==NULL)
		device="default";
//	int len=strlen(device)+1;
//	pcm_play->device=(char *)malloc(len);
//	strncpy(pcm_play->device,device,len-1);
	if(snd_pcm_open(&pcm_play->handle, device, (snd_pcm_stream_t)type, 0)<0)//SND_PCM_STREAM_CAPTURE
	{
		fprintf(stderr,"unable to open pcm device\n");
		return -1;
	}

	err=snd_pcm_hw_params_malloc(&pcm_play->hwparams); //分配hwparam,snd_pcm_hw_params_alloca
	if(err<0)
	{	
		fprintf(stderr,"unable to alloca hwparams\n");
		media_pcm_close(pcm_play);
		return -1;
	}
	if(media_pcm_hwparams_init(pcm_play)<0)	//初始化hwparams
	{
		debug_printf("media_pcm_hwparams_init error\n");
		snd_pcm_hw_params_free(pcm_play->hwparams);
		media_pcm_close(pcm_play);
		return -1;
	}
	pcm_play->adparams =(struct AudioStreamParams *)malloc(sizeof(struct AudioStreamParams));
	pcm_play->ahparams =(struct PcmHardParams *)malloc(sizeof(struct PcmHardParams));
	pcm_play->file_type =AUDIO_FILE_TYPE_NONE;
	return 0;
}

//关闭声卡硬件
int Audio_Device_Close(PIAudioConf *pcm_play)
{
	if(!pcm_play)
		return -1;
	if(pcm_play->hwparams)
		snd_pcm_hw_params_free(pcm_play->hwparams);
	if(pcm_play->handle)
	{
    	media_pcm_drain(pcm_play);
    	media_pcm_close(pcm_play);
	}
	if(pcm_play->adparams)
		free(pcm_play->adparams);
	if(pcm_play->ahparams)
		free(pcm_play->ahparams);
//	if(pcm_play->device)
//		free(pcm_play->device);
	return 0;
}

//向初始化并配置好的设备写入音频流
int Audio_Write_Stream(PIAudioConf *pcm,struct MediaParams *conf,struct AudioStreamParams *hard_params,
							uint8_t *buffer,uint32_t frames,int offset,int delay)
{
	int state=Audio_Get_State(conf);
	if(state!=AUDIO_STATE_PAUSEING && state!=AUDIO_STATE_STOP)
	{
		fprintf(stderr,"声卡只能在未播放音频的时候才能设置\n");
		return -1;
	}
	return audio_stream_write(pcm, conf, buffer, frames, -1, offset, delay);
}

//设置非阻塞(只允许在初始状态/停止状态/暂停状态可以设置)
int Audio_Set_Nonblock(PIAudioConf *pcm_play,struct MediaParams *conf,uint8_t nonblock)
{
	int state=Audio_Get_State(conf);
	if(state!=AUDIO_STATE_PAUSEING && state!=AUDIO_STATE_STOP)	//只有这三种状态可以设置非阻塞
		return -1;
	if(snd_pcm_nonblock(pcm_play->handle,nonblock)<0)
	{
		perror("set nonblock error:");
		return -1;
	}
	return 0;
}

//硬件设置
int Audio_Set_Hard_Params(PIAudioConf *pcm_play,struct MediaParams *conf,uint32_t rate,uint16_t channel,uint16_t bits)
{
	if(!conf)
		return -1;
	int state=Audio_Get_State(conf);
	if(state!=AUDIO_STATE_STOP)
		return -1;
	struct AudioStreamParams hard_params;
	hard_params.nSamplesPersec=rate;
	hard_params.wChannels=channel;
	hard_params.wBitsPerSample=bits;
	if(Audio_Hard_Hand_Init(pcm_play,conf,&hard_params)<0)
	{
		return -1;
	}
	return 0;
}


#ifdef __cplusplus
}
#endif

