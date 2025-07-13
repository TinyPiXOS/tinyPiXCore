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
#ifndef __TP_VSCROLL_H
#define __TP_VSCROLL_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpColors.h"

TP_DEF_VOID_TYPE_VAR(IPitpScrollSet);

class tpScroll : public tpChildWidget
{
public:
	enum
	{
		TP_SCROLL_HORIZONTAL,
		TP_SCROLL_VERTICAL,
	};

public:
	tpScroll(tpChildWidget *parent, bool horizontal = TP_SCROLL_HORIZONTAL);
	virtual ~tpScroll();

public:
	virtual bool direction();

public:
	virtual void setLinePerPage(int lines = 20); // show lines per page
	virtual int32_t pages();					 // devide pages
	virtual int32_t pageIndex();				 // current page
	virtual int32_t linesPerPage();
	virtual int32_t min();
	virtual int32_t max();

public:
	virtual void setMaxRange(int max = 100);
	virtual void zoomRange(int delta = 0);

public:
	virtual void setDirection(bool horizontal = TP_SCROLL_HORIZONTAL);
	virtual void setPosition(int position);
	virtual void setPercent(double percent);

public:
	virtual int position();
	virtual double percent();

public:
	virtual void setRect(tpRect &rect);
	virtual void setRect(tpRect *rect);
	virtual void setRect(ItpRect &rect);
	virtual void setRect(ItpRect *rect);
	virtual void setRect(int32_t x, int32_t y, uint32_t w, uint32_t h);

public:
	virtual void setNormalBarColor(uint32_t color = tpColors::LightGray);
	virtual void setNormalBarColor(tpColors &color);

	virtual void setHoverBarColor(uint32_t color = tpColors::Grey);
	virtual void setHoverBarColor(tpColors &color);

	virtual void setDownBarColor(uint32_t color = tpColors::DarkGoldenrod);
	virtual void setDownBarColor(tpColors &color);

public:
	virtual void setLineColor(uint32_t color = tpColors::White);
	virtual void setLineColor(tpColors &color);

public
signals:
	declare_signal(onScroll, tpScroll *, int, int, int, double); // parameters: position, pages, page index, percent & page percent

public:
	virtual bool onMouseMoveEvent(tpMouseEvent *event) override;
	virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
	virtual bool onFocusEvent(tpObjectFocusEvent *event) override;
	virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;
	virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

private:
	IPitpScrollSet *scrollSet;
};

#endif
