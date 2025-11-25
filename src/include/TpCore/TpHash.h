#ifndef __TP_HASH_H
#define __TP_HASH_H

#include <unordered_map>
#include "TpList.h"

template <typename Key, typename Value>
class TpHash : public std::unordered_map<Key, Value>
{
public:
    using std::unordered_map<Key, Value>::operator[]; // 启用赋值操作

    TpHash() = default;
    TpHash(const TpHash<Key, Value> &others) = default;
    TpHash(TpHash<Key, Value> &&others) noexcept = default;

    /// @brief 获取Map中是否包含某键值
    bool contains(const Key &key) const;

    /// @brief 赋值运算符（简化）
    TpHash &operator=(const TpHash &value) = default;

    /// @brief 移动赋值运算符
    TpHash &operator=(TpHash &&other) noexcept = default;

    /// @brief 根据属性获取第一个匹配成功的键值
    const Key &key(const Value &value, const Key &defaultValue = Key()) const;

    /// @brief 获取所有键值
    TpList<Key> keys() const;

    /// @brief 根据键值获取属性
    const Value &value(const Key &key, const Value &defaultValue = Value()) const;

    /// @brief 获取所有value
    TpList<Value> values() const;
};

template <typename Key, typename Value>
inline bool TpHash<Key, Value>::contains(const Key &key) const
{
    return this->find(key) != this->end();
}

template <typename Key, typename Value>
inline const Key & TpHash<Key, Value>::key(const Value &value, const Key &defaultValue) const
{
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        if (value == it->second)
            return it->first;
    }
    return defaultValue;
}

template <typename Key, typename Value>
inline TpList<Key> TpHash<Key, Value>::keys() const
{
    TpList<Key> keyList;
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        keyList.append(it->first);
    }
    return keyList;
}

template <typename Key, typename Value>
inline const Value &TpHash<Key, Value>::value(const Key &key, const Value &defaultValue) const
{
    auto findIter = this->find(key);
    return (findIter == this->end()) ? defaultValue : findIter->second;
}

template <typename Key, typename Value>
inline TpList<Value> TpHash<Key, Value>::values() const
{
    TpList<Value> valueList;
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        valueList.append(it->second);
    }
    return valueList;
}

#endif