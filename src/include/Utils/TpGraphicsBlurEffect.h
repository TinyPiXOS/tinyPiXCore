#ifndef __TP_GRAPHICSBLUREFFECT_H
#define __TP_GRAPHICSBLUREFFECT_H

#include "TpUtils.h"
#include "TpColors.h"
#include "TpList.h"

TP_DEF_VOID_TYPE_VAR(ITpGraphicsBlurEffectData);
/// @brief 模糊特效类
class TpGraphicsBlurEffect
{
public:
    TpGraphicsBlurEffect();
    virtual ~TpGraphicsBlurEffect();

    /// @brief 设置模糊半径
    /// @param blurRadius 模糊半径
    void setBlurRadius(float blurRadius);
    /// @brief 获取当前模糊半径
    /// @return 模糊半径
    float blurRadius();

public:
    const TpGraphicsBlurEffect &operator=(const TpGraphicsBlurEffect &others);

protected:
    ITpGraphicsBlurEffectData *data_;
};

#endif