#include "TpFileInfoWR.h"
#include <fstream>
#include <iostream>

TpFileInfoWR::TpFileInfoWR()
{
}

TpFileInfoWR::~TpFileInfoWR()
{
}

// 直接返回file文件的内容到字符串(一般用于读一行)
TpString TpFileInfoWR::getValueFromFile(const TpString &file)
{
    return getValueFromFile(file.c_str());
}

// 直接设置file文件的内容(一般用于只有一行的文件)
int TpFileInfoWR::setValueFromFile(const TpString &file, const TpString &value)
{
    return setValueFromFile(file.c_str(), value.c_str());
}

// 直接返回file文件的内容到字符串(一般用于读一行)
TpString TpFileInfoWR::getValueFromFile(const char *file)
{
    TpString value = "";
    std::ifstream fd(file);
    if (fd)
    {
        fd >> value;
        fd.close(); // 关闭文件
    }
	return value;
}

// 直接设置file文件的内容(一般用于只有一行的文件)
int TpFileInfoWR::setValueFromFile(const char *file, const char *value)
{
    std::ofstream fd(file, std::ios::out | std::ios::trunc); // 可写打开并且清空原来内容
    if (!fd)
        return -1;
    fd << value << std::endl;
    fd.close();
    return 0;
}

int TpFileInfoWR::getUintFromFile(const char *file, uint64_t *value)
{
    TpString value_str = getValueFromFile(file);
    if (value_str.empty())
        return -1;

    char *endptr;
    *value = strtoull(value_str.c_str(), &endptr, 10);
    if (*endptr != '\0')
    {
        return -1;
    }
    return 0;
}

int TpFileInfoWR::getUintFromFile(const TpString file, uint64_t *value)
{
    return getUintFromFile(file.c_str(), value);
}

int TpFileInfoWR::getBoolFromFile(const char * file, bool * value)
{
	return 0;
}

int TpFileInfoWR::getBoolFromFile(const TpString file, bool *value)
{
    return 0;
}

int TpFileInfoWR::getUintByKeyValue(const TpString file, tpUInt64 *value)
{
	
	return 0;
}
