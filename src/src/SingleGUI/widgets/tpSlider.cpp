#include "tpSlider.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include "tpRange.h"
#include "tpRect.h"

struct tpSliderData
{
	int32_t maxValue = 100;
	int32_t minValue = 0;
	double value = 0;

	tpSlider::SliderDirect direct = tpSlider::Horizon;

	// 顶点矩形区域
	bool isPressVertex = false;
	ItpRect vertexRect;
	ItpPoint pressPoint;

	// 一个间隔值对应的像素
	double valuePx = 0;

	// 是否正在拖拽调整进度，拖拽过程不响应setValue事件
	bool isDrag = false;
};

tpSlider::tpSlider(tpChildWidget *parent, const SliderDirect &direct)
	: tpChildWidget(parent)
{
	tpSliderData *sliderData = new tpSliderData();
	data_ = sliderData;

	refreshBaseCss();
}

tpSlider::~tpSlider()
{
	tpSliderData *sliderData = static_cast<tpSliderData *>(data_);

	if (sliderData)
	{
		delete sliderData;
		sliderData = nullptr;
		data_ = nullptr;
	}
}

void tpSlider::setRange(const int32_t &min, const int32_t &max)
{
	tpSliderData *sliderData = static_cast<tpSliderData *>(data_);
	sliderData->minValue = min;
	sliderData->maxValue = max;

	if (sliderData->maxValue < sliderData->minValue)
		sliderData->maxValue = sliderData->minValue + 1;

	if (sliderData->value < sliderData->minValue)
		sliderData->value = sliderData->minValue;
	else if (sliderData->value > sliderData->maxValue)
		sliderData->value = sliderData->maxValue;
	else
	{
	}

	rangeChanged.emit(sliderData->minValue, sliderData->maxValue);

	update();
}

void tpSlider::setDirection(const SliderDirect &direct)
{
	tpSliderData *sliderData = static_cast<tpSliderData *>(data_);
	sliderData->direct = direct;

	update();
}

int32_t tpSlider::value()
{
	tpSliderData *sliderData = static_cast<tpSliderData *>(data_);
	return sliderData->value;
}

void tpSlider::setValue(const int32_t &value)
{
	tpSliderData *sliderData = static_cast<tpSliderData *>(data_);

	if (sliderData->isDrag)
		return;

	sliderData->value = value;

	if (sliderData->value < sliderData->minValue)
		sliderData->value = sliderData->minValue;
	else if (sliderData->value > sliderData->maxValue)
		sliderData->value = sliderData->maxValue;
	else
	{
	}

	update();
}

bool tpSlider::onMousePressEvent(tpMouseEvent *event)
{
	tpSliderData *sliderData = static_cast<tpSliderData *>(data_);

	sliderData->isPressVertex = false;
	sliderData->isDrag = false;

	if (event->button() != BUTTON_LEFT)
		return true;

	ItpPoint mousePoint = event->pos();

	if (sliderData->vertexRect.contains(mousePoint))
	{
		if (sliderData->direct == tpSlider::Horizon)
			sliderData->valuePx = 1.0 * (sliderData->maxValue - sliderData->minValue) / width();
		else
			sliderData->valuePx = 1.0 * (sliderData->maxValue - sliderData->minValue) / height();

		sliderData->pressPoint = mousePoint;
		sliderData->isPressVertex = true;
		sliderData->isDrag = true;
	}

	return true;
}

bool tpSlider::onMouseRleaseEvent(tpMouseEvent *event)
{
	tpSliderData *sliderData = static_cast<tpSliderData *>(data_);

	sliderData->isPressVertex = false;
	sliderData->isDrag = false;

	return true;
}

bool tpSlider::onMouseMoveEvent(tpMouseEvent *event)
{
	tpSliderData *sliderData = static_cast<tpSliderData *>(data_);

	if (sliderData->isPressVertex)
	{
		ItpPoint curMotionPoint = event->pos();

		int32_t offsetPx = 0;
		if (sliderData->direct == tpSlider::Horizon)
		{
			offsetPx = curMotionPoint.x - sliderData->pressPoint.x;
		}
		else
		{
			offsetPx = sliderData->pressPoint.y - curMotionPoint.y;
		}

		// std::cout << " offsetPx  " << offsetPx << std::endl;

		if (std::abs(offsetPx) >= sliderData->valuePx)
		{
			int32_t oldValue = sliderData->value;

			sliderData->value += (offsetPx * sliderData->valuePx);
			if (sliderData->value > sliderData->maxValue)
			{
				sliderData->value = sliderData->maxValue;
			}
			else if (sliderData->value < sliderData->minValue)
			{
				sliderData->value = sliderData->minValue;
			}
			else
			{
			}

			// std::cout << " sliderData->value  " << sliderData->value << std::endl;

			int32_t newValue = sliderData->value;
			if (newValue != oldValue)
			{
				valueChanged.emit(newValue);
			}
		}

		sliderData->pressPoint = curMotionPoint;

		update();
	}

	return true;
}

bool tpSlider::onLeaveEvent(tpObjectLeaveEvent *event)
{
	// tpSliderData *sliderData = static_cast<tpSliderData *>(data_);
	// sliderData->isPressVertex = false;
	return true;
}

bool tpSlider::onPaintEvent(tpObjectPaintEvent *event)
{
	tpSliderData *sliderData = static_cast<tpSliderData *>(data_);

	// tpChildWidget::onPaintEvent(event);
	tpShared<tpCssData> curCssData = currentStatusCss();

	tpCanvas *painter = event->canvas();

	// 整体高度、宽度；分成4份。进度条1份，顶点2份，浅色顶点4份
	uint32_t bgWidth = width();
	uint32_t bgHeight = height();
	uint32_t bgX = 0;
	uint32_t bgY = 0;

	// 不能用父类绘制，绘制背景色
	ItpRect rect = event->rect();
	if (objectType() == TP_FLOAT_OBJECT)
	{
		if ((curCssData->backgroundColor() & 0xff) != 0xff)
		{
			painter->erase();
		}
	}

	if (sliderData->direct == tpSlider::Horizon)
	{
		bgHeight = height() / 4.0;
		bgY = (height() - bgHeight) / 2.0;
		painter->roundedBox(0, bgY, rect.w, bgY + bgHeight, roundCorners(), curCssData->backgroundColor());
	}
	else
	{
		bgWidth = width() / 4.0;
		bgX = (width() - bgWidth) / 2.0;
		painter->roundedBox(bgX, 0, bgX + bgWidth, rect.h, roundCorners(), curCssData->backgroundColor());
	}

	// 填充宽度
	uint32_t valueWidth = 0;
	double valuePercent = (1.0 * sliderData->value - sliderData->minValue) / (1.0 * sliderData->maxValue - sliderData->minValue);

	int32_t subColor = curCssData->subColor();
	int32_t lightSubColor = _RGBA(_R(subColor), _G(subColor), _B(subColor), 90);

	// 圆形顶点半径
	uint32_t circleRadius = 0;

	if (sliderData->direct == tpSlider::Horizon)
	{
		circleRadius = height() / 4.0 * 2.0 / 2.0;

		valueWidth = valuePercent * width();
		if (valueWidth != 0)
			painter->roundedBox(0, bgY, valueWidth, bgY + bgHeight, roundCorners(), curCssData->subColor());

		int32_t circleX = valueWidth;

		if (circleX == 0)
		{
			circleX = circleRadius;
		}
		else if (circleX == width())
		{
			circleX = width() - circleRadius;
		}
		else
		{
		}

		// 绘制淡色圆形顶点
		painter->filledCircle(circleX, height() / 2.0, height() / 2.0, lightSubColor);

		// 绘制圆形顶点
		painter->filledCircle(circleX, height() / 2.0, circleRadius, subColor);

		// 记录顶点区域
		sliderData->vertexRect.x = circleX - circleRadius;
		sliderData->vertexRect.y = height() / 2.0 - circleRadius;
		sliderData->vertexRect.w = circleRadius * 2;
		sliderData->vertexRect.h = circleRadius * 2;
	}
	else
	{
		circleRadius = width() / 4.0 * 2.0 / 2.0;

		valueWidth = valuePercent * height();
		if (valueWidth != 0)
			painter->roundedBox(bgX, height() - valueWidth, bgX + bgWidth, height(), roundCorners(), subColor);

		int32_t circleY = height() - valueWidth;

		if (circleY == 0)
		{
			circleY = circleRadius;
		}
		else if (circleY == height())
		{
			circleY = height() - circleRadius;
		}
		else
		{
		}

		// 绘制淡色圆形顶点
		painter->filledCircle(width() / 2.0, circleY, width() / 2.0, lightSubColor);

		// 绘制圆形顶点
		painter->filledCircle(width() / 2.0, circleY, circleRadius, subColor);

		// 记录顶点区域
		sliderData->vertexRect.x = width() / 2.0 - circleRadius;
		sliderData->vertexRect.y = circleY - circleRadius;
		sliderData->vertexRect.w = circleRadius * 2;
		sliderData->vertexRect.h = circleRadius * 2;
	}

	return true;
}
