#ifndef __TP_POINTF_H
#define __TP_POINTF_H

#include "TpCore.h"

/// @brief 点坐标处理工具类
class TpPointF
{
public:
    /// @brief 默认构造函数
    TpPointF();
    /// @brief 拷贝构造
    /// @param other 拷贝的点信息
    TpPointF(const TpPointF &other);
    /// @brief 带坐标参数的构造函数
    /// @param xpos x坐标值
    /// @param ypos y坐标值
    TpPointF(double xpos, double ypos);
    /// @brief 析构函数
    ~TpPointF();

    /// @brief 判断点是否为空（即(0,0)）
    /// @return true-空点，false-非空点
    bool isNull() const;

    /// @brief 获取x坐标
    /// @return x坐标值
    double x() const;

    /// @brief 获取y坐标
    /// @return y坐标值
    double y() const;

    /// @brief 设置x坐标
    /// @param x 新的x坐标值
    void setX(double x);

    /// @brief 设置y坐标
    /// @param y 新的y坐标值
    void setY(double y);

    /// @brief 计算曼哈顿长度（|x|+|y|）
    /// @return 曼哈顿长度
    double manhattanLength() const;

    /// @brief 交换x和y坐标
    /// @return 交换后的点
    TpPointF transposed() const noexcept;

    /// @brief 获取x坐标的引用（可修改）
    /// @return x坐标的引用
    double &rx();

    /// @brief 获取y坐标的引用（可修改）
    /// @return y坐标的引用
    double &ry();

    /// @brief 拷贝运算符
    /// @param others 要拷贝的对象
    /// @return 当前点的引用
    const TpPointF &operator=(const TpPointF &p);

    /// @brief 点加法赋值
    /// @param p 要加的点
    /// @return 当前点的引用
    TpPointF &operator+=(const TpPointF &p);

    /// @brief 点减法赋值
    /// @param p 要减的点
    /// @return 当前点的引用
    TpPointF &operator-=(const TpPointF &p);

    /// @brief 点乘以浮点数因子（赋值）
    /// @param factor 因子
    /// @return 当前点的引用
    TpPointF &operator*=(float factor);

    /// @brief 点乘以双精度浮点数因子（赋值）
    /// @param factor 因子
    /// @return 当前点的引用
    TpPointF &operator*=(double factor);

    /// @brief 点乘以整数因子（赋值）
    /// @param factor 因子
    /// @return 当前点的引用
    TpPointF &operator*=(int32_t factor);

    /// @brief 点除以浮点数（赋值）
    /// @param divisor 除数
    /// @return 当前点的引用
    TpPointF &operator/=(float divisor);

    /// @brief 点除以双精度浮点数（赋值）
    /// @param divisor 除数
    /// @return 当前点的引用
    TpPointF &operator/=(double divisor);

    /// @brief 点除以整数（赋值）
    /// @param divisor 除数
    /// @return 当前点的引用
    TpPointF &operator/=(int32_t divisor);

    /// @brief 点积（静态函数）
    /// @param p1 第一个点
    /// @param p2 第二个点
    /// @return 点积结果
    static double dotProduct(const TpPointF &p1, const TpPointF &p2);

    friend bool operator==(const TpPointF &, const TpPointF &);
    friend bool operator!=(const TpPointF &, const TpPointF &);
    friend const TpPointF operator+(const TpPointF &, const TpPointF &);
    friend const TpPointF operator-(const TpPointF &, const TpPointF &);
    friend const TpPointF operator*(const TpPointF &, float);
    friend const TpPointF operator*(float, const TpPointF &);
    friend const TpPointF operator*(const TpPointF &, double);
    friend const TpPointF operator*(double, const TpPointF &);
    friend const TpPointF operator*(const TpPointF &, int32_t);
    friend const TpPointF operator*(int32_t, const TpPointF &);
    friend const TpPointF operator+(const TpPointF &);
    friend const TpPointF operator-(const TpPointF &);
    friend const TpPointF operator/(const TpPointF &, float);
    friend const TpPointF operator/(const TpPointF &, double);
    friend const TpPointF operator/(const TpPointF &, int32_t);

private:
    tpDouble x_, y_;
};

#endif