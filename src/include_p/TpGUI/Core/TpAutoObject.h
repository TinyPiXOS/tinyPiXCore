#ifndef __TP_VAUTO_OBJECT_H
#define __TP_VAUTO_OBJECT_H

#include <TpUtils.h>
#include <mutex>
#include <list>

class TpAutoObject
{
public:
	friend class TpApp;

private:
	TpAutoObject();

public:
	virtual ~TpAutoObject();

public:
	static TpAutoObject *Inst();

public:
	static int32_t selfCounterIncrease();
	static bool isExist(void *ptr);
	static bool addObjectLife(void *ptr);
	static bool removeObjectLife(void *ptr);

private:
	static TpAutoObject *objectHandle;
	static std::mutex autoMutex;
	static std::list<void *> objectLife;
	static int32_t counter;
	static bool autoFreeObject;
};

#endif
