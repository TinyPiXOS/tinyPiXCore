/*** 
 * @Author: hywang
 * @Date: 2024-06-03 15:06:03
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-20 10:09:38
 * @FilePath: /pix-singlegui/PixSingleGUI/include/Utils/tpVector.h
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
class tpVector : public std::vector<T>
{
public:
    bool contains(const T &value);
    void remove(uint32_t i);

    void insertData(uint32_t i, const T &value);
};

template <typename T>
inline bool tpVector<T>::contains(const T &value)
{
    auto findIter = std::find(this->begin(), this->end(), value);
    if (findIter == this->end())
        return false;

    return true;
}

template <typename T>
inline void tpVector<T>::remove(uint32_t i)
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
inline void tpVector<T>::insertData(uint32_t i, const T &value)
{
    if (i < 0)
        return;
    if (i >= this->size())
        this->emplace_back(value);
    else
        this->insert(this->begin() + i, value);
}

#endif
