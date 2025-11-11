#ifndef __TP_POINT_H
#define __TP_POINT_H

#include "TpCore.h"

TP_DEF_VOID_TYPE_VAR(ITpPointData);
/// @brief 点坐标处理工具类
class TpPoint
{
public:
    /// @brief 默认构造函数
    TpPoint();
    /// @brief 拷贝构造
    /// @param other 点信息
    TpPoint(const TpPoint& other);
    /// @brief 带坐标参数的构造函数
    /// @param xpos x坐标值
    /// @param ypos y坐标值
    TpPoint(int32_t xpos, int32_t ypos);
    /// @brief 析构函数
    ~TpPoint();

    /// @brief 判断点是否为空（即(0,0)）
    /// @return true-空点，false-非空点
    bool isNull() const;

    /// @brief 获取x坐标
    /// @return x坐标值
    int32_t x() const;

    /// @brief 获取y坐标
    /// @return y坐标值
    int32_t y() const;

    /// @brief 设置x坐标
    /// @param x 新的x坐标值
    void setX(int32_t x);

    /// @brief 设置y坐标
    /// @param y 新的y坐标值
    void setY(int32_t y);

    /// @brief 计算曼哈顿长度（|x|+|y|）
    /// @return 曼哈顿长度
    int32_t manhattanLength() const;

    /// @brief 交换x和y坐标
    /// @return 交换后的点
    TpPoint transposed() const noexcept;

    /// @brief 获取x坐标的引用（可修改）
    /// @return x坐标的引用
    int32_t &rx();

    /// @brief 获取y坐标的引用（可修改）
    /// @return y坐标的引用
    int32_t &ry();

    /// @brief 拷贝运算符
    /// @param others 要拷贝的对象
    /// @return 当前点的引用
    const TpPoint &operator=(const TpPoint &p);

    /// @brief 点加法赋值
    /// @param p 要加的点
    /// @return 当前点的引用
    TpPoint &operator+=(const TpPoint &p);

    /// @brief 点减法赋值
    /// @param p 要减的点
    /// @return 当前点的引用
    TpPoint &operator-=(const TpPoint &p);

    /// @brief 点乘以浮点数因子（赋值）
    /// @param factor 因子
    /// @return 当前点的引用
    TpPoint &operator*=(float factor);

    /// @brief 点乘以双精度浮点数因子（赋值）
    /// @param factor 因子
    /// @return 当前点的引用
    TpPoint &operator*=(double factor);

    /// @brief 点乘以整数因子（赋值）
    /// @param factor 因子
    /// @return 当前点的引用
    TpPoint &operator*=(int32_t factor);

    /// @brief 点除以浮点数（赋值）
    /// @param divisor 除数
    /// @return 当前点的引用
    TpPoint &operator/=(float divisor);

    /// @brief 点除以双精度浮点数（赋值）
    /// @param divisor 除数
    /// @return 当前点的引用
    TpPoint &operator/=(double divisor);

    /// @brief 点除以整数（赋值）
    /// @param divisor 除数
    /// @return 当前点的引用
    TpPoint &operator/=(int32_t divisor);

    /// @brief 点积（静态函数）
    /// @param p1 第一个点
    /// @param p2 第二个点
    /// @return 点积结果
    static int32_t dotProduct(const TpPoint &p1, const TpPoint &p2);

    friend bool operator==(const TpPoint &, const TpPoint &);
    friend bool operator!=(const TpPoint &, const TpPoint &);
    friend const TpPoint operator+(const TpPoint &, const TpPoint &);
    friend const TpPoint operator-(const TpPoint &, const TpPoint &);
    friend const TpPoint operator*(const TpPoint &, float);
    friend const TpPoint operator*(float, const TpPoint &);
    friend const TpPoint operator*(const TpPoint &, double);
    friend const TpPoint operator*(double, const TpPoint &);
    friend const TpPoint operator*(const TpPoint &, int32_t);
    friend const TpPoint operator*(int32_t, const TpPoint &);
    friend const TpPoint operator+(const TpPoint &);
    friend const TpPoint operator-(const TpPoint &);
    friend const TpPoint operator/(const TpPoint &, float);
    friend const TpPoint operator/(const TpPoint &, double);
    friend const TpPoint operator/(const TpPoint &, int32_t);

private:
    ITpPointData *data_;
};

#endif