#ifndef __TP_LINEAR_GRADIENT_H
#define __TP_LINEAR_GRADIENT_H

#include "TpGradient.h"

/// @brief 线性渐变工具类
class TpLinearGradient : public TpGradient
{
public:
    TpLinearGradient();
    TpLinearGradient(float x1, float y1, float x2, float y2);
    TpLinearGradient(const ItpPointF &start, const ItpPointF &finalStop);

    virtual ~TpLinearGradient();

public:
    /// @brief 设置渐变起始坐标
    /// @param start 起始坐标
    void setStart(const ItpPointF &start);
    /// @brief 设置渐变起始坐标
    /// @param x 起始X坐标
    /// @param y 起始Y坐标
    void setStart(float x, float y);
    /// @brief 获取渐变起始坐标
    /// @return 起始坐标
    ItpPointF start() const;

    /// @brief 设置渐变终止坐标
    /// @param stop 终止坐标
    void setFinalStop(const ItpPointF &stop);
    /// @brief 设置渐变终止坐标
    /// @param x 终止X坐标
    /// @param y 终止Y坐标
    void setFinalStop(float x, float y);
    /// @brief 获取渐变终止坐标
    /// @return 终止坐标
    ItpPointF finalStop() const;
};

#endif