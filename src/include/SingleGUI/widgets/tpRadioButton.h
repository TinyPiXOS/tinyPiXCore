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
#ifndef __TP_VRADIO_BUTTON_H
#define __TP_VRADIO_BUTTON_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpRadioButtonData);

class tpColors;
class tpFont;
class tpString;
/// @brief 单选按钮
class tpRadioButton : public tpChildWidget
{
public:
	tpRadioButton(tpChildWidget *parent = nullptr);
	tpRadioButton(const tpString &text, tpChildWidget *parent = nullptr);

	virtual ~tpRadioButton();

public:
	virtual void setAutoFit(bool enable = false); // according to font width and height to render, when be set, align will be invalidate
public:
	virtual void setSpace(uint32_t space = 1);

public:
	virtual void setBoxColor(uint32_t color);
	virtual void setBoxColor(tpColors &color);
	virtual void setCheckColor(uint32_t color);
	virtual void setCheckColor(tpColors &color);

public:
	virtual void setChecked(bool checked);
	virtual bool checked();

public:
	virtual bool addToGroup(tpRadioButton *group);
	virtual bool delFromGroup(tpRadioButton *group);

public:
	virtual ItpRadioButtonData *radioSet();

public:
	virtual void setRect(tpRect &rect);
	virtual void setRect(tpRect *rect);
	virtual void setRect(ItpRect &rect);
	virtual void setRect(ItpRect *rect);
	virtual void setRect(int32_t x, int32_t y, uint32_t w, uint32_t h);

public:
	virtual void setText(const tpString &text);

public:
	virtual tpFont *font();
public
signals:
	declare_signal(onClicked, bool);

public:
	virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
	virtual bool onPaintEvent(tpObjectPaintEvent *event);

private:
	ItpRadioButtonData *radioButtonSet;
};

#endif
