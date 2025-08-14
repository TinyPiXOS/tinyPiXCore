#include "TpSemiCircleProgressBar.h"
#include "tpCanvas.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

struct TpSemiCircleProgressBarData
{
    int32_t minValue = 0;
    int32_t maxValue = 100;
    int32_t curValue = 0;

    TpSemiCircleProgressBarData()
    {
    }
};

TpSemiCircleProgressBar::TpSemiCircleProgressBar(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    TpSemiCircleProgressBarData *progressData = new TpSemiCircleProgressBarData();
    data_ = progressData;
}

TpSemiCircleProgressBar::~TpSemiCircleProgressBar()
{
    TpSemiCircleProgressBarData *progressData = static_cast<TpSemiCircleProgressBarData *>(data_);
    if (progressData)
    {
        delete progressData;
        progressData = nullptr;
    }
}

void TpSemiCircleProgressBar::setRange(const int32_t &min, const int32_t &max)
{
    TpSemiCircleProgressBarData *progressData = static_cast<TpSemiCircleProgressBarData *>(data_);
    progressData->minValue = min;
    progressData->maxValue = max;

    if (progressData->minValue >= progressData->maxValue)
        progressData->minValue = progressData->maxValue - 10;
}

void TpSemiCircleProgressBar::setValue(const int32_t &value)
{
    TpSemiCircleProgressBarData *progressData = static_cast<TpSemiCircleProgressBarData *>(data_);
    progressData->curValue = value;

    if (progressData->curValue > progressData->maxValue)
        progressData->curValue = progressData->maxValue;

    if (progressData->curValue < progressData->minValue)
        progressData->curValue = progressData->minValue;
}

int32_t TpSemiCircleProgressBar::value()
{
    TpSemiCircleProgressBarData *progressData = static_cast<TpSemiCircleProgressBarData *>(data_);
    return progressData->curValue;
}

bool TpSemiCircleProgressBar::onPaintEvent(tpObjectPaintEvent *event)
{
    // tpChildWidget::onPaintEvent(event);
    tpCanvas *painter = event->canvas();
    painter->arc(50, 50, 40, 140, 40, _RGBA(204, 179, 230, 204), 15, true);
    // painter->arc(50, 50, 40, 315, 225, _RGBA(204, 179, 230, 204), 15);

    return true;
}

SMART_DEVICE_GUI_NAMESPACE_END
