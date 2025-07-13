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
#ifndef __TP_RECT_H
#define __TP_RECT_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(IPitpRect);

class tpPoint;
class tpRect
{
public:
	tpRect();
	tpRect(const tpRect &);
	tpRect(tpRect *);
	tpRect(const ItpRect &);
	tpRect(ItpRect *);
	tpRect(const int32_t& x, const int32_t& y, const int32_t& w, const uint32_t h);

public:
	virtual ~tpRect();

public:
	virtual void set(int32_t x, int32_t y, int32_t w, unsigned h);
	virtual void set(tpRect &);
	virtual void set(tpRect *);
	virtual void set(ItpRect &);
	virtual void set(ItpRect *);

public:
	virtual void setX0(int32_t x);
	virtual void setY0(int32_t y);
	virtual void setX1(int32_t x);
	virtual void setY1(int32_t y);

	virtual void setWidth(int32_t width);
	virtual void setHeight(int32_t height);

public:
	virtual void creaseXY(int32_t dx, int32_t dy);
	virtual void creaseWH(int32_t dw, int32_t dh);

public:
	virtual ItpRect get() const;
	virtual void get(int32_t *x, int32_t *y, uint32_t *w, uint32_t *h) const;
	virtual void get(tpRect &);
	virtual void get(tpRect *);
	virtual void get(ItpRect *) const;

public:
	virtual int32_t X0() const;
	virtual int32_t Y0() const;
	virtual int32_t X1() const;
	virtual int32_t Y1() const;

	virtual int32_t width() const;
	virtual int32_t height() const;

public:
	virtual bool in(int32_t x, int32_t y);
	virtual bool in(tpPoint &);
	virtual bool in(tpPoint *);
	virtual bool in(ItpPoint &);
	virtual bool in(ItpPoint *);

public:
	virtual bool available();
	virtual bool available(ItpRect &);
	virtual bool available(ItpRect *);

public:
	/// @brief 判断两个区域是否相交
	/// @param  矩形参数类
	/// @return 返回true相交，false不相交
	virtual bool intersect(tpRect &);
	virtual bool intersect(tpRect *);
	virtual bool intersect(ItpRect &);
	virtual bool intersect(ItpRect *);
	virtual bool intersect(int32_t x, int32_t y, uint32_t w, uint32_t h);

public:
	virtual bool unions(tpRect &);
	virtual bool unions(tpRect *);
	virtual bool unions(ItpRect &);
	virtual bool unions(ItpRect *);
	virtual bool unions(int32_t x, int32_t y, uint32_t w, uint32_t h);

public:
	tpRect operator=(const tpRect &self);
	tpRect operator=(const ItpRect &self);

	bool operator==(const tpRect &self);
	bool operator==(const ItpRect &self);

	bool operator!=(const tpRect &self);
	bool operator!=(const ItpRect &self);

private:
	IPitpRect *rectSet;
};

#endif
