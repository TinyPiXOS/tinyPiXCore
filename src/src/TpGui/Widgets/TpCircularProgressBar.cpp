#include "TpCircularProgressBar.h"
#include "TpPainter.h"
#include "TpFont.h"

struct TpCircularProgressBarData
{
    int32_t minValue = 0;
    int32_t maxValue = 100;
    int32_t curValue = 0;

    // TpFont* percentTextFont;
    uint32_t lineWidth = 16;

    TpCircularProgressBarData()
    {
    }
};

TpCircularProgressBar::TpCircularProgressBar(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    TpCircularProgressBarData *progressData = new TpCircularProgressBarData();

    // progressData->percentTextFont = new TpFont();

    data_ = progressData;
}

TpCircularProgressBar::~TpCircularProgressBar()
{
    TpCircularProgressBarData *progressData = static_cast<TpCircularProgressBarData *>(data_);
    if (progressData)
    {
        delete progressData;
        progressData = nullptr;
    }
}

void TpCircularProgressBar::setRange(const int32_t &min, const int32_t &max)
{
    TpCircularProgressBarData *progressData = static_cast<TpCircularProgressBarData *>(data_);
    progressData->minValue = min;
    progressData->maxValue = max;

    if (progressData->minValue >= progressData->maxValue)
        progressData->minValue = progressData->maxValue - 10;

    update();
}

void TpCircularProgressBar::setValue(const int32_t &value)
{
    TpCircularProgressBarData *progressData = static_cast<TpCircularProgressBarData *>(data_);
    progressData->curValue = value;

    if (progressData->curValue > progressData->maxValue)
        progressData->curValue = progressData->maxValue;

    if (progressData->curValue < progressData->minValue)
        progressData->curValue = progressData->minValue;

    update();
}

int32_t TpCircularProgressBar::value()
{
    TpCircularProgressBarData *progressData = static_cast<TpCircularProgressBarData *>(data_);
    return progressData->curValue;
}

void TpCircularProgressBar::setLineWidth(const uint32_t &width)
{
    TpCircularProgressBarData *progressData = static_cast<TpCircularProgressBarData *>(data_);
    progressData->lineWidth = width;
}

uint32_t TpCircularProgressBar::lineWidth()
{
    TpCircularProgressBarData *progressData = static_cast<TpCircularProgressBarData *>(data_);
    return progressData->lineWidth;
}

bool TpCircularProgressBar::onPaintEvent(TpPaintEvent *event)
{
    TpCircularProgressBarData *progressData = static_cast<TpCircularProgressBarData *>(data_);

    // TpChildWidget::onPaintEvent(event);
    TpPainter *painter = event->painter();
    // painter->arc(50, 50, 40, 140, 40, _RGBA(204, 179, 230, 204), 15, true);

    int32_t circlePoint = (width() < height() ? width() : height()) / 2.0;

    painter->setPen(_RGB(217, 217, 217));
    painter->pen().setWidth(progressData->lineWidth);

    // 半径要减去线宽的一半
    painter->drawEllipse(circlePoint, circlePoint, circlePoint - progressData->lineWidth / 2.0, circlePoint - progressData->lineWidth / 2.0);

    painter->setPen(_RGB(128, 94, 243));
    painter->pen().setWidth(progressData->lineWidth);

    // 根据进度绘制填充
    if (progressData->curValue == progressData->minValue)
    {
    }
    else if (progressData->curValue == progressData->maxValue)
    {
        painter->drawEllipse(circlePoint, circlePoint, circlePoint - progressData->lineWidth / 2.0, circlePoint - progressData->lineWidth / 2.0);
    }
    else
    {
        // 计算百分比
        double percent = 1.0 * (progressData->curValue - progressData->minValue) / (progressData->maxValue - progressData->minValue);
        double percentAngle = -90 + 360.0 * percent;

        painter->drawArc(circlePoint, circlePoint, circlePoint - progressData->lineWidth / 2.0, -90, percentAngle);
    }

    return true;
}
