#ifndef _VARIABLE_ARRAY_H_
#define _VARIABLE_ARRAY_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct VariableArray{
    void *data;         // 指向动态缓冲区
	size_t elem_size;	// 每个元素的大小
    size_t size;        // 当前已存储元素个数
    size_t capacity;    // 缓冲区总容量

	//追加元素(拷贝)
	int (*append_deep)(struct VariableArray *arr, const void *elem_ptr);	
	//追加元素(赋值)
	int (*append_shallow)(struct VariableArray *arr, void *elem_ptr);
	// 访问元素
	void* (*get)(struct VariableArray *arr, size_t index);

	size_t (*get_size)(struct VariableArray *arr);

} VariableArray;



void delete_variable_array(VariableArray *arr);
VariableArray *creat_variable_array(size_t elem_size, size_t initial_capacity) ;


#endif

