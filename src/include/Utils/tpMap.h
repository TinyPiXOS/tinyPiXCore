#ifndef __TP_MAP_H
#define __TP_MAP_H

#include <map>
#include "tpList.h"

template <typename Key, typename Value>
class tpMap : public std::map<Key, Value>
{
public:
    tpMap() = default;
    tpMap(const tpMap<Key, Value> &others) = default;
    tpMap(tpMap<Key, Value> &&others) noexcept = default;

    // 赋值运算符
    tpMap &operator=(const tpMap &) = default;
    tpMap &operator=(tpMap &&) noexcept = default;

    // 启用索引操作符
    using std::map<Key, Value>::operator[];

    /// @brief 检查键是否存在
    bool contains(const Key &key) const;

    /// @brief 查找值对应的第一个键
    const Key &key(const Value &value, const Key &defaultValue = Key()) const;

    /// @brief 获取所有键
    tpList<Key> keys() const;

    /// @brief 获取键对应的值
    const Value &value(const Key &key, const Value &defaultValue = Value()) const;

    /// @brief 获取所有值
    tpList<Value> values() const;
};

// 实现部分
template <typename Key, typename Value>
inline bool tpMap<Key, Value>::contains(const Key &key) const
{
    return this->find(key) != this->end();
}

template <typename Key, typename Value>
inline const Key &tpMap<Key, Value>::key(const Value &value, const Key &defaultValue) const
{
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        if (value == it->second)
            return it->first;
    }
    return defaultValue;
}

template <typename Key, typename Value>
inline tpList<Key> tpMap<Key, Value>::keys() const
{
    tpList<Key> keyList;
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        keyList.append(it->first);
    }
    return keyList;
}

template <typename Key, typename Value>
inline const Value &tpMap<Key, Value>::value(const Key &key, const Value &defaultValue) const
{
    auto findIter = this->find(key);
    return (findIter == this->end()) ? defaultValue : findIter->second;
}

template <typename Key, typename Value>
inline tpList<Value> tpMap<Key, Value>::values() const
{
    tpList<Value> valueList;
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        valueList.append(it->second);
    }
    return valueList;
}

#endif
