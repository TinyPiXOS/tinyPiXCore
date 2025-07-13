#ifndef __TP_MEMORY_INFO_H
#define __TP_MEMORY_INFO_H

#include "tpUtils.h"

/// @brief 三种获取方式
/// @brief 1.构造后直接读param或：直接返回构造时候的信息
/// @brief 2.使用getMemory...：返回调用时刻的信息
/// @brief 3.执行upatat后直接读param:返回刷新时候的信息
TP_DEF_VOID_TYPE_VAR(ItpMemoryInfoData);

class tpMemory
{
public:
	tpMemory(bool enable = true, uint16_t samp = 1000);
	~tpMemory();

public:
	/// @brief 获取内存总空间
	/// @return 内存空间Byte
	uint64_t getTotalSize();
	/// @brief 获取内存可用空间
	/// @return 内存空间Byte
	uint64_t getAvailableSize();
	/// @brief 获取内存空闲空间
	/// @return 内存空间Byte
	uint64_t getFreeSize();
	/// @brief 获取内存使用率
	/// @return 使用率 %
	double getUsage(bool unupdate = false);//内存使用率
	tpMemory getMemoryInfo(bool unupdate = false);

private:
	void update();
	uint64_t getMemoryValue(const char *value);

private:
	ItpMemoryInfoData *data_;
};

#endif
