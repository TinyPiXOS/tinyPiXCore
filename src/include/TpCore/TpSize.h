#ifndef __TP_SIZE_H
#define __TP_SIZE_H

#include "TpCore.h"

/// @brief 尺寸处理工具类
class TpSize
{
public:
    /// @brief 默认构造函数
    TpSize() noexcept;
    /// @brief 拷贝构造
    /// @param other 拷贝对象
    TpSize(const TpSize &other) noexcept;
    /// @brief 带参数的构造函数
    /// @param w 宽度
    /// @param h 高度
    TpSize(int32_t w, int32_t h) noexcept;
    /// @brief 析构函数
    ~TpSize();

    /// @brief 判断尺寸是否为空（宽高均为0）
    /// @return true-空尺寸，false-非空尺寸
    bool isNull() const noexcept;

    /// @brief 判断尺寸是否无效（宽或高为负）
    /// @return true-无效尺寸，false-有效尺寸
    bool isEmpty() const noexcept;

    /// @brief 判断尺寸是否有效（宽高均为非负）
    /// @return true-有效尺寸，false-无效尺寸
    bool isValid() const noexcept;

    /// @brief 获取宽度
    /// @return 宽度值
    int32_t width() const noexcept;

    /// @brief 获取高度
    /// @return 高度值
    int32_t height() const noexcept;

    ///  @brief 设置宽度
    ///  @param w 新的宽度值
    void setWidth(int32_t w) noexcept;

    /// @brief 设置高度
    /// @param h 新的高度值
    void setHeight(int32_t h) noexcept;

    /// @brief 返回交换宽高后的尺寸
    /// @return 交换后的尺寸
    TpSize transposed() const noexcept;

    /// @brief 扩展当前尺寸至指定尺寸（取宽高的最大值）
    /// @param otherSize 指定尺寸
    /// @return 扩展后的尺寸
    TpSize expandedTo(const TpSize &otherSize) const noexcept;

    /// @brief 限制当前尺寸至指定尺寸（取宽高的最小值）
    /// @param otherSize 指定尺寸
    /// @return 限制后的尺寸
    TpSize boundedTo(const TpSize &otherSize) const noexcept;

    /// @brief 获取宽度的引用（可修改）
    /// @return 宽度的引用
    int32_t &rwidth() noexcept;

    /// @brief 获取高度的引用（可修改）
    /// @return 高度的引用
    int32_t &rheight() noexcept;

    /// @brief 拷贝运算符
    /// @param others 要拷贝的对象
    /// @return 当前引用
    const TpSize &operator=(const TpSize &other) noexcept;

    /// @brief 尺寸加法赋值
    /// @param other 要加的尺寸
    /// @return 当前尺寸的引用
    TpSize &operator+=(const TpSize &other) noexcept;

    /// @brief 尺寸减法赋值
    /// @param other 要减的尺寸
    /// @return 当前尺寸的引用
    TpSize &operator-=(const TpSize &other) noexcept;

    /// @brief 尺寸乘以浮点数因子（赋值）
    /// @param c 因子
    /// @return 当前尺寸的引用
    TpSize &operator*=(float c) noexcept;

    /// @brief 尺寸除以浮点数（赋值）
    /// @param c 除数
    /// @return 当前尺寸的引用
    TpSize &operator/=(float c);

    friend bool operator==(const TpSize &, const TpSize &) noexcept;
    friend bool operator!=(const TpSize &, const TpSize &) noexcept;
    friend const TpSize operator+(const TpSize &, const TpSize &) noexcept;
    friend const TpSize operator-(const TpSize &, const TpSize &) noexcept;
    friend const TpSize operator*(const TpSize &, float) noexcept;
    friend const TpSize operator*(float, const TpSize &) noexcept;
    friend const TpSize operator/(const TpSize &, float);

private:
    tpInt32 w_, h_;
};

#endif