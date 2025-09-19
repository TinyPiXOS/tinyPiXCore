#ifndef __TP_PAINTER_PATH_H
#define __TP_PAINTER_PATH_H

#include "TpUtils.h"
#include "TpRect.h"
#include "TpPoint.h"

TP_DEF_VOID_TYPE_VAR(ITpPainterPathData);
class TpPainterPath
{
public:
    TpPainterPath();
    TpPainterPath(const TpPainterPath &others);

    /// @brief 构造函数，以给定起点初始化路径
    /// @param startPoint 路径起始点
    explicit TpPainterPath(const TpPoint &startPoint);

    /// @brief 移动当前点到指定位置（不绘制）
    /// @param point 目标位置
    void moveTo(const TpPoint &point);

    /// @brief 添加直线到指定点
    /// @param endPoint 线段终点
    void lineTo(const TpPoint &endPoint);

    /// @brief 添加三次贝塞尔曲线
    /// @param controlPoint1 第一控制点
    /// @param controlPoint2 第二控制点
    /// @param endPoint 结束点
    void cubicTo(const TpPoint &controlPoint1,
                 const TpPoint &controlPoint2,
                 const TpPoint &endPoint);

    /// @brief 添加矩形路径
    /// @param rect 要添加的矩形区域
    void addRect(const TpRect &rect);

    /// @brief 添加椭圆路径
    /// @param rect 椭圆的外接矩形
    void addEllipse(const TpRect &rect);

    /// @brief 添加圆角矩形路径
    /// @param rect 矩形区域
    /// @param xRadius X方向圆角半径
    /// @param yRadius Y方向圆角半径
    void addRoundedRect(const TpRect &rect, float radius);

    /// @brief 添加圆弧路径
    /// @param center 圆心坐标
    /// @param radius 半径
    /// @param startAngle 起始角度（度）0度：指向右侧（正X轴方向）90度：指向下方（正Y轴方向）
    /// @param endAngle 终止角度（度）要比起始角度大
    void addArc(const TpPoint &center, float radius, float startAngle, float endAngle);

    /// @brief 添加扇形路径
    /// @param center 圆心坐标
    /// @param radius 半径
    /// @param startAngle 起始角度（度）0度：指向右侧（正X轴方向）90度：指向下方（正Y轴方向）
    /// @param endAngle 终止角度（度）要比起始角度大
    void addPie(const TpPoint &center, float radius, float startAngle, float endAngle);

    /// @brief 闭合路径（添加起点到当前点的线段）
    void closeSubpath();

    /// @brief 清除当前路径的所有元素
    void clear();

    /// @brief 判断路径是否为空
    /// @return true表示路径无任何元素
    bool isEmpty() const;

    /// @brief 获取路径边界矩形
    /// @return 包含整个路径的矩形
    TpRect boundingRect() const;

    /// @brief 连接两个路径
    /// @param other 要连接的路径
    TpPainterPath operator+(const TpPainterPath &other) const;
    /// @brief 连接两个路径
    /// @param other 要连接的路径
    TpPainterPath &operator+=(const TpPainterPath &other);

    friend class TpPainter;

private:
    ITpPainterPathData *data_;
};
#endif
