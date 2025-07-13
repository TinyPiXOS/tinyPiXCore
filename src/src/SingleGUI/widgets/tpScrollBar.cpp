/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
#include "tpScrollBar.h"
#include "tpButton.h"
#include "tpRect.h"
#include "tpCanvas.h"
#include "tpColors.h"
#include "tpEvent.h"
#include "tpFont.h"
#include "tpLabel.h"

#define LT_BUTTON_INDEX 0
#define RB_BUTTON_INDEX 1
#define SC_BUTTON_NUMBERS 2

#define LT_INCREASE_BAR 0
#define RB_DECREASE_BAR 1

class tpRectPanel : public tpLabel
{
public:
	tpRectPanel(tpChildWidget *parent, int32_t type) : tpLabel(parent)
	{
		this->down = false;
		this->hoverBarColor = tpColors::Grey;
		this->downBarColor = tpColors::LightGray;
		this->normalBarColor = tpColors::Grey;
		this->setBackGroundColor(this->normalBarColor);
		this->setEnabledBorderColor(false);
		this->type = type;
	}

	virtual void setNormalBarColor(int32_t color = tpColors::LightGray)
	{
		this->normalBarColor = color;
	}

	virtual void setHoverBarColor(int32_t color = tpColors::Grey)
	{
		this->hoverBarColor = color;
	}

	virtual void setDownBarColor(int32_t color = tpColors::DarkGoldenrod)
	{
		this->downBarColor = color;
	}

	virtual bool onMousePressEvent(tpMouseEvent *event) override
	{
		this->down = true;
		this->setBackGroundColor(this->downBarColor);
		onClicked.emit(this->type);

		this->update();
		return true;
	}

	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override
	{
		this->down = false;
		this->setBackGroundColor(this->hoverBarColor);

		this->update();
		return true;
	}

	virtual bool onMouseMoveEvent(tpMouseEvent *event)
	{
		if (event->state() &&
			this->down)
		{
			this->setBackGroundColor(this->downBarColor);
		}
		else
		{
			this->setBackGroundColor(this->normalBarColor);
		}

		this->update();
		return true;
	}

	virtual bool onLeaveEvent(tpObjectLeaveEvent *event)
	{
		if (event->leave() == false)
		{
			if (this->down == false)
			{
				this->setBackGroundColor(this->normalBarColor);
			}
		}

		this->update();
		return true;
	}

public:
	declare_signal(onClicked, int32_t);

private:
	int32_t hoverBarColor;
	int32_t downBarColor;
	int32_t normalBarColor;

private:
	bool down;
	int32_t type;
};

typedef struct
{
	tpScroll *scroll;
	tpRectPanel *rectButton[SC_BUTTON_NUMBERS];
} ItpScrollBarSet;

tpScrollBar::tpScrollBar(tpChildWidget *parent, bool horizontal) : tpChildWidget(parent)
{
	ItpScrollBarSet *set = new ItpScrollBarSet();

	if (set)
	{
		set->scroll = new tpScroll(this, horizontal);
		set->rectButton[0] = new tpRectPanel(this, LT_INCREASE_BAR);
		set->rectButton[1] = new tpRectPanel(this, RB_DECREASE_BAR);

		set->scroll->setVisible(true);
		set->rectButton[0]->setVisible(true);
		set->rectButton[1]->setVisible(true);

		switch (horizontal)
		{
		case false:
		{
			set->rectButton[0]->setText("◀");
			set->rectButton[1]->setText("▶");
		}
		break;
		case true:
		{
			set->rectButton[0]->setText("▲");
			set->rectButton[1]->setText("▼");
		}
		break;
		}

		set->rectButton[0]->font()->setFontSize(8);
		set->rectButton[1]->font()->setFontSize(8);

		connect(set->rectButton[0], SIGNALS(tpRectPanel, onClicked, int32_t), this, SLOTS(tpScrollBar, pageScroll, int32_t));
		connect(set->rectButton[1], SIGNALS(tpRectPanel, onClicked, int32_t), this, SLOTS(tpScrollBar, pageScroll, int32_t));
		connect(set->scroll, SIGNALS(tpScroll, onScroll, tpScroll *, int32_t, int32_t, int32_t, double), this, SLOTS(tpScrollBar, mapSignal, tpScroll *, int32_t, int32_t, int32_t, double));

		this->setEnabledBorderColor(false);
		this->scrollBarSet = set;
	}
}

tpScrollBar::~tpScrollBar()
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		int32_t i;
		for (i = 0; i < SC_BUTTON_NUMBERS; i++)
		{
			if (set->rectButton[i])
			{
				delete set->rectButton[i];
			}
		}

		if (set->scroll)
		{
			delete set->scroll;
		}

		delete set;
	}
}

void tpScrollBar::setRect(tpRect &rect)
{
	this->setRect(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

void tpScrollBar::setRect(tpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->X0(), rect->Y0(), rect->width(), rect->height());
	}
}

void tpScrollBar::setRect(ItpRect &rect)
{
	this->setRect(rect.x, rect.y, rect.w, rect.h);
}

void tpScrollBar::setRect(ItpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->x, rect->y, rect->w, rect->h);
	}
}

void tpScrollBar::setRect(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		int32_t deltaW = TP_MIN(w + 2, h + 2);
		bool horizontal = set->scroll->direction();

		switch (horizontal)
		{
		case false:
		{
			set->rectButton[0]->setRect(0, 0, deltaW, deltaW);
			set->scroll->setRect(deltaW, 0, w - 2 * deltaW, h);
			set->rectButton[1]->setRect(w - deltaW, 0, deltaW, deltaW);
		}
		break;
		case true:
		{
			set->rectButton[0]->setRect(0, 0, deltaW, deltaW);
			set->scroll->setRect(0, deltaW, w, h - 2 * deltaW);
			set->rectButton[1]->setRect(0, h - deltaW, deltaW, deltaW);
		}
		break;
		}

		int32_t fontSize = 2 * deltaW / 5;

		if (fontSize <= 0)
		{
			fontSize = 1;
		}

		set->rectButton[0]->font()->setFontSize(fontSize);
		set->rectButton[1]->font()->setFontSize(fontSize);

		tpChildWidget::setRect(x, y, w, h);
	}
}

void tpScrollBar::setLinePerPage(int32_t lines)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		set->scroll->setLinePerPage(lines);
	}
}

int32_t tpScrollBar::pages()
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;
	int32_t pages = 0;

	if (set)
	{
		pages = set->scroll->pages();
	}

	return pages;
}

int32_t tpScrollBar::pageIndex()
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;
	int32_t pagesIndex = 0;

	if (set)
	{
		pagesIndex = set->scroll->pageIndex();
	}

	return pagesIndex;
}

void tpScrollBar::setMaxRange(int32_t max)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		set->scroll->setMaxRange(max);
	}
}

void tpScrollBar::zoomRange(int32_t delta)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		set->scroll->zoomRange(delta);
	}
}

void tpScrollBar::pageScroll(int32_t type)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		int32_t lines = set->scroll->linesPerPage();
		int32_t step = TP_MAX(lines / 10, 1);
		int32_t position = set->scroll->position();

		switch (type)
		{
		case 0:
		{
			position -= step;
			set->scroll->setPosition(position);
		}
		break;
		case 1:
		{
			position += step;
			set->scroll->setPosition(position);
		}
		break;
		}

		this->update();
	}
}

void tpScrollBar::mapSignal(tpScroll *scroll, int32_t position, int32_t page, int32_t pageIndex, double percent)
{
	this->onScroll.emit(scroll, position, page, pageIndex, percent);
}

void tpScrollBar::setPosition(int32_t position)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		set->scroll->setPosition(position);
	}
}

void tpScrollBar::setPercent(double percent)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		set->scroll->setPercent(percent);
	}
}

int32_t tpScrollBar::position()
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;
	int32_t position = 0;

	if (set)
	{
		position = set->scroll->position();
	}

	return position;
}

double tpScrollBar::percent()
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;
	double percent = 0;

	if (set)
	{
		percent = set->scroll->percent();
	}

	return percent;
}

void tpScrollBar::setNormalBarColor(uint32_t color)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		set->scroll->setNormalBarColor(color);
	}
}

void tpScrollBar::setNormalBarColor(tpColors &color)
{
	this->setNormalBarColor(color.rgba());
}

void tpScrollBar::setHoverBarColor(uint32_t color)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		set->scroll->setHoverBarColor(color);
	}
}

void tpScrollBar::setHoverBarColor(tpColors &color)
{
	this->setHoverBarColor(color.rgba());
}

void tpScrollBar::setDownBarColor(uint32_t color)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		set->scroll->setDownBarColor(color);
	}
}

void tpScrollBar::setDownBarColor(tpColors &color)
{
	this->setDownBarColor(color.rgba());
}

void tpScrollBar::setLineColor(uint32_t color)
{
	ItpScrollBarSet *set = (ItpScrollBarSet *)this->scrollBarSet;

	if (set)
	{
		set->scroll->setLineColor(color);
	}
}

void tpScrollBar::setLineColor(tpColors &color)
{
	this->setLineColor(color.rgba());
}
