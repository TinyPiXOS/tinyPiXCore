/*///------------------------------------------------------------------------------------------------------------------------//
		不定长数组
说 明 : 
日 期 : 2025.5.17

/*///------------------------------------------------------------------------------------------------------------------------//


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "variable_array.h"


static int va_ensure_capacity(VariableArray *va) {
    if (va->size < va->capacity) return 0;
	printf("扩容\n");
    size_t new_cap = va->capacity * 2;
    void *p = realloc(va->data, new_cap * va->elem_size);
    if (!p) return -1;
    va->data     = p;
    va->capacity = new_cap;
    return 0;
}

// 追加元素（拷贝入缓冲区）
static int byte_array_append_deep(VariableArray *va, const void *elem) {
    if (va_ensure_capacity(va) != 0) return -1;
    void *dest = (char*)va->data + va->size * va->elem_size;
    memcpy(dest, elem, va->elem_size);
    va->size++;
    return 0;
}

/* 浅拷贝：直接存储指针，elem_size must == sizeof(void*) */
static int byte_array_append_shallow(VariableArray *va, void *elem_ptr) {

	if (va->elem_size != sizeof(void*)) return -1;  // 要求 elem_size == 指针大小
    if (va_ensure_capacity(va) != 0) return -1;
    void **dest = (void**)((char*)va->data + va->size * va->elem_size);
    *dest = elem_ptr;
    va->size++;
    return 0;
}


// 访问元素
static void* byte_array_get(VariableArray *va, size_t index) {
    if (index >= va->size) return NULL;
   	return (char*)va->data + index * va->elem_size;
}

static size_t byte_array_get_size(VariableArray *arr)
{
	return arr->size;
}

// 释放
void delete_variable_array(VariableArray *arr) {
	if(!arr)
		return ;
    free(arr->data);  // :contentReference[oaicite:4]{index=4}
    arr->data = NULL;
    arr->size = arr->capacity = 0;
	free(arr);
}


// 初始化
VariableArray *creat_variable_array(size_t elem_size, size_t initial_capacity) {
	VariableArray *array=(VariableArray *)malloc(sizeof(VariableArray));
    array->elem_size = elem_size;
    array->data      = malloc(elem_size * initial_capacity);
    array->size      = 0;
    array->capacity  = array->data ? initial_capacity : 0;

	array->append_deep=	byte_array_append_deep;
	array->append_shallow=byte_array_append_shallow;
	array->get=byte_array_get;
	array->get_size=byte_array_get_size;

	return array;
}
