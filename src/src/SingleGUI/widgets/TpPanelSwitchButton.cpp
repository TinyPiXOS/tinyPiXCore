#include "TpPanelSwitchButton.h"
#include "TpCanvas.h"
#include "TpLabel.h"
#include "TpFont.h"
#include "TpImage.h"

struct TpPanelSwitchButtonData
{
    TpLabel *textLabel;
    TpLabel *iconLabel;

    bool checkable = true;

    bool isChecked = false;
};

TpPanelSwitchButton::TpPanelSwitchButton(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    setEnabledBorderColor(false);

    TpPanelSwitchButtonData *buttonData = new TpPanelSwitchButtonData();
    data_ = buttonData;

    buttonData->textLabel = new TpLabel(this);
    buttonData->textLabel->installEventFilter(this);
    buttonData->textLabel->setAlign(tinyPiX::AlignHCenter);

    buttonData->iconLabel = new TpLabel(this);
    buttonData->iconLabel->installEventFilter(this);

    refreshBaseCss();
}

TpPanelSwitchButton::~TpPanelSwitchButton()
{
}

void TpPanelSwitchButton::setIcon(const TpString &iconPath)
{
    TpPanelSwitchButtonData *buttonData = static_cast<TpPanelSwitchButtonData *>(data_);

    buttonData->iconLabel->setBackGroundImage(TpImage(iconPath));
    buttonData->iconLabel->update();
}

void TpPanelSwitchButton::setText(const TpString &text)
{
    TpPanelSwitchButtonData *buttonData = static_cast<TpPanelSwitchButtonData *>(data_);
    buttonData->textLabel->setText(text);
}

TpString TpPanelSwitchButton::text()
{
    TpPanelSwitchButtonData *buttonData = static_cast<TpPanelSwitchButtonData *>(data_);

    return buttonData->textLabel->text();
}

bool TpPanelSwitchButton::onResizeEvent(TpObjectResizeEvent *event)
{
    tpShared<TpCssData> curCssData = currentStatusCss();

    TpPanelSwitchButtonData *buttonData = static_cast<TpPanelSwitchButtonData *>(data_);
    buttonData->textLabel->setSize(rect().w, buttonData->textLabel->font()->pixelHeight());

    buttonData->textLabel->move(0, rect().h - buttonData->textLabel->font()->pixelHeight() - curCssData->paddingBottom());

    // uint32_t iconSize = curCssData->iconSize();
    uint32_t iconSize = ((width() > height()) ? height() : width()) * 0.32679;

    uint32_t iconX = (width() - iconSize) / 2.0;
    // uint32_t iconRad =  curCssData->roundCorners();

    buttonData->iconLabel->setRoundCorners(curCssData->roundCorners());
    buttonData->iconLabel->setSize(iconSize, iconSize);
    buttonData->iconLabel->move(iconX, curCssData->paddingTop());

    return true;
}

bool TpPanelSwitchButton::onMousePressEvent(TpMouseEvent *event)
{
    TpChildWidget::onMousePressEvent(event);

    return false;
}

bool TpPanelSwitchButton::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpChildWidget::onMouseRleaseEvent(event);

    onClicked.emit(checked());

    return true;
}

bool TpPanelSwitchButton::onPaintEvent(TpObjectPaintEvent *event)
{
    TpPanelSwitchButtonData *buttonData = static_cast<TpPanelSwitchButtonData *>(data_);

    // std::cout << "TpPanelSwitchButton::onPaintEvent " << std::endl;
    TpChildWidget::onPaintEvent(event);

    TpCanvas *paintCanvas = event->canvas();

    // 判断组件当前状态，决定取出哪个CSS样式
    tpShared<TpCssData> curCssData = currentStatusCss();

    TpFont *textLabelFont = buttonData->textLabel->font();
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

bool TpPanelSwitchButton::onLeaveEvent(TpObjectLeaveEvent *event)
{
    TpChildWidget::onLeaveEvent(event);
    // std::cout << " TpPanelSwitchButton::onLeaveEvent " << event->leave() << std::endl;

    return true;
}

bool TpPanelSwitchButton::eventFilter(TpObject *watched, TpEvent *event)
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
