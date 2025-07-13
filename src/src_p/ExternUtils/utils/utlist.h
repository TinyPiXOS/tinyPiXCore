#ifndef _UTILS_LIST_H_
#define _UTILS_LIST_H_

#include <stdio.h>
#include <stdlib.h>

// 定义链表节点结构体
struct Node{
	void *data;         // 数据指针，指向链表存储的数据
	struct Node *next;  // 指向下一个节点的指针
	struct Node *prev;  // 指向前一个节点的指针
};

// 定义链表结构体
typedef struct LinkedList {
    struct Node *head;  // 头指针
    struct Node *tail;  // 尾指针
	struct Node *pos;	// 当前节点
    size_t size; // 链表大小

	//需要回调的接口(data类型未知，需要使用用户传入的节点释放和创建的函数接口)
	//节点释放
	void (*free_node_callback)(void *data);
	//节点创建(为了防止上层用户代码中返回导致直接被释放由上层自行创建节点)
	void *(*init_node_callback)(void *data);

	//通用接口
	//头插
    int (*push_front)(struct LinkedList *list, void *data);	
	//尾插	
	int (*push_back)(struct LinkedList *list, void *data);
	//删除节点
    void (*deleteNode)(struct LinkedList *list, struct Node *node);
	//写一个节点
	struct Node* (*get_next_node)(struct LinkedList *list);  // 获取下一个节点
	//获取传入的节点的前一个节点
	struct Node* (*get_prev_by_node)(struct Node *node);  // 获取前一个节点
	//获取传入的节点的后一个节点
	struct Node* (*get_next_by_node)(struct Node *node);  // 获取下一个节点
	//查找节点
    struct Node* (*find)(struct LinkedList *list, void *data, int (*cmp)(void *, void *));
	//遍历
    void (*traverse)(struct LinkedList *list, void (*callback)(void *));
	//清空
    void (*clear)(struct LinkedList *list);
}LinkedList;



struct LinkedList *creat_linked_list();
void delete_linked_list(struct LinkedList *list);


#endif