#include "testLight.h"
#include "TpCanvas.h"

testLight::testLight(TpChildWidget *parent)
    : TpChildWidget(parent), maxCount_(4), count_(0)
{
}

testLight::~testLight()
{
}

bool testLight::onMousePressEvent(TpMouseEvent *event)
{
    count_++;
    if (count_ > maxCount_)
        count_ = 0;
    return true;
}

bool testLight::onPaintEvent(TpObjectPaintEvent *event)
{
    TpChildWidget::onPaintEvent(event);

    TpCanvas *painter = event->canvas();

    painter->box(0, 0, width(), height(), _RGB(255, 255, 255));

    int spacing = 3;
    int singleWidth = (width() - (maxCount_ + 1) * spacing) / maxCount_;

    for (int i = 0; i < count_; ++i)
    {
        int drawX = spacing + i * (singleWidth + spacing);
        painter->box(drawX, spacing, drawX + singleWidth, height() - spacing, _RGB(128, 255, 128));
    }

    return true;
}
