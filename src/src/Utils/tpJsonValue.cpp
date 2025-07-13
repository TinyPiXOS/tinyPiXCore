#include "tpJsonValue.h"
#include "tpJsonArray.h"
#include "tpJsonObject.h"

bool tpJsonValue::toBool() const
{
    if (isBool())
        return value_.GetBool();
    return false;
}

int32_t tpJsonValue::toInt() const
{
    if (isInt())
        return value_.GetInt();
    return 0;
}

uint32_t tpJsonValue::toUint() const
{
    if (isUint())
        return value_.GetUint();
    return 0;
}
int64_t tpJsonValue::toInt64() const
{
    if (isInt64())
        return value_.GetInt64();
    return 0;
}

uint64_t tpJsonValue::toUint64() const
{
    if (isUint64())
        return value_.GetUint64();
    return 0;
}

double tpJsonValue::toDouble() const
{
    if (isDouble())
        return value_.GetDouble();
    return 0;
}

tpString tpJsonValue::toString() const
{
    if (isString())
        return value_.GetString();
    return "";
}

tpJsonObject tpJsonValue::toObject() const
{
    if (!isObject())
        return tpJsonObject();

    tpJsonObject tmpJsonObj;
    tmpJsonObj.doc_.CopyFrom(value_, tmpJsonObj.doc_.GetAllocator());

    return tmpJsonObj;
}

tpJsonArray tpJsonValue::toArray() const
{
    tpJsonArray tmpJsonArr;
    tmpJsonArr.doc_.CopyFrom(value_, tmpJsonArr.doc_.GetAllocator());

    return tmpJsonArr;
}

tpJsonValue& tpJsonValue::operator=(const tpJsonValue &others)
{
    rapidjson::Document jsonDoc;
    jsonDoc.CopyFrom(others.value_, jsonDoc.GetAllocator());

    value_.CopyFrom(others.value_, jsonDoc.GetAllocator(), true);  // true 表示深拷贝

    return *this;
}
