#ifndef __TP_REGEX_H
#define __TP_REGEX_H

#include "TpCore.h"

TP_DEF_VOID_TYPE_VAR(ITpRegexData);

class TpString;
class TpRegex
{
public:
    TpRegex();
    TpRegex(const TpString &regexStr);
    ~TpRegex();

    /// @brief 设置正则字符串，已设置过则会覆盖
    /// @param regexStr 正则表达式字符串
    void setRegexStr(const TpString &regexStr);

    /// @brief 获取正则字符串
    /// @return 正则字符串
    TpString regexStr() const;

private:
    ITpRegexData *data_;
};

#endif