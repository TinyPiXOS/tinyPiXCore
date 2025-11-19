#ifndef __TP_TIMER_H
#define __TP_TIMER_H

#include "TpUtils.h"
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(IPiTimerData);
/// @brief 定时器功能类
class TpTimer
{
public:
	enum TimerType
	{
		PreciseTimer,
		CoarseTimer,
		VeryCoarseTimer
	};

public:
	TpTimer(int32_t msec = 100);
	virtual ~TpTimer();

	bool isActive() const;
	uint32_t timerId() const;

	void setInterval(uint32_t msec);
	uint32_t interval() const;

	// 本接口暂时无效，都是精准定时器
	void setTimerType(TpTimer::TimerType atype);
	TpTimer::TimerType timerType();

	void start(int32_t msec);
	void start();

	void stop();

	static void sleep(const uint64_t& msec);
	
public
signals:
	// declare_signal(onTimer, int32_t, unsigned long long);
	declare_signal(timeout);

private:
	IPiTimerData *timerSet_;

	void timerFunction();
};

#endif
