#ifndef _MEDIA_FILE_LIST_H_
#define _MEDIA_FILE_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

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


struct MediaFileList *creat_media_file_list();
int delete_media_file_list(struct MediaFileList *list);


#ifdef __cplusplus
}
#endif

#endif
