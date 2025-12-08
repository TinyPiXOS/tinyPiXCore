/*///------------------------------------------------------------------------------------------------------------------------//
		媒体文件列表管理
说 明 : 
日 期 : 2025.7.30

/*///------------------------------------------------------------------------------------------------------------------------//

#include "Media/media_file_list.h"

#ifdef DEBUG_AUUDIO
#include "Log/elog.h"
#define debug_printf(...) 	elog_d("MediaFileList", ##__VA_ARGS__)
//#define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...)  // 如果不定义DEBUG，什么也不做
#endif

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
	if(p->file)
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