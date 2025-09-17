#include "TpTemperatureWidget.h"
#include "TpPainter.h"
#include "TpLinearGradient.h"
#include "TpFont.h"

struct TpTemperatureWidgetData
{
    TpString titleText = "温度";

    // 最大、最小温度；当前温度
    int32_t maxTemperature = 40;
    int32_t minTemperature = -10;
    int32_t curTemperature = 30;

    int32_t lineWidth = 3;

    TpVector<int32_t> colorList;

    TpFont curTempFont;
    TpFont minMaxTemptFont;
};

TpTemperatureWidget::TpTemperatureWidget(TpChildWidget *parent) : TpChildWidget(parent)
{
    TpTemperatureWidgetData *tempData = new TpTemperatureWidgetData();
    data_ = tempData;

    tempData->colorList.emplace_back(_RGB(160, 234, 255));
    tempData->colorList.emplace_back(_RGB(134, 226, 252));
    tempData->colorList.emplace_back(_RGB(246, 130, 132));
    tempData->colorList.emplace_back(_RGB(123, 1, 1));

    tempData->curTempFont.setFontColor(_RGB(54, 59, 100), _RGB(54, 59, 100));
    tempData->curTempFont.setFontSize(15);

    tempData->minMaxTemptFont.setFontColor(_RGB(54, 59, 100), _RGB(54, 59, 100));
    tempData->minMaxTemptFont.setFontSize(13);
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

bool TpTemperatureWidget::onPaintEvent(TpPaintEvent *event)
{
    TpTemperatureWidgetData *tempData = static_cast<TpTemperatureWidgetData *>(data_);

    TpPainter *painter = event->painter();

    // painter->box(0, 0, width(), height(), _RGB(255, 0, 0));

    // 宽度三分之一的位置绘制温度条
    int32_t thermometerWidth = width() * 0.27;
    int32_t thermometerHeight = height() * 0.88;

    // 矩形部分宽度
    int32_t rectangleWidth = thermometerWidth * 0.75;
    int32_t rectangleHeight = thermometerHeight * 0.75;

    int32_t rectangleX = (width() - rectangleWidth) / 2.0;
    int32_t rectangleY = (height() - thermometerHeight) / 2.0;

    // 绘制下半部分圆环
    int32_t circleCenterX = rectangleX + rectangleWidth / 2.0;
    int32_t circleCenterY = rectangleY + rectangleHeight;
    int32_t circleRadius = thermometerWidth / 2.0;

    painter->setPen(_RGB(116, 121, 150));
    painter->pen().setWidth(tempData->lineWidth);
    painter->drawArc(circleCenterX, circleCenterY, circleRadius, -45, 225);

    // 绘制上半部分圆角矩形;使用遮罩圆角box绘制，实现遮挡圆和矩形交接处的线条  - tempData->lineWidth * 2
    TpHollowMask hollowMask;
    hollowMask.addRectHollow(TpHollowMask::RectHollow(TpRect(tempData->lineWidth, tempData->lineWidth, rectangleWidth - tempData->lineWidth * 2, rectangleY + rectangleHeight), rectangleWidth / 2.0));
    hollowMask.addCircleHollow(circleCenterX - rectangleX, rectangleHeight, circleRadius - tempData->lineWidth + 1);

    painter->setPen(_RGB(116, 121, 150));
    painter->setBrush(TpBrush(_RGB(116, 121, 150)));
    painter->drawRect(rectangleX, rectangleY, rectangleWidth, rectangleHeight, rectangleWidth / 2.0, hollowMask);

    // 根据当前温度值绘制填充矩形
    // 构建渐变效果
    int32_t colorListSize = tempData->colorList.size();

    TpLinearGradient lineGradient;
    for (int i = 0; i < colorListSize; ++i)
    {
        lineGradient.setColorAt(1.0 * i / (colorListSize - 1), tempData->colorList.at(i));
    }

    if (colorListSize > 0)
    {
        lineGradient.setStart(circleCenterX, circleCenterY + circleRadius);
        lineGradient.setFinalStop(circleCenterX, rectangleY);
        painter->setBrush(TpBrush(&lineGradient));
    }

    // 计算填充高度
    if (tempData->curTemperature >= tempData->maxTemperature)
    {
        // 和最大温度相同；完全填充
        painter->drawEllipse(circleCenterX, circleCenterY, circleRadius - tempData->lineWidth + 1, circleRadius - tempData->lineWidth + 1);
        painter->drawRect(rectangleX + tempData->lineWidth, rectangleY + tempData->lineWidth, rectangleWidth - tempData->lineWidth * 2, rectangleHeight - tempData->lineWidth, rectangleWidth / 2.0);
    }
    else if (tempData->curTemperature <= tempData->minTemperature)
    {
        // 和最小温度相同；什么都不绘制
    }
    else
    {
        float tempPercent = 1.0 * (tempData->curTemperature - tempData->minTemperature) / (tempData->maxTemperature - tempData->minTemperature);
        int32_t valueHeight = tempPercent * thermometerHeight;

        int32_t fillCircleRadius = circleRadius - tempData->lineWidth + 1;
        // 绘制圆形
        TpHollowMask hallowMask;

        if (valueHeight == fillCircleRadius)
        {
            painter->drawPie(circleCenterX, circleCenterY, fillCircleRadius, 0, 180);
        }
        else if (valueHeight < (fillCircleRadius * 2))
        {
            // 先计算弦与圆相交的两个点坐标
            TpPoint leftPoint, rightPoint;

            // 弦的一半长度
            float lineW = 0;

            // 镂空一个三角形
            TpHollowMask::PolygonHollow polygonHollow;

            float startAngle = 0;
            float endAngle = 0;

            double cosValue = 0;
            // 高度小于直径且小于半径；在下半圆
            if (valueHeight < fillCircleRadius)
            {
                lineW = std::sqrt(std::pow(fillCircleRadius, 2) - std::pow(fillCircleRadius - valueHeight, 2));

                leftPoint.setX(circleCenterX - lineW);
                leftPoint.setY(circleCenterY + fillCircleRadius - valueHeight);

                rightPoint.setX(circleCenterX + lineW);
                rightPoint.setY(circleCenterY + fillCircleRadius - valueHeight);

                // 在下半圆时是在扇形基础上镂空一个三角形
                polygonHollow.posintList.emplace_back(TpPoint(circleCenterX, circleCenterY));
                polygonHollow.posintList.emplace_back(TpPoint(leftPoint.x(), leftPoint.y()));
                polygonHollow.posintList.emplace_back(TpPoint(rightPoint.x(), rightPoint.y()));
                hallowMask.addPolygonHollow(polygonHollow);

                // 余弦定理 计算弦两个顶点的角度
                cosValue = (std::pow(lineW, 2) + std::pow(fillCircleRadius - valueHeight, 2) - std::pow(fillCircleRadius, 2)) / (2.0 * lineW * (fillCircleRadius - valueHeight));
                float angle = std::acos(cosValue) * 180.0 / M_PI;
                startAngle = 90 - angle;
                endAngle = 90 + angle;
            }
            else
            {
                lineW = std::sqrt(std::pow(fillCircleRadius, 2) - std::pow(valueHeight - fillCircleRadius, 2));

                leftPoint.setX(circleCenterX - lineW);
                leftPoint.setY(circleCenterY - (valueHeight - fillCircleRadius));

                rightPoint.setX(circleCenterX + lineW);
                rightPoint.setY(circleCenterY - (valueHeight - fillCircleRadius));

                // 在下半圆，是在扇形基础上再绘制一个三角形
                TpVector<TpPoint> addPolygonPointList;
                addPolygonPointList.emplace_back(TpPoint(circleCenterX, circleCenterY));
                addPolygonPointList.emplace_back(TpPoint(leftPoint.x(), leftPoint.y()));
                addPolygonPointList.emplace_back(TpPoint(rightPoint.x(), rightPoint.y()));
                painter->drawPolygon(addPolygonPointList);

                // 大于半径且小于执行，在上半圆
                // 余弦定理 计算弦两个顶点的角度
                cosValue = (std::pow(fillCircleRadius, 2) + std::pow(lineW, 2) - std::pow(fillCircleRadius - valueHeight, 2)) / (2.0 * lineW * fillCircleRadius);
                float angle = std::acos(cosValue) * 180.0 / M_PI;
                startAngle = -angle;
                endAngle = 180 + angle;
            }

            // 防止浮点误差超出范围
            if (cosValue < -1.0)
                cosValue = -1.0;
            else if (cosValue > 1.0)
                cosValue = 1.0;

            painter->drawPie(circleCenterX, circleCenterY, fillCircleRadius, startAngle, endAngle, hallowMask);
        }
        else
        {
            painter->drawEllipse(circleCenterX, circleCenterY, circleRadius - tempData->lineWidth + 1, circleRadius - tempData->lineWidth + 1);

            // 绘制圆角矩形; 矩形终止点与矩形边框相同；起始点Y通过 （值高度 - 圆半径）计算
            int32_t rectLeftY = rectangleY + rectangleHeight - (valueHeight - fillCircleRadius);
            painter->drawRect(rectangleX + tempData->lineWidth, rectLeftY, rectangleWidth - tempData->lineWidth * 2, rectangleHeight, rectangleWidth / 2.0);
        }
    }

    // 重置渐变
    painter->setBrush(TpBrush(tinyPiX::NoBrush));

    // 绘制刻度线
    painter->setPen(_RGB(116, 121, 150));

    // 将矩形区域去除圆角部分，然后五等分，绘制四根刻度线  (rectangleWidth / 2.0)为圆角值
    int32_t singleLineHeight = (rectangleHeight - (rectangleWidth / 2.0 * 2.0)) / 5.0;
    for (int i = 0; i < 4; ++i)
    {
        painter->drawHLine(rectangleX + tempData->lineWidth, rectangleX + rectangleWidth * 0.65, rectangleY + (rectangleWidth / 2.0) + singleLineHeight + singleLineHeight * i);
    }

    // 绘制文本
    // 绘制最高温度
    // 计算最高温度X坐标，左侧空白位置居中
    tempData->minMaxTemptFont.setText(TpString::number(tempData->maxTemperature) + "°");
    int32_t maxTempTextWidth = tempData->minMaxTemptFont.pixelWidth();
    int32_t maxTempTextX = ((circleCenterX - circleRadius) - maxTempTextWidth) / 2.0;
    painter->drawText(tempData->minMaxTemptFont, maxTempTextX, rectangleY * 2);

    // 绘制最低温度
    tempData->minMaxTemptFont.setText(TpString::number(tempData->minTemperature) + "°");
    int32_t minTempTextWidth = tempData->minMaxTemptFont.pixelWidth();
    int32_t minTempTextHeight = tempData->minMaxTemptFont.pixelHeight();
    int32_t minTempTextX = ((circleCenterX - circleRadius) - maxTempTextWidth) / 2.0;
    painter->drawText(tempData->minMaxTemptFont, minTempTextX, circleCenterY + circleRadius - minTempTextHeight);

    // 绘制当前温度值
    tempData->curTempFont.setText(TpString::number(tempData->curTemperature) + "°");
    int32_t curTempTextWidth = tempData->curTempFont.pixelWidth();
    int32_t curTempTextHeight = tempData->curTempFont.pixelHeight();
    int32_t curTempTextX = (circleCenterX + circleRadius) + (width() - (circleCenterX + circleRadius) - maxTempTextWidth) / 2.0;
    int32_t curTempTextY = rectangleY + ((circleCenterY + circleRadius) - rectangleY - curTempTextHeight) / 2.0;
    painter->drawText(tempData->curTempFont, curTempTextX, curTempTextY);

    // 绘制标题文本
    tempData->minMaxTemptFont.setText(tempData->titleText);
    int32_t titleTextWidth = tempData->minMaxTemptFont.pixelWidth();
    int32_t titleTextHeight = tempData->minMaxTemptFont.pixelHeight();
    int32_t titleTextX = (width() - titleTextWidth) / 2.0;
    int32_t titleTextY = height() - titleTextHeight;
    painter->drawText(tempData->minMaxTemptFont, titleTextX, titleTextY);

    return true;
}
