#ifndef __TP_THREAD_H
#define __TP_THREAD_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpThreadData);
class tpThread
{
public:
	tpThread();
	virtual ~tpThread();

	/// @brief 启动线程
	/// @return 启动结果
	virtual bool start();
	/// @brief 强行终止线程
	virtual void terminate();
	/// @brief 等待线程执行完毕后终止
	virtual void stop();

	/// @brief 设置线程是否仅执行一次
	/// @param runOnce 是否仅执行一次
	virtual void setRunOnce(bool runOnce);
	/// @brief 获取线程是否仅执行一次
	/// @return 是否仅执行一次
	virtual bool getRunOnce();

	/// @brief 获取线程ID
	/// @return 线程ID
	virtual int32_t getThreadID();
	/// @brief 线程是否已结束
	/// @return 结束标识
	virtual bool isFinished();
	/// @brief 线程是否正在运行
	/// @return 运行标识
	virtual bool isRunning();

	/// @brief 线程运行函数；子类重写并实现
	virtual void run() {}

private:
	ItpThreadData *threadSet;
};

#endif
