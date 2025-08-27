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

    // 线性渐变属性对象
    // tvg::LinearGradient *linearGradient = nullptr;
    ItpPointF lineStartPos;
    ItpPointF lineStopPos;

    // 径向渐变属性对象

    ~TpGradientData()
    {
        // if (linearGradient)
        // {
        //     delete linearGradient;
        //     linearGradient = nullptr;
        // }
    }

    const TpGradientData &operator=(const TpGradientData &others)
    {
        type = others.type;
        colorInfo = others.colorInfo;
        lineStartPos = others.lineStartPos;
        lineStopPos = others.lineStopPos;
        return *this;
    }
};

#endif
