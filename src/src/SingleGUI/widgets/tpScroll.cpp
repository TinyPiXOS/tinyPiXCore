#include "tpScroll.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include "tpRange.h"
#include "tpRect.h"
#include <cstring>

typedef struct
{
	tpRange range;
	tpRange rectRange;
	tpRange mapRange;

	struct
	{
		bool direction;
		bool down;
	};

	ItpRect barRect;

	struct
	{
		int32_t normalBarColor;
		int32_t hoverBarColor;
		int32_t downBarColor;
	};

	struct
	{
		int32_t lineColor;
		int32_t barColor;
	};

	int32_t lines;
	int32_t delta;
	int32_t lastBarPos;
} ItpScrollSet;

static inline void adjustBar(tpScroll *scroll, ItpScrollSet *set)
{
	double percent = set->range.percent();
	int32_t barW = TP_MIN(scroll->width(), scroll->height()) - 2;
	int32_t barLength = (int32_t)(set->lines * set->rectRange.length() / (double)set->range.length());

	if (barLength < TP_MIN(scroll->width() - 1, scroll->height() - 1))
	{
		barLength = TP_MIN(scroll->width() - 1, scroll->height() - 1);
	}

	switch (set->direction)
	{
	case false:
	{
		set->barRect.x = (int32_t)(percent * set->rectRange.length());

		if ((set->barRect.x + barLength) > (scroll->width() - 1))
		{
			set->barRect.x = (int32_t)(scroll->width() - barLength - 1);
		}

		if (set->barRect.x < 1)
		{
			set->barRect.x = 1;
		}

		set->barRect.y = 1;
		set->barRect.w = barLength;
		set->barRect.h = barW;

		set->mapRange.setRange(1, scroll->width() - barLength - 1);
	}
	break;
	case true:
	{
		set->barRect.y = (int32_t)(percent * set->rectRange.length());

		if ((set->barRect.y + barLength) > (scroll->height() - 1))
		{
			set->barRect.y = (int32_t)(scroll->height() - barLength - 1);
		}

		if (set->barRect.y < 1)
		{
			set->barRect.y = 1;
		}

		set->barRect.x = 1;
		set->barRect.w = barW;
		set->barRect.h = barLength;

		set->mapRange.setRange(1, scroll->height() - barLength - 1);
	}
	break;
	}

	set->mapRange.setPercent(percent);
}

tpScroll::tpScroll(tpChildWidget *parent, bool horizontal) : tpChildWidget(parent)
{
	ItpScrollSet *set = new ItpScrollSet();

	if (set)
	{
		set->range.setRange(0, 100);
		set->rectRange.setRange(0, 100);
		set->mapRange.setRange(1, 99);
		set->direction = horizontal;
		set->down = false;
		set->lineColor = tpColors::White;
		set->lines = 20;
		set->delta = 0;
		set->lastBarPos = 1;

		set->normalBarColor = tpColors::LightGray;
		set->hoverBarColor = tpColors::Grey;
		set->downBarColor = tpColors::DarkGoldenrod;
		set->barColor = tpColors::LightGray;

		memset(&set->barRect, 0, sizeof(ItpRect));

		this->setBackGroundColor(tpColors::White);
		this->scrollSet = set;
	}
}

tpScroll::~tpScroll()
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		delete set;
	}
}

bool tpScroll::direction()
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	bool direction = false;

	if (set)
	{
		direction = set->direction;
	}

	return direction;
}

void tpScroll::setLinePerPage(int32_t lines)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		if (lines <= 0)
		{
			lines = 1;
		}

		if (lines >= set->range.length())
		{
			lines = set->range.length();
		}

		set->lines = lines;
		adjustBar(this, set);
	}
}

int32_t tpScroll::pages()
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	int32_t page = 0;

	if (set)
	{
		int32_t mod = set->range.length() % set->lines;
		page = set->range.length() / set->lines;

		if (mod)
		{
			page++;
		}
	}

	return page;
}

int32_t tpScroll::pageIndex()
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	int32_t index = 0;

	if (set)
	{
		int32_t mod = set->range.length() % set->lines;
		int32_t page = set->range.length() / set->lines - 1;

		if (mod)
		{
			page++;
		}

		index = (int32_t)(page * set->range.percent());
	}

	return index;
}

int32_t tpScroll::linesPerPage()
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	if (!set)
		return 0;

	return set->lines;
}

int32_t tpScroll::min()
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	if (!set)
		return 0;

	return set->range.min();
}

int32_t tpScroll::max()
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	if (!set)
		return 0;

	return set->range.max();
}

void tpScroll::setMaxRange(int32_t max)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		if (max < 0)
		{
			max = 0;
		}

		double percent = set->range.percent();
		set->range.setRange(0, max);
		set->range.setPercent(percent);
		set->rectRange.setPercent(percent);

		adjustBar(this, set);
	}
}

void tpScroll::zoomRange(int32_t delta)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		int32_t max = set->range.max() + delta;

		if (max < 0)
		{
			max = 0;
		}

		double percent = set->range.percent();
		set->range.setRange(0, max);
		set->range.setPercent(percent);
		set->rectRange.setPercent(percent);

		adjustBar(this, set);
	}
}

void tpScroll::setDirection(bool horizontal)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		set->direction = horizontal;
		int32_t w = this->width(), h = this->height();

		switch (set->direction)
		{
		case false:
		{
			set->rectRange.setRange(0, w);
		}
		break;
		case true:
		{
			set->rectRange.setRange(0, h);
		}
		break;
		}

		adjustBar(this, set);
	}
}

void tpScroll::setPosition(int32_t position)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (!set)
		return;

	if (position < set->range.min())
	{
		position = set->range.min();
	}

	if (position > set->range.max())
	{
		position = set->range.max();
	}

	if (position == set->range.position())
	{
		return;
	}

	set->range.setPosition(position);
	double percent = set->range.percent();
	set->rectRange.setPercent(percent);
	set->mapRange.setPercent(percent);

	switch (set->direction)
	{
	case false:
	{
		set->barRect.x = set->mapRange.position();
		onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
	}
	break;
	case true:
	{
		set->barRect.y = set->mapRange.position();
		onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
	}
	break;
	}
}

void tpScroll::setPercent(double percent)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		set->range.setPercent(percent);
		percent = set->range.percent();
		set->rectRange.setPercent(percent);
		set->mapRange.setPercent(percent);
		switch (set->direction)
		{
		case false:
		{
			set->barRect.x = set->mapRange.position();
			onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
		}
		break;
		case true:
		{
			set->barRect.y = set->mapRange.position();
			onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
		}
		break;
		}
	}
}

int32_t tpScroll::position()
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	int32_t position = 0;

	if (set)
	{
		position = set->range.position();
	}

	return position;
}

double tpScroll::percent()
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	double percent = 0;

	if (set)
	{
		percent = set->range.percent();
	}

	return percent;
}

void tpScroll::setRect(tpRect &rect)
{
	this->setRect(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

void tpScroll::setRect(tpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->X0(), rect->Y0(), rect->width(), rect->height());
	}
}

void tpScroll::setRect(ItpRect &rect)
{
	this->setRect(rect.x, rect.y, rect.w, rect.h);
}

void tpScroll::setRect(ItpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->x, rect->y, rect->w, rect->h);
	}
}

void tpScroll::setRect(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		int32_t bar = TP_MIN(w, h);
		double percent = set->range.percent();

		switch (set->direction)
		{
		case false:
		{
			set->rectRange.setRange(0, w);
		}
		break;
		case true:
		{
			set->rectRange.setRange(0, h);
		}
		break;
		}

		set->rectRange.setPercent(percent);
		tpChildWidget::setRect(x, y, w, h);
		adjustBar(this, set);
	}
}

void tpScroll::setNormalBarColor(uint32_t color)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		set->normalBarColor = color;
	}
}

void tpScroll::setNormalBarColor(tpColors &color)
{
	this->setNormalBarColor(color.rgba());
}

void tpScroll::setHoverBarColor(uint32_t color)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		set->hoverBarColor = color;
	}
}

void tpScroll::setHoverBarColor(tpColors &color)
{
	this->setHoverBarColor(color.rgba());
}

void tpScroll::setDownBarColor(uint32_t color)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		set->downBarColor = color;
	}
}

void tpScroll::setDownBarColor(tpColors &color)
{
	this->setDownBarColor(color.rgba());
}

void tpScroll::setLineColor(uint32_t color)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (set)
	{
		set->lineColor = color;
	}
}

void tpScroll::setLineColor(tpColors &color)
{
	this->setLineColor(color.rgba());
}

bool tpScroll::onMouseMoveEvent(tpMouseEvent *event)
{
	bool ret = tpChildWidget::onMouseMoveEvent(event);

	if (!ret)
		return ret;

	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	if (!set)
		return ret;

	int32_t nBarPos = 0, position = 0;
	int32_t barLength = 0;

	if (event->state())
	{
		if (set->down)
		{
			switch (set->direction)
			{
			case false:
			{
				if (this->width() == set->barRect.w)
				{
					return false;
				}

				nBarPos = event->pos().x - set->delta;

				if (nBarPos <= 1)
				{
					nBarPos = 1;
				}
				else if (nBarPos >= (this->width() - set->barRect.w - 1))
				{
					nBarPos = this->width() - set->barRect.w - 1;
				}

				barLength = set->barRect.w;
				set->barRect.x = nBarPos;
			}
			break;
			case true:
			{
				if (this->height() == set->barRect.h)
				{
					return false;
				}

				nBarPos = event->pos().y - set->delta;

				if (nBarPos <= 1)
				{
					nBarPos = 1;
				}
				else if (nBarPos >= (this->height() - set->barRect.h - 1))
				{
					nBarPos = this->height() - set->barRect.h - 1;
				}

				barLength = set->barRect.h;
				set->barRect.y = nBarPos;
			}
			break;
			}

			if (set->lastBarPos != nBarPos)
			{
				set->mapRange.setPosition(nBarPos);
				double percent = set->mapRange.percent();
				set->rectRange.setPercent(percent);
				set->range.setPercent(percent);
				position = set->range.position();
				onScroll.emit(this, position, this->pages(), this->pageIndex(), percent);
				set->lastBarPos = nBarPos;
			}
		}
		else
		{
			ItpPoint point = event->pos();
			tpRect barRRect(set->barRect);

			if (barRRect.in(point))
			{
				set->barColor = set->hoverBarColor;
			}

			set->down = false;
		}
	}
	else
	{
		ItpPoint point = event->pos();
		tpRect barRRect(set->barRect);

		if (barRRect.in(point))
		{
			set->barColor = set->hoverBarColor;
		}
		else
		{
			set->barColor = set->normalBarColor;
		}

		set->down = false;
	}

	this->update();

	return ret;
}

bool tpScroll::onMousePressEvent(tpMouseEvent *event)
{
	tpChildWidget::onMousePressEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	set->down = true;
	set->barColor = set->downBarColor;
	tpRect barRRect(set->barRect);

	switch (set->direction)
	{
	case false:
	{
		ItpPoint point;
		point.x = event->pos().x;
		point.y = 1;

		if (barRRect.in(point) == false)
		{
			set->rectRange.setPosition(event->pos().x);
			double percent = set->rectRange.percent();
			set->range.setPercent(percent);
			set->mapRange.setPercent(percent);
			set->barRect.x = set->mapRange.position();
			onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
		}
		else
		{
			set->delta = event->pos().x - set->barRect.x;
		}
	}
	break;
	case true:
	{
		ItpPoint point;
		point.x = 1;
		point.y = event->pos().y;

		if (barRRect.in(point) == false)
		{
			set->rectRange.setPosition(event->pos().y);
			double percent = set->rectRange.percent();
			set->range.setPercent(percent);
			set->mapRange.setPercent(percent);
			set->barRect.y = set->mapRange.position();
			onScroll.emit(this, set->range.position(), this->pages(), this->pageIndex(), percent);
		}
		else
		{
			set->delta = event->pos().y - set->barRect.y;
		}
	}
	break;
	}

	return true;
}

bool tpScroll::onMouseRleaseEvent(tpMouseEvent *event)
{
	tpChildWidget::onMouseRleaseEvent(event);
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;

	set->down = false;
	set->barColor = set->hoverBarColor;
	update();

	return true;
}

bool tpScroll::onFocusEvent(tpObjectFocusEvent *event)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	bool ret = false;

	if (set)
	{
		ret = tpChildWidget::onFocusEvent(event);

		if (ret)
		{
			if (event->focused() == false)
			{
				set->down = false;
				set->barColor = set->normalBarColor;
				this->update();
			}
		}
	}

	return ret;
}

bool tpScroll::onLeaveEvent(tpObjectLeaveEvent *event)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	bool ret = false;

	if (!set)
		return ret;

	ret = tpChildWidget::onLeaveEvent(event);

	if (!ret)
		return ret;

	if (event->leave() == false)
	{
		if (set->down == false)
		{
			set->barColor = set->normalBarColor;
			this->update();
		}
	}

	return ret;
}

bool tpScroll::onPaintEvent(tpObjectPaintEvent *event)
{
	ItpScrollSet *set = (ItpScrollSet *)this->scrollSet;
	bool ret = false;

	if (!set)
		return ret;

	ret = tpChildWidget::onPaintEvent(event);

	if (!ret)
		return ret;

	tpCanvas *canvas = event->canvas();
	uint8_t alpha = mapAlpha((uint8_t)(set->barColor & 0x000000ff), this->alpha());
	int32_t x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	switch (set->direction)
	{
	case false:
	{
		x0 = set->barRect.x;
		y0 = set->barRect.y;
		x1 = x0 + set->barRect.w - 1;
		y1 = set->barRect.h;
	}
	break;
	case true:
	{
		x0 = set->barRect.x;
		y0 = set->barRect.y;
		x1 = set->barRect.w;
		y1 = y0 + set->barRect.h - 1;
	}
	break;
	}

	canvas->box(x0, y0, x1, y1, (set->barColor & 0xffffff00) | alpha);
	canvas->rectangle(0, 0, this->width(), this->height(), (set->lineColor & 0xffffff00) | alpha);

	return ret;
}
