#ifndef __TP_LINEAR_GRADIENT_H
#define __TP_LINEAR_GRADIENT_H

#include "TpGradient.h"
#include "TpPointF.h"

/// @brief 线性渐变工具类
class TpLinearGradient : public TpGradient
{
public:
    TpLinearGradient();
    TpLinearGradient(float x1, float y1, float x2, float y2);
    TpLinearGradient(const TpPointF &start, const TpPointF &finalStop);

    virtual ~TpLinearGradient();

public:
    /// @brief 设置渐变方向角度；设置后渐变坐标属性无效
    ///        ​​0度​​：表示从下到上（垂直向上）
    ///        ​​90度​​：表示从左到右（水平向右）
    ///        ​​180度​​：表示从上到下（垂直向下）
    ///        ​​270度​​：表示从右到左（水平向左）
    /// @param angle 角度值
    void setAngle(float angle);
    /// @brief 获取当前渐变角度
    /// @return 渐变角度
    float angle();
    /// @brief 是否设置了渐变角度
    /// @return 
    bool hasAngle();

    /// @brief 设置渐变起始坐标
    /// @param start 起始坐标
    void setStart(const TpPointF &start);
    /// @brief 设置渐变起始坐标
    /// @param x 起始X坐标
    /// @param y 起始Y坐标
    void setStart(float x, float y);
    /// @brief 获取渐变起始坐标
    /// @return 起始坐标
    TpPointF start() const;

    /// @brief 设置渐变终止坐标
    /// @param stop 终止坐标
    void setFinalStop(const TpPointF &stop);
    /// @brief 设置渐变终止坐标
    /// @param x 终止X坐标
    /// @param y 终止Y坐标
    void setFinalStop(float x, float y);
    /// @brief 获取渐变终止坐标
    /// @return 终止坐标
    TpPointF finalStop() const;
};

#endif