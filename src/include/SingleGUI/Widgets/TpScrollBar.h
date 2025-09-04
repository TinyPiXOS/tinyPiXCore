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
#ifndef __TP_VSCROLL_BAR_H
#define __TP_VSCROLL_BAR_H

#include "TpScroll.h"

TP_DEF_VOID_TYPE_VAR(IPitpScrollBarSet);

class TpScrollBar : public TpChildWidget
{
public:
	TpScrollBar(TpChildWidget *parent, bool horizontal = TpScroll::TP_SCROLL_HORIZONTAL);
	virtual ~TpScrollBar();

public:
	virtual void setRect(const TpRect &rect);
	virtual void setRect(int32_t x, int32_t y, uint32_t w, uint32_t h);

public:
	virtual void setLinePerPage(int32_t lines = 20); // show lines per page
	virtual int pages();							 // devide pages
	virtual int pageIndex();						 // current page

public:
	virtual void setMaxRange(int32_t max = 100);
	virtual void zoomRange(int32_t delta = 0);

public:
	virtual void setPosition(int32_t position);
	virtual void setPercent(double percent);

private:
	virtual void pageScroll(int32_t type);
	virtual void mapSignal(TpScroll *scroll, int32_t position, int32_t page, int32_t pageIndex, double percent);

public:
	virtual int position();
	virtual double percent();

public:
	virtual void setNormalBarColor(uint32_t color = TpColors::LightGray);
	virtual void setNormalBarColor(TpColors &color);

	virtual void setHoverBarColor(uint32_t color = TpColors::Grey);
	virtual void setHoverBarColor(TpColors &color);

	virtual void setDownBarColor(uint32_t color = TpColors::DarkGoldenrod);
	virtual void setDownBarColor(TpColors &color);

public:
	virtual void setLineColor(uint32_t color = TpColors::White);
	virtual void setLineColor(TpColors &color);

public
signals:
	declare_signal(onScroll, TpScroll *, int32_t, int32_t, int32_t, double);

private:
	IPitpScrollBarSet *scrollBarSet;
};

#endif
