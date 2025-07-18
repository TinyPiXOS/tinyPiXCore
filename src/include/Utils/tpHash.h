#ifndef __TP_HASH_H
#define __TP_HASH_H

#include <unordered_map>
#include "tpList.h"

template <typename Key, typename Value>
class tpHash : public std::unordered_map<Key, Value>
{
public:
    using std::unordered_map<Key, Value>::operator[]; // 启用赋值操作

    tpHash() = default;
    tpHash(const tpHash<Key, Value> &others) = default;
    tpHash(tpHash<Key, Value> &&others) noexcept = default;

    /// @brief 获取Map中是否包含某键值
    bool contains(const Key &key) const;

    /// @brief 赋值运算符（简化）
    tpHash &operator=(const tpHash &value) = default;

    /// @brief 移动赋值运算符
    tpHash &operator=(tpHash &&other) noexcept = default;

    /// @brief 根据属性获取第一个匹配成功的键值
    const Key &key(const Value &value, const Key &defaultValue = Key()) const;

    /// @brief 获取所有键值
    tpList<Key> keys() const;

    /// @brief 根据键值获取属性
    const Value &value(const Key &key, const Value &defaultValue = Value()) const;

    /// @brief 获取所有value
    tpList<Value> values() const;
};

template <typename Key, typename Value>
inline bool tpHash<Key, Value>::contains(const Key &key) const
{
    return this->find(key) != this->end();
}

template <typename Key, typename Value>
inline const Key & tpHash<Key, Value>::key(const Value &value, const Key &defaultValue) const
{
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        if (value == it->second)
            return it->first;
    }
    return defaultValue;
}

template <typename Key, typename Value>
inline tpList<Key> tpHash<Key, Value>::keys() const
{
    tpList<Key> keyList; // 修复：返回Key列表而不是Value列表
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        keyList.append(it->first);
    }
    return keyList;
}

template <typename Key, typename Value>
inline const Value &tpHash<Key, Value>::value(const Key &key, const Value &defaultValue) const
{
    auto findIter = this->find(key);
    return (findIter == this->end()) ? defaultValue : findIter->second;
}

template <typename Key, typename Value>
inline tpList<Value> tpHash<Key, Value>::values() const
{
    tpList<Value> valueList;
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        valueList.append(it->second);
    }
    return valueList;
}

#endif