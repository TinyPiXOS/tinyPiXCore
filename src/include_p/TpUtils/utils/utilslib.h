#ifndef _UTILS_LIBRARY_H_
#define _UTILS_LIBRARY_H_

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