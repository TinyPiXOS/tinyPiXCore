/***
 * @Author: hywang
 * @Date: 2024-08-09 09:49:04
 * @LastEditors: hywang
 * @LastEditTime: 2024-08-09 09:50:38
 * @FilePath: /pix-singlegui/PixSingleGUI/include/Utils/tpHash.h
 * @Description: 哈希map
 * @
 * @PiXOS
 */
#ifndef __TP_HASH_H
#define __TP_HASH_H

#include <unordered_map>

template <typename Key, typename Value>
class tpHash : public std::unordered_map<Key, Value>
{
public:
    bool contains(const Key &key) const;

    tpHash &operator=(const tpHash &value)
    {
        std::unordered_map<Key, Value>::operator=(value);
        // *this = value;
        return *this;
    }

    Value value(const Key key);
};

template <typename Key, typename Value>
inline bool tpHash<Key, Value>::contains(const Key &key) const
{
    return this->find(key) != this->end();
}

template <typename Key, typename Value>
inline Value tpHash<Key, Value>::value(const Key key)
{
    auto findIter = this->find(key);
    if (findIter == this->end())
        return Value();
    return findIter->second;
}

#endif
