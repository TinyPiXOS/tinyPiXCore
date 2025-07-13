/*///------------------------------------------------------------------------------------------------------------------------//
		音频服务的管理
说 明 : 当音频以独立的服务运行的时候需要此文件，若只是调用接口则只需codec，play，hard，record
日 期 : 2024.11.27

/*///------------------------------------------------------------------------------------------------------------------------//


#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "audio_manage.h"
#include "audio_play.h"
#include "audio_record.h"

struct AudServerNode* AudListRoot;

//线程传参使用，外部禁止使用
struct AudsThreadTR{
	struct MediaParams *config;
	struct AudServer *server;
	int connect;
};


static struct AudServerNode* createHeadNode() {
    struct AudServerNode* head = (struct AudServerNode*)malloc(sizeof(struct AudServerNode));
    if (!head) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
//    head->data = NULL;   
    head->next = NULL;   // 头结点的 next 初始为 NULL
    return head;
}

static int insertAtHead(struct AudServerNode* head,struct AudServer *auds) {
	struct AudServerNode* newNode = (struct AudServerNode*)malloc(sizeof(struct AudServerNode));
	if (!newNode) {
		printf("Memory allocation failed!\n");
		return -1;
	}
	//newNode->data = auds;
	memcpy(&newNode->data, auds, sizeof(struct AudServer));
	newNode->next = head;  // 新节点的 next 指向原链表的第一个元素
	head->next = newNode;        // 头结点的 next 指向新节点
	return 0;
}


static int deleteById(struct AudServerNode* head, int value) {
	struct AudServerNode* current = head->next;  // 跳过头结点
    struct AudServerNode* prev = head;

    while (current != NULL) {
        if (current->data.index == value) {
            prev->next = current->next;  // 删除当前节点
            free(current);
            return 0;
        }
        prev = current;
        current = current->next;
    }

    printf("Value %d not found in the list!\n", value);
	return -1;
}

//查找
static struct AudServerNode* findByValue(struct AudServerNode* head, tpAudId value) 
{
	struct AudServerNode* current = head->next;  // 跳过头结点
	while (current != NULL) {
		if (current->data.aud_id == value) {
			return current;;
		}
		current = current->next;
	}
	return NULL;
}

static int updateByValue(struct AudServerNode* head, int oldValue, int newValue) {
	struct AudServerNode* current = head->next;  // 跳过头结点
	while (current != NULL) {
		if (current->data.index == oldValue) {
			current->data.index = newValue;
			return 0;
		}
		current = current->next;
	}
	printf("Value %d not found in the list!\n", oldValue);
	return -1;
}

static int deleteList(struct AudServerNode *head)
{
	struct AudServerNode *node;
	if(head == NULL)
		return 0;
	while(head->next)
	{
		node=head->next;
		head->next=node->next;
		free(node);
	}
	return 0;
}

struct AudServerList* creat_audio_server_list()
{
	struct AudServerList *list=(struct AudServerList*)malloc(sizeof(struct AudServerList));
	if(list==NULL)
		return NULL;
	struct AudServerNode* head=createHeadNode();
	if(head==NULL){
		free(list);
		return NULL;
	}

	list->aud_snb=0;
	list->head=head;
	list->insert_head=insertAtHead;
	list->find=findByValue;
	return list;
}

int delete_audio_server_list(struct AudServerList *list)
{
	if(list==NULL)
		return 0;
	deleteList(list->head);
	free(list);
	return 0;
}




int get_aud_id()
{
	return getpid();
}


int creat_shared_memeory()
{

	return 0;
}


//等待接收远程通信的数据
int recv_data_msg(int msgid,struct MsgData *msg)
{
	long mtype=msg->mtype;
	printf("recv:%d,type=%ld\n", msgid,mtype);
	if(msgrcv(msgid, msg, sizeof(struct MsgData)-sizeof(long), mtype,0) == -1) {//IPC_NOWAIT
		perror("msgrcv failed");
		return -1;
	}
	printf("recv:%d,type=%d\n", msgid,msg->audid);
	return 0;
}

int send_data_msg(int msgid,struct MsgData *msg)
{
	printf("send:msgid=%d,type=%ld\n",msgid,msg->mtype);
	if(msgsnd(msgid, msg, sizeof(struct MsgData)-sizeof(long), 0) == -1) {
		perror("msgsnd failed");
		return -1;
	}
	return 0;
}

//创建消息队列
int creat_message_queue()
{
	key_t key;
	key = ftok("/tmp", getpid());  // 使用 /tmp 作为基础路径，进程 PID 作为项目 ID
	if (key == -1) {
		perror("ftok");
		return -1;
	}
	int msg_id = msgget(key, 0666 | IPC_CREAT);
	if (msg_id == -1) {
		perror("msgget");
		return -1;
	}
	return msg_id;
}

int delete_message_queue(int msgid)
{
	if (msgctl(msgid, IPC_RMID, NULL) == -1) {
		perror("msgctl failed");
		return -1;
	}
	return 0;
}

int creat_ipc_handle(struct IpcHandle *ipc)
{
	int msgid=creat_message_queue();
	if(msgid<0)
		return -1;
	ipc->send=send_data_msg;	//
	ipc->recv=recv_data_msg;	//
	ipc->connect=msgid;	//
	ipc->data=(struct MsgData *)malloc(sizeof(struct MsgData));
	return 0;
}

int delete_ipc_handle(struct IpcHandle *ipc)
{
	delete_message_queue(ipc->connect);
	free(ipc->data);
	return 0;
}



//单个服务的消息解析
int audio_server_data_analysis(int msgid,struct MediaParams *conf,struct MsgData *msg)
{
	struct MsgData msg_send;
	msg_send.cmd=AUDIO_CMD_REPLY;
	msg_send.data.reply.cmd=msg->cmd;
	msg_send.audid = msgid;
	switch(msg->cmd)
	{
		case AUDIO_CMD_CLOSE_SERVER:
			Audio_Set_Close(conf);
			break;
		case AUDIO_CMD_START:

			break;
		case AUDIO_CMD_STOP:
		    Audio_Set_Stop(conf);
			break;
		case AUDIO_CMD_SUSPEND:			//
			Audio_Set_Suspend(conf);
			break;
		case AUDIO_CMD_CONTINUE:
			Audio_Set_Continue(conf);
			break;
		case AUDIO_CMD_ADD_FILE:
			Audio_Add_File(conf,(const char *)msg->data.file);
			break;	
		case AUDIO_CMD_DEL_FILE:
			break;
		case AUDIO_CMD_SET_VOLUMEL:
			Audio_Set_Volume(conf,msg->data.volume);
			break;
		case AUDIO_CMD_GET_VOLUMEL:
		{
			uint8_t vol=(uint8_t)Audio_Get_Volume(conf);
			msg_send.data.reply.data.volume=vol;
			send_data_msg(msgid,&msg_send);
			break;
		}
		case AUDIO_CMD_SET_POSTION:
			Audio_Set_Position(conf,msg->data.postion);
			break;
		case AUDIO_CMD_GET_POSTION:

			break;
		default:
			break;
	}
	return 0;
}

void *pthread_audio_server_data(void *arg)
{
	struct AudsThreadTR *thread_arg=(struct AudsThreadTR *)arg;
	struct AudServer *auds=thread_arg->server;
	struct MediaParams *conf=thread_arg->config;

	struct MsgData msg;

	int msgid=auds->connect;		//仅接收自己的消息
	while(1)
	{
		msg.mtype=auds->aud_id;
		if(recv_data_msg(msgid,&msg))
			audio_server_data_analysis(msgid,conf,&msg);
	}
}

//单个音频播放服务
int audio_server(int connect)
{
	int msgid=connect;
	struct MediaParams *conf=media_user_config_creat();	//当前服务的相关用户配置(设置)
	struct IpcHandle ipc;		//进程通信句柄
	ipc.send=send_data_msg;	//
	ipc.recv=recv_data_msg;	//
	ipc.connect=msgid;	//
	ipc.data=(struct MsgData *)malloc(sizeof(struct MsgData));
//	ipc.msg = &msg;
	
	struct AudServer auds;
	auds.aud_id=get_aud_id();
	
	/*int msgid = msgget(key, 0);  // 只需要打开已存在的队列，不需要IPC_CREAT
	if (msgid == -1) {
		perror("msgget failed");
		return -1;
	}*/
	auds.connect=msgid;


	struct MsgData msg;
	msg.cmd = AUDIO_CMD_REPLY;  // 设置消息类型
	msg.audid = auds.aud_id;
	msg.data.reply.cmd=AUDIO_CMD_OPEN_PLAYER;
	msg.mtype=msg.audid;
	printf("child: msgid=%d\n",msgid);
	PIAudioConf *pcm_play=Audio_Play_Open(NULL);		//当前服务的硬件参数
	if(pcm_play==NULL){
		msg.data.reply.error=MSG_CODE_ERROR;
		ipc.send(msgid, &msg);
		free(ipc.data);
		return -1;
	}
	else{
		msg.data.reply.error=MSG_CODE_SUCCESS;
		ipc.send(msgid, &msg);
	}
	//创建消息接收和处理的线程
	pthread_t thread_data;
	struct AudsThreadTR thread_arg;
	thread_arg.config = conf;
	thread_arg.server = &auds;
	if((pthread_create(&thread_data, NULL, pthread_audio_server_data, (void*)(&thread_arg))) != 0) {     
		printf("音频进程的通信线程创建失败!\n");
		free(ipc.data);
		return -1;
	}


	Audio_Add_File(conf,"/home/pix/Media/phone.wav");
	//Audio_Add_File(&conf,"/home/pix/Media/test.mp3");
	//Audio_Add_File(&conf,"/home/pix/Media/MeiNanBian.flac");
	printf("准备循环播放\n");
	while(1)
	{
		//循环播放
		Audio_Play_Main(pcm_play,conf);
	}

	free(ipc.data);
	media_user_config_free(conf);
	Audio_Device_Close(pcm_play);
	return 0;
}

int record_server()
{
	return 0;
}


//进程创建
int audio_process_creat(int key,uint8_t priority)
{
	pid_t pid;  // 定义进程 ID 变
	// 创建新进程
	pid = fork();
	if (pid < 0) {  // fork 失败
		perror("fork failed");
		return -1;
	} 
	else if (pid == 0) {  // 子进程代码
		printf("This is the child process. PID: %d, Parent PID: %d\n", getpid(), getppid());
		audio_server(key);
		//释放
		exit(-1);
	} 
	else {  // 父进程代码
		printf("This is the parent process. PID: %d, Child PID: %d\n", getpid(), pid);
	}
	return pid;
}

int audio_server_wait_exit(tpAudId audid)
{
	int status;
	waitpid(audid, &status, 0);
	return 0;
}

//所有服务的播放暂停等的优先级管理，比如高优先级的音频服务开始播放之前需要关闭所有的低优先级
int audio_manage_all_process()
{
	return 0;
}

//消息队列里的回复消息处理
int Reply_Data_Analysis(struct MsgData *msg,struct AudServerList *list,int *aud_snb)
{
	
	switch(msg->data.reply.cmd)
	{
		case AUDIO_CMD_OPEN_PLAYER:
			printf("音频播放服务创建成功\n");
			struct AudServer aud;
			aud.index=*aud_snb;
			aud.aud_id=msg->audid;
			//优先级暂时不需要关注
			if(list->insert_head(list->head,&aud)<0){
				//添加杀死子进程
				return -1;
			}
			(*aud_snb)++;
			break;
		case AUDIO_CMD_CLOSE_SERVER:
			printf("音频播放服务关闭\n");
			//等待服务结束
			//audio_server_wait_exit(msg->audid);
			//从链表中删除
			list->remove_node(list->head,msg->audid);
			break;
		default:
			break;

	}
	return 0;
}



int Audio_Open_Server(int key,uint8_t priority)
{
	int pid=audio_process_creat(key,priority);

	return 0;
}

int Record_Open_Server(int connect)
{
	return 0;
}
int Audio_Close_Server(tpAudId audid,struct AudServerList *server_list,struct IpcHandle *ipc_audio)
{
	return 0;
}

int Audio_Play_File(const char *name)
{
	//1.获取文件信息

	//2.设置硬件参数

	return 0;
}


//进程通信接收数据解析
int Ipc_Data_Analysia(struct MsgData *msg,struct AudServerList *server_list,struct IpcHandle *ipc_audio)
{
	switch(msg->cmd)
	{
		case AUDIO_CMD_OPEN_PLAYER:		//打开一个音频播放服务
			Audio_Open_Server(ipc_audio->connect ,msg->data.priority);
			break;
		case AUDIO_CMD_REPLY:
			Reply_Data_Analysis(msg,server_list,&(server_list->aud_snb));
			break;
		case AUDIO_CMD_OPEN_RECORD:	
			Record_Open_Server(ipc_audio->connect);
			break;	
		case AUDIO_CMD_CLOSE_SERVER:			//关闭一个音频服务
		case AUDIO_CMD_START:
		case AUDIO_CMD_STOP:
		case AUDIO_CMD_SUSPEND:			//转发到对应线程(进程)
		case AUDIO_CMD_CONTINUE:
		case AUDIO_CMD_ADD_FILE:		
		case AUDIO_CMD_DEL_FILE:
		case AUDIO_CMD_SET_VOLUMEL:
		case AUDIO_CMD_GET_VOLUMEL:
		case AUDIO_CMD_SET_POSTION:
		case AUDIO_CMD_GET_POSTION:
		{
			struct MsgData msg_send;		
			struct AudServerNode *server_node=server_list->find(server_list->head, msg->audid);
			if(server_node==NULL){		//没有这个服务
				break;
			}
			memcpy(&msg_send,msg,sizeof(struct MsgData));
			msg_send.mtype=(long)server_node->data.aud_id;

			ipc_audio->send(ipc_audio->connect ,&msg_send);
			break;
		}
		case AUDIO_CMD_WR_STREAM:				//写流数据
			break;
		case AUDIO_CMD_SET_HARD_PARAM:		//设置硬件参数
			break;
		case AUDIO_CMD_GET_HARD_PARAM:		
			break;
		case AUDIO_CMD_SET_PRIORITY:
		{
			struct AudServerNode *server_node=server_list->find(server_list->head, msg->audid);
			if(server_node==NULL){		//没有这个服务
				break;
			}
			server_node->data.priority=msg->data.priority;
			break;
		}
		case AUDIO_CMD_GET_PRIORITY:
			break;
		default:
			break;
	}
	return 0;
}

int Audio_Manage_Main()
{
	int connect;			//远程的链接
	int key;
	struct MsgData msg_r;		//远程remote的消息内容
	struct MsgData msg_l;		//音频服务内部（本地）的消息内容

	//所有音频服务的消息队列
	struct IpcHandle ipc_audio;
	if((key=creat_ipc_handle(&ipc_audio))<0)		//用于服务内部通信的消息队列
	{
		perror("creat_message_queue");
		return -1;
	}
	struct AudServerList *server_list=creat_audio_server_list();
	if(server_list==NULL)
	{
		perror("creat_audio_server_list");
		return -1;
	}

	printf("message queue: msgid=%d,key=%d\n",ipc_audio.connect,ipc_audio.key);
	while(1)
	{
		//recv_data(connect,&msg_r);//
		msg_r.audid = 0;
		msg_r.cmd=AUDIO_CMD_OPEN_PLAYER;
		Ipc_Data_Analysia(&msg_r,server_list,&ipc_audio);
		usleep(100000);
		msg_l.mtype=0;		//每次接收之前
		ipc_audio.recv(ipc_audio.connect,&msg_l);
		printf("mtype:%ld,audid:%d\n",msg_l.mtype,msg_l.audid);
		Ipc_Data_Analysia(&msg_l,server_list,&ipc_audio);
		break;
	}

	while(1);
	delete_audio_server_list(server_list);
	delete_ipc_handle(&ipc_audio);

	return 0;
}


int Media_Test()
{
	//test_play();
	PIAudioConf *pcm_play=Audio_Play_Open(NULL);	//当前服务的硬件参数
	if(pcm_play==NULL){
		printf("audio pcm open error\n");
		return -1;
	}
	
	printf("debug:,handle_p=%p,hewparams_p=%p\n",pcm_play->handle,pcm_play->hwparams);	
	//Audio_Play_Test(&pcm_play,"/home/pix/Media/MeiNanBian.mp3");//home/pix/Media/test_video.webm
	Audio_Play_Test(pcm_play,"/home/pix/Media/test_video.webm");
    Audio_Device_Close(pcm_play);
	return 0;
}

int Record_Test()
{
	PIAudioConf *pcm_play=Audio_Record_Open(NULL);		//当前服务的硬件参数
	if(pcm_play==NULL){
		printf("audio pcm open error\n");
		return -1;
	}
	
	Audio_Record_Test(pcm_play,"/home/pix/Media/record.wav");
	Audio_Device_Close(pcm_play);
	return 0;
}

#ifdef __cplusplus
}
#endif