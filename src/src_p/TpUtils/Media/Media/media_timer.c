
/*///------------------------------------------------------------------------------------------------------------------------//
		时钟同步
说 明 : 
日 期 : 2025.2.9

/*///------------------------------------------------------------------------------------------------------------------------//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Media/media_timer.h"


#ifdef DEBUG_MEDIA_TIMER
#include "Log/elog.h"
#define debug_printf(...) elog_d("MediaPlayer.Timer", ##__VA_ARGS__)
//#define debug_printf(fmt, ...) elog_d(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...)  // 如果不定义DEBUG，什么也不做
#endif


// 获取当前时间（微秒）
static long get_current_time_us() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000) + tv.tv_usec;
}

// 启动计时器
static void start_timer_ofday(struct TimerHandle *timer) {
	if(timer->running)
		return ;
	pthread_rwlock_wrlock(&timer->rw_mut);  // 获取写锁
	gettimeofday(&timer->start_time, NULL);
	timer->paused_us = 0;
	timer->running = true;
	pthread_rwlock_unlock(&timer->rw_mut);  // 获取写锁
	debug_printf("Stopwatch started.\n");
}

// 暂停计时器
static void pause_timer_ofday(struct TimerHandle *timer) {
	if (!timer->running)
		return ;
	pthread_rwlock_wrlock(&timer->rw_mut);  // 获取写锁
	gettimeofday(&timer->pause_time, NULL);
	timer->running = false;
	pthread_rwlock_unlock(&timer->rw_mut); 
	debug_printf("Stopwatch paused.\n");
}

// 继续计时器
static void resume_timer_ofday(struct TimerHandle *timer) {
	pthread_rwlock_wrlock(&timer->rw_mut);  // 获取写锁
	if (!timer->running) {
		struct timeval resume_time;
		gettimeofday(&resume_time, NULL);
		long paused_duration = (resume_time.tv_sec - timer->pause_time.tv_sec) * 1000000 +		
								(resume_time.tv_usec - timer->pause_time.tv_usec);		//计算本次暂停的时间段内的时长
		timer->paused_us += paused_duration; // 累积暂停时间
		timer->running = true;
		debug_printf("Stopwatch resumed.\n");
	}
	pthread_rwlock_unlock(&timer->rw_mut); 
}

// 获取已运行时间（微秒）
static long get_elapsed_time(struct TimerHandle *timer) {
    long long elapsed_physical_us; // 使用long long防止溢出
    pthread_rwlock_rdlock(&timer->rw_mut);

    if (timer->running) {
        long long current_time_us = get_current_time_us();
        long long start_time_us = (timer->start_time.tv_sec * 1000000LL) + timer->start_time.tv_usec;
        elapsed_physical_us = current_time_us - start_time_us - timer->paused_us;
    } else {
        long long pause_time_us = (timer->pause_time.tv_sec * 1000000LL) + timer->pause_time.tv_usec;
        long long start_time_us = (timer->start_time.tv_sec * 1000000LL) + timer->start_time.tv_usec;
        elapsed_physical_us = pause_time_us - start_time_us - timer->paused_us;
    }

    // 核心修正：应用倍速因子，将物理时间转换为媒体时间
    long long media_time_us = (long long)(elapsed_physical_us * timer->speed);

    pthread_rwlock_unlock(&timer->rw_mut);
    return (long)media_time_us;
}

// 重置计时器
static void reset_timer_ofday(struct TimerHandle *timer) {
	pthread_rwlock_wrlock(&timer->rw_mut);  // 获取写锁
    timer->running = false;
    timer->paused_us = 0;
	pthread_rwlock_unlock(&timer->rw_mut);
    debug_printf("Stopwatch reset.\n");
}

//调整时间
static void adjust_timer_ofday(struct TimerHandle *timer, long new_time_us) {

	long now_us = get_current_time_us();
	pthread_rwlock_wrlock(&timer->rw_mut);  // 获取写锁
	

	if (timer->running) {
		long long physical_elapsed_us = (long long)(new_time_us / timer->speed);
        
        // 计算新的start_time：当前时间 - 物理流逝时间
        long long new_start_time_us = now_us - physical_elapsed_us;
        
        timer->start_time.tv_sec = new_start_time_us / 1000000;
        timer->start_time.tv_usec = new_start_time_us % 1000000;
        
        // 重置暂停累计时间，因为时间轴已经重新定位
        timer->paused_us = 0;
	} 
	else {
		// 计时器暂停状态下，更新 pause_time 以保持调整后的时间
		long long physical_elapsed_us = (long long)(new_time_us / timer->speed);
        long long new_pause_time_us = now_us - physical_elapsed_us;
        
        timer->pause_time.tv_sec = new_pause_time_us / 1000000;
        timer->pause_time.tv_usec = new_pause_time_us % 1000000;
        timer->paused_us = 0;
	}
	pthread_rwlock_unlock(&timer->rw_mut);
	debug_printf("Stopwatch adjusted to %ld us (%.3f s).\n", new_time_us, new_time_us / 1000000.0);
}



static void set_speed_impl(struct TimerHandle *timer, double new_speed) {
    if (new_speed <= 0) {
        // 无效的倍速值
        return;
    }

	// 1. 获取改变倍速前这一瞬间的媒体时间（微秒）
    long current_media_time_us = timer->get_run_time(timer);

	// 3. 关键：反向计算新的起始物理时间 start_time
    // 原理：让新的时间体系下，`current_media_time_us` 这个值保持不变
    // 公式：current_media_time_us = (now_us - new_start_time_us) * new_speed
    // 变形：new_start_time_us = now_us - (current_media_time_us / new_speed)
    long long now_us = get_current_time_us(); // 需要实现这个函数，返回自纪元起的微秒数
    long long new_start_time_us = now_us - (long long)(current_media_time_us / new_speed);

    pthread_rwlock_wrlock(&timer->rw_mut);

    // 2. 更新倍速因子
    timer->speed = new_speed;

    // 将计算出的新起始时间赋值给 start_time
    timer->start_time.tv_sec = new_start_time_us / 1000000;
    timer->start_time.tv_usec = new_start_time_us % 1000000;

    // 4. 重置暂停相关的累计值，因为时间轴的连续性已由新的start_time保证
    timer->paused_us = 0;
    // 注意：如果当前正处于暂停状态，可能需要额外处理pause_time，这里假设在运行状态下设置倍速

    pthread_rwlock_unlock(&timer->rw_mut);
    ///debug_printf("Speed adjusted to %.2fx. Media time anchored at %ld us.\n", timer->speed, current_media_time_us);
}


//创建timeofday相关的定时器句柄
struct TimerHandle *timer_ofday_handle_creat()
{
	struct TimerHandle *timer=(struct TimerHandle *)malloc(sizeof(struct TimerHandle));
	if(!timer)
		return NULL;

	timer->running = false;
    timer->paused_us = 0;
	timer->speed=1.0;
	pthread_rwlock_init(&timer->rw_mut, NULL);
	timer->adjust_time=adjust_timer_ofday;
	timer->start=start_timer_ofday;
	timer->pause=pause_timer_ofday;
	timer->resume=resume_timer_ofday;
	timer->get_run_time=get_elapsed_time;
	timer->set_speed =set_speed_impl;
	return timer;
}

//释放
void timer_ofday_handle_delete(struct TimerHandle *timer)
{
	if(!timer)
		return ;
	pthread_rwlock_destroy(&timer->rw_mut);
	free(timer);
	timer=NULL;
}




