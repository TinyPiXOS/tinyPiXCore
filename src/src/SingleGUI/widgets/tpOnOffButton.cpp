
#include "tpOnOffButton.h"
#include "tpCanvas.h"
#include "tpEvent.h"
#include "tpFont.h"
#include "tpLabel.h"
#include "tpAnimation.h"

// 滑块距离边界距离
const uint32_t RectMargin = 3;

struct tpOnOffButtonData
{
	// tpFont *font = nullptr;
	tpOnOffButton::Direction direction = tpOnOffButton::TP_HORIZONTAL;
	bool onOff = false;

	int32_t onColor = _RGB(255, 255, 255);
	int32_t offColor = _RGB(255, 255, 255);

	int32_t onBackColor = _RGB(140, 29, 235);
	int32_t offBackColor = _RGB(217, 217, 217);

	// 滑块Label；使用label来实现动画效果
	tpLabel *onOffRectLabel = nullptr;
	// tpString onText = "";
	// tpString offText = "";

	~tpOnOffButtonData()
	{
		onOffRectLabel->deleteLater();
		// delete font;
		// font = nullptr;
	}
};

tpOnOffButton::tpOnOffButton(tpChildWidget *parent, const Direction &horizontal)
	: tpChildWidget(parent)
{
	tpOnOffButtonData *btnData = new tpOnOffButtonData();
	data_ = btnData;

	btnData->onOffRectLabel = new tpLabel(this);
	btnData->onOffRectLabel->setBackGroundColor(btnData->offColor);
	btnData->onOffRectLabel->installEventFilter(this);
	btnData->onOffRectLabel->move(RectMargin, RectMargin);

	btnData->direction = horizontal;

	// btnData->font = new tpFont();
	// btnData->font->setFontColor(_RGB(0, 0, 0),_RGB(0, 0, 0));

	setEnableBackGroundColor(false);
	setEnabledBorderColor(false);
	setOnOff(false);
}

tpOnOffButton::~tpOnOffButton()
{
	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);
	if (btnData)
	{
		delete btnData;
		btnData = nullptr;
		data_ = nullptr;
	}
}

void tpOnOffButton::setOnOff(bool onOff)
{
	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);

	if (!btnData)
		return;

	if (btnData->onOff != onOff)
	{
		btnData->onOff = onOff;

		tpAnimation *swtichAnimation = new tpAnimation(btnData->onOffRectLabel, tpAnimation::Pos);
		swtichAnimation->setStartValue(btnData->onOffRectLabel->pos());
		swtichAnimation->setDuration(150);

		// 更新滑块位置
		if (btnData->direction == tpOnOffButton::TP_HORIZONTAL)
		{
			if (btnData->onOff)
			{
				swtichAnimation->setEndValue(ItpPoint(width() - btnData->onOffRectLabel->width() - RectMargin, RectMargin));
			}
			else
			{
				swtichAnimation->setEndValue(ItpPoint(RectMargin, RectMargin));
			}
		}
		else
		{
			if (btnData->onOff)
			{
				swtichAnimation->setEndValue(ItpPoint(RectMargin, height() - btnData->onOffRectLabel->height() - RectMargin));
			}
			else
			{
				swtichAnimation->setEndValue(ItpPoint(RectMargin, RectMargin));
			}
		}

		swtichAnimation->start();

		update();
	}
}

bool tpOnOffButton::onOff()
{
	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);
	return btnData->onOff;
}

void tpOnOffButton::setOnColor(const uint32_t &color)
{
	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);
	btnData->onColor = color;
}

void tpOnOffButton::setOffColor(const uint32_t &color)
{
	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);
	btnData->offColor = color;
}

void tpOnOffButton::setOnBackColor(const uint32_t &color)
{
	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);
	btnData->onBackColor = color;
}

void tpOnOffButton::setOffBackColor(const uint32_t &color)
{
	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);
	btnData->offBackColor = color;
}

bool tpOnOffButton::onMousePressEvent(tpMouseEvent *event)
{
	tpChildWidget::onMousePressEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);
	if (!btnData)
		return true;

	setOnOff(!onOff());
	onClicked.emit(onOff());

	return true;
}

bool tpOnOffButton::onPaintEvent(tpObjectPaintEvent *event)
{
	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);
	if (!btnData)
		return true;

	tpChildWidget::onPaintEvent(event);

	tpCanvas *canvas = event->canvas();

	// 绘制背景色
	int32_t backColor = btnData->onOff ? btnData->onBackColor : btnData->offBackColor;
	canvas->roundedBox(0, 0, width(), height(), roundCorners(), backColor);

	return true;
}

bool tpOnOffButton::onResizeEvent(tpObjectResizeEvent *event)
{
	tpOnOffButtonData *btnData = static_cast<tpOnOffButtonData *>(data_);
	if (!btnData)
		return true;

	tpChildWidget::onResizeEvent(event);

	// 尺寸变化同步更改圆角值
	if (btnData->direction == tpOnOffButton::TP_HORIZONTAL)
	{
		setRoundCorners(height() * 0.5);

		int32_t rectWidthHeight = height() - RectMargin * 2;
		if (rectWidthHeight < 0)
			rectWidthHeight = height();
		btnData->onOffRectLabel->setSize(rectWidthHeight, rectWidthHeight);
		btnData->onOffRectLabel->setRoundCorners(rectWidthHeight / 2.0);
	}
	else
	{
		setRoundCorners(width() * 0.5);

		int32_t rectWidthHeight = width() - RectMargin * 2;
		if (rectWidthHeight < 0)
			rectWidthHeight = width();
		btnData->onOffRectLabel->setSize(rectWidthHeight, rectWidthHeight);
		btnData->onOffRectLabel->setRoundCorners(rectWidthHeight / 2.0);
	}

	return true;
}

bool tpOnOffButton::eventFilter(tpObject *watched, tpEvent *event)
{
	if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
	{
		tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
		onMousePressEvent(mouseEvent);
	}

	return false;
}
