#ifndef __TP_BATTERY_H
#define __TP_BATTERY_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"

TP_DEF_VOID_TYPE_VAR(ItpBatteryData);
/// @brief 电池UI组件
class TpBattery : public TpWidget
{
public:
    enum BatteryStyle
    {
        White,
        Black
    };

public:
    TpBattery(TpWidget *parent);

    virtual ~TpBattery();

    /// @brief 设置电池配色主题
    /// @param style 样式主题
    void setStyle(const BatteryStyle &style);

    /// @brief 获取当前样式
    /// @return 样式枚举
    BatteryStyle style();

    /// @brief 设置电量，[0, 100]
    /// @param value 电量值
    void setValue(int32_t value);

    /// @brief 获取当前电量
    /// @return 电量值[0,100]
    int32_t value();

    /// @brief 动画效果更新电量 [0, 100]
    /// @param value 电量值
    void setValueAnimated(int32_t value);

    /// @brief 设置告警值
    /// @param value 告警值
    void setAlamValue(const int32_t &value);
    /// @brief 获取当前告警值
    /// @return
    int32_t alamValue();

    /// @brief 设置告警颜色
    /// @param color 颜色_RGB
    void setAlamColor(const int32_t &color);
    /// @brief 获取告警颜色值
    /// @return 颜色RGB
    int32_t alamColor();

public:
    virtual bool onPaintEvent(TpPaintEvent *event) override;

    virtual TpString pluginType() override { return TO_STRING(TpBattery); }

private:
    ItpBatteryData *data_;
};

#endif
