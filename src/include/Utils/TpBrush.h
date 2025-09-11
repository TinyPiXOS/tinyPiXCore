#ifndef __TP_BRUSH_H
#define __TP_BRUSH_H

#include "TpUtils.h"
#include "TpColors.h"
#include "TpGradient.h"

TP_DEF_VOID_TYPE_VAR(ITpBrushData);

/// @brief 画刷工具类，用于定义图形填充样式
class TpBrush
{
public:
    /// @brief 默认构造函数
    TpBrush();

    /// @brief 使用指定样式创建画刷
    /// @param bs 画刷样式
    explicit TpBrush(tinyPiX::BrushStyle bs);

    /// @brief 使用指定颜色和样式创建画刷
    /// @param color 填充颜色
    /// @param bs 画刷样式（默认为实心填充）
    TpBrush(const TpColors &color, tinyPiX::BrushStyle bs = tinyPiX::SolidPattern);

    /// @brief 复制构造函数
    /// @param brush 要复制的画刷对象
    TpBrush(const TpBrush &brush);

    /// @brief 使用渐变创建画刷
    /// @param gradient 渐变对象
    explicit TpBrush(TpGradient *gradient);

    /// @brief 析构函数
    ~TpBrush();

    /// @brief 赋值运算符重载
    /// @param brush 要赋值的画刷对象
    /// @return 当前画刷对象的引用
    TpBrush &operator=(const TpBrush &brush);

    /// @brief 获取画刷样式
    /// @return 当前画刷样式
    tinyPiX::BrushStyle style() const;

    /// @brief 设置画刷样式
    /// @param bs 要设置的画刷样式
    void setStyle(tinyPiX::BrushStyle bs);

    /// @brief 获取画刷颜色
    /// @return 当前画刷颜色
    const TpColors &color() const;

    /// @brief 设置画刷颜色
    /// @param color 要设置的颜色
    void setColor(const TpColors &color);

    /// @brief 获取渐变对象
    /// @return 指向渐变对象的指针（如果不是渐变画刷则返回nullptr）
    TpGradient *gradient() const;

private:
    ITpBrushData *data_; // 画刷数据实现
};

#endif