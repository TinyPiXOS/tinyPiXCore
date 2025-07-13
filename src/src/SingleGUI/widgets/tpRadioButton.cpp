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
#include "tpRadioButton.h"
#include "tpEvent.h"
#include "tpColors.h"
#include "tpCanvas.h"
#include "tpRect.h"
#include "tpFont.h"
#include "tpString.h"
#include <cstring>
#include <list>

struct tpRadioButtonData
{
	bool checked;
	bool enableFit;
	bool mouseActive;

	int32_t space;
	tpFont *font;
	int32_t boxColor;
	int32_t checkColor;
	tpRadioButton *group;
	std::list<tpRadioButton *> radioList;

	tpRadioButtonData()
	{
	}
};

tpRadioButton::tpRadioButton(tpChildWidget *parent) : tpChildWidget(parent)
{
	tpRadioButtonData *set = new tpRadioButtonData();

	if (set)
	{
		set->checked = false;
		set->enableFit = false;
		set->mouseActive = false;
		set->space = 1;
		set->boxColor = tpColors::Black;
		set->checkColor = tpColors::Black;
		set->font = new tpFont();
		set->group = nullptr;

		this->setEnableBackGroundImage(false);
		this->setEnableBackGroundColor(false);
		this->radioButtonSet = set;

		setVisible(true);
	}
}

tpRadioButton::tpRadioButton(const tpString &text, tpChildWidget *parent)
	: tpChildWidget(parent)
{
	tpRadioButtonData *set = new tpRadioButtonData();

	if (set)
	{
		set->checked = false;
		set->enableFit = false;
		set->mouseActive = false;
		set->space = 1;
		set->boxColor = tpColors::Black;
		set->checkColor = tpColors::Black;
		set->font = new tpFont();
		set->group = nullptr;

		this->setEnableBackGroundImage(false);
		this->setEnableBackGroundColor(false);
		this->radioButtonSet = set;
	}

	setText(text);
	setVisible(true);
}

tpRadioButton::~tpRadioButton()
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;

	if (set)
	{
		if (set->font)
		{
			delete set->font;
		}

		if (set->group)
		{
			set->group->delFromGroup(this);
		}
		else
		{
			std::list<tpRadioButton *>::iterator iter = set->radioList.begin();

			for (; iter != set->radioList.end(); iter++)
			{
				(*iter)->delFromGroup(this);
			}

			set->radioList.clear();
		}

		delete set;
	}
}

void tpRadioButton::setAutoFit(bool enable)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;

	if (set)
	{
		set->enableFit = enable;
		if (enable)
		{
			ItpSize size = set->font->pixelSize();
			this->setRect(this->rect().x, this->rect().y, size.w + size.h / 2 + set->space, size.h);
		}
	}
}

void tpRadioButton::setSpace(uint32_t space)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;

	if (set)
	{
		set->space = space;
	}
}

void tpRadioButton::setBoxColor(uint32_t color)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;

	if (set)
	{
		set->boxColor = color;
	}
}

void tpRadioButton::setBoxColor(tpColors &color)
{
	this->setBoxColor(color.rgba());
}

void tpRadioButton::setCheckColor(uint32_t color)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;

	if (set)
	{
		set->checkColor = color;
	}
}

void tpRadioButton::setCheckColor(tpColors &color)
{
	this->setCheckColor(color.rgba());
}

static inline void broadCastChecked(tpRadioButtonData *set, tpRadioButton *self, bool checked)
{
	std::list<tpRadioButton *>::iterator iter = set->radioList.begin();

	for (; iter != set->radioList.end(); iter++)
	{
		if (*iter != self)
		{
			(*iter)->setChecked(checked);
		}
	}
}

void tpRadioButton::setChecked(bool checked)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;

	if (set)
	{
		if (set->checked != checked)
		{
			set->checked = checked;
			onClicked.emit(set->checked);

			if (set->checked &&
				set->mouseActive)
			{
				if (set->group)
				{
					set->group->setChecked(false);
					tpRadioButtonData *group_set = (tpRadioButtonData *)set->group->radioSet();
					broadCastChecked(group_set, this, false);
				}
				else
				{
					broadCastChecked(set, this, false);
				}
			}

			this->update();
		}
	}
}

bool tpRadioButton::checked()
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;
	bool ret = false;

	if (set)
	{
		ret = set->checked;
	}

	return ret;
}

bool tpRadioButton::addToGroup(tpRadioButton *group)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;
	bool ret = false;

	if (set)
	{
		if (group == this ||
			set->radioList.size() > 0)
		{
			return false;
		}

		if (group)
		{
			if (set->group == group)
			{
				return false;
			}
			else
			{
				if (set->group)
				{
					set->group->delFromGroup(group);
				}
			}
		}
		else
		{
			if (set->group)
			{
				this->setChecked(false);
				set->group->delFromGroup(group);
				set->group = nullptr;
				return false;
			}
		}

		tpRadioButtonData *group_set = (tpRadioButtonData *)group->radioSet();

		if (group_set)
		{
			auto iter = std::find_if(group_set->radioList.begin(), group_set->radioList.end(), [this](tpRadioButton *value)
									 { return (this == value); });

			if (iter == group_set->radioList.end())
			{
				this->setChecked(false);
				group_set->radioList.push_back(this);
				set->group = group;
				ret = true;
			}
		}
	}

	return ret;
}

bool tpRadioButton::delFromGroup(tpRadioButton *group)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;
	bool ret = false;

	if (set)
	{
		if (group == nullptr ||
			set->group == nullptr)
		{
			return false;
		}

		if (set->group != group)
		{
			return false;
		}

		tpRadioButtonData *group_set = (tpRadioButtonData *)group->radioSet();

		if (group_set)
		{
			auto iter = std::find_if(group_set->radioList.begin(), group_set->radioList.end(), [this](tpRadioButton *value)
									 { return (this == value); });

			if (iter != group_set->radioList.end())
			{
				this->setChecked(false);
				group_set->radioList.remove(this);
				set->group = nullptr;
				ret = true;
			}
		}
	}

	return ret;
}

ItpRadioButtonData *tpRadioButton::radioSet()
{
	return this->radioButtonSet;
}

void tpRadioButton::setRect(tpRect &rect)
{
	this->setRect(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

void tpRadioButton::setRect(tpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->X0(), rect->Y0(), rect->width(), rect->height());
	}
}

void tpRadioButton::setRect(ItpRect &rect)
{
	this->setRect(rect.x, rect.y, rect.w, rect.h);
}

void tpRadioButton::setRect(ItpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->x, rect->y, rect->w, rect->h);
	}
}

void tpRadioButton::setRect(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;

	if (set)
	{
		if (set->enableFit)
		{
			ItpSize size = set->font->pixelSize();
			tpChildWidget::setRect(x, y, size.w + size.h / 2 + set->space, size.h);
			return;
		}

		tpChildWidget::setRect(x, y, w, h);
	}
}

void tpRadioButton::setText(const tpString &text)
{
	if (text.empty())
		return;

	tpChildWidget::setText(text);
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;

	if (!set)
		return;

	set->font->setText(text);
	if (set->enableFit)
	{
		ItpSize size = set->font->pixelSize();
		this->setRect(this->rect().x, this->rect().y, size.w + size.h / 4 + set->space, size.h);
	}
}

tpFont *tpRadioButton::font()
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;
	tpFont *font = nullptr;

	if (set)
	{
		font = set->font;
	}

	return font;
}

typedef struct
{
	int32_t bcolor;
	int32_t ccolor;

	double rad;

	tpObjectPaintEvent *event;
	tpRadioButtonData *set;
} drawArgs;

static inline void draw(tpCanvas *canvas, cairo_t *cr, cairo_surface_t *cairo_surface, int32_t offsetX, int32_t offsetY, void *args)
{
	drawArgs *arg = (drawArgs *)args;
	ItpSize size = arg->set->font->pixelSize();
	int32_t lineWidth = TP_MAX(1, arg->rad / 8);
	double cx = (arg->event->rect().w - size.w) / 2.0, cy = (arg->event->rect().h - size.h) / 2.0;
	cx = TP_MAX(cx, arg->rad);

	cairo_set_line_width(cr, lineWidth);
	double r = _R(arg->bcolor) / 255.0, g = _G(arg->bcolor) / 255.0, b = _B(arg->bcolor) / 255.0, a = _A(arg->bcolor) / 255.0;
	cairo_set_source_rgba(cr, r, g, b, a);

	cairo_arc(cr, cx + offsetX, cy + offsetY + 9 * size.h / 16.0, arg->rad, 0, 2 * M_PI);
	cairo_stroke(cr);

	if (arg->set->checked)
	{
		r = _R(arg->ccolor) / 255.0;
		g = _G(arg->ccolor) / 255.0;
		b = _B(arg->ccolor) / 255.0;
		a = _A(arg->ccolor) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_arc(cr, cx + offsetX, cy + offsetY + 9 * size.h / 16.0, arg->rad / 2.0, 0, 2 * M_PI);
		cairo_stroke_preserve(cr);
		cairo_fill(cr);
	}

	canvas->renderText(*arg->set->font, cx + arg->rad + arg->set->space, cy);
}

bool tpRadioButton::onMousePressEvent(tpMouseEvent *event)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;

	int32_t x = event->pos().x;
	int32_t y = event->pos().y;

	ItpSize size = set->font->pixelSize();
	int32_t cx = 0, cy = (int32_t)(this->height() - size.h) / 2;
	tpRect actRect(cx, cy, 3 * size.h / 4, 3 * size.h / 4);
	bool ret = actRect.in(x, y);

	if (!ret)
		return true;

	set->checked = !set->checked;
	set->mouseActive = true;

	if (set->group)
	{
		set->group->setChecked(false);
		tpRadioButtonData *group_set = (tpRadioButtonData *)set->group->radioSet();
		broadCastChecked(group_set, this, false);
	}
	else
	{
		broadCastChecked(set, this, false);
	}

	this->update();

	return false;
}

bool tpRadioButton::onMouseRleaseEvent(tpMouseEvent *event)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;
	int32_t x = event->pos().x;
	int32_t y = event->pos().y;

	ItpSize size = set->font->pixelSize();
	int32_t cx = 0, cy = (int32_t)(this->height() - size.h) / 2;
	tpRect actRect(cx, cy, 3 * size.h / 4, 3 * size.h / 4);
	bool ret = actRect.in(x, y);

	if (!ret)
		return true;

	set->mouseActive = false;

	return false;
}

bool tpRadioButton::onPaintEvent(tpObjectPaintEvent *event)
{
	tpRadioButtonData *set = (tpRadioButtonData *)this->radioButtonSet;
	bool ret = false;

	if (set)
	{
		ret = tpChildWidget::onPaintEvent(event);

		if (ret)
		{
			tpCanvas *canvas = event->canvas();
			ItpSize size = set->font->pixelSize();
			uint8_t alpha1 = mapAlpha((uint8_t)(set->boxColor & 0x000000ff), this->alpha());
			uint8_t alpha2 = mapAlpha((uint8_t)(set->checkColor & 0x000000ff), this->alpha());
			int32_t cx = (int32_t)(event->rect().w - size.w) / 2, cy = (int32_t)(event->rect().h - size.h) / 2 + size.h / 8;

			drawArgs args;

			args.bcolor = set->boxColor & 0xffffff00 | alpha1;
			args.ccolor = set->checkColor & 0xffffff00 | alpha2;

			args.event = event;
			args.set = set;

			args.rad = size.h / 4.0;

			event->canvas()->customizedCarioMethod(draw, &args);
#if 0
			//draw checked box
			int32_t radius = size.h/4;
			cx = TP_MAX(cx, radius);
			canvas->antiAliasCircle(cx, cy + size.h/2, radius, set->boxColor & 0xffffff00 | alpha1);
			
			if(set->checked){
				canvas->filledCircle(cx, cy + size.h/2, radius/2, set->checkColor & 0xffffff00 | alpha2);
			}
			//render text
			canvas->renderText(*set->font, cx + radius + set->space, cy);
#endif
		}
	}

	return ret;
}
