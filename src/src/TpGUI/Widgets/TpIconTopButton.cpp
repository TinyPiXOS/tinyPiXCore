#include "TpIconTopButton.h"
#include "TpLabel.h"
#include "TpButton.h"
#include "TpImage.h"
#include "SystemInfo/TpDisplay.h"
#include "TpEvent.h"
#include "TpFont.h"

#define BTN_MARGIN TpDisplay::dp2Px(8)

struct TpIconTopButtonData
{
    TpLabel *iconLabel;
    TpLabel *textLabel;
};

TpIconTopButton::TpIconTopButton(TpWidget *parent)
    : TpWidget(parent)
{
    Init();
}

TpIconTopButton::TpIconTopButton(const TpString &iconPath, const TpString &text, TpWidget *parent)
    : TpWidget(parent)
{
    Init();

    setIcon(iconPath);
    setText(text);
}

TpIconTopButton::~TpIconTopButton()
{
    TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);
    if (buttonData)
    {
        delete buttonData;
        buttonData = nullptr;
    }
}

void TpIconTopButton::setText(const TpString &text)
{
    if (text.empty())
        return;

    TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);
    buttonData->textLabel->setText(text);
}

void TpIconTopButton::setIcon(const TpString &iconPath)
{
    TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);

    buttonData->iconLabel->setBackGroundImage(TpImage(iconPath));
}

TpFont *TpIconTopButton::font()
{
    TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);

    return buttonData->textLabel->font();
}

void TpIconTopButton::setIconSize(const uint32_t &width, const uint32_t &height)
{
    TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);

    buttonData->iconLabel->setWidth(width);
    buttonData->iconLabel->setHeight(height);

    buttonData->textLabel->setWidth(width);
    buttonData->textLabel->setHeight(buttonData->textLabel->font()->pixelHeight());
    buttonData->textLabel->move(0, height + BTN_MARGIN);

    setWidth(width);
    setHeight(height + buttonData->textLabel->font()->pixelHeight() + BTN_MARGIN);
}

void TpIconTopButton::setIconSize(const TpSize &size)
{
    setIconSize(size.width(), size.height());
}

TpSize TpIconTopButton::iconSize()
{
    TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);
    return buttonData->iconLabel->size();
}

void TpIconTopButton::setTextVisible(const bool &visible)
{
    TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);
    buttonData->textLabel->setVisible(visible);

    if (visible)
    {
        setHeight(buttonData->iconLabel->height() + buttonData->textLabel->font()->pixelHeight() + BTN_MARGIN);
    }
    else
    {
        setHeight(buttonData->iconLabel->height());
    }
}

void TpIconTopButton::setRoundCorners(const uint32_t &round)
{
    TpWidget::setRoundCorners(round);

    TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);
    buttonData->iconLabel->setRoundCorners(round);
}

void TpIconTopButton::setParent(TpObject *parent)
{
    TpWidget::setParent(parent);
}

bool TpIconTopButton::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    onPressed.emit();

    return true;
}

bool TpIconTopButton::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpWidget::onMouseRleaseEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    TpPoint mouseGlobalPos = event->globalPos();

    if (toScreen().contains(mouseGlobalPos))
    {
        onClicked.emit(checked());
    }

    return false;
}

bool TpIconTopButton::onMouseLongPressEvent(TpMouseEvent *event)
{
    onLongPress.emit();
    return true;
}

bool TpIconTopButton::onLeaveEvent(TpLeaveEvent *event)
{
    TpWidget::onLeaveEvent(event);

    return true;
}

bool TpIconTopButton::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);

    tpShared<TpCssData> curCssData = currentStatusCss();

    TpFont *textLabelFont = buttonData->textLabel->font();
    textLabelFont->setFontForeColor(curCssData->color());
    textLabelFont->setFontSize(curCssData->fontSize());

    return true;
}

bool TpIconTopButton::onResizeEvent(TpResizeEvent *event)
{
    // TpIconTopButtonData *buttonData = static_cast<TpIconTopButtonData *>(data_);
    // buttonData->iconButton->setRect(0, 0, rect().w, rect().h * 0.8);

    // buttonData->textLabel->setRect(0, rect().h * 0.8, rect().w, rect().h * 0.2);

    return true;
}

bool TpIconTopButton::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        onMousePressEvent((TpMouseEvent *)event);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        onMouseRleaseEvent((TpMouseEvent *)event);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_MOVE_TYPE)
    {
        onMouseMoveEvent((TpMouseEvent *)event);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_LONG_PRESS_TYPE)
    {
        onMouseLongPressEvent((TpMouseEvent *)event);
    }
    else
    {
    }

    return false;
}

void TpIconTopButton::onThemeChangeEvent(TpThemeChangeEvent *event)
{
}

void TpIconTopButton::Init()
{
    TpIconTopButtonData *buttonData = new TpIconTopButtonData();
    data_ = buttonData;

    buttonData->iconLabel = new TpLabel(this);
    buttonData->iconLabel->move(0, 0);
    buttonData->iconLabel->installEventFilter(this);

    buttonData->textLabel = new TpLabel(this);
    buttonData->textLabel->installEventFilter(this);

    buttonData->iconLabel->setEnableBackGroundColor(false);
    buttonData->textLabel->setEnableBackGroundColor(false);

    buttonData->textLabel->setAlign(Tp::AlignCenter);
    // buttonData->textLabel->setBackGroundColor(_RGB(255,0,0));

    tpShared<TpCssData> curCssData = currentStatusCss();
    setIconSize(curCssData->iconSize(), curCssData->iconSize());
}
