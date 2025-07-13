#ifndef _AUDIO_MANAGE_H_
#define _AUDIO_MANAGE_H_

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "tpAudioDevice.h"


typedef pid_t tpAudId;

typedef enum {
	AUDIO_CMD_OPEN_PLAYER=0X0001,		//打开一个音频播放服务
	AUDIO_CMD_OPEN_RECORD,		
	AUDIO_CMD_CLOSE_SERVER,				//关闭一个音频服务
	AUDIO_CMD_START,
	AUDIO_CMD_STOP,
	AUDIO_CMD_SUSPEND,				//暂停播放
	AUDIO_CMD_CONTINUE,				//继续播放
	AUDIO_CMD_ADD_FILE,
	AUDIO_CMD_DEL_FILE,
	AUDIO_CMD_GET_DURATION,				//获取时长
	AUDIO_CMD_WR_STREAM,				//写流数据
	AUDIO_CMD_SET_HARD_PARAM,			//设置硬件参数
	AUDIO_CMD_GET_HARD_PARAM,
	AUDIO_CMD_SET_PRIORITY,			//播放优先级
	AUDIO_CMD_GET_PRIORITY,
	AUDIO_CMD_SET_VOLUMEL,			//设置音量
	AUDIO_CMD_GET_VOLUMEL,
	AUDIO_CMD_SET_POSTION,			//设置播放位置
	AUDIO_CMD_GET_POSTION,


	

	AUDIO_CMD_REPLY			//消息回复
}MsgDataCmdType;



typedef enum{
	MSG_CODE_SUCCESS	= 0x00,
	MSG_CODE_ERROR		= 0x01
}MsgReplyCodeType;

/*struct IpcParams{
	int key;		//共享内存
	int shmid;		//共享内存id
	size_t size;	//
	char *shmaddr;
};*/
struct IpcParams{
	int msg_id;
};

//每个音频线程(进程)的信息
struct AudServer{
	int index;		//音频服务编号(index)
	tpAudId aud_id;	//音频服务的服务号（进程编号或者线程编号）
	int connect;	//通信的连接（msgid）
	uint8_t priority;	//优先级
	int state;        //音频的状态
	struct IpcParams ipc_params;
};

//音频服务中所有音频线程(进程)的节点
struct AudServerNode{
	struct AudServer data;
	struct AudServerNode *next;
};

//音频服务的所有音频线程链表信息以及其操作
struct AudServerList{
	struct AudServerNode *head;
	int aud_snb;
	int (*insert_head)(struct AudServerNode* head,struct AudServer *auds);
	struct AudServerNode* (*find)(struct AudServerNode* head, tpAudId value);
	int (*remove_node)(struct AudServerNode* head, tpAudId value);
};



struct MsgReplyData{
	MsgDataCmdType cmd;
	MsgReplyCodeType error;		//错误码，=0表示成功
	union{
		uint8_t priority;	//优先级
		uint8_t volume; 	//音量
		int postion;		//播放位置
	}data;
};

//进程通信的数据内容，管道/网络/消息队列/共享内存均按照此格式
struct MsgData{
	long mtype;			//为适应消息队列拼接，消息队列可以用于指定收发的进程编号
	MsgDataCmdType cmd;		//通信的命令类型
	tpAudId audid;			//
	uint32_t data_size;		//数据的长度
	union {
		uint8_t priority;	//优先级，(不设置默认最低)
		uint8_t volume;		//音量
		int postion;		//播放位置（实时流不允许设置）
		char *file;		//文件名
		uint8_t *stream;	//流数据，
		struct MsgReplyData reply;		//错误码，=0表示成功
	}data;
};


struct IpcHandle{
	int connect;
	int key;
	struct MsgData *data;
	int (*send)(int connect,struct MsgData *data);
	int (*recv)(int connect,struct MsgData *data);
};

int Audio_Manage_Main();
int Media_Test();
int Record_Test();

#endif
