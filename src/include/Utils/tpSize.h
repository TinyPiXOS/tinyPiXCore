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
#ifndef __TP_SIZE_H
#define __TP_SIZE_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(IPitpSize);

class tpSize
{
public:
	tpSize();
	tpSize(tpSize &);
	tpSize(tpSize *);
	tpSize(int32_t width, int32_t height);
public:
	virtual ~tpSize();
public:
	virtual void set(int32_t width, int32_t height);
	virtual void set(tpSize &);
	virtual void set(tpSize *);
public:
	virtual ItpSize get();
public:
	virtual int32_t width();
	virtual int32_t height();
public:
	virtual void crease(int32_t dx = 1, int32_t dy = 1);
public:
	tpSize operator = (tpSize &);
	tpSize operator = (ItpSize &);
	
	bool operator == (tpSize &);
	bool operator == (ItpSize &);
	
	bool operator != (tpSize &);
	bool operator != (ItpSize &);
private:
	IPitpSize *sizeSet;
};

#endif
