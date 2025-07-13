#ifndef __TP_THREAD_MANAGE_H
#define __TP_THREAD_MANAGE_H

#include "tpString.h"

class tpFileInfoWR
{
public:
	tpFileInfoWR();
	~tpFileInfoWR();

public:
	tpString getValueFromFile(const char *file);
	int setValueFromFile(const char *file, const char *value);
	tpString getValueFromFile(const tpString &file);
	int setValueFromFile(const tpString &file, const tpString &value);
	int getUintFromFile(const char *file, uint64_t *value);
	int getUintFromFile(const tpString file, uint64_t *value);
	int getBoolFromFile(const char *file, bool *value);
	int getBoolFromFile(const tpString file, bool *value);
	int getUintByKeyValue(const tpString file, tpUInt64 *value);
};

#endif