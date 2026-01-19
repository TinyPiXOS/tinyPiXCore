#include "TpProgressBar.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpRange.h"
#include "TpFont.h"

struct TpProgressBarData
{
    TpRange valueRange;
    TpProgressBar::Direct direction = TpProgressBar::Horizon;
    bool showText = true;
};

TpProgressBar::TpProgressBar(TpWidget *parent, const Direct &direct) : TpWidget(parent)
{
    TpProgressBarData *progressData = new TpProgressBarData();
    progressData->direction = direct;
    progressData->valueRange.setRange(0, 100);
    data_ = progressData;

    refreshBaseCss();
}

TpProgressBar::~TpProgressBar()
{
    TpProgressBarData *progressData = static_cast<TpProgressBarData *>(data_);

    if (progressData)
    {
        delete progressData;
        progressData = nullptr;
        data_ = nullptr;
    }
}

void TpProgressBar::setRange(const int32_t &min, const int32_t &max)
{
    TpProgressBarData *progressData = static_cast<TpProgressBarData *>(data_);
    progressData->valueRange.setRange(min, max);
    update();
}

void TpProgressBar::setDirection(const Direct &direct)
{
    TpProgressBarData *progressData = static_cast<TpProgressBarData *>(data_);
    progressData->direction = direct;
    update();
}

TpProgressBar::Direct TpProgressBar::direction() const
{
    TpProgressBarData *progressData = static_cast<TpProgressBarData *>(data_);
    return progressData->direction;
}

int32_t TpProgressBar::value() const
{
    TpProgressBarData *progressData = static_cast<TpProgressBarData *>(data_);
    return progressData->valueRange.value();
}

void TpProgressBar::setValue(const int32_t &value)
{
    TpProgressBarData *progressData = static_cast<TpProgressBarData *>(data_);
    progressData->valueRange.setValue(value);
    update();
}

void TpProgressBar::setTextVisible(bool visible)
{
    TpProgressBarData *progressData = static_cast<TpProgressBarData *>(data_);
    progressData->showText = visible;
    update();
}

bool TpProgressBar::textVisible() const
{
    TpProgressBarData *progressData = static_cast<TpProgressBarData *>(data_);
    return progressData->showText;
}

bool TpProgressBar::onPaintEvent(TpPaintEvent *event)
{
    TpProgressBarData *progressData = static_cast<TpProgressBarData *>(data_);

    TpPainter *painter = event->painter();

    tpShared<TpCssData> curCssData = currentStatusCss();

    int32_t barWidth = width();
    int32_t barHeight = height();

    if (progressData->direction == TpProgressBar::Horizon)
    {
        // 绘制背景色
        painter->setBrush(TpBrush(curCssData->backgroundColor()));
        painter->drawRect(0, 0, barWidth, height(), roundCorners());

        // 绘制填充色
        int32_t valueWidth = progressData->valueRange.percent() * barWidth;
        painter->setBrush(TpBrush(curCssData->subColor()));
        painter->drawRect(0, 0, valueWidth, height(), roundCorners());
    }
    else
    {
        // 绘制背景色
        painter->setBrush(TpBrush(curCssData->backgroundColor()));
        painter->drawRect(0, height() - barHeight, width(), barHeight, roundCorners());

        // 绘制填充色
        int32_t valueHeight = progressData->valueRange.percent() * barHeight;
        painter->setBrush(TpBrush(curCssData->subColor()));
        painter->drawRect(0, height() - valueHeight, width(), valueHeight, roundCorners());
    }

    // 绘制文本
    if (progressData->showText)
    {
        TpFont textFont;
        textFont.setFontColor(curCssData->color());
        textFont.setFontSize(curCssData->fontSize());

        TpString valueText = TpString::number(int32_t(progressData->valueRange.percent() * 100)) + "%";
        textFont.setText(valueText);

        if (progressData->direction == TpProgressBar::Horizon)
        {
            int32_t textY = (height() - textFont.pixelHeight()) / 2.0;
            painter->drawText(textFont, (width() - textFont.pixelWidth()) / 2.0, textY);
        }
        else
        {
            int32_t textX = (width() - textFont.pixelWidth()) / 2.0;
            painter->drawText(textFont, textX, (height() - textFont.pixelHeight()) / 2.0);
        }
    }

    return true;
}
