#ifndef __TP_RECT_H
#define __TP_RECT_H

#include "TpUtils.h"
#include "TpSize.h"
#include "TpPoint.h"

TP_DEF_VOID_TYPE_VAR(ITpRectData);
/// @brief 矩形区域处理工具类
class TpRect
{
public:
    /// @brief 默认构造函数
    TpRect();
    /// @brief 拷贝构造函数
    /// @param other 源矩形对象
    TpRect(const TpRect &);
    /// @brief 通过两个点构造矩形
    /// @param leftTop 左上角坐标
    /// @param rightBottom 右下角坐标
    TpRect(const TpPoint &leftTop, const TpPoint &rightBottom);
    /// @brief 通过点和尺寸构造矩形
    /// @param leftTop 左上角坐标
    /// @param size 矩形尺寸
    TpRect(const TpPoint &leftTop, const TpSize &size);
    /// @brief 通过点和尺寸构造矩形
    /// @param x 左上角坐标X
    /// @param y 左上角坐标Y
    /// @param w 宽度
    /// @param h 高度
    TpRect(int32_t x, int32_t y, int32_t w, int32_t h);

    /// @brief 析构函数
    virtual ~TpRect();

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
    int32_t left() const noexcept;

    /// @brief 获取矩形上边界
    /// @return 上边界y坐标
    int32_t top() const noexcept;

    /// @brief 获取矩形右边界
    /// @return 右边界x坐标
    int32_t right() const noexcept;

    /// @brief 获取矩形下边界
    /// @return 下边界y坐标
    int32_t bottom() const noexcept;

    /// @brief 获取矩形左上角x坐标
    /// @return 左上角x坐标
    int32_t x() const noexcept;

    /// @brief 获取矩形左上角y坐标
    /// @return 左上角y坐标
    int32_t y() const noexcept;

    /// @brief 设置矩形左边界
    /// @param pos 新的左边界x坐标
    void setLeft(int32_t pos) noexcept;

    /// @brief 设置矩形上边界
    /// @param pos 新的上边界y坐标
    void setTop(int32_t pos) noexcept;

    /// @brief 设置矩形右边界
    /// @param pos 新的右边界x坐标
    void setRight(int32_t pos) noexcept;

    /// @brief 设置矩形下边界
    /// @param pos 新的下边界y坐标
    void setBottom(int32_t pos) noexcept;

    /// @brief 设置矩形左上角x坐标
    /// @param x 新的左上角x坐标
    void setX(int32_t x) noexcept;

    /// @brief 设置矩形左上角y坐标
    /// @param y 新的左上角y坐标
    void setY(int32_t y) noexcept;

    /// @brief 设置矩形左上角坐标
    /// @param p 新的左上角坐标点
    void setTopLeft(const TpPoint &p) noexcept;

    /// @brief 设置矩形右下角坐标
    /// @param p 新的右下角坐标点
    void setBottomRight(const TpPoint &p) noexcept;

    /// @brief 设置矩形右上角坐标
    /// @param p 新的右上角坐标点
    void setTopRight(const TpPoint &p) noexcept;

    /// @brief 设置矩形左下角坐标
    /// @param p 新的左下角坐标点
    void setBottomLeft(const TpPoint &p) noexcept;

    /// @brief 获取矩形左上角坐标
    /// @return 左上角坐标点
    TpPoint topLeft() const noexcept;

    /// @brief 获取矩形右下角坐标
    /// @return 右下角坐标点
    TpPoint bottomRight() const noexcept;

    /// @brief 获取矩形右上角坐标
    /// @return 右上角坐标点
    TpPoint topRight() const noexcept;

    /// @brief 获取矩形左下角坐标
    /// @return 左下角坐标点
    TpPoint bottomLeft() const noexcept;

    /// @brief 获取矩形中心点坐标
    /// @return 中心点坐标
    TpPoint center() const noexcept;

    /// @brief 设置矩形位置和尺寸
    /// @param x 左上角x坐标
    /// @param y 左上角y坐标
    /// @param w 矩形宽度
    /// @param h 矩形高度
    void setRect(int32_t x, int32_t y, int32_t w, int32_t h) noexcept;

    /// @brief 获取矩形位置和尺寸
    /// @param x 输出左上角x坐标
    /// @param y 输出左上角y坐标
    /// @param w 输出矩形宽度
    /// @param h 输出矩形高度
    void getRect(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const;

    /// @brief 设置矩形对角坐标
    /// @param x1 左上角x坐标
    /// @param y1 左上角y坐标
    /// @param x2 右下角x坐标
    /// @param y2 右下角y坐标
    void setCoords(int32_t x1, int32_t y1, int32_t x2, int32_t y2) noexcept;

    /// @brief 获取矩形对角坐标
    /// @param x1 输出左上角x坐标
    /// @param y1 输出左上角y坐标
    /// @param x2 输出右下角x坐标
    /// @param y2 输出右下角y坐标
    void getCoords(int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2) const;

    /// @brief 获取矩形尺寸
    /// @return 矩形尺寸对象
    TpSize size() const noexcept;

    /// @brief 获取矩形宽度
    /// @return 矩形宽度
    int32_t width() const noexcept;

    /// @brief 获取矩形高度
    /// @return 矩形高度
    int32_t height() const noexcept;

    /// @brief 设置矩形宽度
    /// @param w 新的宽度
    void setWidth(int32_t w) noexcept;

    /// @brief 设置矩形高度
    /// @param h 新的高度
    void setHeight(int32_t h) noexcept;

    /// @brief 设置矩形尺寸
    /// @param s 新的尺寸对象
    void setSize(const TpSize &s) noexcept;

public:
    /// @brief 判断点是否在矩形内
    /// @param x 点的x坐标
    /// @param y 点的y坐标
    /// @return true-点在矩形内，false-点不在矩形内
    virtual bool contains(int32_t x, int32_t y);

    /// @brief 判断点是否在矩形内
    /// @param point 点坐标
    /// @return true-点在矩形内，false-点不在矩形内
    virtual bool contains(const TpPoint &);

public:
    /// @brief 判断两个区域是否相交
    /// @param rect 另一个矩形
    /// @return true-相交，false-不相交
    virtual bool intersect(const TpRect &);

    /// @brief 判断矩形是否与指定区域相交
    /// @param x 区域左上角x坐标
    /// @param y 区域左上角y坐标
    /// @param w 区域宽度
    /// @param h 区域高度
    /// @return true-相交，false-不相交
    virtual bool intersect(int32_t x, int32_t y, uint32_t w, uint32_t h);

public:
    /// @brief 将另一个矩形合并到当前矩形
    /// @param rect 要合并的矩形
    /// @return true-合并成功，false-合并失败
    virtual bool unions(const TpRect &);

    /// @brief 将指定区域合并到当前矩形
    /// @param x 区域左上角x坐标
    /// @param y 区域左上角y坐标
    /// @param w 区域宽度
    /// @param h 区域高度
    /// @return true-合并成功，false-合并失败
    virtual bool unions(int32_t x, int32_t y, uint32_t w, uint32_t h);

public:
    /// @brief 赋值运算符重载
    /// @param other 源矩形对象
    /// @return 当前矩形对象的引用
    TpRect operator=(const TpRect &other);

    /// @brief 相等运算符重载
    /// @param other 比较的矩形对象
    /// @return true-两个矩形相等，false-不相等
    bool operator==(const TpRect &other);

    /// @brief 不等运算符重载
    /// @param other 比较的矩形对象
    /// @return true-两个矩形不相等，false-相等
    bool operator!=(const TpRect &other);

private:
    ITpRectData *data_;
};

#endif