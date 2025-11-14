#include "TpSlideProgressBar.h"
#include "TpImage.h"
#include "SystemInfo/TpDisplay.h"
#include "TpPainter.h"

struct TpSlideProgressBarData
{
    int32_t minValue = 0;
    int32_t maxValue = 0;
    double curValue = 0;

    TpImage iconSurface;

    TpPoint pressPoint;
    bool mouseLeftPress = false;

    TpSlideProgressBarData()
    {
    }

    ~TpSlideProgressBarData()
    {
    }
};

TpSlideProgressBar::TpSlideProgressBar(TpWidget *parent)
    : TpWidget(parent)
{
    data_ = new TpSlideProgressBarData();

    refreshBaseCss();

    setVisible(true);
}

TpSlideProgressBar::~TpSlideProgressBar()
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    if (progressData)
    {
        delete progressData;
        progressData = nullptr;
        data_ = nullptr;
    }
}

void TpSlideProgressBar::setIcon(const TpString &iconPath)
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    progressData->iconSurface.load(iconPath);
}

void TpSlideProgressBar::setRange(const int32_t &minValue, const int32_t &maxValue)
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);

    progressData->minValue = minValue;
    progressData->maxValue = maxValue;

    if (progressData->maxValue <= progressData->minValue)
    {
        progressData->maxValue = progressData->minValue + 1;
    }
}

void TpSlideProgressBar::setValue(const int32_t &value)
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    progressData->curValue = value;

    if (progressData->curValue < progressData->minValue)
        progressData->curValue = progressData->minValue;
    else if (progressData->curValue > progressData->maxValue)
        progressData->curValue = progressData->maxValue;
    else
    {
    }

    // onValueChanged.emit(progressData->curValue);

    update();
}

int32_t TpSlideProgressBar::value()
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    return progressData->curValue;
}

void TpSlideProgressBar::setParent(TpObject *parent)
{
    if (parent)
    {
        tpShared<TpCssData> normalCss = enabledCss();
        if (normalCss)
        {
            setRoundCorners(normalCss->roundCorners());
        }
    }

    TpWidget::setParent(parent);
}

bool TpSlideProgressBar::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);

    if (event->button() == BUTTON_LEFT)
    {
        progressData->mouseLeftPress = event->state();
        progressData->pressPoint = event->globalPos();
    }

    return true;
}

bool TpSlideProgressBar::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpWidget::onMouseRleaseEvent(event);

    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);

    if (event->button() == BUTTON_LEFT)
    {
        progressData->mouseLeftPress = event->state();
    }

    return true;
}

bool TpSlideProgressBar::onMouseMoveEvent(TpMouseEvent *event)
{
    TpWidget::onMouseMoveEvent(event);

    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    if (progressData->mouseLeftPress)
    {
        TpPoint curPos = event->globalPos();
        int32_t offsetX = curPos.x() - progressData->pressPoint.x();

        progressData->pressPoint = curPos;

        // value偏移对应像素  (1.0 / (100 - 0))
        // double curValue = value() + (1.0 * offsetX) / ((width() - 4) / (progressData->maxValue - progressData->minValue));
        // setValue(curValue);

        progressData->curValue += (1.0 * offsetX) / ((width() - 4) / (progressData->maxValue - progressData->minValue));
        if (progressData->curValue < progressData->minValue)
            progressData->curValue = progressData->minValue;
        else if (progressData->curValue > progressData->maxValue)
            progressData->curValue = progressData->maxValue;
        else
        {
        }

        onValueChanged.emit(progressData->curValue);
        update();

        // std::cout << "progressData->curValue  " << progressData->curValue << std::endl;
    }
    return true;
}

bool TpSlideProgressBar::onLeaveEvent(TpLeaveEvent *event)
{
    // TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    // progressData->mouseLeftPress = false;

    TpWidget::onLeaveEvent(event);

    return true;
}

bool TpSlideProgressBar::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);

    return true;
}

bool TpSlideProgressBar::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);

    tpShared<TpCssData> curCssData = enabledCss();

    TpPainter *paintCanvas = event->painter();

    // 绘制填充
    double valuePercent = 1.0 * (progressData->curValue - progressData->minValue) / (progressData->maxValue - progressData->minValue);

    uint32_t valueWidth = valuePercent * (width() - 4);
    if (valueWidth > width())
        valueWidth = width();

    if (valueWidth > 0)
    {
        // uint32_t minRad = (valueWidth > (height() - 4) ? (height() - 4) : valueWidth) * roundCorners();
        double minRad = 1.0 * roundCorners() / height();

        minRad *= (height() - 4 - 2);

        paintCanvas->setPen(curCssData->color());
        paintCanvas->setBrush(TpBrush(curCssData->color()));
        paintCanvas->drawRect(2, 2, valueWidth, height() - 4, minRad);
    }

    // 绘制图标
    if (!progressData->iconSurface.isNull())
    {
        TpImage drawSurface = progressData->iconSurface.scaled(curCssData->iconSize(), curCssData->iconSize());

        int32_t imageWidth = drawSurface.width();
        int32_t imageHeight = drawSurface.height();

        TpPainter *canvas = event->painter();

        int32_t cy = (rect().height() - imageHeight) / 2;

        paintCanvas->drawImage(cy, cy, drawSurface);
    }

    return true;
}

void TpSlideProgressBar::onThemeChangeEvent(TpThemeChangeEvent *event)
{
    update();
}
