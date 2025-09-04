#ifndef __TP_GRADIENT_H
#define __TP_GRADIENT_H

#include "TpUtils.h"
#include "TpColors.h"
#include "TpList.h"

TP_DEF_VOID_TYPE_VAR(ITpGradientData);
/// @brief 渐变处理工具类
class TpGradient
{
public:
    /// @brief 渐变类型
    enum GradientType
    {
        /// @brief 线性渐变
        LinearGradient,
        /// @brief 径向渐变
        RadialGradient
    };

    /// @brief 指定如何填充渐变区域以外的区域
    enum Spread
    {
        /// @brief 剩余区域将填充最接近的停止色;默认模式
        PadSpread = 0,
        /// @brief 渐变图案会延伸到渐变区域之外，直到填充到预期区域
        ReflectSpread,
        /// @brief 渐变图案在渐变区域之外持续重复，直至填充预期区域
        RepeatSpread
    };

public:
    TpGradient();
    TpGradient(const TpGradient &other);
    virtual ~TpGradient();

public:
    /// @brief 获取当前渐变类型
    /// @return 渐变类型枚举
    GradientType gradientType();

public:
    /// @brief 在给定位置使用给定颜色创建一个停止点。给定位置必须在0到1的范围内。
    /// @param position 给定位置；取值范围[0, 1]
    /// @param color 颜色_RGB(值)
    void setColorAt(float position, int32_t color);
    /// @brief 在给定位置使用给定颜色创建一个停止点。给定位置必须在0到1的范围内。
    /// @param position 给定位置；取值范围[0, 1]
    /// @param color 颜色对象
    void setColorAt(float position, const TpColors &color);

    /// @brief 获取所有颜色停止点
    /// @return 包含所有位置和颜色对的向量，按位置排序
    TpList<std::pair<float, int32_t>> getColors() const;

    /// @brief 指定此渐变应使用的扩散方法;仅对线性渐变和径向渐变有效
    /// @param spread 扩散方法枚举
    void setSpread(Spread spread);
    Spread spread();

public:
    const TpGradient &operator=(const TpGradient &others);

protected:
    ITpGradientData *data_;
};

#endif