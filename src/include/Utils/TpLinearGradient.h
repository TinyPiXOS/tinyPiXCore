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
    void setFinalStop(const ItpPointF &stop);
    void setFinalStop(float x, float y);
    ItpPointF finalStop() const;

    void setStart(const ItpPointF &start);
    void setStart(float x, float y);
    ItpPointF start() const;
};

#endif