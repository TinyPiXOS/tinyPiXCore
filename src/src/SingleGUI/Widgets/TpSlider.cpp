#include "TpSlider.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpRange.h"
#include "TpRect.h"

struct TpSliderData
{
    int32_t maxValue = 100;
    int32_t minValue = 0;
    double value = 0;

    TpSlider::SliderDirect direct = TpSlider::Horizon;

    // 顶点矩形区域
    bool isPressVertex = false;
    TpRect vertexRect;
    TpPoint pressPoint;

    // 一个间隔值对应的像素
    double valuePx = 0;

    // 是否正在拖拽调整进度，拖拽过程不响应setValue事件
    bool isDrag = false;
};

TpSlider::TpSlider(TpChildWidget *parent, const SliderDirect &direct)
    : TpChildWidget(parent)
{
    TpSliderData *sliderData = new TpSliderData();
    data_ = sliderData;

    refreshBaseCss();
}

TpSlider::~TpSlider()
{
    TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

    if (sliderData)
    {
        delete sliderData;
        sliderData = nullptr;
        data_ = nullptr;
    }
}

void TpSlider::setRange(const int32_t &min, const int32_t &max)
{
    TpSliderData *sliderData = static_cast<TpSliderData *>(data_);
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

void TpSlider::setDirection(const SliderDirect &direct)
{
    TpSliderData *sliderData = static_cast<TpSliderData *>(data_);
    sliderData->direct = direct;

    update();
}

int32_t TpSlider::value()
{
    TpSliderData *sliderData = static_cast<TpSliderData *>(data_);
    return sliderData->value;
}

void TpSlider::setValue(const int32_t &value)
{
    TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

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

bool TpSlider::onMousePressEvent(TpMouseEvent *event)
{
    TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

    sliderData->isPressVertex = false;
    sliderData->isDrag = false;

    if (event->button() != BUTTON_LEFT)
        return true;

    TpPoint mousePoint = event->pos();

    if (sliderData->vertexRect.contains(mousePoint))
    {
        if (sliderData->direct == TpSlider::Horizon)
            sliderData->valuePx = 1.0 * (sliderData->maxValue - sliderData->minValue) / width();
        else
            sliderData->valuePx = 1.0 * (sliderData->maxValue - sliderData->minValue) / height();

        sliderData->pressPoint = mousePoint;
        sliderData->isPressVertex = true;
        sliderData->isDrag = true;
    }
    else
    {
        // 根据点击位置，自动切换值到点击位置
        if (sliderData->direct == TpSlider::Horizon)
        {
            // 点击位置占宽度的百分比
            float pressPointPercent = 1.0 * mousePoint.x() / width();
            float pressValue = pressPointPercent * (sliderData->maxValue - sliderData->minValue) + sliderData->minValue;

            if (pressValue != sliderData->value)
                valueChanged.emit(pressValue);

            setValue(pressValue);

            update();
        }
        else
        {
            // 点击位置占高度度的百分比
            float pressPointPercent = 1.0 * mousePoint.y() / height();
            float pressValue = pressPointPercent * (sliderData->maxValue - sliderData->minValue) + sliderData->minValue;

            if (pressValue != sliderData->value)
                valueChanged.emit(pressValue);

            setValue(pressValue);

            update();
        }
    }

    return true;
}

bool TpSlider::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

    sliderData->isPressVertex = false;
    sliderData->isDrag = false;

    return true;
}

bool TpSlider::onMouseMoveEvent(TpMouseEvent *event)
{
    TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

    if (sliderData->isPressVertex)
    {
        TpPoint curMotionPoint = event->pos();

        int32_t offsetPx = 0;
        if (sliderData->direct == TpSlider::Horizon)
        {
            offsetPx = curMotionPoint.x() - sliderData->pressPoint.x();
        }
        else
        {
            offsetPx = sliderData->pressPoint.y() - curMotionPoint.y();
        }

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

bool TpSlider::onLeaveEvent(TpLeaveEvent *event)
{
    // TpSliderData *sliderData = static_cast<TpSliderData *>(data_);
    // sliderData->isPressVertex = false;
    return true;
}

bool TpSlider::onPaintEvent(TpPaintEvent *event)
{
    TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

    // TpChildWidget::onPaintEvent(event);
    tpShared<TpCssData> curCssData = currentStatusCss();

    TpPainter *painter = event->painter();

    // 整体高度、宽度；分成4份。进度条1份，顶点2份，浅色顶点4份
    uint32_t bgWidth = width();
    uint32_t bgHeight = height();
    uint32_t bgX = 0;
    uint32_t bgY = 0;

    // 不能用父类绘制，绘制背景色
    TpRect rect = event->rect();
    if (objectType() == TP_FLOAT_OBJECT)
    {
        if ((curCssData->backgroundColor() & 0xff) != 0xff)
        {
            painter->erase();
        }
    }

    if (sliderData->direct == TpSlider::Horizon)
    {
        bgHeight = height() / 4.0;
        bgY = (height() - bgHeight) / 2.0;
        painter->setBrush(TpBrush(curCssData->backgroundColor()));
        painter->drawRect(0, bgY, rect.width(), bgHeight, roundCorners());
    }
    else
    {
        bgWidth = width() / 4.0;
        bgX = (width() - bgWidth) / 2.0;
        painter->setBrush(TpBrush(curCssData->backgroundColor()));
        painter->drawRect(bgX, 0, bgWidth, rect.height(), roundCorners());
    }

    // 填充宽度
    uint32_t valueWidth = 0;
    double valuePercent = (1.0 * sliderData->value - sliderData->minValue) / (1.0 * sliderData->maxValue - sliderData->minValue);

    int32_t subColor = curCssData->subColor();
    int32_t lightSubColor = _RGBA(_R(subColor), _G(subColor), _B(subColor), 90);

    // 圆形顶点半径
    uint32_t circleRadius = 0;

    if (sliderData->direct == TpSlider::Horizon)
    {
        circleRadius = height() / 4.0 * 2.0 / 2.0;

        valueWidth = valuePercent * width();
        if (valueWidth != 0)
        {
            painter->setBrush(TpBrush(curCssData->subColor()));
            painter->drawRect(0, bgY, valueWidth, bgHeight, roundCorners());
        }

        int32_t circleX = valueWidth;

        if (circleX == 0)
        {
            circleX = circleRadius;
        }
        else if (circleX == width())
        {
            circleX = width() - circleRadius;
        }
        else
        {
        }

        // 绘制淡色圆形顶点
        painter->setBrush(TpBrush(lightSubColor));
        painter->drawEllipse(circleX, height() / 2.0, height() / 2.0, height() / 2.0);

        // 绘制圆形顶点
        painter->setBrush(TpBrush(subColor));
        painter->drawEllipse(circleX, height() / 2.0, circleRadius, circleRadius);

        // 记录顶点区域
        sliderData->vertexRect.setRect(circleX - circleRadius, height() / 2.0 - circleRadius,
                                       circleRadius * 2, circleRadius * 2);
    }
    else
    {
        circleRadius = width() / 4.0 * 2.0 / 2.0;

        valueWidth = valuePercent * height();
        if (valueWidth != 0)
        {
            painter->setBrush(TpBrush(subColor));
            painter->drawRect(bgX, height() - valueWidth, bgWidth, valueWidth, roundCorners());
        }

        int32_t circleY = height() - valueWidth;

        if (circleY == 0)
        {
            circleY = circleRadius;
        }
        else if (circleY == height())
        {
            circleY = height() - circleRadius;
        }
        else
        {
        }

        // 绘制淡色圆形顶点
        painter->setBrush(TpBrush(lightSubColor));
        painter->drawEllipse(width() / 2.0, circleY, width() / 2.0, width() / 2.0);

        // 绘制圆形顶点
        painter->setBrush(TpBrush(subColor));
        painter->drawEllipse(width() / 2.0, circleY, circleRadius, circleRadius);

        // 记录顶点区域
        sliderData->vertexRect.setRect(width() / 2.0 - circleRadius, circleY - circleRadius,
                                       circleRadius * 2, circleRadius * 2);
    }

    return true;
}
