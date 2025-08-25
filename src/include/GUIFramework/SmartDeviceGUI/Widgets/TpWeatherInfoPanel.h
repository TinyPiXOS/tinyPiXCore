#ifndef __TP_SMART_GUI_WEATHER_INFO_PANEL_H
#define __TP_SMART_GUI_WEATHER_INFO_PANEL_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpString.h"
#include "tpVector.h"
#include "SmartDeviceGUI/FrameworkGlobal.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ITpWeatherInfoPanelData);
/// @brief 天气预报面板
class TpWeatherInfoPanel : public tpChildWidget
{
public:
    /// @brief 天气类型
    enum WeatherType
    {
        /// @brief ​​晴天​​
        Sunny,
        /// @brief ​​多云​​
        Cloudy,
        /// @brief ​​阴天​​
        Overcast,
        /// @brief ​​小雨​​
        LightRain,
        /// @brief ​​中雨​​
        ModerateRain,
        /// @brief ​​大雨​​
        HeavyRain,
        /// @brief ​​暴雨​​
        TorrentialRain,
        /// @brief ​​雷阵雨​​
        Thunderstorm,
        /// @brief ​​小雪​​
        LightSnow,
        /// @brief ​​中雪​​
        ModerateSnow,
        /// @brief ​​大雪​​
        HeavySnow,
        /// @brief ​​暴雪​​
        Blizzard,
        /// @brief ​​雨夹雪​​
        Sleet,
        /// @brief ​​雾​​
        Fog,
        /// @brief ​​雾霾​​
        Haze,
        /// @brief ​​沙尘暴​​
        Sandstorm,
        /// @brief ​​冰雹​​
        Hail
    };

    struct WeatherInfo
    {
        /// @brief 一般用于显示日期，可自定义显示内容；ex: 今天
        tpString text;
        /// @brief 天气类型
        WeatherType weatherType;
        /// @brief 一般用于显示气温，可自定义显示内容；ex: 15°-30°
        tpString subText;

        WeatherInfo() : text(""), weatherType(TpWeatherInfoPanel::Sunny), subText("")
        {
        }

        WeatherInfo(const tpString &text, const WeatherType &type, const tpString &subText)
            : text(text), weatherType(type), subText(subText)
        {
        }
    };

public:
    TpWeatherInfoPanel(tpChildWidget *parent = nullptr);

    virtual ~TpWeatherInfoPanel();

    /// @brief 设置显示天气的数量
    /// @param count 取值范围[1, 15]
    void setCount(const int32_t &count = 5);

    /// @brief 设置当前选中显示的索引
    /// @param index 索引值，取值范围[0, count - 1]
    void setSelectIndex(const int32_t &index);

    /// @brief 当前选中的索引值
    /// @return 索引值
    int32_t selectIndex();

    /// @brief 设置天气信息列表；使用前需先设置天气数量
    /// @param weatherInfoList 若Size为0则设置失败，Size超过count值则只显示Count数
    void setWeatherList(const tpVector<WeatherInfo>& weatherInfoList);

    /// @brief 指定索引设置天气信息
    /// @param index 索引值；取值范围[0, count - 1]
    /// @param weatherInfo 天气信息
    bool setWeatherInfo(const int32_t& index, const WeatherInfo& weatherInfo);

public:
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual tpString pluginType() override { return TO_STRING(TpWeatherInfoPanel); }

private:
    ITpWeatherInfoPanelData *data_;
};

SMART_DEVICE_GUI_NAMESPACE_END

#endif
