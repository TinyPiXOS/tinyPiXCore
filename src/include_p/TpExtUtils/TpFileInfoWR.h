#ifndef __TP_THREAD_MANAGE_H
#define __TP_THREAD_MANAGE_H

#include "TpString.h"

class TpFileInfoWR
{
public:
	TpFileInfoWR();
	~TpFileInfoWR();

public:
	TpString getValueFromFile(const char *file);
	int setValueFromFile(const char *file, const char *value);
	TpString getValueFromFile(const TpString &file);
	int setValueFromFile(const TpString &file, const TpString &value);
	int getUintFromFile(const char *file, uint64_t *value);
	int getUintFromFile(const TpString file, uint64_t *value);
	int getBoolFromFile(const char *file, bool *value);
	int getBoolFromFile(const TpString file, bool *value);
	int getUintByKeyValue(const TpString file, tpUInt64 *value);
};

#endif