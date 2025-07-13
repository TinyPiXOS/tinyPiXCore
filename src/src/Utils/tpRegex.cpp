#include "tpRegex.h"
#include "tpString.h"

struct tpRegexData
{
    tpString regStr;

    tpRegexData() : regStr("")
    {
    }
};

tpRegex::tpRegex()
{
    data_ = new tpRegexData();
}

tpRegex::tpRegex(const tpString &regexStr)
{
    data_ = new tpRegexData();

    tpRegexData *regData = static_cast<tpRegexData *>(data_);
    regData->regStr = regexStr;
}

tpRegex::~tpRegex()
{
    tpRegexData *regData = static_cast<tpRegexData *>(data_);
    if (regData)
    {
        delete regData;
        regData = nullptr;
        data_ = nullptr;
    }
}

void tpRegex::setRegexStr(const tpString &regexStr)
{
    tpRegexData *regData = static_cast<tpRegexData *>(data_);
    regData->regStr = regexStr;
}

tpString tpRegex::regexStr() const
{
    tpRegexData *regData = static_cast<tpRegexData *>(data_);
    return regData->regStr;
}
