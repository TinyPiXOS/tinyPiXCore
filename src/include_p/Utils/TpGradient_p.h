#ifndef __TP_GRADIENT_PRIVATE_H
#define __TP_GRADIENT_PRIVATE_H

#include "TpString.h"
#include "TpList.h"
#include "thorVG/thorvg.h"

struct ColorPosInfo
{
    float pos;
    int32_t color;

    ColorPosInfo() : pos(0), color(0)
    {
    }
    ColorPosInfo(float pos, int32_t color) : pos(pos), color(color)
    {
    }
};

struct TpGradientData
{
    TpList<ColorPosInfo> colorInfo;

    // 线性渐变对象
    tvg::LinearGradient *linearGradient = nullptr;

    ~TpGradientData()
    {
        if (linearGradient)
        {
            delete linearGradient;
            linearGradient = nullptr;
        }
    }
};

#endif
