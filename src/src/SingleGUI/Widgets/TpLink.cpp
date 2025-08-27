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
#include "TpLink.h"
#include "TpEvent.h"
#include "TpCanvas.h"
#include "TpFont.h"
#include "TpRect.h"
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
	TpFont *font;
} ItpLinkSet;

TpLink::TpLink(TpChildWidget *parent) : TpChildWidget(parent)
{
	ItpLinkSet *set = new ItpLinkSet();

	if (set)
	{
		set->normalColor = TpColors::Blue;
		set->hoverColor = TpColors::Red;
		set->downColor = TpColors::Black;

		set->font = new TpFont();

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

TpLink::~TpLink()
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

void TpLink::setAutoFit(bool enable)
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

void TpLink::setNormalColor(int32_t color)
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		set->normalColor = color;
	}
}

void TpLink::setNormalColor(TpColors &color)
{
	this->setNormalColor(color.rgba());
}

void TpLink::setHoverColor(int32_t color)
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		set->hoverColor = color;
	}
}

void TpLink::setHoverColor(TpColors &color)
{
	this->setHoverColor(color.rgba());
}

void TpLink::setDownColor(int32_t color)
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		set->downColor = color;
	}
}

void TpLink::setDownColor(TpColors &color)
{
	this->setDownColor(color.rgba());
}

void TpLink::setText(const char *text)
{
	if (text == nullptr ||
		strlen(text) == 0)
	{
		return;
	}

	TpChildWidget::setText(text);
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

void TpLink::setText(std::string &text)
{
	this->setText(text.c_str());
}

TpFont *TpLink::font()
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;
	TpFont *font = nullptr;

	if (set)
	{
		font = set->font;
	}

	return font;
}

void TpLink::setRect(TpRect &rect)
{
	this->setRect(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

void TpLink::setRect(TpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->X0(), rect->Y0(), rect->width(), rect->height());
	}
}

void TpLink::setRect(ItpRect &rect)
{
	this->setRect(rect.x, rect.y, rect.w, rect.h);
}

void TpLink::setRect(ItpRect *rect)
{
	if (rect)
	{
		this->setRect(rect->x, rect->y, rect->w, rect->h);
	}
}

void TpLink::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	if (set)
	{
		if (set->enableFit)
		{
			ItpSize size = set->font->pixelSize();
			TpChildWidget::setRect(x, y, size.w, size.h);
			return;
		}

		TpChildWidget::setRect(x, y, w, h);
	}
}

bool TpLink::onMousePressEvent(TpMouseEvent *event)
{
	TpChildWidget::onMousePressEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	set->font->setFontForeColor(set->downColor);
	this->update();
	onClicked.emit(true);

	return true;
}

bool TpLink::onMouseRleaseEvent(TpMouseEvent *event)
{
	TpChildWidget::onMouseRleaseEvent(event);

	if (event->button() != BUTTON_LEFT)
		return true;

	ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

	set->font->setFontForeColor(set->normalColor);
	this->update();
	onClicked.emit(false);

	return true;
}

bool TpLink::onMouseMoveEvent(TpMouseEvent *event)
{
	bool ret = TpChildWidget::onMouseMoveEvent(event);

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

bool TpLink::onLeaveEvent(TpObjectLeaveEvent *event)
{
	bool ret = TpChildWidget::onLeaveEvent(event);

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

bool TpLink::onPaintEvent(TpObjectPaintEvent *event)
{
	bool ret = TpChildWidget::onPaintEvent(event);

	if (ret)
	{
		ItpLinkSet *set = (ItpLinkSet *)this->linkSet;

		if (set)
		{
			TpCanvas *canvas = event->canvas();
			ItpSize size = set->font->pixelSize();
			int32_t cx = 0, cy = (int32_t)(event->rect().h - size.h) / 2;
			canvas->renderText(*set->font, cx, cy);
		}
	}

	return ret;
}
