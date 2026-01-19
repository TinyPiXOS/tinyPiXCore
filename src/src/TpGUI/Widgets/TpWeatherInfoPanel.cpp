#include <TpWeatherInfoPanel.h>
#include <TpImage.h>
#include <TpPainter.h>
#include <TpFont.h>
#include <TpLinearGradient.h>
#include <TpEvent.h>

struct TpWeatherInfoPanelData
{
    int32_t count = 5;
    int32_t selectIndex = 0;

    TpFont titleFont;
    TpFont subTextFont;

    TpVector<TpWeatherInfoPanel::WeatherInfo> weatherInfoList;

    TpWeatherInfoPanelData()
    {
    }
};

static inline TpString weatherIconPath(const TpWeatherInfoPanel::WeatherType &weatherType)
{
    TpString iconRootPath = "/usr/res/TinyPiX/TpWeatherInfoPanel/";

    switch (weatherType)
    {
    case TpWeatherInfoPanel::Sunny:
        return iconRootPath + "晴天.png";
    case TpWeatherInfoPanel::Cloudy:
        return iconRootPath + "多云.png";
    case TpWeatherInfoPanel::Overcast:
        return iconRootPath + "阴天.png";
    case TpWeatherInfoPanel::LightRain:
        return iconRootPath + "小雨.png";
    case TpWeatherInfoPanel::ModerateRain:
        return iconRootPath + "中雨.png";
    case TpWeatherInfoPanel::HeavyRain:
        return iconRootPath + "​大雨.png";
    case TpWeatherInfoPanel::TorrentialRain:
        return iconRootPath + "​暴雨.png";
    case TpWeatherInfoPanel::Thunderstorm:
        return iconRootPath + "​雷阵雨.png";
    case TpWeatherInfoPanel::LightSnow:
        return iconRootPath + "​小雪.png";
    case TpWeatherInfoPanel::ModerateSnow:
        return iconRootPath + "​中雪.png";
    case TpWeatherInfoPanel::HeavySnow:
        return iconRootPath + "​大雪.png";
    case TpWeatherInfoPanel::Blizzard:
        return iconRootPath + "​暴雪.png";
    case TpWeatherInfoPanel::Sleet:
        return iconRootPath + "雨夹雪.png";
    case TpWeatherInfoPanel::Fog:
        return iconRootPath + "​雾.png";
    case TpWeatherInfoPanel::Haze:
        return iconRootPath + "​​雾霾.png";
    case TpWeatherInfoPanel::Sandstorm:
        return iconRootPath + "沙尘暴.png";
    case TpWeatherInfoPanel::Hail:
        return iconRootPath + "冰雹.png";
        break;
    default:
        break;
    }

    return iconRootPath + "晴天.png";
}

TpWeatherInfoPanel::TpWeatherInfoPanel(TpWidget *parent)
    : TpWidget(parent)
{
    TpWeatherInfoPanelData *weatherData = new TpWeatherInfoPanelData();
    data_ = weatherData;

    weatherData->titleFont.setFontSize(13);
    weatherData->subTextFont.setFontSize(13);

    setBackGroundColor(_RGB(255, 255, 255));
    setRoundCorners(20);
}

TpWeatherInfoPanel::~TpWeatherInfoPanel()
{
    TpWeatherInfoPanelData *weatherData = static_cast<TpWeatherInfoPanelData *>(data_);
    if (weatherData)
    {
        delete weatherData;
        weatherData = nullptr;
        data_ = nullptr;
    }
}

void TpWeatherInfoPanel::setCount(const int32_t &count)
{
    TpWeatherInfoPanelData *weatherData = static_cast<TpWeatherInfoPanelData *>(data_);
    weatherData->count = count;

    if (weatherData->count < 1)
        weatherData->count = 1;
    else if (weatherData->count > 15)
        weatherData->count = 15;
    else
    {
    }

    update();
}

void TpWeatherInfoPanel::setSelectIndex(const int32_t &index)
{
    TpWeatherInfoPanelData *weatherData = static_cast<TpWeatherInfoPanelData *>(data_);
    weatherData->selectIndex = index;
    update();
}

int32_t TpWeatherInfoPanel::selectIndex()
{
    TpWeatherInfoPanelData *weatherData = static_cast<TpWeatherInfoPanelData *>(data_);
    return weatherData->selectIndex;
}

void TpWeatherInfoPanel::setWeatherList(const TpVector<TpWeatherInfoPanel::WeatherInfo> &weatherInfoList)
{
    TpWeatherInfoPanelData *weatherData = static_cast<TpWeatherInfoPanelData *>(data_);
    weatherData->weatherInfoList = weatherInfoList;
    update();
}

bool TpWeatherInfoPanel::setWeatherInfo(const int32_t &index, const TpWeatherInfoPanel::WeatherInfo &weatherInfo)
{
    TpWeatherInfoPanelData *weatherData = static_cast<TpWeatherInfoPanelData *>(data_);
    if (index >= weatherData->weatherInfoList.size())
        return false;
    weatherData->weatherInfoList[index] = weatherInfo;
    return true;
}

bool TpWeatherInfoPanel::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpWeatherInfoPanelData *weatherData = static_cast<TpWeatherInfoPanelData *>(data_);

    int32_t weatherSize = weatherData->weatherInfoList.size();
    if (weatherSize == 0)
        return true;

    int32_t paintWeatherCount = weatherData->count < weatherSize ? weatherData->count : weatherSize;

    int32_t singleWeatherWidth = width() / paintWeatherCount;
    if (singleWeatherWidth == 0)
        return true;

    TpPainter *painter = event->painter();
    int32_t iconSize = singleWeatherWidth * 0.5;
    int32_t iconX = (singleWeatherWidth - iconSize) / 2.0;
    int32_t iconY = (height() - iconSize) / 2.0;

    for (int i = 0; i < paintWeatherCount; ++i)
    {
        WeatherInfo weatherInfo = weatherData->weatherInfoList.at(i);

        int32_t titleTextFontColor = _RGB(160, 152, 174);
        int32_t subTextFontColor = _RGB(54, 59, 100);

        // 绘制选中底色
        if (i == weatherData->selectIndex)
        {
            // 字体颜色变为白色
            titleTextFontColor = _RGB(255, 255, 255);
            subTextFontColor = _RGB(255, 255, 255);

            // 设置渐变背景
            TpLinearGradient lineGradient(i * singleWeatherWidth, 0, i * singleWeatherWidth + singleWeatherWidth, height());
            lineGradient.setColorAt(0, _RGB(107, 80, 246));
            lineGradient.setColorAt(1, _RGB(204, 143, 237));

            painter->setBrush(TpBrush(&lineGradient));

            painter->drawRect(i * singleWeatherWidth, 0, singleWeatherWidth, height(), 20);

            // 重置渐变效果
            painter->setBrush(TpBrush(Tp::NoBrush));
        }

        TpImage weatherIcon(weatherIconPath(weatherInfo.weatherType));

        weatherData->titleFont.setText(weatherInfo.text);
        weatherData->titleFont.setFontColor(titleTextFontColor);

        int32_t titleTextX = (singleWeatherWidth - weatherData->titleFont.pixelWidth()) / 2.0;
        int32_t titleTextY = ((height() - iconSize) / 2.0 - weatherData->titleFont.pixelHeight()) / 2.0;
        painter->drawText(weatherData->titleFont, titleTextX + i * singleWeatherWidth, titleTextY, weatherInfo.text);

        painter->drawImage(iconX + i * singleWeatherWidth, iconY, weatherIcon.scaled(iconSize, iconSize));

        weatherData->subTextFont.setText(weatherInfo.subText);
        weatherData->subTextFont.setFontColor(subTextFontColor);

        int32_t subTitleTextX = (singleWeatherWidth - weatherData->subTextFont.pixelWidth()) / 2.0;
        int32_t subTitleTextY = ((height() - iconSize) / 2.0 - weatherData->subTextFont.pixelHeight()) / 2.0;
        painter->drawText(weatherData->subTextFont, subTitleTextX + i * singleWeatherWidth, iconY + iconSize + subTitleTextY, weatherInfo.subText);
    }

    return true;
}

bool TpWeatherInfoPanel::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);

    return true;
}
