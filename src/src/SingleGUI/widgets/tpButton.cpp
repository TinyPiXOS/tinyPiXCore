#include "tpButton.h"
#include "tpCanvas.h"
#include "tpEvent.h"
#include "tpFont.h"
#include "tpSurface.h"
#include "tpCanvas.h"
#include "tpRect.h"
#include "tpLabel.h"
#include <iostream>
#include <thread>

struct tpButtonData
{
	tpLabel *iconLabel;
	tpLabel *textLabel;

	tpButton::ButtonTextStyle buttonStyle = tpButton::TextOnly;
	ItpSize iconSize;

	tpButtonData()
	{
	}
};

tpButton::tpButton(tpChildWidget *parent)
	: tpChildWidget(parent)
{
	Init();
}

tpButton::tpButton(const tpString &iconPath, const tpString &text, tpChildWidget *parent)
	: tpChildWidget(parent)
{
	Init();

	setIcon(iconPath);
	setText(text);
}

tpButton::tpButton(const tpString &text, tpChildWidget *parent)
	: tpChildWidget(parent)
{
	Init();

	setText(text);
}

tpButton::~tpButton()
{
	tpButtonData *set = (tpButtonData *)this->data_;

	if (set)
	{
		delete set;
		set = nullptr;
		data_ = nullptr;
	}
}

void tpButton::setText(const tpString &text)
{
	tpButtonData *buttonData = static_cast<tpButtonData *>(this->data_);
	if (!buttonData)
		return;

	tpChildWidget::setText(text);

	buttonData->textLabel->setText(text);
	buttonData->textLabel->update();
}

tpFont *tpButton::font()
{
	tpButtonData *set = (tpButtonData *)this->data_;
	tpFont *pFont = nullptr;

	if (set)
	{
		pFont = set->textLabel->font();
	}

	return pFont;
}

void tpButton::setIcon(const tpString &iconPath)
{
	tpButtonData *set = (tpButtonData *)this->data_;
	tpShared<tpSurface> iconSurface = tpMakeShared<tpSurface>();
	iconSurface->fromFile(iconPath);

	// set->iconLabel->setBackGroundColor(_RGB(255, 50, 50));
	set->iconLabel->setBackGroundImage(iconSurface);
	set->iconLabel->update();
}

void tpButton::setIconSize(const ItpSize &size)
{
	setIconSize(size.w, size.h);
}

void tpButton::setIconSize(const uint32_t &width, const uint32_t &height)
{
	tpButtonData *set = (tpButtonData *)this->data_;
	set->iconSize.w = width;
	set->iconSize.h = height;
}

void tpButton::setButtonStyle(tpButton::ButtonTextStyle buttonStyle)
{
	tpButtonData *set = (tpButtonData *)this->data_;
	if (!set)
		return;

	set->buttonStyle = buttonStyle;

	if (buttonStyle == tpButton::TextOnly)
	{
		set->textLabel->setVisible(true);
		set->iconLabel->setVisible(false);
		set->textLabel->setAlign(tinyPiX::AlignHCenter);
	}
	else if (buttonStyle == tpButton::IconOnly)
	{
		set->textLabel->setVisible(false);
		set->iconLabel->setVisible(true);
	}
	else if (buttonStyle == tpButton::TextBesideIcon)
	{
		set->textLabel->setVisible(true);
		set->iconLabel->setVisible(true);
		set->textLabel->setAlign(tinyPiX::AlignHCenter);
	}
	else
	{
	}

	onResizeEvent(nullptr);
}

void tpButton::setRoundCorners(const uint32_t &roundPercent)
{
	tpChildWidget::setRoundCorners(roundPercent);

	// tpButtonData *buttonData = static_cast<tpButtonData *>(data_);
	// buttonData->iconLabel->setRoundCorners(roundPercent);
	// buttonData->textLabel->setRoundCorners(roundPercent);
}

bool tpButton::onMousePressEvent(tpMouseEvent *event)
{
	tpChildWidget::onMousePressEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	return true;
}

bool tpButton::onMouseRleaseEvent(tpMouseEvent *event)
{
	tpChildWidget::onMouseRleaseEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	ItpPoint mouseGlobalPos = event->globalPos();
	if (toScreen().contains(mouseGlobalPos))
	{
		onClicked.emit(checked());
	}

	update();

	return true;
}

bool tpButton::onPaintEvent(tpObjectPaintEvent *event)
{
	tpChildWidget::onPaintEvent(event);

	tpButtonData *buttonData = static_cast<tpButtonData *>(data_);

	if (!buttonData)
		return false;

	// 判断组件当前状态，决定取出哪个CSS样式
	tpShared<tpCssData> curCssData = currentStatusCss();

	tpFont *textLabelFont = buttonData->textLabel->font();
	textLabelFont->setFontForeColor(curCssData->color());
	textLabelFont->setFontSize(curCssData->fontSize());

	return true;
}

bool tpButton::onResizeEvent(tpObjectResizeEvent *event)
{
	tpButtonData *buttonData = static_cast<tpButtonData *>(data_);

	tpShared<tpCssData> curCssData = currentStatusCss();

	if (buttonData->buttonStyle == tpButton::TextOnly)
	{
		buttonData->textLabel->setWidth(rect().w - curCssData->paddingLeft() - curCssData->paddingRight());
		buttonData->textLabel->setHeight(rect().h - curCssData->paddingTop() - curCssData->paddingBottom());
		buttonData->textLabel->move(curCssData->paddingLeft(), curCssData->paddingTop());
		// buttonData->textLabel->update();
	}
	else if (buttonData->buttonStyle == tpButton::IconOnly)
	{
		uint32_t iconWidth = (buttonData->iconSize.w == 0) ? (rect().w - curCssData->paddingLeft() - curCssData->paddingRight()) : buttonData->iconSize.w;
		uint32_t iconHeight = (buttonData->iconSize.h == 0) ? (rect().h - curCssData->paddingTop() - curCssData->paddingBottom()) : buttonData->iconSize.h;

		int32_t iconX = (buttonData->iconSize.w == 0) ? curCssData->paddingLeft() : (width() - iconWidth) / 2.0;
		int32_t iconY = (buttonData->iconSize.h == 0) ? curCssData->paddingTop() : (height() - iconHeight) / 2.0;

		buttonData->iconLabel->setWidth(iconWidth);
		buttonData->iconLabel->setHeight(iconHeight);
		buttonData->iconLabel->move(iconX, iconY);
	}
	else if (buttonData->buttonStyle == tpButton::TextBesideIcon)
	{
		// 计算可用空间
		int availableWidth = rect().w - curCssData->paddingLeft() - curCssData->paddingRight();
		int availableHeight = rect().h - curCssData->paddingTop() - curCssData->paddingBottom();

		// 图标保持正方形 (高度决定宽度)
		int iconWidth = buttonData->textLabel->font()->pixelHeight();
		int iconHeight = iconWidth;

		// 判断是否文字超出显示区域
		bool isOverland = (buttonData->textLabel->font()->pixelWidth() + iconWidth + curCssData->gap()) > availableWidth;

		// 文本占据剩余宽度 (至少保证不小于0)
		int textWidth = isOverland ? (availableWidth - iconWidth - curCssData->gap()) : buttonData->textLabel->font()->pixelWidth();

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

	return true;
}

bool tpButton::eventFilter(tpObject *watched, tpEvent *event)
{
	// std::cout << "event->eventType() " << event->eventType() << std::endl;

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
	else if (event->eventType() == tpEvent::EVENT_OBJECT_LEAVE_TYPE)
	{
		// std::cout << " tpButton::eventFilter "  << std::endl;

		// onLeaveEvent((tpObjectLeaveEvent *)event);
	}
	else
	{
	}

	return false;
}

void tpButton::onThemeChangeEvent(tpThemeChangeEvent *event)
{
	// tpShared<tpCssData> normalCssData = readCss(TO_STRING(tpButton), tpCssParser::Normal);
}

void tpButton::Init()
{
	tpButtonData *set = new tpButtonData();
	data_ = set;

	set->textLabel = new tpLabel(this);
	set->textLabel->installEventFilter(this);
	set->textLabel->setVisible(true);
	set->textLabel->setAlign(tinyPiX::AlignHCenter);

	set->iconLabel = new tpLabel(this);
	set->iconLabel->installEventFilter(this);
	set->iconLabel->setVisible(false);
	set->iconLabel->setAlign(tinyPiX::AlignHCenter);

	setEnableBackGroundColor(true);
	refreshBaseCss();
}
