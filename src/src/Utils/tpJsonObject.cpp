#include <tpJsonObject.h>
#include <tpJsonArray.h>

tpJsonObject::tpJsonObject()
{
    // doc_.SetNull();
    doc_.SetObject();
}

tpJsonObject::tpJsonObject(const tpJsonObject &others)
{
    *this = others;
}

tpList<tpString> tpJsonObject::keys() const
{
    tpList<tpString> keyList;

    if (!doc_.IsObject())
        return keyList;

    for (auto memberIter = doc_.MemberBegin(); memberIter != doc_.MemberEnd(); ++memberIter)
    {
        tpString keyStr = memberIter->name.GetString();

        keyList.emplace_back(keyStr);
    }

    return keyList;
}

bool tpJsonObject::isEmpty() const
{
    return doc_.IsNull();
}

bool tpJsonObject::contains(const tpString &key) const
{
    if (!doc_.IsObject())
        return false;

    return doc_.HasMember(key.c_str());
}

void tpJsonObject::insert(const tpString &key, const tpJsonValue &value)
{
    if (!doc_.IsObject())
        return;

    rapidjson::Document::AllocatorType &allocator = doc_.GetAllocator();

    rapidjson::Value jsonKey;
    jsonKey.SetString(key.c_str(), key.length(), allocator);

    tpJsonValue &tmpJsonValue = const_cast<tpJsonValue &>(value);

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
        // doc_.AddMember(jsonKey, tmpJsonValue.value_, allocator);
    }
}

void tpJsonObject::insert(const tpString &key, const tpJsonObject &value)
{
    if (!doc_.IsObject())
        return;

    rapidjson::Document::AllocatorType &allocator = doc_.GetAllocator();

    rapidjson::Value jsonKey;
    jsonKey.SetString(key.c_str(), key.length(), allocator);

    tpJsonObject &tmpJsonValue = const_cast<tpJsonObject &>(value);

    // 已有key值则覆盖
    if (doc_.HasMember(key.c_str()))
    {
        // 获取 "name" 键的迭代器
        rapidjson::Document::MemberIterator iter = doc_.FindMember(key.c_str());
        if (iter != doc_.MemberEnd())
        {
            // 覆盖已有的键值
            rapidjson::Value &jsonValue = tmpJsonValue.doc_;
            iter->value = jsonValue;
        }
    }
    else
    {
        doc_.AddMember(jsonKey, rapidjson::Value(tmpJsonValue.doc_, tmpJsonValue.doc_.GetAllocator()).Move(), allocator);
    }
}

void tpJsonObject::insert(const tpString &key, const tpJsonArray &value)
{
    if (!doc_.IsObject())
        return;

    rapidjson::Document::AllocatorType &allocator = doc_.GetAllocator();

    rapidjson::Value jsonKey;
    jsonKey.SetString(key.c_str(), key.length(), allocator);

    tpJsonArray &tmpJsonValue = const_cast<tpJsonArray &>(value);

    // 已有key值则覆盖
    if (doc_.HasMember(key.c_str()))
    {
        // 获取 "name" 键的迭代器
        rapidjson::Document::MemberIterator iter = doc_.FindMember(key.c_str());
        if (iter != doc_.MemberEnd())
        {
            // 覆盖已有的键值
            rapidjson::Value &jsonValue = tmpJsonValue.doc_;
            iter->value = jsonValue;
        }
    }
    else
    {
        doc_.AddMember(jsonKey, rapidjson::Value(tmpJsonValue.doc_, tmpJsonValue.doc_.GetAllocator()).Move(), allocator);
    }
}

tpJsonValue tpJsonObject::value(const tpString &key)
{
    if (!doc_.IsObject())
        return tpJsonValue();

    if (!doc_.HasMember(key.c_str()))
        return tpJsonValue();

    rapidjson::Value &jsonValue = doc_[key.c_str()];

    // 数据拷贝一份
    rapidjson::Document tmpJsonDoc;
    tmpJsonDoc.CopyFrom(jsonValue, tmpJsonDoc.GetAllocator());

    tpJsonValue tmpJsonValue;
    tmpJsonValue.value_.Swap(tmpJsonDoc);

    return tmpJsonValue;
}

void tpJsonObject::remove(const tpString &key)
{
    if (!doc_.IsObject())
        return;

    if (!doc_.HasMember(key.c_str()))
        return;

    doc_.RemoveMember(key.c_str());
}

tpJsonObject &tpJsonObject::operator=(const tpJsonObject &others)
{
    doc_.CopyFrom(others.doc_, doc_.GetAllocator());

    return *this;
}
