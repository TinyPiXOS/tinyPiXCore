
#include "utthread.h"


// 创建线程对象
UtilsThread* utils_thread_create(const char* name, void* (*task_func)(void*), void* arg) {
    UtilsThread* thread = (UtilsThread*)malloc(sizeof(UtilsThread));
    if (!thread) 
		return NULL;
    
    snprintf(thread->name, sizeof(thread->name), "%s", name);
    thread->status = THREAD_NEW;
    thread->should_stop = false;
    thread->task_func = task_func;
    thread->task_arg = arg;
    pthread_mutex_init(&thread->lock, NULL);
    return thread;
}

// 启动线程[4,6](@ref)
bool utils_thread_start(UtilsThread* thread) {
    if (!thread || thread->status != THREAD_NEW) {
		printf("false\n");
		return false;
	}
		
    
    pthread_mutex_lock(&thread->lock);
    thread->status = THREAD_RUNNING;
    pthread_mutex_unlock(&thread->lock);
    
    return pthread_create(&thread->tid, NULL, thread->task_func, thread) == 0;
}

// 停止线程（优雅退出）[11,12](@ref)
void utils_thread_stop(UtilsThread* thread) {
    if (!thread || thread->status != THREAD_RUNNING) 
		return;
    
    pthread_mutex_lock(&thread->lock);
    thread->should_stop = true;
    pthread_mutex_unlock(&thread->lock);
}

// 强制终止线程（不推荐）[9](@ref)
void utils_thread_terminate(UtilsThread* thread) {
    if (!thread || thread->status != THREAD_RUNNING) 
		return;
    pthread_cancel(thread->tid);
}

// 等待线程结束[4,6](@ref)
void utils_thread_join(UtilsThread* thread) {
    if (!thread || thread->status == THREAD_NEW) 
		return;
    pthread_join(thread->tid, NULL);
    
    pthread_mutex_lock(&thread->lock);
    thread->status = THREAD_STOPPED;
    pthread_mutex_unlock(&thread->lock);
}

// 检查线程状态[6](@ref)
ThreadStatus utils_thread_get_status(UtilsThread* thread) {
    if (!thread) 
		return THREAD_STOPPED;
    
    ThreadStatus status;
    pthread_mutex_lock(&thread->lock);
    status = thread->status;
    pthread_mutex_unlock(&thread->lock);
    return status;
}

// 检查停止标志（在任务函数中使用）[11](@ref)
bool utils_thread_should_stop(UtilsThread* thread) {
    if (!thread) 
		return true;
    
    bool should_stop;
    pthread_mutex_lock(&thread->lock);
    should_stop = thread->should_stop;
    pthread_mutex_unlock(&thread->lock);
    return should_stop;
}

// 销毁线程资源
void utils_thread_destroy(UtilsThread* thread) {
    if (!thread) 
		return;
    
    if (thread->status == THREAD_RUNNING) {
        utils_thread_stop(thread);
        utils_thread_join(thread);
    }
    
    pthread_mutex_destroy(&thread->lock);
    free(thread);
}