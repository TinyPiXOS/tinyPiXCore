#ifndef __TP_RECTF_H
#define __TP_RECTF_H

#include "TpCore.h"
#include "TpSizeF.h"
#include "TpPointF.h"

TP_DEF_VOID_TYPE_VAR(ITpRectFData);
/// @brief 矩形区域处理工具类
class TpRectF
{
public:
    /// @brief 默认构造函数
    TpRectF();
    /// @brief 拷贝构造函数
    /// @param other 源矩形对象
    TpRectF(const TpRectF &);
    /// @brief 通过两个点构造矩形
    /// @param leftTop 左上角坐标
    /// @param rightBottom 右下角坐标
    TpRectF(const TpPointF &leftTop, const TpPointF &rightBottom);
    /// @brief 通过点和尺寸构造矩形
    /// @param leftTop 左上角坐标
    /// @param size 矩形尺寸
    TpRectF(const TpPointF &leftTop, const TpSizeF &size);
    /// @brief 通过点和尺寸构造矩形
    /// @param x 左上角坐标X
    /// @param y 左上角坐标Y
    /// @param w 宽度
    /// @param h 高度
    TpRectF(double x, double y, double w, double h);

    /// @brief 析构函数
    virtual ~TpRectF();

public:
    /// @brief 判断矩形是否为空（宽高均为0）
    /// @return true-空矩形，false-非空矩形
    bool isNull() const noexcept;

    /// @brief 判断矩形是否无效（宽或高为负）
    /// @return true-无效矩形，false-有效矩形
    bool isEmpty() const noexcept;

    /// @brief 判断矩形是否有效（宽高均为正数）
    /// @return true-有效矩形，false-无效矩形
    bool isValid() const noexcept;

    /// @brief 获取矩形左边界
    /// @return 左边界x坐标
    double left() const noexcept;

    /// @brief 获取矩形上边界
    /// @return 上边界y坐标
    double top() const noexcept;

    /// @brief 获取矩形右边界
    /// @return 右边界x坐标
    double right() const noexcept;

    /// @brief 获取矩形下边界
    /// @return 下边界y坐标
    double bottom() const noexcept;

    /// @brief 获取矩形左上角x坐标
    /// @return 左上角x坐标
    double x() const noexcept;

    /// @brief 获取矩形左上角y坐标
    /// @return 左上角y坐标
    double y() const noexcept;

    /// @brief 设置矩形左边界
    /// @param pos 新的左边界x坐标
    void setLeft(double pos) noexcept;

    /// @brief 设置矩形上边界
    /// @param pos 新的上边界y坐标
    void setTop(double pos) noexcept;

    /// @brief 设置矩形右边界
    /// @param pos 新的右边界x坐标
    void setRight(double pos) noexcept;

    /// @brief 设置矩形下边界
    /// @param pos 新的下边界y坐标
    void setBottom(double pos) noexcept;

    /// @brief 设置矩形左上角x坐标
    /// @param x 新的左上角x坐标
    void setX(double x) noexcept;

    /// @brief 设置矩形左上角y坐标
    /// @param y 新的左上角y坐标
    void setY(double y) noexcept;

    /// @brief 设置矩形左上角坐标
    /// @param p 新的左上角坐标点
    void setTopLeft(const TpPointF &p) noexcept;

    /// @brief 设置矩形右下角坐标
    /// @param p 新的右下角坐标点
    void setBottomRight(const TpPointF &p) noexcept;

    /// @brief 设置矩形右上角坐标
    /// @param p 新的右上角坐标点
    void setTopRight(const TpPointF &p) noexcept;

    /// @brief 设置矩形左下角坐标
    /// @param p 新的左下角坐标点
    void setBottomLeft(const TpPointF &p) noexcept;

    /// @brief 获取矩形左上角坐标
    /// @return 左上角坐标点
    TpPointF topLeft() const noexcept;

    /// @brief 获取矩形右下角坐标
    /// @return 右下角坐标点
    TpPointF bottomRight() const noexcept;

    /// @brief 获取矩形右上角坐标
    /// @return 右上角坐标点
    TpPointF topRight() const noexcept;

    /// @brief 获取矩形左下角坐标
    /// @return 左下角坐标点
    TpPointF bottomLeft() const noexcept;

    /// @brief 获取矩形中心点坐标
    /// @return 中心点坐标
    TpPointF center() const noexcept;

    /// @brief 设置矩形位置和尺寸
    /// @param x 左上角x坐标
    /// @param y 左上角y坐标
    /// @param w 矩形宽度
    /// @param h 矩形高度
    void setRect(double x, double y, double w, double h) noexcept;

    /// @brief 获取矩形位置和尺寸
    /// @param x 输出左上角x坐标
    /// @param y 输出左上角y坐标
    /// @param w 输出矩形宽度
    /// @param h 输出矩形高度
    void getRect(double *x, double *y, double *w, double *h) const;

    /// @brief 设置矩形对角坐标
    /// @param x1 左上角x坐标
    /// @param y1 左上角y坐标
    /// @param x2 右下角x坐标
    /// @param y2 右下角y坐标
    void setCoords(double x1, double y1, double x2, double y2) noexcept;

    /// @brief 获取矩形对角坐标
    /// @param x1 输出左上角x坐标
    /// @param y1 输出左上角y坐标
    /// @param x2 输出右下角x坐标
    /// @param y2 输出右下角y坐标
    void getCoords(double *x1, double *y1, double *x2, double *y2) const;

    /// @brief 获取矩形尺寸
    /// @return 矩形尺寸对象
    TpSizeF size() const noexcept;

    /// @brief 获取矩形宽度
    /// @return 矩形宽度
    double width() const noexcept;

    /// @brief 获取矩形高度
    /// @return 矩形高度
    double height() const noexcept;

    /// @brief 设置矩形宽度
    /// @param w 新的宽度
    void setWidth(double w) noexcept;

    /// @brief 设置矩形高度
    /// @param h 新的高度
    void setHeight(double h) noexcept;

    /// @brief 设置矩形尺寸
    /// @param s 新的尺寸对象
    void setSize(const TpSizeF &s) noexcept;

public:
    /// @brief 判断点是否在矩形内
    /// @param x 点的x坐标
    /// @param y 点的y坐标
    /// @return true-点在矩形内，false-点不在矩形内
    virtual bool contains(double x, double y);

    /// @brief 判断点是否在矩形内
    /// @param point 点坐标
    /// @return true-点在矩形内，false-点不在矩形内
    virtual bool contains(const TpPointF &);

public:
    /// @brief 判断两个区域是否相交
    /// @param rect 另一个矩形
    /// @return true-相交，false-不相交
    virtual bool intersect(const TpRectF &);

    /// @brief 判断矩形是否与指定区域相交
    /// @param x 区域左上角x坐标
    /// @param y 区域左上角y坐标
    /// @param w 区域宽度
    /// @param h 区域高度
    /// @return true-相交，false-不相交
    virtual bool intersect(double x, double y, double w, double h);

public:
    /// @brief 将另一个矩形合并到当前矩形
    /// @param rect 要合并的矩形
    /// @return true-合并成功，false-合并失败
    virtual bool unions(const TpRectF &);

    /// @brief 将指定区域合并到当前矩形
    /// @param x 区域左上角x坐标
    /// @param y 区域左上角y坐标
    /// @param w 区域宽度
    /// @param h 区域高度
    /// @return true-合并成功，false-合并失败
    virtual bool unions(double x, double y, double w, double h);

public:
    /// @brief 赋值运算符重载
    /// @param other 源矩形对象
    /// @return 当前矩形对象的引用
    TpRectF operator=(const TpRectF &other);

    /// @brief 相等运算符重载
    /// @param other 比较的矩形对象
    /// @return true-两个矩形相等，false-不相等
    bool operator==(const TpRectF &other);

    /// @brief 不等运算符重载
    /// @param other 比较的矩形对象
    /// @return true-两个矩形不相等，false-相等
    bool operator!=(const TpRectF &other);

private:
    ITpRectFData *data_;
};

#endif