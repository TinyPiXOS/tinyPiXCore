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
#include "tpRect.h"
#include "tpPoint.h"
#include <stdio.h>

typedef struct
{
	int32_t x;
	int32_t y;
	int32_t w;
	int32_t h;
} tpRectSet;

static inline bool rect_interact(ItpRect *r1, ItpRect *r2, ItpRect *r)
{
	if (r1 == nullptr ||
		r2 == nullptr ||
		r == nullptr)
		return false;

	int32_t Amin, Amax, Bmin, Bmax;

	Amin = r1->x;
	Amax = Amin + r1->w;
	Bmin = r2->x;
	Bmax = Bmin + r2->w;

	if (Bmin > Amin)
		Amin = Bmin;

	r->x = Amin;

	if (Bmax < Amax)
		Amax = Bmax;

	r->w = Amax - Amin > 0 ? Amax - Amin : 0;
	Amin = r1->y;
	Amax = Amin + r1->h;
	Bmin = r2->y;
	Bmax = Bmin + r2->h;

	if (Bmin > Amin)
		Amin = Bmin;

	r->y = Amin;

	if (Bmax < Amax)
		Amax = Bmax;
	r->h = Amax - Amin > 0 ? Amax - Amin : 0;

	return ((r->w && r->h) ? true : false);
}

static inline bool rect_union(ItpRect *s1, ItpRect *s2, ItpRect *r)
{
	int32_t minx = 0, miny = 0, maxx = 0, maxy = 0;
	int32_t x11 = 0, y11 = 0, x12 = 0, y12 = 0, x21 = 0, y21 = 0, x22 = 0, y22 = 0;

	if (s1 == nullptr ||
		s2 == nullptr ||
		r == nullptr)
	{
		return false;
	}

	x11 = s1->x;
	y11 = s1->y;
	x12 = s1->w + s1->x;
	y12 = s1->h + s1->y;

	x21 = s2->x;
	y21 = s2->y;
	x22 = s2->w + s2->x;
	y22 = s2->h + s2->y;

	minx = TP_MIN(x11, x21);
	miny = TP_MIN(y11, y21);

	maxx = TP_MAX(x12, x22);
	maxy = TP_MAX(y12, y22);

	r->x = minx;
	r->y = miny;
	r->w = maxx - minx;
	r->h = maxy - miny;
	printf("r->w=%d, r->h=%d, ret=%d\n", r->w, r->h, (r->w == 0 || r->h));
	return ((r->w == 0 || r->h) == 0 ? false : true);
}

tpRect::tpRect()
{
	tpRectSet *set = new tpRectSet();

	if (set)
	{
		set->x = 0;
		set->y = 0;
		set->w = 0;
		set->h = 0;

		this->rectSet = set;
	}
}

tpRect::tpRect(const tpRect &r)
{
	tpRectSet *set = new tpRectSet();

	if (set)
	{
		*this = r;
		this->rectSet = set;
	}
}

tpRect::tpRect(tpRect *r)
{
	tpRectSet *set = new tpRectSet();

	if (set)
	{
		set->x = 0;
		set->y = 0;
		set->w = 0;
		set->h = 0;

		this->rectSet = set;

		if (r)
		{
			*this = *r;
		}
	}
}

tpRect::tpRect(const ItpRect &r)
{
	tpRectSet *set = new tpRectSet();

	if (set)
	{
		set->x = r.x;
		set->y = r.y;
		set->w = r.w;
		set->h = r.h;

		this->rectSet = set;
	}
}

tpRect::tpRect(ItpRect *r)
{
	tpRectSet *set = new tpRectSet();

	if (set)
	{
		set->x = (r != nullptr) ? r->x : 0;
		set->y = (r != nullptr) ? r->y : 0;
		set->w = (r != nullptr) ? r->w : 0;
		set->h = (r != nullptr) ? r->h : 0;

		this->rectSet = set;
	}
}

tpRect::tpRect(const int32_t& x, const int32_t& y, const int32_t& w, const uint32_t h)
{
	tpRectSet *set = new tpRectSet();

	if (set)
	{
		set->x = x;
		set->y = y;
		set->w = w;
		set->h = h;

		this->rectSet = set;
	}
}

tpRect::~tpRect()
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		delete set;
	}
}

void tpRect::set(int32_t x, int32_t y, int32_t w, unsigned h)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		set->x = x;
		set->y = y;
		set->w = w;
		set->h = h;
	}
}

void tpRect::set(tpRect &r)
{
	*this = r;
}

void tpRect::set(tpRect *r)
{
	*this = *r;
}

void tpRect::set(ItpRect &r)
{
	*this = r;
}

void tpRect::set(ItpRect *r)
{
	*this = *r;
}

void tpRect::setX0(int32_t x)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		set->x = x;
	}
}

void tpRect::setY0(int32_t y)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		set->y = y;
	}
}

void tpRect::setX1(int32_t x)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		int32_t x0 = set->x, x1 = set->x + x;
		int32_t min = TP_MIN(x0, x1), max = TP_MIN(x0, x1);
		set->x = min;
		set->w = max - min;
	}
}

void tpRect::setY1(int32_t y)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		int32_t y0 = set->y, y1 = set->y + y;
		int32_t min = TP_MIN(y0, y1), max = TP_MIN(y0, y1);
		set->y = min;
		set->h = max - min;
	}
}

void tpRect::setWidth(int32_t width)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		set->w = width;
	}
}

void tpRect::setHeight(int32_t height)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		set->h = height;
	}
}

void tpRect::creaseXY(int32_t dx, int32_t dy)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		set->x += dx;
		set->y += dy;
	}
}

void tpRect::creaseWH(int32_t dw, int32_t dh)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		set->w += dw;
		set->h += dh;

		if ((set->w & 0x80000000) == 0x80000000)
		{
			set->w = 0;
		}

		if ((set->h & 0x80000000) == 0x80000000)
		{
			set->h = 0;
		}
	}
}

ItpRect tpRect::get() const
{
	ItpRect result;
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		result.x = set->x;
		result.y = set->y;
		result.w = set->w;
		result.h = set->h;
	}

	return result;
}

void tpRect::get(int32_t *x, int32_t *y, uint32_t *w, uint32_t *h) const
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		if (x)
		{
			*x = set->x;
		}

		if (y)
		{
			*y = set->y;
		}

		if (w)
		{
			*w = set->w;
		}

		if (h)
		{
			*h = set->h;
		}
	}
}

void tpRect::get(tpRect &r)
{
	r = *this;
}

void tpRect::get(tpRect *r)
{
	*r = *this;
}

void tpRect::get(ItpRect *r) const
{
	if (r)
	{
		this->get(&r->x, &r->y, &r->w, &r->h);
	}
}

int32_t tpRect::X0() const
{
	tpRectSet *set = (tpRectSet *)this->rectSet;
	if (!set)
		return 0;

	return set->x;
}

int32_t tpRect::Y0() const
{
	tpRectSet *set = (tpRectSet *)this->rectSet;
	if (!set)
		return 0;

	return set->y;
}

int32_t tpRect::X1() const
{
	tpRectSet *set = (tpRectSet *)this->rectSet;
	if (!set)
		return 0;

	return (set->x + set->w);
}

int32_t tpRect::Y1() const
{
	tpRectSet *set = (tpRectSet *)this->rectSet;
	if (!set)
		return 0;

	return (set->y + set->h);
}

int32_t tpRect::width() const
{
	tpRectSet *set = (tpRectSet *)this->rectSet;
	if (!set)
		return 0;

	return set->w;
}

int32_t tpRect::height() const
{
	tpRectSet *set = (tpRectSet *)this->rectSet;
	if (!set)
		return 0;

	return set->h;
}

bool tpRect::in(int32_t x, int32_t y)
{
	bool ret = false;
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		ret = ((x >= set->x) && (y >= set->y) && (x <= (set->x + set->w)) && (y <= (set->y + set->h)));
	}

	return ret;
}

bool tpRect::in(tpPoint &p)
{
	int32_t x = p.get().x, y = p.get().y;
	return this->in(x, y);
}

bool tpRect::in(tpPoint *p)
{
	bool ret = false;

	if (p)
	{
		ret = this->in(*p);
	}

	return ret;
}

bool tpRect::in(ItpPoint &p)
{
	return this->in(p.x, p.y);
}

bool tpRect::in(ItpPoint *p)
{
	bool ret = false;

	if (p)
	{
		ret = this->in(p->x, p->y);
	}

	return ret;
}

bool tpRect::available()
{
	bool ret = false;
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		ret = ((set->w != 0) && (set->h != 0));
	}

	return ret;
}

bool tpRect::available(ItpRect &r)
{
	return ((r.w != 0) && (r.h != 0));
}

bool tpRect::available(ItpRect *r)
{
	bool ret = false;

	if (r)
	{
		ret = ((r->w != 0) && (r->h != 0));
	}

	return ret;
}

bool tpRect::intersect(tpRect &r)
{
	bool ret = false;
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		ItpRect result, r1 = r.get(), r2 = this->get();
		ret = rect_interact(&r1, &r2, &result);

		if (ret)
		{
			set->x = result.x;
			set->y = result.y;
			set->w = result.w;
			set->h = result.h;
		}
	}

	return ret;
}

bool tpRect::intersect(tpRect *r)
{
	return this->intersect(*r);
}

bool tpRect::intersect(ItpRect &r)
{
	tpRect tr = r;
	return this->intersect(tr);
}

bool tpRect::intersect(ItpRect *r)
{
	return this->intersect(*r);
}

bool tpRect::intersect(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	ItpRect r(x, y, w, h);
	return this->intersect(r);
}

bool tpRect::unions(tpRect &r)
{
	bool ret = false;
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		ItpRect result, r1 = r.get(), r2 = this->get();
		ret = rect_union(&r1, &r2, &result);

		if (ret)
		{
			set->x = result.x;
			set->y = result.y;
			set->w = result.w;
			set->h = result.h;
		}
	}

	return ret;
}

bool tpRect::unions(tpRect *r)
{
	return this->unions(*r);
}

bool tpRect::unions(ItpRect &r)
{
	return this->unions(r);
}

bool tpRect::unions(ItpRect *r)
{
	return this->unions(*r);
}

bool tpRect::unions(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	ItpRect result(x, y, w, h);
	return this->unions(result);
}

tpRect tpRect::operator=(const tpRect &self)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;
	ItpRect result;
	if (set)
	{
		result = self.get();

		set->x = result.x;
		set->y = result.y;
		set->w = result.w;
		set->h = result.h;
	}

	return *this;
}

tpRect tpRect::operator=(const ItpRect &self)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;

	if (set)
	{
		set->x = self.x;
		set->y = self.y;
		set->w = self.w;
		set->h = self.h;
	}

	return *this;
}

bool tpRect::operator==(const tpRect &self)
{
	tpRectSet *set = (tpRectSet *)this->rectSet;
	bool ret = false;

	if (set)
	{
		ItpRect r = self.get();
		ret = ((r.x == set->x) && (r.y == set->y) && (r.w == set->w) && (r.h == set->h));
	}

	return ret;
}

bool tpRect::operator==(const ItpRect &self)
{
	tpRect r = self;
	return (*this == r);
}

bool tpRect::operator!=(const tpRect &self)
{
	return !(*this == self);
}

bool tpRect::operator!=(const ItpRect &self)
{
	return !(*this == self);
}
