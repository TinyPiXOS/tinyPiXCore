#include <TpJsonArray.h>
#include <TpJsonObject.h>

TpJsonArray::TpJsonArray()
{
    doc_.SetArray();
}

TpJsonArray::TpJsonArray(const TpJsonArray &array)
{
    *this = array;
}

uint32_t TpJsonArray::count() const
{
    if (!doc_.IsArray())
        return 0;

    return doc_.Size();
}

bool TpJsonArray::isEmpty() const
{
    return doc_.IsNull();
}

TpJsonValue TpJsonArray::at(const uint32_t &index) const
{
    if (!doc_.IsArray())
        return TpJsonValue();

    if (index >= doc_.Size())
        return TpJsonValue();

    const rapidjson::Value &jsonValue = doc_[index];

    // 数据拷贝一份
    rapidjson::Document tmpJsonDoc;
    tmpJsonDoc.CopyFrom(jsonValue, tmpJsonDoc.GetAllocator());

    TpJsonValue tmpJsonValue;
    tmpJsonValue.value_.Swap(tmpJsonDoc);

    return tmpJsonValue;

    // TpJsonValue *tmpJsonValue = new TpJsonValue();
    // tmpJsonValue->value_.Swap(tmpJsonDoc);

    // return *tmpJsonValue;
}

void TpJsonArray::append(const TpJsonValue &value)
{
    if (!doc_.IsArray())
        return;

    if (value.isNull())
        return;

    rapidjson::Value newValue;
    newValue.CopyFrom(value.value_, doc_.GetAllocator());
    doc_.PushBack(newValue, doc_.GetAllocator());
}

void TpJsonArray::append(const TpJsonObject &object)
{
    if (!doc_.IsArray())
        return;

    TpJsonObject &tmpJsonObject = const_cast<TpJsonObject &>(object);

    rapidjson::Value newValue;
    newValue.CopyFrom(tmpJsonObject.doc_, doc_.GetAllocator());
    doc_.PushBack(newValue, doc_.GetAllocator());
}

void TpJsonArray::append(const TpJsonArray &array)
{
    if (!doc_.IsArray())
        return;

    TpJsonArray &tmpJsonArray = const_cast<TpJsonArray &>(array);

    rapidjson::Value newValue;
    newValue.CopyFrom(tmpJsonArray.doc_, doc_.GetAllocator());
    doc_.PushBack(newValue, doc_.GetAllocator());
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
