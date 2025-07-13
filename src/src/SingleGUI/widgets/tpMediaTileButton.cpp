#include "tpMediaTileButton.h"
#include "tpLabel.h"
#include "tpSurface.h"
#include "tpFont.h"
#include "tpEvent.h"

struct tpMediaTileButtonData
{
    tpLabel *iconLabel;
    tpLabel *textLabel;
};

tpMediaTileButton::tpMediaTileButton(tpChildWidget *parent)
    : tpChildWidget(parent), data_(nullptr)
{
    tpShared<tpCssData> curCssData = currentStatusCss();

    tpMediaTileButtonData *buttonData = new tpMediaTileButtonData();

    buttonData->iconLabel = new tpLabel(this);
    buttonData->iconLabel->setFixedSize(curCssData->iconSize(), curCssData->iconSize());
    buttonData->iconLabel->installEventFilter(this);

    buttonData->textLabel = new tpLabel(this);
    buttonData->textLabel->setAlign(tinyPiX::AlignHCenter);
    buttonData->textLabel->font()->setFontSize(curCssData->fontSize());
    buttonData->textLabel->font()->setFontColor(curCssData->color(), curCssData->color());
    buttonData->textLabel->installEventFilter(this);

    data_ = buttonData;

    refreshBaseCss();
    setCheckable(true);
}

tpMediaTileButton::~tpMediaTileButton()
{
    tpMediaTileButtonData *buttonData = static_cast<tpMediaTileButtonData *>(data_);
    if (buttonData)
    {
        delete buttonData;
        buttonData = nullptr;
        data_ = nullptr;
    }
}

void tpMediaTileButton::setIcon(const tpString &iconPath)
{
    tpMediaTileButtonData *buttonData = static_cast<tpMediaTileButtonData *>(data_);
    if (!buttonData)
        return;

    tpShared<tpSurface> iconSurface = tpMakeShared<tpSurface>();

    iconSurface->fromFile(iconPath);
    buttonData->iconLabel->setBackGroundImage(iconSurface);
}

void tpMediaTileButton::setText(const tpString &text)
{
    tpMediaTileButtonData *buttonData = static_cast<tpMediaTileButtonData *>(data_);
    if (!buttonData)
        return;
    buttonData->textLabel->setText(text);
}

bool tpMediaTileButton::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    return false;
}

bool tpMediaTileButton::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    onClicked.emit(checked());

    return false;
}

bool tpMediaTileButton::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    return true;
}

bool tpMediaTileButton::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);

    tpMediaTileButtonData *buttonData = static_cast<tpMediaTileButtonData *>(data_);
    if (!buttonData)
        return true;

    buttonData->textLabel->setSize(width(), buttonData->textLabel->font()->pixelHeight());

    tpShared<tpCssData> curCssData = currentStatusCss();

    // 重新计算label坐标
    uint32_t iconX = (width() - buttonData->iconLabel->width()) / 2.0;
    // std::cout << "width() " << width() << " iconX " << iconX << " width " << buttonData->iconLabel->width() << std::endl;

    uint32_t iconY = (height() - buttonData->iconLabel->height() - buttonData->textLabel->height() - curCssData->gap()) / 2.0;

    buttonData->iconLabel->move(iconX, iconY);

    buttonData->textLabel->move(0, iconY + buttonData->iconLabel->height() + curCssData->gap());

    return true;
}

bool tpMediaTileButton::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else
    {
    }

    return false;
}

void tpMediaTileButton::onThemeChangeEvent(tpThemeChangeEvent *event)
{
}
