#ifndef _UTILS_THREAD_H_
#define _UTILS_THREAD_H_

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// 线程状态枚举
typedef enum {
    THREAD_NEW,     // 新建未启动
    THREAD_RUNNING, // 运行中
    THREAD_STOPPED, // 已停止
    THREAD_SUSPENDED// 暂停中
} ThreadStatus;

// 线程控制结构体
typedef struct UtilsThread{
    pthread_t tid;          // 线程ID
    char name[32];          // 线程名称
    ThreadStatus status;    // 线程状态
    bool should_stop;       // 停止标志
    void* (*task_func)(void*); // 任务函数指针
    void* task_arg;         // 任务参数（用户的参数）
    pthread_mutex_t lock;   // 状态锁
}UtilsThread;



// 创建线程对象
UtilsThread* utils_thread_create(const char* name, void* (*task_func)(void*), void* arg);

// 启动线程[4,6](@ref)
bool utils_thread_start(UtilsThread* thread);

// 停止线程（优雅退出）[11,12](@ref)
void utils_thread_stop(UtilsThread* thread);

// 强制终止线程（不推荐）[9](@ref)
void utils_thread_terminate(UtilsThread* thread);

// 等待线程结束[4,6](@ref)
void utils_thread_join(UtilsThread* thread);

// 检查线程状态[6](@ref)
ThreadStatus utils_thread_get_status(UtilsThread* thread);

// 检查停止标志（在任务函数中使用）[11](@ref)
bool utils_thread_should_stop(UtilsThread* thread);

// 销毁线程资源
void utils_thread_destroy(UtilsThread* thread);


#endif