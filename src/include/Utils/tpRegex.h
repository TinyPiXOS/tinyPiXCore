#ifndef __TP_REGEX_H
#define __TP_REGEX_H

#include "typesDef.h"

TP_DEF_VOID_TYPE_VAR(ItpRegexData);

class tpString;
class tpRegex
{
public:
    tpRegex();
    tpRegex(const tpString &regexStr);
    ~tpRegex();

    /// @brief 设置正则字符串，已设置过则会覆盖
    /// @param regexStr 正则表达式字符串
    void setRegexStr(const tpString &regexStr);

    /// @brief 获取正则字符串
    /// @return 正则字符串
    tpString regexStr() const;

private:
    ItpRegexData *data_;
};

#endif