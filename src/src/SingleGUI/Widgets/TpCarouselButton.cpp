#include "TpCarouselButton.h"
#include "TpCanvas.h"
#include "TpEvent.h"
#include "TpDisplay.h"

struct TpCarouselButtonData
{
    uint32_t maxCount = 3;
    uint32_t curIndex = 0;

    uint32_t spacing = TpDisplay::dp2Px(10);
    uint32_t singleButtonRadius = TpDisplay::dp2Px(8);

    TpCarouselButton::ShowMode showMode = TpCarouselButton::Horizon;

    int32_t selectColor = _RGB(255, 255, 255);
    int32_t defaultColor = _RGB(110, 110, 110);
};

TpCarouselButton::TpCarouselButton(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    Init();
}

TpCarouselButton::TpCarouselButton(const uint32_t &maxCount, TpChildWidget *parent)
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

void TpCarouselButton::setMode(const ShowMode &mode)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    buttonData->showMode = mode;
    update();
}

TpCarouselButton::ShowMode TpCarouselButton::mode()
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    return buttonData->showMode;
}

void TpCarouselButton::setSelectColor(int32_t color)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    buttonData->selectColor = color;
}

int32_t TpCarouselButton::selectColor()
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    return buttonData->selectColor;
}

void TpCarouselButton::setDefaultColor(int32_t color)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    buttonData->defaultColor = color;
}

int32_t TpCarouselButton::defaultColor()
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    return buttonData->defaultColor;
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

    // uint32_t buttonWidth = buttonData->singleButtonRadius * (count - 1) + buttonData->spacing * (count - 1) + (buttonData->singleButtonRadius * 2 + buttonData->contentMargin);
    // setWidth(buttonWidth);
    // setHeight(buttonData->singleButtonRadius);

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

void TpCarouselButton::setSpacing(int32_t spacing)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    buttonData->spacing = spacing;
    update();
}

int32_t TpCarouselButton::spacing()
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    return buttonData->spacing;
}

bool TpCarouselButton::onMousePressEvent(TpMouseEvent *event)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    ItpPoint pressPoint = event->globalPos();
    
    return true;
}

bool TpCarouselButton::onPaintEvent(TpObjectPaintEvent *event)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);

    TpCanvas *paintCanvas = event->canvas();

    // 依次绘制色块
    uint32_t startX = 0;

    for (int i = 0; i < buttonData->maxCount; ++i)
    {
        if (i == buttonData->curIndex)
        {
            paintCanvas->roundedBox(startX, 0, startX + buttonData->singleButtonRadius * 2 + buttonData->spacing, height(), buttonData->singleButtonRadius, _RGB(255, 255, 255));

            startX += buttonData->singleButtonRadius * 2 + buttonData->spacing * 2;
        }
        else
        {
            paintCanvas->roundedBox(startX, 0, startX + buttonData->singleButtonRadius, height(), buttonData->singleButtonRadius, _RGBA(255, 255, 255, 51));
            // paintCanvas->roundedBox(startX, 0, startX + buttonData->singleButtonRadius, height(), minRad, _RGBA(255, 0, 0, 255));

            startX += buttonData->singleButtonRadius + buttonData->spacing;
        }
    }

    return true;
}

void TpCarouselButton::Init()
{
    data_ = new TpCarouselButtonData();

    setRoundCorners(5);
}
