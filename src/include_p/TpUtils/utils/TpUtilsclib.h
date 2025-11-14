//C语言部分通用的库/工具
#ifndef _TP_UTILS_C_LIB_H_
#define _TP_UTILS_C_LIB_H_

#include "utilslib.h"				//基础功能接口
#include "utlist.h"					//链表
#include "variable_array.h"			//动态数组
#include <pthread.h>

#define MIN_VALUE(a, b) ((a) < (b) ? (a) : (b))
#define MAX_VALUE(a, b) ((a) > (b) ? (a) : (b))

typedef enum{
	B_FALSE	= 0,
	B_TRUE	= 1
}UtilsBool;

//回调结构体
struct CallbackData{
	void (*callback)(const void *indata,void *userdata);
	void *userdata;
	pthread_mutex_t lock;
};

#endif