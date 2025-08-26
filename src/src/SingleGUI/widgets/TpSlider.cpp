#include "TpSlider.h"
#include "TpEvent.h"
#include "TpCanvas.h"
#include "TpRange.h"
#include "TpRect.h"

struct TpSliderData
{
	int32_t maxValue = 100;
	int32_t minValue = 0;
	double value = 0;

	TpSlider::SliderDirect direct = TpSlider::Horizon;

	// 顶点矩形区域
	bool isPressVertex = false;
	ItpRect vertexRect;
	ItpPoint pressPoint;

	// 一个间隔值对应的像素
	double valuePx = 0;

	// 是否正在拖拽调整进度，拖拽过程不响应setValue事件
	bool isDrag = false;
};

TpSlider::TpSlider(TpChildWidget *parent, const SliderDirect &direct)
	: TpChildWidget(parent)
{
	TpSliderData *sliderData = new TpSliderData();
	data_ = sliderData;

	refreshBaseCss();
}

TpSlider::~TpSlider()
{
	TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

	if (sliderData)
	{
		delete sliderData;
		sliderData = nullptr;
		data_ = nullptr;
	}
}

void TpSlider::setRange(const int32_t &min, const int32_t &max)
{
	TpSliderData *sliderData = static_cast<TpSliderData *>(data_);
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

void TpSlider::setDirection(const SliderDirect &direct)
{
	TpSliderData *sliderData = static_cast<TpSliderData *>(data_);
	sliderData->direct = direct;

	update();
}

int32_t TpSlider::value()
{
	TpSliderData *sliderData = static_cast<TpSliderData *>(data_);
	return sliderData->value;
}

void TpSlider::setValue(const int32_t &value)
{
	TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

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

bool TpSlider::onMousePressEvent(TpMouseEvent *event)
{
	TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

	sliderData->isPressVertex = false;
	sliderData->isDrag = false;

	if (event->button() != BUTTON_LEFT)
		return true;

	ItpPoint mousePoint = event->pos();

	if (sliderData->vertexRect.contains(mousePoint))
	{
		if (sliderData->direct == TpSlider::Horizon)
			sliderData->valuePx = 1.0 * (sliderData->maxValue - sliderData->minValue) / width();
		else
			sliderData->valuePx = 1.0 * (sliderData->maxValue - sliderData->minValue) / height();

		sliderData->pressPoint = mousePoint;
		sliderData->isPressVertex = true;
		sliderData->isDrag = true;
	}

	return true;
}

bool TpSlider::onMouseRleaseEvent(TpMouseEvent *event)
{
	TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

	sliderData->isPressVertex = false;
	sliderData->isDrag = false;

	return true;
}

bool TpSlider::onMouseMoveEvent(TpMouseEvent *event)
{
	TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

	if (sliderData->isPressVertex)
	{
		ItpPoint curMotionPoint = event->pos();

		int32_t offsetPx = 0;
		if (sliderData->direct == TpSlider::Horizon)
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

bool TpSlider::onLeaveEvent(TpObjectLeaveEvent *event)
{
	// TpSliderData *sliderData = static_cast<TpSliderData *>(data_);
	// sliderData->isPressVertex = false;
	return true;
}

bool TpSlider::onPaintEvent(TpObjectPaintEvent *event)
{
	TpSliderData *sliderData = static_cast<TpSliderData *>(data_);

	// TpChildWidget::onPaintEvent(event);
	tpShared<TpCssData> curCssData = currentStatusCss();

	TpCanvas *painter = event->canvas();

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

	if (sliderData->direct == TpSlider::Horizon)
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

	if (sliderData->direct == TpSlider::Horizon)
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
