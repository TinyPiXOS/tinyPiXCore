/***
 * @Author: hywang
 * @Date: 2024-06-03 15:06:03
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-20 10:09:38
 * @FilePath: /pix-singlegui/PixSingleGUI/include/Utils/TpVector.h
 * @Description:
 * @
 * @PiXOS
 */
#ifndef __TP_VECTPR_H
#define __TP_VECTPR_H

#include <vector>
#include <algorithm>
#include <cstdint>

template <typename T>
class TpVector : public std::vector<T>
{
public:
    TpVector() = default;
    // 初始化列表构造函数
    TpVector(std::initializer_list<T> initList) : std::vector<T>(initList) {}
    // 拷贝构造函数
    TpVector(const TpVector &other) : std::vector<T>(other) {}
    // 移动构造函数
    TpVector(TpVector &&other) noexcept : std::vector<T>(std::move(other)) {}

    // 赋值运算符
    TpVector &operator=(const TpVector &other)
    {
        std::vector<T>::operator=(other);
        return *this;
    }

    // 移动赋值运算符
    TpVector &operator=(TpVector &&other) noexcept
    {
        std::vector<T>::operator=(std::move(other));
        return *this;
    }

    bool contains(const T &value);
    void remove(uint32_t i);

    void insertData(uint32_t i, const T &value);
    bool isEmpty() const;
};

template <typename T>
inline bool TpVector<T>::contains(const T &value)
{
    auto findIter = std::find(this->begin(), this->end(), value);
    if (findIter == this->end())
        return false;

    return true;
}

template <typename T>
inline void TpVector<T>::remove(uint32_t i)
{
    if (i >= this->size())
        return;

    uint32_t index = 0;
    for (auto iter = this->begin(); iter != this->end(); ++iter)
    {
        if (index == i)
        {
            this->erase(iter);
            break;
        }
        ++index;
    }
}

template <typename T>
inline void TpVector<T>::insertData(uint32_t i, const T &value)
{
    if (i < 0)
        return;
    if (i >= this->size())
        this->emplace_back(value);
    else
        this->insert(this->begin() + i, value);
}

template <typename T>
inline bool TpVector<T>::isEmpty() const
{
    return this->empty();
}

#endif
