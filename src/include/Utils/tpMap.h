// /***
//  * @Author: hywang
//  * @Date: 2024-05-27 11:35:14
//  * @LastEditors: hywang
//  * @LastEditTime: 2024-05-27 11:42:18
//  * @FilePath: /pix-singlegui/PixSingleGUI/include/Utils/tpMap.h
//  * @Description: std::map的封装
//  * @
//  * @PiXOS
//  */

#ifndef __TP_MAP_H
#define __TP_MAP_H

#include <map>

template <typename Key, typename Value>
class tpMap : public std::map<Key, Value>
{
public:
    bool contains(const Key &key);

    tpMap &operator=(const tpMap &value)
    {
        std::map<Key, Value>::operator=(value);
        // *this = value;
        return *this;
    }

    Value value(const Key key);
};

template <typename Key, typename Value>
inline bool tpMap<Key, Value>::contains(const Key &key)
{
    auto findIter = this->find(key);
    if (findIter == this->end())
        return false;
    return true;
}

#endif
