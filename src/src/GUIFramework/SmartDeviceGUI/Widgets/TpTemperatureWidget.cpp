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

    tempData->colorList.emplace_back(_RGB(123, 1, 1));
    tempData->colorList.emplace_back(_RGB(246, 130, 132));
    tempData->colorList.emplace_back(_RGB(134, 226, 252));
    tempData->colorList.emplace_back(_RGB(160, 234, 255));
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

    if (tempData->curTemperature > tempData->maxTemperature)
        tempData->curTemperature = tempData->maxTemperature;
    if (tempData->curTemperature < tempData->minTemperature)
        tempData->curTemperature = tempData->minTemperature;

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
    int32_t circleCenterX = rectangleX + rectangleWidth / 2.0;
    int32_t circleCenterY = rectangleY + rectangleHeight;
    int32_t circleRadius = thermometerWidth / 2.0;

    painter->arc(circleCenterX, circleCenterY, circleRadius, -45, 225, _RGB(116, 121, 150), tempData->lineWidth);

    // 绘制上半部分圆角矩形;使用遮罩圆角box绘制，实现遮挡圆和矩形交接处的线条  - tempData->lineWidth * 2
    HollowMask hollowMask;
    hollowMask.addRectHollow(HollowMask::RectHollow(ItpRect(tempData->lineWidth, tempData->lineWidth, rectangleWidth - tempData->lineWidth * 2, rectangleY + rectangleHeight), rectangleWidth / 2.0));
    hollowMask.addCircleHollow(circleCenterX - rectangleX, rectangleHeight, circleRadius - tempData->lineWidth + 1);
    painter->roundedBox(rectangleX, rectangleY, rectangleX + rectangleWidth, rectangleY + rectangleHeight, rectangleWidth / 2.0, _RGB(116, 121, 150), hollowMask);

    // 根据当前温度值绘制填充矩形
    // 构建渐变效果
    int32_t colorListSize = tempData->colorList.size();

    TpLinearGradient lineGradient;
    for (int i = 0; i < colorListSize; ++i)
    {
        lineGradient.setColorAt(i / (colorListSize - 1), tempData->colorList.at(i));
    }

    if (colorListSize > 0)
    {
        lineGradient.setStart(circleCenterX, circleCenterY + circleRadius);
        lineGradient.setFinalStop(circleCenterX, rectangleY);
        painter->setGradient(&lineGradient);
    }

    // 计算填充高度
    if (tempData->curTemperature == tempData->maxTemperature)
    {
    }
    else if (tempData->curTemperature == tempData->minTemperature)
    {
    }
    else
    {
        float tempPercent = 1.0 * (tempData->curTemperature - tempData->minTemperature) / (tempData->maxTemperature - tempData->minTemperature);
        int32_t valueHeight = tempPercent * thermometerHeight;

        int32_t fillCircleRadius = circleRadius - tempData->lineWidth + 1;
        // 绘制圆形
        HollowMask hallowMask;

        if (valueHeight == circleRadius)
        {
            painter->filledPie(circleCenterX, circleCenterY, circleRadius - tempData->lineWidth + 1, 0, 180, _RGB(0, 0, 0));
        }
        else if (valueHeight < circleRadius)
        {
            // 先计算弦与圆相交的两个点坐标
            ItpPoint leftPoint, rightPoint;

            // 弦的一半长度
            float lineW = std::sqrt(std::pow(fillCircleRadius, 2) - std::pow(fillCircleRadius - valueHeight, 2));

            leftPoint.x = circleCenterX - lineW;
            leftPoint.y = circleCenterY + fillCircleRadius - valueHeight;

            rightPoint.x = circleCenterX + lineW;
            rightPoint.y = circleCenterY + fillCircleRadius - valueHeight;

            // 镂空一个三角形
            HollowMask::PolygonHollow polygonHollow;
            polygonHollow.posintList.emplace_back(ItpPoint(circleCenterX, circleCenterY));
            polygonHollow.posintList.emplace_back(ItpPoint(leftPoint.x, leftPoint.y));
            polygonHollow.posintList.emplace_back(ItpPoint(rightPoint.x, rightPoint.y));
            hallowMask.addPolygonHollow(polygonHollow);

            // 余弦定理 计算弦两个顶点的角度
            double cos_value = (std::pow(lineW, 2) + std::pow(fillCircleRadius - valueHeight, 2) - std::pow(fillCircleRadius, 2)) / (2.0 * lineW * (fillCircleRadius - valueHeight));
            // 防止浮点误差超出范围
            if (cos_value < -1.0)
                cos_value = -1.0;
            else if (cos_value > 1.0)
                cos_value = 1.0;

            float angle = std::acos(cos_value) * 180.0 / M_PI;

            painter->filledPie(circleCenterX, circleCenterY, fillCircleRadius, 90 - angle, 90 + angle, _RGB(0, 0, 0), hallowMask);
        }
        else if (valueHeight < (circleRadius * 2))
        {
            // 高度大于半径但小于直径
        }
        else
        {
            painter->filledCircle(circleCenterX, circleCenterY, circleRadius - tempData->lineWidth + 1, _RGB(0, 0, 0));

            // 绘制圆角矩形
        }

        // painter->filledPie(circleCenterX, circleCenterY, circleRadius - tempData->lineWidth + 1, , , _RGB(0, 0, 0), hallowMask);
        // painter->filledCircle(circleCenterX, circleCenterY, circleRadius - tempData->lineWidth + 1, _RGB(0, 0, 0), hallowMask);
    }

    return true;
}

bool TpTemperatureWidget::onResizeEvent(TpObjectResizeEvent *event)
{
    TpChildWidget::onResizeEvent(event);

    return true;
}

SMART_DEVICE_GUI_NAMESPACE_END
