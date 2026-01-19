#include "TpJsonValue.h"
#include "TpJsonArray.h"
#include "TpJsonObject.h"

bool TpJsonValue::toBool() const
{
    if (isBool())
        return value_.GetBool();
    return false;
}

int32_t TpJsonValue::toInt() const
{
    if (isInt())
        return value_.GetInt();
    return 0;
}

uint32_t TpJsonValue::toUint() const
{
    if (isUint())
        return value_.GetUint();
    return 0;
}
int64_t TpJsonValue::toInt64() const
{
    if (isInt64())
        return value_.GetInt64();
    return 0;
}

uint64_t TpJsonValue::toUint64() const
{
    if (isUint64())
        return value_.GetUint64();
    return 0;
}

double TpJsonValue::toDouble() const
{
    if (isDouble())
        return value_.GetDouble();
    return 0;
}

TpString TpJsonValue::toString() const
{
    if (isString())
        return value_.GetString();
    return "";
}

TpJsonObject TpJsonValue::toObject() const
{
    if (!isObject())
        return TpJsonObject();

    TpJsonObject tmpJsonObj;
    tmpJsonObj.doc_.CopyFrom(value_, tmpJsonObj.doc_.GetAllocator());

    return tmpJsonObj;
}

TpJsonArray TpJsonValue::toArray() const
{
    TpJsonArray tmpJsonArr;
    tmpJsonArr.doc_.CopyFrom(value_, tmpJsonArr.doc_.GetAllocator());

    return tmpJsonArr;
}

TpJsonValue& TpJsonValue::operator=(const TpJsonValue &others)
{
    // rapidjson::Document jsonDoc;
    document_.CopyFrom(others.document_, document_.GetAllocator());
    value_.CopyFrom(others.value_, document_.GetAllocator(), true);  // true 表示深拷贝

    return *this;
}
