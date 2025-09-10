#include "TpHumidityWidget.h"
#include "TpCanvas.h"
#include "TpLinearGradient.h"
#include "TpFont.h"

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

    tempData->colorList.emplace_back(_RGB(204, 242, 252));
    tempData->colorList.emplace_back(_RGB(34, 132, 255));

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

bool TpHumidityWidget::onPaintEvent(TpPaintEvent *event)
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

    // // 控制点1坐标
    TpPoint leftCPoint1(arcX - arcRadius, arcY - arcRadius * 0.6);
    // 控制点2坐标
    TpPoint leftCPoint2(arcX - arcRadius * 0.6, startY + arcRadius * 0.5);

    painter->cubicTo(
        arcX - arcRadius, arcY,           // 起点：圆弧左端点
        leftCPoint1.x(), leftCPoint1.y(), // 控制点1
        leftCPoint2.x(), leftCPoint2.y(), // 控制点2
        arcX, startY,                     // 终点：顶点
        _RGB(116, 121, 150), tempData->lineWidth);

    // 绘制右侧曲线：从顶点到圆弧右端点
    // // 控制点1坐标
    TpPoint rightCPoint1(arcX + arcRadius * 0.6, startY + arcRadius * 0.5);
    // 控制点2坐标
    TpPoint rightCPoint2(arcX + arcRadius, arcY - arcRadius * 0.6);

    painter->cubicTo(
        arcX, startY,                       // 起点：顶点
        rightCPoint1.x(), rightCPoint1.y(), // 控制点1
        rightCPoint2.x(), rightCPoint2.y(), // 控制点2
        arcX + arcRadius, arcY,             // 终点：圆弧右端点
        _RGB(116, 121, 150), tempData->lineWidth);

    // 计算填充值百分比
    float percentValue = 1.0 * (tempData->curValue - tempData->minValue) / (tempData->maxValue - tempData->minValue);

    // 绘制填充渐变
    // 构建渐变效果
    int32_t colorListSize = tempData->colorList.size();

    TpLinearGradient lineGradient;
    for (int i = 0; i < colorListSize; ++i)
    {
        lineGradient.setColorAt(1.0 * i / (colorListSize - 1), tempData->colorList.at(i));
    }

    if (colorListSize > 0)
    {
        lineGradient.setStart(arcX, arcY + arcRadius);
        lineGradient.setFinalStop(arcX, startY);
        painter->setGradient(&lineGradient);
    }

    // 水滴总高度，用于计算填充绘制高度
    int32_t humidituAllHeight = startY + arcY + arcRadius;
    float valueHeight = percentValue * humidituAllHeight;
    int32_t fillCircleRadius = arcRadius - tempData->lineWidth + 1;

    if (tempData->curValue <= tempData->minValue)
    {
        // 小于等于最小值；什么也不绘制
    }
    else if (tempData->curValue >= tempData->maxValue)
    {
        // 大于等于最大值，绘制完全填充
        painter->filledPie(arcX, arcY, fillCircleRadius, 0, 180, _RGB(0, 0, 0));

        // 绘制上半区域
    }
    else
    {
        if (valueHeight < arcRadius)
        {
            // 先计算弦与圆相交的两个点坐标
            TpPoint leftPoint, rightPoint;

            // 弦的一半长度
            float lineW = 0;

            // 镂空一个三角形
            HollowMask hollowMask;
            HollowMask::PolygonHollow polygonHollow;

            float startAngle = 0;
            float endAngle = 0;
            double cosValue = 0;

            lineW = std::sqrt(std::pow(fillCircleRadius, 2) - std::pow(fillCircleRadius - valueHeight, 2));

            leftPoint.setX(arcX - lineW);
            leftPoint.setY(arcY + fillCircleRadius - valueHeight);

            rightPoint.setX(arcX + lineW);
            rightPoint.setY(arcY + fillCircleRadius - valueHeight);

            // 在扇形基础上镂空一个三角形
            polygonHollow.posintList.emplace_back(TpPoint(arcX, arcY));
            polygonHollow.posintList.emplace_back(TpPoint(leftPoint.x(), leftPoint.y()));
            polygonHollow.posintList.emplace_back(TpPoint(rightPoint.x(), rightPoint.y()));
            hollowMask.addPolygonHollow(polygonHollow);

            // 余弦定理 计算弦两个顶点的角度
            cosValue = (std::pow(lineW, 2) + std::pow(fillCircleRadius - valueHeight, 2) - std::pow(fillCircleRadius, 2)) / (2.0 * lineW * (fillCircleRadius - valueHeight));
            float angle = std::acos(cosValue) * 180.0 / M_PI;
            startAngle = 90 - angle;
            endAngle = 90 + angle;

            // 防止浮点误差超出范围
            if (cosValue < -1.0)
                cosValue = -1.0;
            else if (cosValue > 1.0)
                cosValue = 1.0;

            painter->filledPie(arcX, arcY, fillCircleRadius, startAngle, endAngle, _RGB(0, 0, 0), hollowMask);
        }
        else
        {
            // 高度末尾处于上半区域
            // 先绘制半圆
            painter->filledPie(arcX, arcY, fillCircleRadius, 0, 180, _RGB(0, 0, 0));

            if (valueHeight > arcRadius)
            {
            }
        }
    }

    // 重置渐变
    painter->setGradient(nullptr);

    // 绘制百分比文本
    tempData->percentFont.setText(TpString::number(int32_t(percentValue * 100)) + "%");
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
