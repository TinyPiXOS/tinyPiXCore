#ifndef __TP_RADIAL_GRADIENT_H
#define __TP_RADIAL_GRADIENT_H

#include "TpGradient.h"

/// @brief 径向渐变工具类
class TpRadialGradient : public TpGradient
{
public:
    TpRadialGradient();
    /// @brief 构建径向渐变对象
    /// @param cx 中心点X坐标
    /// @param cy 中心点Y坐标
    /// @param centerRadius 中心半径
    /// @param fx 焦点X坐标
    /// @param fy 焦点Y坐标
    /// @param focalRadius 焦点半径
    TpRadialGradient(float cx, float cy, float centerRadius, float fx, float fy, float focalRadius);
    TpRadialGradient(const ItpPointF &center, float centerRadius, const ItpPointF &focalPoint, float focalRadius);
    /// @brief 构建径向渐变对象；焦点坐标与中心坐标相同；焦点半径为0
    /// @param cx 中心点X坐标
    /// @param cy 中心点Y坐标
    /// @param radius 中心半径
    TpRadialGradient(float cx, float cy, float radius);
    TpRadialGradient(const ItpPointF &center, float radius);
    /// @brief 构建径向渐变对象；焦点半径为0；如果焦点在圆心之外，则移动焦点至中心点与焦点连线与中心圆的交叉点
    /// @param cx 中心点X坐标
    /// @param cy 中心点Y坐标
    /// @param radius 中心半径
    /// @param fx 焦点X坐标
    /// @param fy 焦点Y坐标
    TpRadialGradient(float cx, float cy, float radius, float fx, float fy);
    TpRadialGradient(const ItpPointF &center, float radius, const ItpPointF &focalPoint);

    virtual ~TpRadialGradient();

    ItpPointF center() const;
    void setCenter(const ItpPointF &center);
    void setCenter(float x, float y);

    float centerRadius() const;
    void setCenterRadius(float radius);

    ItpPointF focalPoint() const;
    void setFocalPoint(const ItpPointF &focalPoint);
    void setFocalPoint(float x, float y);

    float focalRadius() const;
    void setFocalRadius(float radius);

public:
};

#endif