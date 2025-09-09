#ifndef __TP_SMART_GUI_TEMPERATURE_WIDGET_H
#define __TP_SMART_GUI_TEMPERATURE_WIDGET_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "TpVector.h"
#include "SmartDeviceGUI/FrameworkGlobal.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ITpTemperatureWidgetData);
/// @brief 温度计组件
class TpTemperatureWidget : public TpChildWidget
{
public:
    TpTemperatureWidget(TpChildWidget *parent = nullptr);

    virtual ~TpTemperatureWidget();

    /// @brief 设置标题文本
    /// @param title 文本字符串
    void setTitle(const TpString& title);
    /// @brief 获取当前标题文本
    /// @return 文本字符串
    TpString title() const;

    /// @brief 设置温度范围
    /// @param minTemp 最小温度
    /// @param maxTemp 最大温度
    void setRange(int32_t minTemp, int32_t maxTemp);

    /// @brief 设置当前温度值
    /// @param currentTemp 当前温度值
    void setValue(int32_t currentTemp);
    /// @brief 获取当前温度值
    /// @return 温度值
    int32_t value() const;

    /// @brief 设置颜色列表；从最低温度到最高温度分段渐变，建议不要超过四种颜色
    /// @param colorList 颜色列表（_RGB()）
    void setColorList(const TpVector<int32_t> &colorList);

    /// @brief 设置线条宽度
    /// @param width 线宽
    void setLineWidth(int32_t width);
    /// @brief 获取线条宽度
    /// @return 线宽
    int32_t lineWidth() const;

public:
    virtual bool onPaintEvent(TpPaintEvent *event) override;

    virtual TpString pluginType() override { return TO_STRING(TpTemperatureWidget); }

private:
    ITpTemperatureWidgetData *data_;
};

SMART_DEVICE_GUI_NAMESPACE_END

#endif
