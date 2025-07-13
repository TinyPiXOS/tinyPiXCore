#include "tpPanelSwitchButton.h"
#include "tpCanvas.h"
#include "tpLabel.h"
#include "tpFont.h"
#include "tpSurface.h"

struct tpPanelSwitchButtonData
{
    tpLabel *textLabel;
    tpLabel *iconLabel;

    bool checkable = true;

    bool isChecked = false;
};

tpPanelSwitchButton::tpPanelSwitchButton(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    setEnabledBorderColor(false);

    tpPanelSwitchButtonData *buttonData = new tpPanelSwitchButtonData();
    data_ = buttonData;

    buttonData->textLabel = new tpLabel(this);
    buttonData->textLabel->installEventFilter(this);
    buttonData->textLabel->setAlign(tinyPiX::AlignHCenter);

    buttonData->iconLabel = new tpLabel(this);
    buttonData->iconLabel->installEventFilter(this);

    refreshBaseCss();
}

tpPanelSwitchButton::~tpPanelSwitchButton()
{
}

void tpPanelSwitchButton::setIcon(const tpString &iconPath)
{
    tpPanelSwitchButtonData *buttonData = static_cast<tpPanelSwitchButtonData *>(data_);

    tpShared<tpSurface> iconSurface = tpMakeShared<tpSurface>();
    iconSurface->fromFile(iconPath);
    buttonData->iconLabel->setBackGroundImage(iconSurface);
    buttonData->iconLabel->update();
}

void tpPanelSwitchButton::setText(const tpString &text)
{
    tpPanelSwitchButtonData *buttonData = static_cast<tpPanelSwitchButtonData *>(data_);
    buttonData->textLabel->setText(text);
}

tpString tpPanelSwitchButton::text()
{
    tpPanelSwitchButtonData *buttonData = static_cast<tpPanelSwitchButtonData *>(data_);

    return buttonData->textLabel->text();
}

bool tpPanelSwitchButton::onResizeEvent(tpObjectResizeEvent *event)
{
    tpShared<tpCssData> curCssData = currentStatusCss();

    tpPanelSwitchButtonData *buttonData = static_cast<tpPanelSwitchButtonData *>(data_);
    buttonData->textLabel->setSize(rect().w, buttonData->textLabel->font()->pixelHeight());

    buttonData->textLabel->move(0, rect().h - buttonData->textLabel->font()->pixelHeight() - curCssData->paddingBottom());

    uint32_t iconSize = curCssData->iconSize();
    uint32_t iconX = (width() - iconSize) / 2.0;
    // uint32_t iconRad =  curCssData->roundCorners();

    buttonData->iconLabel->setRoundCorners(curCssData->roundCorners());
    buttonData->iconLabel->setSize(iconSize, iconSize);
    buttonData->iconLabel->move(iconX, curCssData->paddingTop());

    return true;
}

bool tpPanelSwitchButton::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    return false;
}

bool tpPanelSwitchButton::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    onClicked.emit(checked());

    return true;
}

bool tpPanelSwitchButton::onPaintEvent(tpObjectPaintEvent *event)
{
    tpPanelSwitchButtonData *buttonData = static_cast<tpPanelSwitchButtonData *>(data_);

    // std::cout << "tpPanelSwitchButton::onPaintEvent " << std::endl;
    tpChildWidget::onPaintEvent(event);

    tpCanvas *paintCanvas = event->canvas();

    // 判断组件当前状态，决定取出哪个CSS样式
    tpShared<tpCssData> curCssData = currentStatusCss();

    tpFont *textLabelFont = buttonData->textLabel->font();
    textLabelFont->setFontForeColor(curCssData->color());
    textLabelFont->setFontSize(curCssData->fontSize());

    // uint32_t iconSize = curCssData->iconSize();

    // uint32_t iconX = (width() - iconSize) / 2.0;

    // // 绘制icon,如果没有icon，绘制一个灰色背景
    // uint32_t iconRad = iconSize * curCssData->roundCorners();
    // int32_t iconColor = curCssData->iconBackground();
    // paintCanvas->roundedBox(iconX, curCssData->paddingTop(), iconX + iconSize, curCssData->paddingTop() + iconSize, iconRad, curCssData->iconBackground());

    return true;
}

bool tpPanelSwitchButton::onLeaveEvent(tpObjectLeaveEvent *event)
{
    tpChildWidget::onLeaveEvent(event);
    // std::cout << " tpPanelSwitchButton::onLeaveEvent " << event->leave() << std::endl;

    return true;
}

bool tpPanelSwitchButton::eventFilter(tpObject *watched, tpEvent *event)
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
