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

    /// @brief 获取容器内是否存在某个值
    /// @param value 值
    /// @return 存在返回true，否则返回false
    bool contains(const T &value);
    /// @brief 移除指定索引的值
    /// @param i 索引
    void remove(uint32_t i);

    /// @brief 在指定索引处插入值
    /// @param i 索引
    /// @param value 插入值
    void insertData(uint32_t i, const T &value);
    /// @brief 容器是否为空
    /// @return 为空返回true，否则返回false
    bool isEmpty() const;

    /// @brief 添加单个元素到向量末尾
    /// @param value 要添加的元素
    void append(const T &value) { this->emplace_back(value); }

    /// @brief 添加多个元素到向量末尾
    /// @param list 初始化列表
    void append(std::initializer_list<T> list) { this->insert(this->end(), list.begin(), list.end()); }

    /// @brief 添加另一个向量的所有元素到当前向量末尾
    /// @param other 另一个向量
    void append(const TpVector<T> &other) { this->insert(this->end(), other.begin(), other.end()); }
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
