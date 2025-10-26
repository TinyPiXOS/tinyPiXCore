#ifndef __TP_GRAPHICSEFFECT_H
#define __TP_GRAPHICSEFFECT_H

#include "TpUtils.h"
#include "TpColors.h"
#include "TpList.h"

TP_DEF_VOID_TYPE_VAR(ITpGraphicsEffectData);
/// @brief 特效类基类
class TpGraphicsEffect
{
public:
    /// @brief 特效类型
    enum EffectType
    {
        /// @brief 模糊
        BlurEffect,
    };

public:
    TpGraphicsEffect();
    TpGraphicsEffect(const TpGraphicsEffect &other);
    virtual ~TpGraphicsEffect();

public:
    /// @brief 获取当前特效类型
    /// @return 特效类型枚举
    EffectType gradientType();

public:
    // const TpGraphicsEffect &operator=(const TpGraphicsEffect &others);

protected:
    ITpGraphicsEffectData *data_;
};

#endif