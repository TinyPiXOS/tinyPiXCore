#include "TpHumidityWidget.h"
#include "TpCanvas.h"
#include "TpLinearGradient.h"
#include "TpFont.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

struct TpHumidityWidgetData
{
    TpString titleText = "湿度";

    // 最大、最小温度；当前温度
    int32_t maxValue = 100;
    int32_t minValue = 0;
    int32_t curValue = 60;

    int32_t lineWidth = 3;

    TpVector<int32_t> colorList;

    TpFont titleTextFont;
    TpFont percentFont;
};

TpHumidityWidget::TpHumidityWidget(TpChildWidget *parent) : TpChildWidget(parent)
{
    TpHumidityWidgetData *tempData = new TpHumidityWidgetData();
    data_ = tempData;

    tempData->colorList.emplace_back(_RGB(34, 132, 255));
    tempData->colorList.emplace_back(_RGB(204, 242, 252));

    tempData->titleTextFont.setFontColor(_RGB(54, 59, 100), _RGB(54, 59, 100));
    tempData->titleTextFont.setFontSize(13);

    tempData->percentFont.setFontColor(_RGB(54, 59, 100), _RGB(54, 59, 100));
    tempData->percentFont.setFontSize(13);
}

TpHumidityWidget::~TpHumidityWidget()
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    if (tempData)
    {
        delete tempData;
        tempData = nullptr;
        data_ = nullptr;
    }
}

void TpHumidityWidget::setTitle(const TpString &title)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->titleText = title;
}

TpString TpHumidityWidget::title() const
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    return tempData->titleText;
}

void TpHumidityWidget::setRange(int32_t minTemp, int32_t maxTemp)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->minValue = minTemp;
    tempData->maxValue = maxTemp;

    if (tempData->minValue >= tempData->maxValue)
        tempData->minValue = tempData->maxValue - 1;

    update();
}

void TpHumidityWidget::setValue(int32_t currentTemp)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->curValue = currentTemp;

    if (tempData->curValue > tempData->maxValue)
        tempData->curValue = tempData->maxValue;
    if (tempData->curValue < tempData->minValue)
        tempData->curValue = tempData->minValue;

    update();
}

int32_t TpHumidityWidget::value() const
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    return tempData->curValue;
}

void TpHumidityWidget::setColorList(const TpVector<int32_t> &colorList)
{
    if (colorList.size() == 0)
        return;
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->colorList = colorList;
}

void TpHumidityWidget::setLineWidth(int32_t width)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->lineWidth = width;
    update();
}

int32_t TpHumidityWidget::lineWidth() const
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    return tempData->lineWidth;
}

bool TpHumidityWidget::onPaintEvent(TpObjectPaintEvent *event)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);

    int32_t startY = 10;

    int32_t humidityWidth = width() * 0.8;
    int32_t humidityHeight = height() * 0.75;

    TpCanvas *painter = event->canvas();

    // painter->box(0, 0, width(), height(), _RGB(255, 0, 0));

    int32_t arcRadius = humidityWidth / 2.0;

    // 先绘制半圆弧，X位置窗口中心位置
    int32_t arcX = (width() - humidityWidth) / 2.0 + arcRadius;
    int32_t arcY = startY + humidityHeight - arcRadius;

    painter->arc(arcX, arcY, arcRadius, 1, 179, _RGB(116, 121, 150), tempData->lineWidth);

    // 绘制左侧曲线（从圆弧左端点到水滴底部尖点）
    // 顶点坐标（arcX, startY）
    // 绘制左侧曲线：从圆弧左端点到顶点
    ItpPoint leftCPoint1;
    leftCPoint1.x = arcX - arcRadius;       // 控制点1 X坐标
    leftCPoint1.y = arcY - arcRadius * 0.6; // 控制点1 Y坐标

    ItpPoint leftCPoint2;
    leftCPoint2.x = arcX - arcRadius * 0.6;   // 控制点2 X坐标
    leftCPoint2.y = startY + arcRadius * 0.5; // 控制点2 Y坐标

    painter->cubicTo(
        arcX - arcRadius, arcY,       // 起点：圆弧左端点
        leftCPoint1.x, leftCPoint1.y, // 控制点1
        leftCPoint2.x, leftCPoint2.y, // 控制点2
        arcX, startY,                 // 终点：顶点
        _RGB(116, 121, 150), tempData->lineWidth);

    // 绘制右侧曲线：从顶点到圆弧右端点
    ItpPoint rightCPoint1;
    rightCPoint1.x = arcX + arcRadius * 0.6;   // 控制点1 X坐标
    rightCPoint1.y = startY + arcRadius * 0.5; // 控制点1 Y坐标

    ItpPoint rightCPoint2;
    rightCPoint2.x = arcX + arcRadius;       // 控制点2 X坐标
    rightCPoint2.y = arcY - arcRadius * 0.6; // 控制点2 Y坐标

    painter->cubicTo(
        arcX, startY,                   // 起点：顶点
        rightCPoint1.x, rightCPoint1.y, // 控制点1
        rightCPoint2.x, rightCPoint2.y, // 控制点2
        arcX + arcRadius, arcY,         // 终点：圆弧右端点
        _RGB(116, 121, 150), tempData->lineWidth);

    // 绘制百分比文本
    float percentValue = 100.0 * (tempData->curValue - tempData->minValue) / (tempData->maxValue - tempData->minValue);
    tempData->percentFont.setText(TpString::number(int32_t(percentValue)) + "%");
    int32_t percentTextWidth = tempData->percentFont.pixelWidth();
    int32_t percentTextHeight = tempData->percentFont.pixelHeight();
    int32_t percentTextX = (width() - percentTextWidth) / 2.0;
    // int32_t percentTextY = height() - percentTextHeight;
    painter->renderText(tempData->percentFont, percentTextX, arcY);

    // 绘制标题文本
    tempData->titleTextFont.setText(tempData->titleText);
    int32_t titleTextWidth = tempData->titleTextFont.pixelWidth();
    int32_t titleTextHeight = tempData->titleTextFont.pixelHeight();
    int32_t titleTextX = (width() - titleTextWidth) / 2.0;
    int32_t titleTextY = height() - titleTextHeight;
    painter->renderText(tempData->titleTextFont, titleTextX, titleTextY);

    return true;
}

SMART_DEVICE_GUI_NAMESPACE_END
