
/*///------------------------------------------------------------------------------------------------------------------------//
		时钟同步
说 明 : 
日 期 : 2025.2.9

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "media_timer.h"


#ifdef DEBUG_MEDIA_TIMER
    #define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
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
	gettimeofday(&timer->start_time, NULL);
	timer->paused_us = 0;
	timer->running = true;
	debug_printf("Stopwatch started.\n");
}

// 暂停计时器
static void pause_timer_ofday(struct TimerHandle *timer) {
	if (timer->running) {
		gettimeofday(&timer->pause_time, NULL);
		timer->running = false;
		debug_printf("Stopwatch paused.\n");
	}
}

// 继续计时器
static void resume_timer_ofday(struct TimerHandle *timer) {
	if (!timer->running) {
		struct timeval resume_time;
		gettimeofday(&resume_time, NULL);
		long paused_duration = (resume_time.tv_sec - timer->pause_time.tv_sec) * 1000000 +		
								(resume_time.tv_usec - timer->pause_time.tv_usec);		//计算本次暂停的时间段内的时长
		timer->paused_us += paused_duration; // 累积暂停时间
		timer->running = true;
		debug_printf("Stopwatch resumed.\n");
	}
}

// 获取已运行时间（微秒）
static long get_elapsed_time(struct TimerHandle *timer) {
	if (timer->running) {
		long current_time = get_current_time_us();
		long start_time = (timer->start_time.tv_sec * 1000000) + timer->start_time.tv_usec;
		return current_time - start_time - timer->paused_us;
	} 
	else {
		long pause_time = (timer->pause_time.tv_sec * 1000000) + timer->pause_time.tv_usec;
		long start_time = (timer->start_time.tv_sec * 1000000) + timer->start_time.tv_usec;
		return pause_time - start_time - timer->paused_us;
	}
}

// 重置计时器
static void reset_timer_ofday(struct TimerHandle *timer) {
    timer->running = false;
    timer->paused_us = 0;
    debug_printf("Stopwatch reset.\n");
}

//调整时间
static void adjust_timer_ofday(struct TimerHandle *timer, long new_time_us) {
	long now_us = get_current_time_us();

	if (timer->running) {
		// 计算新 start_time 以便 get_elapsed_time() 返回 new_time_us
		timer->start_time.tv_sec = (now_us - new_time_us) / 1000000;
		timer->start_time.tv_usec = (now_us - new_time_us) % 1000000;
	} 
	else {
		// 计时器暂停状态下，更新 pause_time 以保持调整后的时间
		timer->pause_time.tv_sec = (now_us - new_time_us) / 1000000;
		timer->pause_time.tv_usec = (now_us - new_time_us) % 1000000;
	}

	debug_printf("Stopwatch adjusted to %ld us (%.3f s).\n", new_time_us, new_time_us / 1000000.0);
}

//创建timeofday相关的定时器句柄
struct TimerHandle *timer_ofday_handle_creat()
{
	struct TimerHandle *timer=(struct TimerHandle *)malloc(sizeof(struct TimerHandle));
	if(!timer)
		return NULL;

	timer->running = false;
    timer->paused_us = 0;
	pthread_rwlock_init(&timer->rw_mut, NULL);
	timer->adjust_time=adjust_timer_ofday;
	timer->start=start_timer_ofday;
	timer->pause=pause_timer_ofday;
	timer->resume=resume_timer_ofday;
	timer->get_run_time=get_elapsed_time;
	return timer;
}

//释放
void timer_ofday_handle_free(struct TimerHandle *timer)
{
	if(!timer)
		return ;
	pthread_rwlock_destroy(&timer->rw_mut);
	free(timer);
}



#ifdef __cplusplus
}
#endif
