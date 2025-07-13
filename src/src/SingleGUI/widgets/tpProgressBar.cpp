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
#include "tpProgressBar.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include "tpColors.h"
#include "tpRange.h"
#include "tpFont.h"
#include "tpRect.h"
#include "tpApp.h"

typedef struct
{
	tpRange range;
	tpRange rectRange;
	tpFont *font;
	int32_t bkColor;
	int32_t topLeftColor;
	int32_t bottomRightColor;
} ItpProgressSet;

tpProgressBar::tpProgressBar( tpChildWidget* parent) : tpChildWidget(parent)
{
	ItpProgressSet *set = new ItpProgressSet();

	if (set)
	{
		set->range.setRange(0, 100);
		set->range.setPercent(0);
		set->rectRange.setRange(0, 100);
		set->rectRange.setPercent(0);
		set->bkColor = tpColors::Green;
		set->topLeftColor = tpColors::Black;
		set->bottomRightColor = tpColors::LightSlateGray;

		set->font = new tpFont();

		if (set->font)
		{
			set->font->setFontForeColor(tpColors::Black);
		}

		this->progressSet = set;
	}
}

tpProgressBar::~tpProgressBar()
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

	if (set)
	{
		if (set->font)
		{
			delete set->font;
		}

		delete set;
	}
}

void tpProgressBar::setRangeFrom(int32_t min, int32_t max)
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

	if (set)
	{
		set->range.setRange(min, max);
	}
}

void tpProgressBar::setPosition(int32_t pos)
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

	if (set)
	{
		set->range.setPosition(pos);
		double percent = set->range.percent();
		set->rectRange.setPercent(percent);
		this->update();
	}
}

void tpProgressBar::setPercent(double percent)
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

	if (set)
	{
		set->range.setPercent(percent);
		double percent = set->range.percent();
		set->rectRange.setPercent(percent);
		this->update();
	}
}

int32_t tpProgressBar::position()
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;
	int32_t pos = 0;

	if (set)
	{
		pos = set->range.position();
	}

	return pos;
}

double tpProgressBar::percent()
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;
	double percent = 0;

	if (set)
	{
		percent = set->range.percent();
	}

	return percent;
}

tpFont *tpProgressBar::font()
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;
	tpFont *font = nullptr;

	if (set)
	{
		font = set->font;
	}

	return font;
}

void tpProgressBar::setProgressBackColor(uint32_t bkColor)
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

	if (set)
	{
		set->bkColor = bkColor;
	}
}

void tpProgressBar::setProgressBackColor(tpColors &color)
{
	this->setProgressBackColor(color.rgba());
}

void tpProgressBar::setTopLeftLineColor(uint32_t color)
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

	if (set)
	{
		set->topLeftColor = color;
	}
}

void tpProgressBar::setTopLeftLineColor(tpColors &color)
{
	this->setTopLeftLineColor(color.rgba());
}

void tpProgressBar::setRightBottomLineColor(uint32_t color)
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

	if (set)
	{
		set->bottomRightColor = color;
	}
}

void tpProgressBar::setRightBottomLineColor(tpColors &color)
{
	this->setRightBottomLineColor(color.rgba());
}

void tpProgressBar::setRect(tpRect &rect)
{
	this->setRect(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

void tpProgressBar::setRect(tpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->X0(), rect->Y0(), rect->width(), rect->height());
	}
}

void tpProgressBar::setRect(ItpRect &rect)
{
	this->setRect(rect.x, rect.y, rect.w, rect.h);
}

void tpProgressBar::setRect(ItpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->x, rect->y, rect->w, rect->h);
	}
}

void tpProgressBar::setRect(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

	if (set)
	{
		double percent = set->rectRange.percent();
		set->rectRange.setRange(0, w - 4);
		set->rectRange.setPercent(percent);
		tpChildWidget::setRect(x, y, w, h);

		int32_t nWH = TP_MIN(w, h);
		// int32_t fontSize = (int32_t)(nWH * tpApp::Inst()->appConfigSet()->ratio);
		int32_t fontSize = 15;
		
		fontSize = TP_MAX(1, fontSize);
		set->font->setFontSize(fontSize);
	}
}

bool tpProgressBar::onPaintEvent(tpObjectPaintEvent *event)
{
	ItpProgressSet *set = (ItpProgressSet *)this->progressSet;
	bool ret = false;

	if (set)
	{
		ret = tpChildWidget::onPaintEvent(event);

		if (ret)
		{
			ItpRect rect = event->rect();
			tpCanvas *canvas = event->canvas();

			uint8_t alpha1 = mapAlpha((uint8_t)(set->topLeftColor & 0x000000ff), this->alpha());
			uint8_t alpha2 = mapAlpha((uint8_t)(set->bottomRightColor & 0x000000ff), this->alpha());
			uint8_t alpha3 = mapAlpha((uint8_t)(set->bkColor & 0x000000ff), this->alpha());

			int32_t x0 = 0, y0 = 0, x1 = rect.w - 1, y1 = rect.h - 1;

			canvas->line(x0, y0, x0, y1, (set->topLeftColor & 0xffffff00) | alpha1);
			canvas->line(x0, y0, x1, y0, (set->topLeftColor & 0xffffff00) | alpha1);
			canvas->line(x0, y1, x1, y1, (set->bottomRightColor & 0xffffff00) | alpha2);
			canvas->line(x1, y0, x1, y1, (set->bottomRightColor & 0xffffff00) | alpha2);

			int32_t position = set->rectRange.position();

			if (position)
			{
				canvas->box(x0 + 1, y0 + 1, x0 + position + 2, y1 - 1, (set->bkColor & 0xffffff00) | alpha3);
			}

			int32_t percent = (int32_t)(set->rectRange.percent() * 100);
			tpString perString = tpString::number(percent);
			perString += "%";
			set->font->setText(perString.c_str());
			ItpSize size = set->font->pixelSize();

			int32_t cx = (rect.w - size.w) / 2, cy = (rect.h - size.h) / 2;
			canvas->renderText(*set->font, cx, cy);
		}
	}

	return ret;
}
