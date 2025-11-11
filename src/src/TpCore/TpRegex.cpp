#include "TpRegex.h"
#include "TpString.h"

struct TpRegexData
{
    TpString regStr;

    TpRegexData() : regStr("")
    {
    }
};

TpRegex::TpRegex()
{
    data_ = new TpRegexData();
}

TpRegex::TpRegex(const TpString &regexStr)
{
    data_ = new TpRegexData();

    TpRegexData *regData = static_cast<TpRegexData *>(data_);
    regData->regStr = regexStr;
}

TpRegex::~TpRegex()
{
    TpRegexData *regData = static_cast<TpRegexData *>(data_);
    if (regData)
    {
        delete regData;
        regData = nullptr;
        data_ = nullptr;
    }
}

void TpRegex::setRegexStr(const TpString &regexStr)
{
    TpRegexData *regData = static_cast<TpRegexData *>(data_);
    regData->regStr = regexStr;
}

TpString TpRegex::regexStr() const
{
    TpRegexData *regData = static_cast<TpRegexData *>(data_);
    return regData->regStr;
}
