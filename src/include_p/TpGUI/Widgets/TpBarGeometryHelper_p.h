#ifndef TP_BAR_GEOMETRY_HELPER_P_H
#define TP_BAR_GEOMETRY_HELPER_P_H

#include "TpAxis.h"
#include <TpRect.h>
#include <cstdint>
#include <algorithm>
#include <cmath>

static inline TpRect tpBuildBarRect(const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect, double xValue, double yValue,
                                    int32_t seriesIndex, int32_t seriesCount, double groupWidthRatio = 0.6)
{
    int32_t rectX = rect.x();
    int32_t rectY = rect.y();
    int32_t rectW = rect.width();
    int32_t rectH = rect.height();

    int32_t yZero = axisY.ZeroPixel(rectH, rectY, true);

    int32_t x0 = axisX.mapToPixel(axisX.min(), rectW, rectX, false);
    int32_t x1 = axisX.mapToPixel(axisX.min() + 1.0, rectW, rectX, false);
    int32_t unitPixelWidth = std::abs(x1 - x0);
    if (unitPixelWidth <= 0)
    {
        unitPixelWidth = 50;
    }

    if (groupWidthRatio <= 0.0)
    {
        groupWidthRatio = 0.6;
    }

    int32_t groupWidth = static_cast<int32_t>(unitPixelWidth * groupWidthRatio);
    if (groupWidth < 1)
    {
        groupWidth = 1;
    }

    int32_t barCount = seriesCount > 0 ? seriesCount : 1;
    if (seriesIndex < 0)
    {
        seriesIndex = 0;
    }

    int32_t barWidth = groupWidth / barCount;
    if (barWidth < 1)
    {
        barWidth = 1;
    }

    int32_t xCenter = axisX.mapToPixel(xValue, rectW, rectX, false);
    int32_t barLeft = xCenter - (groupWidth / 2) + (seriesIndex * barWidth);
    int32_t yVal = axisY.mapToPixel(yValue, rectH, rectY, true);
    int32_t top = std::min(yVal, yZero);
    int32_t height = std::abs(yVal - yZero);
    if (height == 0)
    {
        height = 1;
    }

    return TpRect(barLeft, top, barWidth, height);
}

#endif
