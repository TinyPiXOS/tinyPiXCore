#ifndef _THREAD_MANAGE_H_
#define _THREAD_MANAGE_H_

#include "tpUtils.h"
#include <functional>

TP_DEF_VOID_TYPE_VAR(ItpThreadManageData);
class tpThreadManage
{
public:
	tpThreadManage(std::function<void(int)> callback, std::function<void(int)> callback2, uint16_t time_samp = 1000);
	~tpThreadManage();

public:
	void threadUpdateStat(std::function<void(int)> callback, std::function<void(int)> callback2, uint16_t time_samp); // 主循环

private:
	ItpThreadManageData *data_;
};

#endif
