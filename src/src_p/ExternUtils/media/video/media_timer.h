#ifndef _CLOCK_TIME_H_
#define _CLOCK_TIME_H_

#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>




//定时器
struct TimerHandle{
//	strcut{
		struct timeval start_time;  // 记录开始时间
		struct timeval pause_time;  // 记录暂停时刻的时间
		long paused_us;             // 记录暂停的总时长
		bool running;               // 是否正在计时
		pthread_rwlock_t rw_mut;	// 读写锁
//	};

	void (*start)(struct TimerHandle *timer);			//开始计时
	void (*pause)(struct TimerHandle *timer);			//暂停计时
	void (*resume)(struct TimerHandle *timer);			//继续计时
	void (*adjust_time)(struct TimerHandle *timer,long interval_us);		//调整当前计时时间
	long (*get_run_time)(struct TimerHandle *timer);
};


struct TimerHandle *timer_ofday_handle_creat();
void timer_ofday_handle_free(struct TimerHandle *timer);










#endif
