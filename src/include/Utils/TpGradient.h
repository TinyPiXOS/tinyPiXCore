#ifndef __TP_GRADIENT_H
#define __TP_GRADIENT_H

#include "TpUtils.h"
#include "TpString.h"
#include "TpColors.h"

TP_DEF_VOID_TYPE_VAR(ITpGradientData);
/// @brief 渐变处理工具类
class TpGradient
{
public:
    TpGradient();
    virtual ~TpGradient();

public:
    /// @brief 在给定位置使用给定颜色创建一个停止点。给定位置必须在0到1的范围内。
    /// @param position 给定位置；取值范围[0, 1]
    /// @param color 颜色_RGB(值)
    void setColorAt(float position, int32_t color);
    /// @brief 在给定位置使用给定颜色创建一个停止点。给定位置必须在0到1的范围内。
    /// @param position 给定位置；取值范围[0, 1]
    /// @param color 颜色对象
    void setColorAt(float position, const TpColors& color);

private:
    ITpGradientData *data_;
};

#endif