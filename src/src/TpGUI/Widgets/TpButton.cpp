#include "TpButton.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpFont.h"
#include "TpImage.h"
#include "TpPainter.h"
#include "TpRect.h"
#include "TpLabel.h"
#include <iostream>
#include <thread>

struct TpButtonData
{
    TpLabel *iconLabel;
    TpLabel *textLabel;

    TpButton::ButtonTextStyle buttonStyle = TpButton::TextOnly;
    TpSize iconSize;

    TpButtonData()
    {
    }
};

TpButton::TpButton(TpWidget *parent)
    : TpWidget(parent)
{
    init();
}

TpButton::TpButton(const TpString &iconPath, const TpString &text, TpWidget *parent)
    : TpWidget(parent)
{
    init();

    setIcon(iconPath);
    setText(text);
}

TpButton::TpButton(const TpString &text, TpWidget *parent)
    : TpWidget(parent)
{
    init();

    setText(text);
}

TpButton::~TpButton()
{
    TpButtonData *set = (TpButtonData *)this->data_;

    if (set)
    {
        delete set;
        set = nullptr;
        data_ = nullptr;
    }
}

void TpButton::setText(const TpString &text)
{
    TpButtonData *buttonData = static_cast<TpButtonData *>(this->data_);
    if (!buttonData)
        return;

    buttonData->textLabel->setText(text);
    buttonData->textLabel->update();

    refreshLayout();
}

TpString TpButton::text() const
{
    TpButtonData *buttonData = static_cast<TpButtonData *>(this->data_);
    if (!buttonData)
        return TpString();
    return buttonData->textLabel->text();
}

TpFont *TpButton::font()
{
    TpButtonData *set = (TpButtonData *)this->data_;
    TpFont *pFont = nullptr;

    if (set)
    {
        pFont = set->textLabel->font();
    }

    return pFont;
}

void TpButton::setIcon(const TpString &iconPath)
{
    TpButtonData *set = (TpButtonData *)this->data_;

    set->iconLabel->setBackGroundImage(TpImage(iconPath));
    set->iconLabel->update();
}

void TpButton::setIconSize(const TpSize &size)
{
    setIconSize(size.width(), size.height());
}

void TpButton::setIconSize(const uint32_t &width, const uint32_t &height)
{
    TpButtonData *set = (TpButtonData *)this->data_;
    set->iconSize.setWidth(width);
    set->iconSize.setHeight(height);

    refreshLayout();
}

void TpButton::setButtonStyle(TpButton::ButtonTextStyle buttonStyle)
{
    TpButtonData *set = (TpButtonData *)this->data_;
    if (!set)
        return;

    set->buttonStyle = buttonStyle;

    if (buttonStyle == TpButton::TextOnly)
    {
        set->textLabel->setVisible(true);
        set->iconLabel->setVisible(false);
        set->textLabel->setAlign(Tp::AlignCenter);
    }
    else if (buttonStyle == TpButton::IconOnly)
    {
        set->textLabel->setVisible(false);
        set->iconLabel->setVisible(true);
    }
    else if (buttonStyle == TpButton::TextBesideIcon)
    {
        set->textLabel->setVisible(true);
        set->iconLabel->setVisible(true);
        set->textLabel->setAlign(Tp::AlignCenter);
    }
    else
    {
    }

    onResizeEvent(nullptr);
}

bool TpButton::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    return true;
}

bool TpButton::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpWidget::onMouseRleaseEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    TpPoint mouseGlobalPos = event->globalPos();
    if (toScreen().contains(mouseGlobalPos))
    {
        onClicked.emit(checked());
    }

    update();

    return true;
}

bool TpButton::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpButtonData *buttonData = static_cast<TpButtonData *>(data_);

    if (!buttonData)
        return false;

    // 判断组件当前状态，决定取出哪个CSS样式
    tpShared<TpCssData> curCssData = currentStatusCss();

    TpFont *textLabelFont = buttonData->textLabel->font();
    textLabelFont->setFontColor(curCssData->color());
    textLabelFont->setFontSize(curCssData->fontSize());

    return true;
}

bool TpButton::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);
    refreshLayout();

    return true;
}

bool TpButton::eventFilter(TpObject *watched, TpEvent *event)
{
    // std::cout << "event->eventType() " << event->eventType() << std::endl;

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
    else if (event->eventType() == TpEvent::EVENT_OBJECT_LEAVE_TYPE)
    {
        // std::cout << " TpButton::eventFilter "  << std::endl;
        // onLeaveEvent((TpLeaveEvent *)event);
    }
    else
    {
    }

    return false;
}

void TpButton::onThemeChangeEvent(TpThemeChangeEvent *event)
{
    // tpShared<TpCssData> normalCssData = readCss(TO_STRING(TpButton), TpCssParser::Normal);
}

void TpButton::init()
{
    TpButtonData *set = new TpButtonData();
    data_ = set;

    set->textLabel = new TpLabel(this);
    set->textLabel->installEventFilter(this);
    set->textLabel->setVisible(true);
    set->textLabel->setAlign(Tp::AlignCenter);

    set->iconLabel = new TpLabel(this);
    set->iconLabel->installEventFilter(this);
    set->iconLabel->setVisible(false);
    set->iconLabel->setAlign(Tp::AlignCenter);

    setEnableBackGroundColor(true);
    refreshBaseCss();
}

void TpButton::refreshLayout()
{
    // 尺寸变化、文本变化重新计算布局
    TpButtonData *buttonData = static_cast<TpButtonData *>(data_);

    tpShared<TpCssData> curCssData = currentStatusCss();

    if (buttonData->buttonStyle == TpButton::TextOnly)
    {
        buttonData->textLabel->setWidth(width() - curCssData->paddingLeft() - curCssData->paddingRight());
        buttonData->textLabel->setHeight(height() - curCssData->paddingTop() - curCssData->paddingBottom());
        buttonData->textLabel->move(curCssData->paddingLeft(), curCssData->paddingTop());
        // buttonData->textLabel->update();
    }
    else if (buttonData->buttonStyle == TpButton::IconOnly)
    {
        uint32_t iconWidth = (buttonData->iconSize.width() == 0) ? (width() - curCssData->paddingLeft() - curCssData->paddingRight()) : buttonData->iconSize.width();
        uint32_t iconHeight = (buttonData->iconSize.height() == 0) ? (height() - curCssData->paddingTop() - curCssData->paddingBottom()) : buttonData->iconSize.height();

        int32_t iconX = (width() - iconWidth) / 2.0;
        int32_t iconY = (height() - iconHeight) / 2.0;

        buttonData->iconLabel->setWidth(iconWidth);
        buttonData->iconLabel->setHeight(iconHeight);
        buttonData->iconLabel->move(iconX, iconY);
    }
    else if (buttonData->buttonStyle == TpButton::TextBesideIcon)
    {
        // 计算可用空间
        int32_t availableWidth = width() - curCssData->paddingLeft() - curCssData->paddingRight();
        int32_t availableHeight = height() - curCssData->paddingTop() - curCssData->paddingBottom();

        // 图标保持正方形 (高度决定宽度)
        uint32_t iconWidth = buttonData->iconSize.width();
        uint32_t iconHeight = buttonData->iconSize.height();
        if (iconWidth == 0)
            iconWidth = buttonData->textLabel->font()->pixelHeight() * 1.2;
        if (iconHeight == 0)
            iconHeight = iconWidth;

        // 判断是否文字超出显示区域
        bool isOverland = (buttonData->textLabel->font()->pixelWidth() + iconWidth + curCssData->gap()) > availableWidth;

        // 文本占据剩余宽度 (至少保证不小于0)
        int32_t textWidth = isOverland ? (availableWidth - iconWidth - curCssData->gap()) : buttonData->textLabel->font()->pixelWidth();

        uint32_t startX = isOverland ? curCssData->paddingLeft() : ((availableWidth - iconWidth - curCssData->gap() - textWidth) / 2.0);

        // 调整图标
        uint32_t iconY = (height() > iconHeight) ? (height() - iconHeight) / 2.0 : 0;
        buttonData->iconLabel->setSize(iconWidth, iconHeight);
        buttonData->iconLabel->move(startX, iconY);

        // 调整文本
        buttonData->textLabel->setWidth(textWidth);
        buttonData->textLabel->setHeight(availableHeight);
        buttonData->textLabel->move(
            startX + iconWidth + curCssData->gap(), // 紧贴图标右侧
            curCssData->paddingTop());

        // 更新控件
        // buttonData->iconLabel->update();
        // buttonData->textLabel->update();
    }
    else
    {
    }
}
