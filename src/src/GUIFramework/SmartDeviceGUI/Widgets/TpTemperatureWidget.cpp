#include "TpTemperatureWidget.h"
#include "TpCanvas.h"
#include "TpLinearGradient.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

struct TpTemperatureWidgetData
{
    TpString titleText = "";

    // 最大、最小温度；当前温度
    int32_t maxTemperature = 40;
    int32_t minTemperature = -10;
    int32_t curTemperature = 30;

    int32_t lineWidth = 3;

    TpVector<int32_t> colorList;
};

TpTemperatureWidget::TpTemperatureWidget(TpChildWidget *parent) : TpChildWidget(parent)
{
    TpTemperatureWidgetData *tempData = new TpTemperatureWidgetData();
    data_ = tempData;

    tempData->colorList.emplace_back(_RGB(160, 234, 255));
    tempData->colorList.emplace_back(_RGB(134, 226, 252));
    tempData->colorList.emplace_back(_RGB(246, 130, 132));
    tempData->colorList.emplace_back(_RGB(123, 1, 1));
}

TpTemperatureWidget::~TpTemperatureWidget()
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);
    if (tempData)
    {
        delete tempData;
        tempData = nullptr;
        data_ = nullptr;
    }
}

void TpTemperatureWidget::setTitle(const TpString &title)
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);
    tempData->titleText = title;
}

TpString TpTemperatureWidget::title() const
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);
    return tempData->titleText;
}

void TpTemperatureWidget::setRange(int32_t minTemp, int32_t maxTemp)
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);
    tempData->minTemperature = minTemp;
    tempData->maxTemperature = maxTemp;

    if (tempData->minTemperature >= tempData->maxTemperature)
        tempData->minTemperature = tempData->maxTemperature - 1;

    update();
}

void TpTemperatureWidget::setValue(int32_t currentTemp)
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);
    tempData->curTemperature = currentTemp;
    update();
}

int32_t TpTemperatureWidget::value() const
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);
    return tempData->curTemperature;
}

void TpTemperatureWidget::setColorList(const TpVector<int32_t> &colorList)
{
    if (colorList.size() == 0)
        return;
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);
    tempData->colorList = colorList;
}

void TpTemperatureWidget::setLineWidth(int32_t width)
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);
    tempData->lineWidth = width;
    update();
}

int32_t TpTemperatureWidget::lineWidth() const
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);
    return tempData->lineWidth;
}

bool TpTemperatureWidget::onPaintEvent(TpObjectPaintEvent *event)
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);

    TpCanvas *painter = event->canvas();

    // 宽度三分之一的位置绘制温度条
    int32_t thermometerWidth = width() * 0.29;
    int32_t thermometerHeight = height() * 0.93;

    // 矩形部分宽度
    int32_t rectangleWidth = thermometerWidth * 0.75;
    int32_t rectangleHeight = thermometerHeight * 0.75;

    int32_t rectangleX = (width() - rectangleWidth) / 2.0;
    int32_t rectangleY = (height() - thermometerHeight) / 2.0;

    // 绘制下半部分圆环
    painter->arc(rectangleX + rectangleWidth / 2.0, rectangleY + rectangleHeight, thermometerWidth / 2.0, -44, 224, _RGB(116, 121, 150), tempData->lineWidth);

    // 绘制上半部分圆角矩形;使用遮罩圆角box绘制，实现遮挡圆和矩形交接处的线条  - tempData->lineWidth * 2
    HollowMask hollowMask;
    hollowMask.addRoundRectHollow(HollowMask::RoundRectHollow(ItpRect(tempData->lineWidth, tempData->lineWidth, rectangleWidth - tempData->lineWidth * 2, rectangleY + rectangleHeight), rectangleWidth / 2.0));
    hollowMask.addCircleHollow(rectangleWidth / 2.0, rectangleHeight, thermometerWidth / 2.0 - tempData->lineWidth + 1);
    painter->roundedBox(rectangleX, rectangleY, rectangleX + rectangleWidth, rectangleY + rectangleHeight, rectangleWidth / 2.0, _RGB(116, 121, 150), hollowMask);

    // 根据当前温度值绘制填充矩形
    // 构建渐变效果
    int32_t colorListSize = tempData->colorList.size();
    if (colorListSize > 0)
    {
        TpLinearGradient lineGradient;
        for (int i = 0; i < colorListSize; ++i)
        {
            lineGradient.setColorAt(i / (colorListSize - 1), tempData->colorList.at(i));
        }
    }
    return true;
}

bool TpTemperatureWidget::onResizeEvent(TpObjectResizeEvent *event)
{
    TpChildWidget::onResizeEvent(event);

    return true;
}

SMART_DEVICE_GUI_NAMESPACE_END
