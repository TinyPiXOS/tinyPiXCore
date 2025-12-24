#include "TpSliderProgressBar.h"
#include <TpEvent.h>
#include <TpBrush.h>
#include <TpPainter.h>

struct TpSliderProgressBarData
{
    int32_t maxValue = 100;
    int32_t minValue = 0;
    double value = 0;

    // 顶点矩形区域
    bool isPressVertex = false;
    TpRect vertexRect;
    TpPoint pressPoint;

    // 一个间隔值对应的像素
    double valuePx = 0;

    // 是否正在拖拽调整进度，拖拽过程不响应setValue事件
    bool isDrag = false;
};

TpSliderProgressBar::TpSliderProgressBar(TpWidget *parent) : TpWidget(parent)
{
    TpSliderProgressBarData *sliderData = new TpSliderProgressBarData();
    data_ = sliderData;

    refreshBaseCss();
}

TpSliderProgressBar::~TpSliderProgressBar()
{
    TpSliderProgressBarData *sliderData = static_cast<TpSliderProgressBarData *>(data_);

    if (sliderData)
    {
        delete sliderData;
        sliderData = nullptr;
        data_ = nullptr;
    }
}

void TpSliderProgressBar::setRange(const int32_t &min, const int32_t &max)
{
    TpSliderProgressBarData *sliderData = static_cast<TpSliderProgressBarData *>(data_);
    sliderData->minValue = min;
    sliderData->maxValue = max;

    if (sliderData->maxValue < sliderData->minValue)
        sliderData->maxValue = sliderData->minValue + 1;

    if (sliderData->value < sliderData->minValue)
        sliderData->value = sliderData->minValue;
    else if (sliderData->value > sliderData->maxValue)
        sliderData->value = sliderData->maxValue;
    else
    {
    }

    rangeChanged.emit(sliderData->minValue, sliderData->maxValue);

    update();
}

int32_t TpSliderProgressBar::value()
{
    TpSliderProgressBarData *sliderData = static_cast<TpSliderProgressBarData *>(data_);
    return sliderData->value;
}

void TpSliderProgressBar::setValue(const int32_t &value)
{
    TpSliderProgressBarData *sliderData = static_cast<TpSliderProgressBarData *>(data_);
    if (sliderData->isDrag)
        return;

    sliderData->value = value;

    if (sliderData->value < sliderData->minValue)
        sliderData->value = sliderData->minValue;
    else if (sliderData->value > sliderData->maxValue)
        sliderData->value = sliderData->maxValue;
    else
    {
    }

    update();
}

bool TpSliderProgressBar::onMousePressEvent(TpMouseEvent *event)
{
    TpSliderProgressBarData *sliderData = static_cast<TpSliderProgressBarData *>(data_);

    sliderData->isPressVertex = false;
    sliderData->isDrag = false;

    if (event->button() != BUTTON_LEFT)
        return true;

    TpPoint mousePoint = event->pos();

    if (sliderData->vertexRect.contains(mousePoint))
    {
        sliderData->valuePx = 1.0 * (sliderData->maxValue - sliderData->minValue) / width();

        sliderData->pressPoint = mousePoint;
        sliderData->isPressVertex = true;
        sliderData->isDrag = true;
    }
    else
    {
        // 根据点击位置，自动切换值到点击位置
        // 点击位置占宽度的百分比
        float pressPointPercent = 1.0 * mousePoint.x() / width();
        float pressValue = pressPointPercent * (sliderData->maxValue - sliderData->minValue) + sliderData->minValue;

        if (pressValue != sliderData->value)
            valueChanged.emit(pressValue);

        setValue(pressValue);

        update();
    }

    return true;
}

bool TpSliderProgressBar::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpSliderProgressBarData *sliderData = static_cast<TpSliderProgressBarData *>(data_);

    sliderData->isPressVertex = false;
    sliderData->isDrag = false;

    update();

    return true;
}

bool TpSliderProgressBar::onMouseMoveEvent(TpMouseEvent *event)
{
    TpSliderProgressBarData *sliderData = static_cast<TpSliderProgressBarData *>(data_);

    if (sliderData->isPressVertex)
    {
        TpPoint curMotionPoint = event->pos();

        int32_t offsetPx = 0;
        offsetPx = curMotionPoint.x() - sliderData->pressPoint.x();

        // std::cout << " offsetPx  " << offsetPx << std::endl;

        if (std::abs(offsetPx) >= sliderData->valuePx)
        {
            int32_t oldValue = sliderData->value;

            sliderData->value += (offsetPx * sliderData->valuePx);
            if (sliderData->value > sliderData->maxValue)
            {
                sliderData->value = sliderData->maxValue;
            }
            else if (sliderData->value < sliderData->minValue)
            {
                sliderData->value = sliderData->minValue;
            }
            else
            {
            }

            // std::cout << " sliderData->value  " << sliderData->value << std::endl;

            int32_t newValue = sliderData->value;
            if (newValue != oldValue)
            {
                valueChanged.emit(newValue);
            }
        }

        sliderData->pressPoint = curMotionPoint;

        update();
    }

    return true;
}

bool TpSliderProgressBar::onPaintEvent(TpPaintEvent *event)
{
    TpSliderProgressBarData *sliderData = static_cast<TpSliderProgressBarData *>(data_);

    TpWidget::onPaintEvent(event);

    tpShared<TpCssData> curCssData = currentStatusCss();

    TpPainter *painter = event->painter();

    // 圆形顶点半径
    int32_t circleRadius = (height() * 0.8) / 2.0;
    int32_t circleMargin = (height() - circleRadius * 2) / 2.0;

    // 计算当前宽度百分比
    double valuePercent = (1.0 * sliderData->value - sliderData->minValue) / (1.0 * sliderData->maxValue - sliderData->minValue);
    uint32_t valueWidth = valuePercent * (width() - circleRadius * 2 - circleMargin * 2);

    // 绘制填充色
    // if (valueWidth != 0)
    {
        painter->setBrush(TpBrush(curCssData->subColor()));
        painter->drawRect(0, 0, circleRadius * 2 + circleMargin * 2 + valueWidth, height(), roundCorners());
    }

    int32_t circleX = valueWidth + circleRadius + circleMargin;
    if (circleX <= (circleRadius + circleMargin))
    {
        circleX = circleRadius + circleMargin;
    }

    // 绘制圆形顶点
    painter->setBrush(TpBrush(curCssData->color()));
    painter->drawEllipse(circleX, height() / 2.0, circleRadius, circleRadius);

    // 记录顶点区域
    sliderData->vertexRect.setRect(circleX - circleRadius, height() / 2.0 - circleRadius,
                                   circleRadius * 2, circleRadius * 2);

    return true;
}
