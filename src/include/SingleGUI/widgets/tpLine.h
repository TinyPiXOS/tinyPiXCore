#ifndef __TP_LINE_H
#define __TP_LINE_H

#include "tpChildWidget.h"
#include "tpString.h"
#include "tpGlobal.h"

TP_DEF_VOID_TYPE_VAR(ItpLineData);
/// @brief 分割线组件
class tpLine : public tpChildWidget
{
public:
    enum LineType
    {
        VLine,
        HLine
    };

public:
    tpLine(tpChildWidget *parent = nullptr);
    virtual ~tpLine();

    /// @brief 设置线类型是水平还是垂直,设置后会调整line组件的最小宽高
    /// @param type 线类型
    void setLineType(const LineType& type = HLine);

    /// @brief 设置绘制线的固定长度,设置后auto模式为false
    /// @param length 长度 
    void setLineLength(const uint32_t& length);

    /// @brief 设置绘制线的百分比长度,设置后auto模式为false
    /// @param percent 百分比长度[0, 1] 
    void setLinePercentLength(const float& percent);

    /// @brief 设置线长模式，自动则根据组件宽度/高度绘制
    /// @param isAuto true为自动
    void setLineLengthMode(const bool& isAuto = true);

    /// @brief 设置居中模式
    /// @param alignment 居中模式
    void setAlign(const tinyPiX::AlignmentFlag& alignment = tinyPiX::AlignCenter);

    /// @brief 设置线条颜色
    /// @param color 颜色_RGB()
    void setColor(const int32_t& color);

    /// @brief 设置线宽，默认为1
    /// @param width 线宽
    void setLineWidth(const uint32_t& width = 1);

protected:
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
    virtual void onThemeChangeEvent(tpThemeChangeEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpLine); }

private:
    ItpLineData *data_;
};

#endif
