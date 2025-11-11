#ifndef __TP_GRAPHICSBLUREFFECT_H
#define __TP_GRAPHICSBLUREFFECT_H

#include <TpCore.h>
#include "TpColors.h"
#include "TpList.h"
#include "TpGraphicsEffect.h"

TP_DEF_VOID_TYPE_VAR(ITpGraphicsBlurEffectData);
/// @brief 模糊特效类
class TpGraphicsBlurEffect : TpGraphicsEffect
{
public:
    /// @brief 模糊方向
    enum BlurDirection
    {
        /// @brief 双向模糊
        BothDirection,
        /// @brief 水平模糊
        HorizonDirection,
        /// @brief 垂直模糊
        VerticalDirection
    };

    /// @brief 模糊边界处理
    enum BlurBorderType
    {
        /// @brief 复制
        CopyBorder,
        /// @brief 环绕
        SurroundBorder
    };

public:
    TpGraphicsBlurEffect();
    TpGraphicsBlurEffect(const TpGraphicsBlurEffect &other);
    virtual ~TpGraphicsBlurEffect();

    /// @brief 设置模糊半径
    /// @param blurRadius 模糊半径
    void setBlurRadius(float blurRadius);
    /// @brief 获取当前模糊半径
    /// @return 模糊半径
    float blurRadius();

    /// @brief 设置模糊方向
    /// @param direction 模糊方向类型
    void setDirection(BlurDirection direction);
    /// @brief 获取当前模糊方向
    /// @return 模糊方向类型
    BlurDirection direction();

    /// @brief 设置模糊边界处理
    /// @param borderType 模糊边界处理类型
    void setBorder(BlurBorderType borderType);
    /// @brief 获取模糊边界处理 
    /// @return 模糊边界处理类型
    BlurBorderType border();

    /// @brief 设置模糊渲染质量
    /// @param quality 质量等级；取值 [0, 100]
    void setQuality(int32_t quality);
    /// @brief 获取模糊渲染质量
    /// @return quality 质量等级；取值 [0, 100]
    int32_t quality();

public:
    const TpGraphicsBlurEffect &operator=(const TpGraphicsBlurEffect &others);

protected:
    ITpGraphicsBlurEffectData *data_;
};

#endif