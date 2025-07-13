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
#include "tpSize.h"

typedef struct{
	int32_t w;
	int32_t h;
}ItpSizeSet;

tpSize::tpSize()
{
	ItpSizeSet *set = new ItpSizeSet();
	
	if(set){
		set->w = 0;
		set->h = 0;
		
		this->sizeSet = set;
	}
}

tpSize::tpSize(tpSize &s)
{
	ItpSize size = s.get();
	ItpSizeSet *set = new ItpSizeSet();
	
	if(set){
		set->w = size.w;
		set->h = size.h;
		
		this->sizeSet = set;
	}
	
}

tpSize::tpSize(tpSize *s)
{
	ItpSizeSet *set = new ItpSizeSet();
	
	if(set){
		set->w = 0;
		set->h = 0;
		
		this->sizeSet = set;
		
		if(s){
			ItpSize size = s->get();
			
			set->w = size.w;
			set->h = size.h;
		}
	}
}

tpSize::tpSize(int32_t width, int32_t height)
{
	ItpSizeSet *set = new ItpSizeSet();
	
	if(set){
		set->w = width;
		set->h = height;
		
		this->sizeSet = set;
	}
}

tpSize::~tpSize()
{
	ItpSizeSet *set = new ItpSizeSet();
	
	if(set){
		delete set;
	}
}

void tpSize::set(int32_t width, int32_t height)
{
	ItpSizeSet *set = (ItpSizeSet*)this->sizeSet;
	
	if(set){
		set->w = width;
		set->h = height;
	}
}

void tpSize::set(tpSize &p)
{
	ItpSize size = p.get();
	this->set(size.w, size.h);
}

void tpSize::set(tpSize *p)
{
	this->set(*p);
}

ItpSize tpSize::get()
{
	ItpSizeSet *set = (ItpSizeSet*)this->sizeSet;
	ItpSize size = {0, 0};
	
	if(set){
		size.w = set->w;
		size.h = set->h;
	}
	
	return size;
}

int32_t tpSize::width()
{
	return this->get().w;
}

int32_t tpSize::height()
{
	return this->get().h;
}

void tpSize::crease(int32_t dx, int32_t dy)
{
	ItpSizeSet *set = (ItpSizeSet*)this->sizeSet;
	
	if(set){
		set->w += dx;
		set->h += dy;
		
		if((set->w & 0x80000000) == 0x80000000){
			set->w = 0;
		}
		
		if((set->h & 0x80000000) == 0x80000000){
			set->h = 0;
		}
	}
}

tpSize tpSize::operator = (tpSize &s)
{
	ItpSizeSet *set = (ItpSizeSet*)this->sizeSet;
	
	if(set){
		set->w = s.get().w;
		set->h = s.get().h;
	}
	
	return *this;
}

tpSize tpSize::operator = (ItpSize &s)
{
	ItpSizeSet *set = (ItpSizeSet*)this->sizeSet;
	
	if(set){
		set->w = s.w;
		set->h = s.h;
	}
	
	return *this;
}
	
bool tpSize::operator == (tpSize &s)
{
	ItpSizeSet *set = (ItpSizeSet*)this->sizeSet;
	bool ret = false;
	
	if(set){
		ret = ((set->w == s.get().w) && (set->h == s.get().h));
	}
	
	return ret;
}

bool tpSize::operator == (ItpSize &s)
{
	ItpSizeSet *set = (ItpSizeSet*)this->sizeSet;
	bool ret = false;
	
	if(set){
		ret = ((set->w == s.w) && (set->h == s.h));
	}
	
	return ret;
}
	
bool tpSize::operator != (tpSize &s)
{
	return !(*this != s);
}

bool tpSize::operator != (ItpSize &s)
{
	return !(*this != s);
}
