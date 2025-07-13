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
#include "tpLink.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include "tpFont.h"
#include "tpRect.h"
#include <cstring>

typedef struct
{
	struct
	{
		int32_t normalColor;
		int32_t hoverColor;
		int32_t downColor;
	};
	bool enableFit;
	tpFont *font;
} ItpLinkSet;

tpLink::tpLink(tpChildWidget *parent) : tpChildWidget(parent)
{
	ItpLinkSet *set = new ItpLinkSet();

	if (set)
	{
		set->normalColor = tpColors::Blue;
		set->hoverColor = tpColors::Red;
		set->downColor = tpColors::Black;

		set->font = new tpFont();

		if (set->font)
		{
			set->font->setFontStyle(TINY_FONT_UNDERLINE);
			set->font->setFontForeColor(set->normalColor);
		}

		this->setEnableBackGroundImage(false);
		this->setEnableBackGroundColor(false);
		this->linkSet = set;
	}
}

tpLink::~tpLink()
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		if (set->font)
		{
			delete set->font;
		}

		delete set;
	}
}

void tpLink::setAutoFit(bool enable)
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		set->enableFit = enable;
		if (enable)
		{
			ItpSize size = set->font->pixelSize();
			this->setRect(this->rect().x, this->rect().y, size.w, size.h);
		}
	}
}

void tpLink::setNormalColor(int32_t color)
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		set->normalColor = color;
	}
}

void tpLink::setNormalColor(tpColors &color)
{
	this->setNormalColor(color.rgba());
}

void tpLink::setHoverColor(int32_t color)
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		set->hoverColor = color;
	}
}

void tpLink::setHoverColor(tpColors &color)
{
	this->setHoverColor(color.rgba());
}

void tpLink::setDownColor(int32_t color)
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		set->downColor = color;
	}
}

void tpLink::setDownColor(tpColors &color)
{
	this->setDownColor(color.rgba());
}

void tpLink::setText(const char *text)
{
	if (text == nullptr ||
		strlen(text) == 0)
	{
		return;
	}

	tpChildWidget::setText(text);
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		set->font->setText(text);
		if (set->enableFit)
		{
			ItpSize size = set->font->pixelSize();
			this->setRect(this->rect().x, this->rect().y, size.w, size.h);
		}
	}
}

void tpLink::setText(std::string &text)
{
	this->setText(text.c_str());
}

tpFont *tpLink::font()
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;
	tpFont *font = nullptr;

	if (set)
	{
		font = set->font;
	}

	return font;
}

void tpLink::setRect(tpRect &rect)
{
	this->setRect(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

void tpLink::setRect(tpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->X0(), rect->Y0(), rect->width(), rect->height());
	}
}

void tpLink::setRect(ItpRect &rect)
{
	this->setRect(rect.x, rect.y, rect.w, rect.h);
}

void tpLink::setRect(ItpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->x, rect->y, rect->w, rect->h);
	}
}

void tpLink::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		if (set->enableFit)
		{
			ItpSize size = set->font->pixelSize();
			tpChildWidget::setRect(x, y, size.w, size.h);
			return;
		}

		tpChildWidget::setRect(x, y, w, h);
	}
}

bool tpLink::onMousePressEvent(tpMouseEvent *event)
{
	tpChildWidget::onMousePressEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	set->font->setFontForeColor(set->downColor);
	this->update();
	onClicked.emit(true);

	return true;
}

bool tpLink::onMouseRleaseEvent(tpMouseEvent *event)
{
	tpChildWidget::onMouseRleaseEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	set->font->setFontForeColor(set->normalColor);
	this->update();
	onClicked.emit(false);

	return true;
}

bool tpLink::onMouseMoveEvent(tpMouseEvent *event)
{
	bool ret = tpChildWidget::onMouseMoveEvent(event);

	if (ret)
	{
		if (event->state() == false)
		{
			ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

			if (set)
			{
				set->font->setFontForeColor(set->hoverColor);
				this->update();
			}
		}
	}

	return ret;
}

bool tpLink::onLeaveEvent(tpObjectLeaveEvent *event)
{
	bool ret = tpChildWidget::onLeaveEvent(event);

	if (ret)
	{
		if (event->leave() == false)
		{
			ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

			if (set)
			{
				set->font->setFontForeColor(set->normalColor);
				this->update();
			}
		}
	}

	return ret;
}

bool tpLink::onPaintEvent(tpObjectPaintEvent *event)
{
	bool ret = tpChildWidget::onPaintEvent(event);

	if (ret)
	{
		ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

		if (set)
		{
			tpCanvas *canvas = event->canvas();
			ItpSize size = set->font->pixelSize();
			int32_t cx = 0, cy = (int32_t)(event->rect().h - size.h) / 2;
			canvas->renderText(*set->font, cx, cy);
		}
	}

	return ret;
}
