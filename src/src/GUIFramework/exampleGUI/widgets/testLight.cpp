#include "testLight.h"
#include "tpCanvas.h"

EXAMPLE_GUI_NAMESPACE_BEGIN

struct testLightData
{
    int maxCount_;
    int count_;

    testLightData() : maxCount_(0), count_(0)
    {
    }
};

testLight::testLight(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    testLightData *lightData = new testLightData();
    lightData->maxCount_ = 4;
    lightData->count_ = 0;
    data_ = lightData;
}

testLight::~testLight()
{
    testLightData *lightData = static_cast<testLightData *>(data_);
    if (lightData)
    {
        delete lightData;
        lightData = nullptr;
        data_ = nullptr;
    }
}

bool testLight::onMousePressEvent(tpMouseEvent *event)
{
    testLightData *lightData = static_cast<testLightData *>(data_);

    lightData->count_++;
    if (lightData->count_ > lightData->maxCount_)
        lightData->count_ = 0;

    return true;
}

bool testLight::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    testLightData *lightData = static_cast<testLightData *>(data_);

    tpCanvas *painter = event->canvas();

    painter->box(0, 0, width(), height(), _RGB(255, 255, 255));

    int spacing = 3;
    int singleWidth = (width() - (lightData->maxCount_ + 1) * spacing) / lightData->maxCount_;

    for (int i = 0; i < lightData->count_; ++i)
    {
        int drawX = spacing + i * (singleWidth + spacing);
        painter->box(drawX, spacing, drawX + singleWidth, height() - spacing, _RGB(128, 255, 128));
    }

    return true;
}

EXAMPLE_GUI_NAMESPACE_END