#ifndef __TP_STACK_H
#define __TP_STACK_H

#include "tpVector.h"
#include <cstdint>

/// @brief 栈数据处理类
template <typename T>
class tpStack
{
public:
    tpStack();
    tpStack(const tpStack &others);

    /// @brief 入栈一个数据
    /// @param value 数据
    void push(const T &value);

    /// @brief 出栈一个数据，会移除该数据
    /// @return 栈顶数据
    T pop();

    /// @brief 获取栈顶数据，获取并不会移除栈顶数据
    /// @return 栈顶数据
    T top();

    /// @brief 从栈底出栈一个数据，会移除该数据，该接口不建议频繁使用，效率较低
    /// @return 栈底数据
    T popFont();

    /// @brief 获取栈底数据
    /// @return 栈底数据
    T font();

    /// @brief 获取栈大小
    /// @return 栈大小
    uint32_t size();

    /// @brief 栈是否为空
    /// @return 为空返回true，否则返回false
    bool isEmpty();

private:
    tpVector<T> data_;
};

template <typename T>
inline tpStack<T>::tpStack()
{
}

template <typename T>
inline tpStack<T>::tpStack(const tpStack &others)
    : data_(others.data_)
{
}

template <typename T>
inline void tpStack<T>::push(const T &value)
{
    data_.emplace_back(value);
}

template <typename T>
inline T tpStack<T>::pop()
{
    if (isEmpty())
        return T();
    auto backValue = data_.back();
    data_.pop_back();
    return backValue;
}

template <typename T>
inline T tpStack<T>::top()
{
    if (isEmpty())
        return T();

    return data_.back();
}

template <typename T>
inline T tpStack<T>::popFont()
{
    if (isEmpty())
        return T();

    auto fontData = data_.front();
    data_.erase(data_.begin());
    return fontData;
}

template <typename T>
inline T tpStack<T>::font()
{
    if (isEmpty())
        return T();

    return data_.front();
}

template <typename T>
inline uint32_t tpStack<T>::size()
{
    return data_.size();
}

template <typename T>
inline bool tpStack<T>::isEmpty()
{
    return (data_.size() == 0) ? true : false;
}

#endif
