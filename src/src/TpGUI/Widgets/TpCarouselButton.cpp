#include "TpCarouselButton.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpDisplay.h"

struct TpCarouselButtonData
{
    uint32_t maxCount = 3;
    uint32_t curIndex = 0;

    uint32_t spacing = TpDisplay::dp2Px(14);
    uint32_t singleBtnWH = TpDisplay::dp2Px(8);

    TpCarouselButton::ShowMode showMode = TpCarouselButton::Horizon;

    int32_t selectColor = _RGB(255, 255, 255);
    int32_t defaultColor = _RGB(110, 110, 110);

    // 所有点的坐标区域
    bool respond = false;
    TpVector<TpRect> pointRectList;
};

TpCarouselButton::TpCarouselButton(TpWidget *parent)
    : TpWidget(parent)
{
    Init();
}

TpCarouselButton::TpCarouselButton(const uint32_t &maxCount, TpWidget *parent)
    : TpWidget(parent)
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

void TpCarouselButton::setRespondClick(bool respond)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    buttonData->respond = respond;
}

bool TpCarouselButton::onMousePressEvent(TpMouseEvent *event)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    if (!buttonData->respond)
        return true;

    TpPoint pressPoint = event->pos();
    for (int i = 0; i < buttonData->pointRectList.size(); ++i)
    {
        auto curRect = buttonData->pointRectList.at(i);

        if (curRect.contains(pressPoint))
        {
            setCurrentIndex(i);
            onClicked.emit(i);
            break;
        }
    }

    return true;
}

bool TpCarouselButton::onPaintEvent(TpPaintEvent *event)
{
    TpCarouselButtonData *buttonData = static_cast<TpCarouselButtonData *>(data_);
    buttonData->pointRectList.clear();

    TpPainter *paintCanvas = event->painter();

    if (buttonData->showMode == TpCarouselButton::Horizon)
    {
        // 依次绘制色块;因为选中节点占五个默认节点的宽度，所以 +4
        uint32_t startX = (width() - ((buttonData->maxCount + 4) * buttonData->singleBtnWH + (buttonData->maxCount - 1) * buttonData->spacing)) / 2.0;
        int32_t startY = (height() - buttonData->singleBtnWH) / 2.0;

        for (int i = 0; i < buttonData->maxCount; ++i)
        {
            TpRect curRect;
            curRect.setX(startX);
            curRect.setY(startY);
            curRect.setHeight(buttonData->singleBtnWH);

            int32_t curColor;
            if (i == buttonData->curIndex)
            {
                curRect.setWidth(buttonData->singleBtnWH * 5);
                startX += buttonData->singleBtnWH * 4;
                curColor = buttonData->selectColor;
            }
            else
            {
                curRect.setWidth(buttonData->singleBtnWH);
                curColor = buttonData->defaultColor;
            }

            paintCanvas->setPen(curColor);
            paintCanvas->setBrush(TpBrush(curColor));

            paintCanvas->drawRect(curRect.x(), curRect.y(), curRect.width(), curRect.height(), buttonData->singleBtnWH / 2.0);
            buttonData->pointRectList.emplace_back(curRect);

            startX += buttonData->singleBtnWH + buttonData->spacing;
        }
    }
    else
    {
        // 依次绘制色块;因为选中节点占五个默认节点的宽度，所以 +4
        uint32_t startX = (width() - buttonData->singleBtnWH) / 2.0;
        int32_t startY = (height() - ((buttonData->maxCount + 4) * buttonData->singleBtnWH + (buttonData->maxCount - 1) * buttonData->spacing)) / 2.0;

        for (int i = 0; i < buttonData->maxCount; ++i)
        {
            TpRect curRect;
            curRect.setX(startX);
            curRect.setY(startY);
            curRect.setWidth(buttonData->singleBtnWH);

            int32_t curColor;
            if (i == buttonData->curIndex)
            {
                curRect.setHeight(buttonData->singleBtnWH * 5);
                startY += buttonData->singleBtnWH * 4;
                curColor = buttonData->selectColor;
            }
            else
            {
                curRect.setHeight(buttonData->singleBtnWH);
                curColor = buttonData->defaultColor;
            }

            paintCanvas->setPen(curColor);
            paintCanvas->setBrush(TpBrush(curColor));

            paintCanvas->drawRect(curRect.x(), curRect.y(), curRect.width(), curRect.height(), buttonData->singleBtnWH / 2.0);
            buttonData->pointRectList.emplace_back(curRect);

            startY += buttonData->singleBtnWH + buttonData->spacing;
        }
    }

    return true;
}

void TpCarouselButton::Init()
{
    data_ = new TpCarouselButtonData();

    setRoundCorners(5);
}
