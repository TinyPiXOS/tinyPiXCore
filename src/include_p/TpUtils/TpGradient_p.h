#ifndef __TP_GRADIENT_PRIVATE_H
#define __TP_GRADIENT_PRIVATE_H

#include <TpCore.h>
#include "TpString.h"
#include "TpList.h"
#include "thorVG/thorvg.h"
#include "TpPointF.h"

struct TpGradientData
{
    TpGradient::GradientType type;
    TpList<std::pair<float, int32_t>> colorInfo;
    TpGradient::Spread spread = TpGradient::PadSpread;

    // 线性渐变属性对象
    TpPointF lineStartPos;
    TpPointF lineStopPos;
    float angle = 0;
    bool hasAngle = false;
    
    // 径向渐变属性对象
    TpPointF center;
    float centerRadius;
    TpPointF focalPoint;
    float focalRadius;

    virtual ~TpGradientData()
    {
    }
};

#endif
