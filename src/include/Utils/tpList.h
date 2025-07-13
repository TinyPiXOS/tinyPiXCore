/***
 * @Author: hywang
 * @Date: 2024-05-27 18:02:13
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-12 15:28:58
 * @FilePath: /pix-singlegui/PixSingleGUI/include/Utils/tpList.h
 * @Description: std::list 封装
 * @
 * @PiXOS
 */

#ifndef __TP_LIST_H
#define __TP_LIST_H

#include <list>
#include "tpGlobal.h"

template <typename T>
class tpList : public std::list<T>
{
public:
    tpList() {}
    tpList(std::initializer_list<T> list);

    const T &at(int32_t i) const;
    const T &operator[](int32_t i) const;
    T &operator[](int32_t i);

    void append(const T &value);
    void append(const tpList<T> &value);
    void prepend(const T &value);
    void insertData(int32_t i, const T &value);

    bool contains(const T &value);

    void remove(int32_t i);
    void remove(const T &value);
};

template <typename T>
inline tpList<T>::tpList(std::initializer_list<T> list)
{
    for (const auto &value : list)
    {
        this->emplace_back(value);
    }
}

template <typename T>
inline const T &tpList<T>::at(int32_t i) const
{
    TP_ASSERT(i >= 0 && i < this->size(), "tpList<T>::at, index out of range");

    auto iter = this->begin();
    std::advance(iter, i);

    return *iter;
}

template <typename T>
inline const T &tpList<T>::operator[](int32_t i) const
{
    TP_ASSERT(i >= 0 && i < this->size(), "tpList<T>::at, index out of range");

    auto iter = this->begin();
    std::advance(iter, i);

    return *iter;
}

template <typename T>
inline T &tpList<T>::operator[](int32_t i)
{
    TP_ASSERT(i >= 0 && i < this->size(), "tpList<T>::at, index out of range");

    auto iter = this->begin();
    std::advance(iter, i);

    return *iter;
}

template <typename T>
inline void tpList<T>::insertData(int32_t i, const T &value)
{
    if (i > this->size())
        i = this->size();

    auto it = this->begin();
    std::advance(it, i);
    this->insert(it, value);
}

template <typename T>
inline void tpList<T>::append(const T &value)
{
    this->emplace_back(value);
}

template <typename T>
inline void tpList<T>::append(const tpList<T> &value)
{
    this->emplace_back(value);
}

template <typename T>
inline void tpList<T>::prepend(const T &value)
{
    this->emplace_front(value);
}

template <typename T>
inline void tpList<T>::remove(int32_t i)
{
    auto iter = this->begin();
    std::advance(iter, i);

    this->erase(iter);
}

template <typename T>
inline bool tpList<T>::contains(const T &value)
{
    for (int32_t i =0;i < this->size(); ++i)
    {
        if (this->at(i) == value)
            return true;
    }

    return false;
}

template <typename T>
inline void tpList<T>::remove(const T &value)
{
    std::list<T>::remove(value);
}

#endif
