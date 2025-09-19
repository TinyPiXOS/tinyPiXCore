#include "TpMediaTileButton.h"
#include "TpLabel.h"
#include "TpImage.h"
#include "TpFont.h"
#include "TpEvent.h"

struct TpMediaTileButtonData
{
    TpLabel *iconLabel;
    TpLabel *textLabel;
};

TpMediaTileButton::TpMediaTileButton(TpChildWidget *parent)
    : TpChildWidget(parent), data_(nullptr)
{
    tpShared<TpCssData> curCssData = currentStatusCss();

    TpMediaTileButtonData *buttonData = new TpMediaTileButtonData();

    buttonData->iconLabel = new TpLabel(this);
    buttonData->iconLabel->setFixedSize(curCssData->iconSize(), curCssData->iconSize());
    buttonData->iconLabel->installEventFilter(this);

    buttonData->textLabel = new TpLabel(this);
    buttonData->textLabel->setAlign(Tp::AlignHCenter);
    buttonData->textLabel->font()->setFontSize(curCssData->fontSize());
    buttonData->textLabel->font()->setFontColor(curCssData->color(), curCssData->color());
    buttonData->textLabel->installEventFilter(this);

    data_ = buttonData;

    refreshBaseCss();
    setCheckable(true);
}

TpMediaTileButton::~TpMediaTileButton()
{
    TpMediaTileButtonData *buttonData = static_cast<TpMediaTileButtonData *>(data_);
    if (buttonData)
    {
        delete buttonData;
        buttonData = nullptr;
        data_ = nullptr;
    }
}

void TpMediaTileButton::setIcon(const TpString &iconPath)
{
    TpMediaTileButtonData *buttonData = static_cast<TpMediaTileButtonData *>(data_);
    if (!buttonData)
        return;

    buttonData->iconLabel->setBackGroundImage(TpImage(iconPath));
}

void TpMediaTileButton::setText(const TpString &text)
{
    TpMediaTileButtonData *buttonData = static_cast<TpMediaTileButtonData *>(data_);
    if (!buttonData)
        return;
    buttonData->textLabel->setText(text);
}

bool TpMediaTileButton::onMousePressEvent(TpMouseEvent *event)
{
    TpChildWidget::onMousePressEvent(event);

    return false;
}

bool TpMediaTileButton::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpChildWidget::onMouseRleaseEvent(event);

    onClicked.emit(checked());

    return false;
}

bool TpMediaTileButton::onPaintEvent(TpPaintEvent *event)
{
    TpChildWidget::onPaintEvent(event);

    return true;
}

bool TpMediaTileButton::onResizeEvent(TpResizeEvent *event)
{
    TpChildWidget::onResizeEvent(event);

    TpMediaTileButtonData *buttonData = static_cast<TpMediaTileButtonData *>(data_);
    if (!buttonData)
        return true;

    buttonData->textLabel->setSize(width(), buttonData->textLabel->font()->pixelHeight());

    tpShared<TpCssData> curCssData = currentStatusCss();

    // 重新计算label坐标
    uint32_t iconX = (width() - buttonData->iconLabel->width()) / 2.0;
    // std::cout << "width() " << width() << " iconX " << iconX << " width " << buttonData->iconLabel->width() << std::endl;

    uint32_t iconY = (height() - buttonData->iconLabel->height() - buttonData->textLabel->height() - curCssData->gap()) / 2.0;

    buttonData->iconLabel->move(iconX, iconY);

    buttonData->textLabel->move(0, iconY + buttonData->iconLabel->height() + curCssData->gap());

    return true;
}

bool TpMediaTileButton::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else
    {
    }

    return false;
}

void TpMediaTileButton::onThemeChangeEvent(TpThemeChangeEvent *event)
{
}
