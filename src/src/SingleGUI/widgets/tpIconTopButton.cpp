#include "tpIconTopButton.h"
#include "tpLabel.h"
#include "tpButton.h"
#include "tpSurface.h"
#include "tpDisplay.h"
#include "tpEvent.h"
#include "tpFont.h"

struct tpIconTopButtonData
{
    tpLabel *iconLabel;
    tpLabel *textLabel;
};

tpIconTopButton::tpIconTopButton(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    Init();
}

tpIconTopButton::tpIconTopButton(const tpString &iconPath, const tpString &text, tpChildWidget *parent)
    : tpChildWidget(parent)
{
    Init();

    setIcon(iconPath);
    setText(text);
}

tpIconTopButton::~tpIconTopButton()
{
    tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);
    if (buttonData)
    {
        delete buttonData;
        buttonData = nullptr;
    }
}

void tpIconTopButton::setText(const tpString &text)
{
    if (text.empty())
        return;

    tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);
    buttonData->textLabel->setText(text);
}

void tpIconTopButton::setIcon(const tpString &iconPath)
{
    tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);

    tpShared<tpSurface> image = tpMakeShared<tpSurface>();
    image->fromFile(iconPath);
    buttonData->iconLabel->setBackGroundImage(image);
}

tpFont *tpIconTopButton::font()
{
    tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);

    return buttonData->textLabel->font();
}

void tpIconTopButton::setIconSize(const uint32_t &width, const uint32_t &height)
{
    tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);

    buttonData->iconLabel->setWidth(width);
    buttonData->iconLabel->setHeight(height);

    buttonData->textLabel->setWidth(width);
    buttonData->textLabel->setHeight(buttonData->textLabel->font()->pixelHeight());
    buttonData->textLabel->move(0, height + tpDisplay::dp2Px(6));

    setWidth(width);
    setHeight(height + buttonData->textLabel->font()->pixelHeight() + tpDisplay::dp2Px(6));
}

void tpIconTopButton::setIconSize(const ItpSize &size)
{
    setIconSize(size.w, size.h);
}

ItpSize tpIconTopButton::iconSize()
{
    tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);
    return buttonData->iconLabel->size();
}

void tpIconTopButton::setTextVisible(const bool &visible)
{
    tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);
    buttonData->textLabel->setVisible(visible);

    if (visible)
    {
        setHeight(buttonData->iconLabel->height() * 1.25 + tpDisplay::dp2Px(6));
    }
    else
    {
        setHeight(buttonData->iconLabel->height());
    }
}

void tpIconTopButton::setRoundCorners(const uint32_t &round)
{
    tpChildWidget::setRoundCorners(round);

    tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);
    buttonData->iconLabel->setRoundCorners(round);
}

void tpIconTopButton::setParent(tpObject *parent)
{
    tpChildWidget::setParent(parent);
}

bool tpIconTopButton::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    onPressed.emit();

    return true;
}

bool tpIconTopButton::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    ItpPoint mouseGlobalPos = event->globalPos();

    if (toScreen().contains(mouseGlobalPos))
    {
        onClicked.emit(checked());
    }

    return false;
}

bool tpIconTopButton::onMouseLongPressEvent(tpMouseEvent *event)
{
    onLongPress.emit();
    return true;
}

bool tpIconTopButton::onLeaveEvent(tpObjectLeaveEvent *event)
{
    tpChildWidget::onLeaveEvent(event);

    return true;
}

bool tpIconTopButton::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);

    tpShared<tpCssData> curCssData = currentStatusCss();

    tpFont *textLabelFont = buttonData->textLabel->font();
    textLabelFont->setFontForeColor(curCssData->color());
    textLabelFont->setFontSize(curCssData->fontSize());

    return true;
}

bool tpIconTopButton::onResizeEvent(tpObjectResizeEvent *event)
{
    // tpIconTopButtonData *buttonData = static_cast<tpIconTopButtonData *>(data_);
    // buttonData->iconButton->setRect(0, 0, rect().w, rect().h * 0.8);

    // buttonData->textLabel->setRect(0, rect().h * 0.8, rect().w, rect().h * 0.2);

    return true;
}

bool tpIconTopButton::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        onMousePressEvent((tpMouseEvent *)event);
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        onMouseRleaseEvent((tpMouseEvent *)event);
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_MOVE_TYPE)
    {
        onMouseMoveEvent((tpMouseEvent *)event);
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_LONG_PRESS_TYPE)
    {
        onMouseLongPressEvent((tpMouseEvent *)event);
    }
    else
    {
    }

    return false;
}

void tpIconTopButton::onThemeChangeEvent(tpThemeChangeEvent *event)
{
}

void tpIconTopButton::Init()
{
    tpIconTopButtonData *buttonData = new tpIconTopButtonData();
    data_ = buttonData;

    buttonData->iconLabel = new tpLabel(this);
    buttonData->iconLabel->move(0, 0);
    buttonData->iconLabel->installEventFilter(this);

    buttonData->textLabel = new tpLabel(this);
    buttonData->textLabel->installEventFilter(this);

    buttonData->iconLabel->setEnableBackGroundColor(false);
    buttonData->textLabel->setEnableBackGroundColor(false);

    buttonData->textLabel->setAlign(tinyPiX::AlignHCenter);

    tpShared<tpCssData> curCssData = currentStatusCss();
    setIconSize(curCssData->iconSize(), curCssData->iconSize());
}
