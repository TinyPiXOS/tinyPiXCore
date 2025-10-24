#include "TpTestLight.h"
#include "TpPainter.h"

TpTestLight::TpTestLight(TpWidget *parent)
    : TpWidget(parent), maxCount_(4), count_(0)
{
}

TpTestLight::~TpTestLight()
{
}

bool TpTestLight::onMousePressEvent(TpMouseEvent *event)
{
    count_++;
    if (count_ > maxCount_)
        count_ = 0;
    return true;
}

bool TpTestLight::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpPainter *painter = event->painter();

    painter->setPen(_RGB(255, 255, 255));
    painter->setBrush(TpBrush(_RGB(255, 255, 255)));

    painter->drawRect(0, 0, width(), height());

    int spacing = 3;
    int singleWidth = (width() - (maxCount_ + 1) * spacing) / maxCount_;

    painter->setPen(_RGB(128, 255, 128));
    painter->setBrush(TpBrush(_RGB(128, 255, 128)));
    for (int i = 0; i < count_; ++i)
    {
        int drawX = spacing + i * (singleWidth + spacing);
        painter->drawRect(drawX, spacing, singleWidth, height() - spacing * 2);
    }

    return true;
}
