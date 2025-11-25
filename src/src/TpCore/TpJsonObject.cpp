#include <TpJsonObject.h>
#include <TpJsonArray.h>

TpJsonObject::TpJsonObject()
{
    // doc_.SetNull();
    doc_.SetObject();
}

TpJsonObject::TpJsonObject(const TpJsonObject &others)
{
    *this = others;
}

TpList<TpString> TpJsonObject::keys() const
{
    TpList<TpString> keyList;

    if (!doc_.IsObject())
        return keyList;

    for (auto memberIter = doc_.MemberBegin(); memberIter != doc_.MemberEnd(); ++memberIter)
    {
        TpString keyStr = memberIter->name.GetString();

        keyList.emplace_back(keyStr);
    }

    return keyList;
}

bool TpJsonObject::isEmpty() const
{
    return doc_.IsNull();
}

bool TpJsonObject::contains(const TpString &key) const
{
    if (!doc_.IsObject())
        return false;

    return doc_.HasMember(key.c_str());
}

void TpJsonObject::insert(const TpString &key, const TpJsonValue &value)
{
    if (!doc_.IsObject())
        return;

    rapidjson::Document::AllocatorType &allocator = doc_.GetAllocator();

    rapidjson::Value jsonKey;
    jsonKey.SetString(key.c_str(), key.length(), allocator);

    TpJsonValue &tmpJsonValue = const_cast<TpJsonValue &>(value);

    // 已有key值则覆盖
    if (doc_.HasMember(key.c_str()))
    {
        // 获取 "name" 键的迭代器
        rapidjson::Document::MemberIterator iter = doc_.FindMember(key.c_str());
        if (iter != doc_.MemberEnd())
        {
            // 覆盖已有的键值
            iter->value = tmpJsonValue.value_;
        }
    }
    else
    {
        rapidjson::Value newValue;
        newValue.CopyFrom(tmpJsonValue.value_, allocator); // 深拷贝

        doc_.AddMember(jsonKey, newValue, allocator);
    }
}

void TpJsonObject::insert(const TpString &key, const TpJsonObject &value)
{
    if (!doc_.IsObject())
        return;

    rapidjson::Document::AllocatorType &allocator = doc_.GetAllocator();

    rapidjson::Value jsonKey;
    jsonKey.SetString(key.c_str(), key.length(), allocator);

    // 深拷贝
    rapidjson::Value newValue;
    newValue.CopyFrom(value.doc_, allocator);

    if (doc_.HasMember(key.c_str()))
    {
        doc_[key.c_str()] = newValue;
    }
    else
    {
        doc_.AddMember(jsonKey, newValue, allocator);
    }
}

void TpJsonObject::insert(const TpString &key, const TpJsonArray &value)
{
    if (!doc_.IsObject())
        return;

    rapidjson::Document::AllocatorType &allocator = doc_.GetAllocator();

    rapidjson::Value jsonKey;
    jsonKey.SetString(key.c_str(), key.length(), allocator);

    // 深拷贝
    rapidjson::Value newValue;
    newValue.CopyFrom(value.doc_, allocator);

    if (doc_.HasMember(key.c_str()))
    {
        doc_[key.c_str()] = newValue;
    }
    else
    {
        doc_.AddMember(jsonKey, newValue, allocator);
    }
}

TpJsonValue TpJsonObject::value(const TpString &key) const
{
    if (!doc_.IsObject())
        return TpJsonValue();

    if (!doc_.HasMember(key.c_str()))
        return TpJsonValue();

    const rapidjson::Value &jsonValue = doc_[key.c_str()];

    // 数据拷贝一份
    rapidjson::Document tmpJsonDoc;
    tmpJsonDoc.CopyFrom(jsonValue, tmpJsonDoc.GetAllocator());

    TpJsonValue tmpJsonValue;
    tmpJsonValue.value_.Swap(tmpJsonDoc);

    return tmpJsonValue;
}

void TpJsonObject::remove(const TpString &key)
{
    if (!doc_.IsObject())
        return;

    if (!doc_.HasMember(key.c_str()))
        return;

    doc_.RemoveMember(key.c_str());
}

TpJsonObject &TpJsonObject::operator=(const TpJsonObject &others)
{
    doc_.CopyFrom(others.doc_, doc_.GetAllocator());

    return *this;
}
