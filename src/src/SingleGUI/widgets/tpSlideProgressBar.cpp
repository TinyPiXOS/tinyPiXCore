#include "tpSlideProgressBar.h"
#include "tpSurface.h"
#include "tpDisplay.h"
#include "tpCanvas.h"

struct tpSlideProgressBarData
{
    int32_t minValue = 0;
    int32_t maxValue = 0;
    double curValue = 0;

    tpShared<tpSurface> iconSurface;

    ItpPoint pressPoint;
    bool mouseLeftPress = false;

    tpSlideProgressBarData() : iconSurface(tpMakeShared<tpSurface>())
    {
    }

    ~tpSlideProgressBarData()
    {
    }
};

tpSlideProgressBar::tpSlideProgressBar(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    data_ = new tpSlideProgressBarData();

    refreshBaseCss();

    setVisible(true);
}

tpSlideProgressBar::~tpSlideProgressBar()
{
    tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);
    if (progressData)
    {
        delete progressData;
        progressData = nullptr;
        data_ = nullptr;
    }
}

void tpSlideProgressBar::setIcon(const tpString &iconPath)
{
    tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);
    progressData->iconSurface->fromFile(iconPath);
}

void tpSlideProgressBar::setRange(const int32_t &minValue, const int32_t &maxValue)
{
    tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);

    progressData->minValue = minValue;
    progressData->maxValue = maxValue;

    if (progressData->maxValue <= progressData->minValue)
    {
        progressData->maxValue = progressData->minValue + 1;
    }
}

void tpSlideProgressBar::setValue(const int32_t &value)
{
    tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);
    progressData->curValue = value;

    if (progressData->curValue < progressData->minValue)
        progressData->curValue = progressData->minValue;
    else if (progressData->curValue > progressData->maxValue)
        progressData->curValue = progressData->maxValue;
    else
    {
    }

    onValueChanged.emit(progressData->curValue);

    update();
}

int32_t tpSlideProgressBar::value()
{
    tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);
    return progressData->curValue;
}

void tpSlideProgressBar::setParent(tpObject *parent)
{
    if (parent)
    {
        tpShared<tpCssData> normalCss = enabledCss();
        if (normalCss)
        {
            setRoundCorners(normalCss->roundCorners());
        }
    }

    tpChildWidget::setParent(parent);
}

bool tpSlideProgressBar::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);

    if (event->button() == BUTTON_LEFT)
    {
        progressData->mouseLeftPress = event->state();
        progressData->pressPoint = event->globalPos();
    }

    return true;
}

bool tpSlideProgressBar::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);

    if (event->button() == BUTTON_LEFT)
    {
        progressData->mouseLeftPress = event->state();
    }

    return true;
}

bool tpSlideProgressBar::onMouseMoveEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseMoveEvent(event);

    tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);
    if (progressData->mouseLeftPress)
    {
        ItpPoint curPos = event->globalPos();
        int32_t offsetX = curPos.x - progressData->pressPoint.x;

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

        std::cout << "progressData->curValue  " << progressData->curValue << std::endl;
    }
    return true;
}

bool tpSlideProgressBar::onLeaveEvent(tpObjectLeaveEvent *event)
{
    // tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);
    // progressData->mouseLeftPress = false;

    tpChildWidget::onLeaveEvent(event);

    return true;
}

bool tpSlideProgressBar::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);

    return true;
}

bool tpSlideProgressBar::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    tpSlideProgressBarData *progressData = static_cast<tpSlideProgressBarData *>(data_);

    tpShared<tpCssData> curCssData = enabledCss();

    tpCanvas *paintCanvas = event->canvas();

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

        // paintCanvas->roundedBox(2, 2,  valueWidth - 4, height() - 4, minRad, curCssData->color());
        paintCanvas->roundedBox(2, 2, 2 + valueWidth, height() - 4, minRad, curCssData->color());
    }

    // 绘制图标
    if (progressData->iconSurface->hasSurface())
    {
        tpShared<tpSurface> drawSurface = progressData->iconSurface->scaled(curCssData->iconSize(), curCssData->iconSize());
        if (!drawSurface)
            return true;

        int32_t imageWidth = drawSurface->width();
        int32_t imageHeight = drawSurface->height();

        tpCanvas *canvas = event->canvas();

        int32_t cy = (rect().h - imageHeight) / 2;

        paintCanvas->paintSurface(cy, cy, drawSurface);
    }

    return true;
}

void tpSlideProgressBar::onThemeChangeEvent(tpThemeChangeEvent *event)
{
    update();
}
