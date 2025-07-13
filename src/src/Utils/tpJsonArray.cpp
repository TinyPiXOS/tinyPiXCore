#include <tpJsonArray.h>

tpJsonArray::tpJsonArray()
{
    doc_.SetArray();
}

tpJsonArray::tpJsonArray(const tpJsonArray &array)
{
    *this = array;
}

uint32_t tpJsonArray::count()
{
    if (!doc_.IsArray())
        return 0;

    return doc_.Size();
}

bool tpJsonArray::isEmpty() const
{
    return doc_.IsNull();
}

tpJsonValue tpJsonArray::at(const uint32_t &index)
{
    if (!doc_.IsArray())
        return tpJsonValue();

    if (index >= doc_.Size())
        return tpJsonValue();

    rapidjson::Value &jsonValue = doc_[index];

    // 数据拷贝一份
    rapidjson::Document tmpJsonDoc;
    tmpJsonDoc.CopyFrom(jsonValue, tmpJsonDoc.GetAllocator());

    tpJsonValue* tmpJsonValue = new tpJsonValue();
    tmpJsonValue->value_.Swap(tmpJsonDoc);

    return *tmpJsonValue;
}

void tpJsonArray::append(const tpJsonValue &value)
{
    if (!doc_.IsArray())
        return;

    if (value.isNull())
        return;

    tpJsonValue tmpJsonValue = value;

    rapidjson::Value newValue;
    newValue.CopyFrom(value.value_, doc_.GetAllocator());
    doc_.PushBack(newValue, doc_.GetAllocator());

    // doc_.PushBack(tmpJsonValue.value_, doc_.GetAllocator());
}

tpJsonValue tpJsonArray::first()
{
    if (!doc_.IsArray())
        return tpJsonValue();

    if (doc_.Size() < 1)
        return tpJsonValue();

    rapidjson::Value &jsonValue = doc_[0];

    // 数据拷贝一份
    rapidjson::Document tmpJsonDoc;
    tmpJsonDoc.CopyFrom(jsonValue, tmpJsonDoc.GetAllocator());

    tpJsonValue tmpJsonValue;
    tmpJsonValue.value_.Swap(tmpJsonDoc);

    return tmpJsonValue;
}

tpJsonValue tpJsonArray::last()
{
    if (!doc_.IsArray())
        return tpJsonValue();

    if (doc_.Size() < 1)
        return tpJsonValue();

    rapidjson::Value &jsonValue = doc_[doc_.Size() - 1];

    // 数据拷贝一份
    rapidjson::Document tmpJsonDoc;
    tmpJsonDoc.CopyFrom(jsonValue, tmpJsonDoc.GetAllocator());

    tpJsonValue tmpJsonValue;
    tmpJsonValue.value_.Swap(tmpJsonDoc);

    return tmpJsonValue;
}

tpJsonArray &tpJsonArray::operator=(const tpJsonArray &others)
{
    doc_.CopyFrom(others.doc_, doc_.GetAllocator());
    return *this;
}
