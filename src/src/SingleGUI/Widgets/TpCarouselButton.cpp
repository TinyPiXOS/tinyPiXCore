#include "TpCarouselButton.h"
#include "TpCanvas.h"
#include "TpEvent.h"
#include "TpDisplay.h"

struct TpCarouselButtonData
{
    uint32_t maxCount = 0;
    uint32_t curIndex = 0;

    uint32_t contentMargin = TpDisplay::dp2Px(10);
    uint32_t singleButtonRadius = TpDisplay::dp2Px(8);
};

TpCarouselButton::TpCarouselButton(TpChildWidget*parent)
    : TpChildWidget(parent)
{
    Init();
}

TpCarouselButton::TpCarouselButton(const uint32_t &maxCount, TpChildWidget*parent)
    : TpChildWidget(parent)
{
    Init();

    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    buttonData->maxCount = maxCount;
}

TpCarouselButton::~TpCarouselButton()
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    if (buttonData)
    {
        delete buttonData;
        buttonData = nullptr;
        data_ = nullptr;
    }
}

uint32_t TpCarouselButton::count()
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    return buttonData->maxCount;
}

void TpCarouselButton::setCount(const uint32_t &count)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    buttonData->maxCount = count;

    uint32_t buttonWidth = buttonData->singleButtonRadius * (count - 1) + buttonData->contentMargin * (count - 1) + (buttonData->singleButtonRadius * 2 + buttonData->contentMargin);
    setWidth(buttonWidth);
    setHeight(buttonData->singleButtonRadius);

    update();
}

uint32_t TpCarouselButton::currentIndex()
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    return buttonData->curIndex;
}

void TpCarouselButton::setCurrentIndex(const uint32_t &index)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    buttonData->curIndex = index;
    update();
}

void TpCarouselButton::setParent(TpObject *parent)
{
    TpChildWidget::setParent(parent);
}

bool TpCarouselButton::onMousePressEvent(TpMouseEvent *event)
{
    return true;
}

bool TpCarouselButton::onMouseRleaseEvent(TpMouseEvent *event)
{
    return true;
}

bool TpCarouselButton::onPaintEvent(TpObjectPaintEvent *event)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);

    TpCanvas *paintCanvas = event->canvas();

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

bool TpCarouselButton::onResizeEvent(TpObjectResizeEvent *event)
{
    return true;
}

void TpCarouselButton::onThemeChangeEvent(TpThemeChangeEvent *event)
{
}

void TpCarouselButton::Init()
{
    data_ = new TpCarouselButtonData();

    setRoundCorners(5);
}
