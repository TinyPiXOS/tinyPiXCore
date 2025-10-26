#include <TpJsonArray.h>

TpJsonArray::TpJsonArray()
{
    doc_.SetArray();
}

TpJsonArray::TpJsonArray(const TpJsonArray &array)
{
    *this = array;
}

uint32_t TpJsonArray::count()
{
    if (!doc_.IsArray())
        return 0;

    return doc_.Size();
}

bool TpJsonArray::isEmpty() const
{
    return doc_.IsNull();
}

TpJsonValue TpJsonArray::at(const uint32_t &index)
{
    if (!doc_.IsArray())
        return TpJsonValue();

    if (index >= doc_.Size())
        return TpJsonValue();

    rapidjson::Value &jsonValue = doc_[index];

    // 数据拷贝一份
    rapidjson::Document tmpJsonDoc;
    tmpJsonDoc.CopyFrom(jsonValue, tmpJsonDoc.GetAllocator());

    TpJsonValue* tmpJsonValue = new TpJsonValue();
    tmpJsonValue->value_.Swap(tmpJsonDoc);

    return *tmpJsonValue;
}

void TpJsonArray::append(const TpJsonValue &value)
{
    if (!doc_.IsArray())
        return;

    if (value.isNull())
        return;

    TpJsonValue tmpJsonValue = value;

    rapidjson::Value newValue;
    newValue.CopyFrom(value.value_, doc_.GetAllocator());
    doc_.PushBack(newValue, doc_.GetAllocator());

    // doc_.PushBack(tmpJsonValue.value_, doc_.GetAllocator());
}

TpJsonValue TpJsonArray::first()
{
    if (!doc_.IsArray())
        return TpJsonValue();

    if (doc_.Size() < 1)
        return TpJsonValue();

    rapidjson::Value &jsonValue = doc_[0];

    // 数据拷贝一份
    rapidjson::Document tmpJsonDoc;
    tmpJsonDoc.CopyFrom(jsonValue, tmpJsonDoc.GetAllocator());

    TpJsonValue tmpJsonValue;
    tmpJsonValue.value_.Swap(tmpJsonDoc);

    return tmpJsonValue;
}

TpJsonValue TpJsonArray::last()
{
    if (!doc_.IsArray())
        return TpJsonValue();

    if (doc_.Size() < 1)
        return TpJsonValue();

    rapidjson::Value &jsonValue = doc_[doc_.Size() - 1];

    // 数据拷贝一份
    rapidjson::Document tmpJsonDoc;
    tmpJsonDoc.CopyFrom(jsonValue, tmpJsonDoc.GetAllocator());

    TpJsonValue tmpJsonValue;
    tmpJsonValue.value_.Swap(tmpJsonDoc);

    return tmpJsonValue;
}

TpJsonArray &TpJsonArray::operator=(const TpJsonArray &others)
{
    doc_.CopyFrom(others.doc_, doc_.GetAllocator());
    return *this;
}
