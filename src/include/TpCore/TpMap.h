#ifndef __TP_MAP_H
#define __TP_MAP_H

#include <map>
#include <initializer_list>
#include "TpList.h"

template <typename Key, typename Value>
class TpMap : public std::map<Key, Value>
{
public:
    // 添加初始化列表构造函数
    TpMap(std::initializer_list<std::pair<const Key, Value>> initList)
        : std::map<Key, Value>(initList) {}

    TpMap() = default;
    TpMap(const TpMap<Key, Value> &others) = default;
    TpMap(TpMap<Key, Value> &&others) noexcept = default;

    /// @brief 赋值运算符
    TpMap &operator=(const TpMap &) = default;
    /// @brief 赋值运算符
    TpMap &operator=(TpMap &&) noexcept = default;

    /// @brief 启用索引操作符
    using std::map<Key, Value>::operator[];

    /// @brief 检查键是否存在
    bool contains(const Key &key) const;

    /// @brief 查找值对应的第一个键
    const Key &key(const Value &value, const Key &defaultValue = Key()) const;

    /// @brief 获取所有键
    TpList<Key> keys() const;

    /// @brief 获取键对应的值
    const Value &value(const Key &key, const Value &defaultValue = Value()) const;

    /// @brief 获取所有值
    TpList<Value> values() const;
};

// 实现部分（保持不变）
template <typename Key, typename Value>
inline bool TpMap<Key, Value>::contains(const Key &key) const
{
    return this->find(key) != this->end();
}

template <typename Key, typename Value>
inline const Key &TpMap<Key, Value>::key(const Value &value, const Key &defaultValue) const
{
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        if (value == it->second)
            return it->first;
    }
    return defaultValue;
}

template <typename Key, typename Value>
inline TpList<Key> TpMap<Key, Value>::keys() const
{
    TpList<Key> keyList;
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        keyList.append(it->first);
    }
    return keyList;
}

template <typename Key, typename Value>
inline const Value &TpMap<Key, Value>::value(const Key &key, const Value &defaultValue) const
{
    auto findIter = this->find(key);
    return (findIter == this->end()) ? defaultValue : findIter->second;
}

template <typename Key, typename Value>
inline TpList<Value> TpMap<Key, Value>::values() const
{
    TpList<Value> valueList;
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        valueList.append(it->second);
    }
    return valueList;
}

#endif