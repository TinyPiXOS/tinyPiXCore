//utils测试
#include <stdio.h>
#include <string.h>
#include "utils/utlist.h"

struct FileInfo{
	char *name;
	int size;
};

void *new_file(void *data)
{
	struct FileInfo *file=(struct FileInfo *)data;
	struct FileInfo *node_data=(struct FileInfo *)malloc(sizeof(struct FileInfo));
	if(!node_data)
		return NULL;
	node_data->name=strdup(file->name);
	node_data->size=file->size;
	return node_data;
}

void free_file(void *data)
{
	struct FileInfo *file=(struct FileInfo *)data;
	if(!file)
		return 
	free(file->name);
	file->size=0;
}


int main()
{
	struct LinkedList *list=creat_linked_list();
	list->free_node_callback=free_file;
	list->init_node_callback=new_file;

	struct FileInfo file;
	file.name=malloc(100);
	sprintf(file.name,"JiYuchao");
	file.size=100;
	list->push_front(list,&file);

	sprintf(file.name,"HAHAHAHAHA");
	file.size=50;
	list->push_front(list,&file);

	sprintf(file.name,"1234567890");
	file.size=10;
	list->push_front(list,&file);

	printf("size of list:%ld\n",list->size);
	

	/*struct Node *node_temp=list->head;
	for(int i=0;i<list->size;i++)
	{
		node_temp=node_temp->next;
		if(!node_temp)
			break;
		struct FileInfo *file_=(struct FileInfo *)node_temp->data;
		printf("name=%s,size=%d\n",file_->name,file_->size);

	}*/
	for(int i=0;i<list->size;i++)
	{
		struct Node *node_temp=list->get_next_node(list);
		if(!node_temp)
		{
			printf("NULL\n");
			break;
		}
		struct FileInfo *file_=(struct FileInfo *)node_temp->data;
		printf("name=%s,size=%d\n",file_->name,file_->size);
	}

	return 0;

}
