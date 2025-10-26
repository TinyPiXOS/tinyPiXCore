/*///------------------------------------------------------------------------------------------------------------------------//
		链表的封装
说 明 : 
日 期 : 2025.3.12

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include "utlist.h"


// 创建节点
static struct Node *create_node(LinkedList *list,void *data) {
	struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
	if(!new_node)
		return NULL;
	new_node->data = list->init_node_callback(data);
	if(!new_node->data)
	{
		free(new_node);
		return NULL;
	}
	new_node->next = NULL;
	new_node->prev = NULL;
	return new_node;
}

// 头插节点
static int push_front(LinkedList *list, void *data) {
    struct Node *new_node = create_node(list,data);
	if(!new_node)
		return -1;

    new_node->next = list->head->next;
    new_node->prev = list->head;
    
    if (list->head->next) {
        list->head->next->prev = new_node;
    } else {
        list->tail = new_node;  // 如果原来链表为空，tail 也需要指向这个新节点
    }

    list->head->next = new_node;
    list->size++;
	return 0;
}

// 尾插节点
static int push_back(LinkedList *list, void *data) {
	struct Node *new_node = create_node(list,data);
    if(!new_node)
		return -1; 
		
    if (list->tail) {
        list->tail->next = new_node;
        new_node->prev = list->tail;
    } 
	else {
        list->head->next = new_node;  // 如果原来链表为空，head->next 需要指向这个新节点
        new_node->prev = list->head;
    }

    list->tail = new_node;
    list->size++;
	return 0;
}

// 删除节点
static void delete_node(LinkedList *list, struct Node *node) {
    if (node == NULL || node == list->head) {
        return;  // 不能删除头节点
    }

    if (node->prev) {
        node->prev->next = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }

    // 如果删除的是尾节点，需要更新 tail
    if (node == list->tail) {
        list->tail = node->prev;
        if (list->tail == list->head) {
            list->tail = NULL;  // 只有哨兵头节点时，tail 置为空
        }
    }

	if(list->free_node_callback)
	{
		list->free_node_callback(node->data);
	}
	else
		free(node->data);  // 释放数据
    free(node);
    list->size--;
}

// 按照节点获取前一个节点
static struct Node* get_prev_by_node(struct Node *node) {
	return (node && node->prev) ? node->prev : NULL;
}

// 按照节点获取下一个节点
static struct Node* get_next_by_node(struct Node *node) {
	return (node && node->next) ? node->next : NULL;
}

// 获取前一个节点
static struct Node* get_prev(struct LinkedList *list) {
	struct Node* node=get_prev_by_node(list->pos);
	list->pos=list->pos->prev;
	return node;
}

// 获取下一个节点
static struct Node* get_next(struct LinkedList *list) {
	struct Node* node=get_next_by_node(list->pos);
	list->pos=list->pos->next;
	return node;
}

// 查找指定数据的节点
static struct Node* find_node(LinkedList *list, void *data, int (*cmp)(void *, void *)) {
    struct Node *current = list->head->next;
    while (current != list->tail) {
        if (cmp(current->data, data) == 0) {
            return current;  // 找到匹配的数据
        }
        current = current->next;
    }
    return NULL;  // 未找到
}

// 遍历链表并执行回调
void traverse(LinkedList *list, void (*callback)(void *)) {
	struct Node *current = list->head->next;
    while (current != list->tail) {
        callback(current->data);  // 调用回调函数
        current = current->next;
    }
}

// 清空链表
void clear_list(LinkedList *list) 
{
    struct Node *current = list->head->next;
    while (current) {
        struct Node *next_node = current->next;
        free(current);
        current = next_node;
    }
    list->head->next = NULL;
    list->tail = NULL;
    list->size = 0;
}


// 初始化链表
struct LinkedList *creat_linked_list() 
{
	LinkedList *list=(struct LinkedList *)malloc(sizeof(struct LinkedList));
	if(!list)
		return NULL;
	list->head = (struct Node *)malloc(sizeof(struct Node));  // 创建头节点
	if(!list->head)
	{
		free(list);
		return NULL;
	}

	list->head->next = NULL;
    list->head->prev = NULL;
    list->tail = NULL;
	list->pos = list->head;						// 默认偏移位置为头节点
	list->size = 0;                             // 初始化链表大小为 0

	list->push_front = push_front;
	list->push_back = push_back;
	list->deleteNode = delete_node;
	list->find = find_node;
	list->get_next_node = get_next;
	list->get_prev_by_node = get_prev_by_node;
	list->get_next_by_node = get_next_by_node;
	list->traverse = traverse;
	list->clear = clear_list;
	return list;
}

void delete_linked_list(struct LinkedList *list)
{
	if(!list)
		return ;
	clear_list(list);
	free(list);
}




