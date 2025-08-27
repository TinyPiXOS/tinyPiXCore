#ifndef __TP_GRADIENT_PRIVATE_H
#define __TP_GRADIENT_PRIVATE_H

#include "TpUtils.h"
#include "TpString.h"
#include "TpList.h"
#include "thorVG/thorvg.h"

struct TpGradientData
{
    TpGradient::GradientType type;
    TpList<std::pair<float, int32_t>> colorInfo;
    TpGradient::Spread spread = TpGradient::PadSpread;

    // 线性渐变属性对象
    ItpPointF lineStartPos;
    ItpPointF lineStopPos;

    // 径向渐变属性对象
    ItpPointF center;
    float centerRadius;
    ItpPointF focalPoint;
    float focalRadius;

    virtual ~TpGradientData()
    {
    }

    const TpGradientData &operator=(const TpGradientData &others)
    {
        type = others.type;
        colorInfo = others.colorInfo;
        spread = others.spread;

        lineStartPos = others.lineStartPos;
        lineStopPos = others.lineStopPos;

        center = others.center;
        centerRadius = others.centerRadius;
        focalPoint = others.focalPoint;
        focalRadius = others.focalRadius;

        return *this;
    }
};

#endif
