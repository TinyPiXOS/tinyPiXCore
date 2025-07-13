#include "tpCarouselButton.h"
#include "tpCanvas.h"
#include "tpEvent.h"
#include "tpDisplay.h"

struct tpCarouselButtonData
{
    uint32_t maxCount = 0;
    uint32_t curIndex = 0;

    uint32_t contentMargin = tpDisplay::dp2Px(10);
    uint32_t singleButtonRadius = tpDisplay::dp2Px(8);
};

tpCarouselButton::tpCarouselButton(tpChildWidget*parent)
    : tpChildWidget(parent)
{
    Init();
}

tpCarouselButton::tpCarouselButton(const uint32_t &maxCount, tpChildWidget*parent)
    : tpChildWidget(parent)
{
    Init();

    tpCarouselButtonData *buttonData = static_cast<tpCarouselButtonData *>(data_);
    buttonData->maxCount = maxCount;
}

tpCarouselButton::~tpCarouselButton()
{
    tpCarouselButtonData *buttonData = static_cast<tpCarouselButtonData *>(data_);
    if (buttonData)
    {
        delete buttonData;
        buttonData = nullptr;
        data_ = nullptr;
    }
}

uint32_t tpCarouselButton::count()
{
    tpCarouselButtonData *buttonData = static_cast<tpCarouselButtonData *>(data_);
    return buttonData->maxCount;
}

void tpCarouselButton::setCount(const uint32_t &count)
{
    tpCarouselButtonData *buttonData = static_cast<tpCarouselButtonData *>(data_);
    buttonData->maxCount = count;

    uint32_t buttonWidth = buttonData->singleButtonRadius * (count - 1) + buttonData->contentMargin * (count - 1) + (buttonData->singleButtonRadius * 2 + buttonData->contentMargin);
    setWidth(buttonWidth);
    setHeight(buttonData->singleButtonRadius);

    update();
}

uint32_t tpCarouselButton::currentIndex()
{
    tpCarouselButtonData *buttonData = static_cast<tpCarouselButtonData *>(data_);
    return buttonData->curIndex;
}

void tpCarouselButton::setCurrentIndex(const uint32_t &index)
{
    tpCarouselButtonData *buttonData = static_cast<tpCarouselButtonData *>(data_);
    buttonData->curIndex = index;
    update();
}

void tpCarouselButton::setParent(tpObject *parent)
{
    tpChildWidget::setParent(parent);
}

bool tpCarouselButton::onMousePressEvent(tpMouseEvent *event)
{
    return true;
}

bool tpCarouselButton::onMouseRleaseEvent(tpMouseEvent *event)
{
    return true;
}

bool tpCarouselButton::onPaintEvent(tpObjectPaintEvent *event)
{
    tpCarouselButtonData *buttonData = static_cast<tpCarouselButtonData *>(data_);

    tpCanvas *paintCanvas = event->canvas();

    uint32_t minRad = roundCorners();

    // 依次绘制色块
    uint32_t startX = 0; 

    for (int i = 0; i < buttonData->maxCount; ++i)
    {
        if (i == buttonData->curIndex)
        {
            paintCanvas->roundedBox(startX, 0, startX + buttonData->singleButtonRadius * 2 + buttonData->contentMargin, height(), minRad, _RGB(255, 255, 255));

            startX += buttonData->singleButtonRadius * 2 + buttonData->contentMargin * 2;
        }
        else
        {
            paintCanvas->roundedBox(startX, 0, startX + buttonData->singleButtonRadius, height(), minRad, _RGBA(255, 255, 255, 51));
            // paintCanvas->roundedBox(startX, 0, startX + buttonData->singleButtonRadius, height(), minRad, _RGBA(255, 0, 0, 255));

            startX += buttonData->singleButtonRadius + buttonData->contentMargin;
        }
    }

    return true;
}

bool tpCarouselButton::onResizeEvent(tpObjectResizeEvent *event)
{
    return true;
}

void tpCarouselButton::onThemeChangeEvent(tpThemeChangeEvent *event)
{
}

void tpCarouselButton::Init()
{
    data_ = new tpCarouselButtonData();

    setRoundCorners(5);
}
