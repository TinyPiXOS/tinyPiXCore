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
#include "tpPoint.h"

typedef struct{
	int32_t x; 
	int32_t y;
}ItpPointSet;

tpPoint::tpPoint()
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		set->x = 0;
		set->y = 0;
		
		this->pointSet = set;
	}
}

tpPoint::tpPoint(tpPoint &p)
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		ItpPoint point = p.get();
		set->x = point.x;
		set->y = point.y;
		
		this->pointSet = set;
	}
}

tpPoint::tpPoint(tpPoint *p)
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		set->x = 0;
		set->y = 0;
		
		this->pointSet = set;
		
		if(p){
			ItpPoint point = p->get();
			set->x = point.x;
			set->y = point.y;			
		}
	}
}

tpPoint::tpPoint(ItpPoint &p)
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		set->x = p.x;
		set->y = p.y;
		
		this->pointSet = set;
	}
}

tpPoint::tpPoint(ItpPoint *p)
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		set->x = 0;
		set->y = 0;
		
		this->pointSet = set;
		
		if(p){
			set->x = p->x;
			set->y = p->y;			
		}
	}
}

tpPoint::tpPoint(int32_t x, int32_t y)
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		set->x = x;
		set->y = y;
		
		this->pointSet = set;
	}
}

tpPoint::~tpPoint()
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	
	if(set){
		delete set;
	}
}

ItpPoint tpPoint::get()
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	ItpPoint point = {0, 0};
	
	if(set){
		point.x = set->x;
		point.y = set->y;
	}
	
	return point;
}

tpPoint tpPoint::operator = (tpPoint &p)
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	ItpPoint point = p.get();
	
	if(set){
		set->x = point.x;
		set->y = point.y;
	}
	
	return *this;
}

tpPoint tpPoint::operator = (ItpPoint &p)
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	
	if(set){
		set->x = p.x;
		set->y = p.y;
	}
	
	return *this;
}

bool tpPoint::operator == (tpPoint &p)
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	ItpPoint point = p.get();
	bool ret = false;
	
	if(set){
		ret = ((set->x == point.x) && (set->y == point.y));
	}
	
	return ret;
}

bool tpPoint::operator == (ItpPoint &p)
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	bool ret = false;
	
	if(set){
		ret = ((set->x == p.x) && (set->y == p.y));
	}
	
	return ret;
}

bool tpPoint::operator != (tpPoint &p)
{
	return !(*this == p);
}

bool tpPoint::operator != (ItpPoint &p)
{
	return !(*this == p);
}
