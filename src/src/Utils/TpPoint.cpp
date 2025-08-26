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
#include "TpPoint.h"

typedef struct{
	int32_t x; 
	int32_t y;
}ItpPointSet;

TpPoint::TpPoint()
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		set->x = 0;
		set->y = 0;
		
		this->pointSet = set;
	}
}

TpPoint::TpPoint(TpPoint &p)
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		ItpPoint point = p.get();
		set->x = point.x;
		set->y = point.y;
		
		this->pointSet = set;
	}
}

TpPoint::TpPoint(TpPoint *p)
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

TpPoint::TpPoint(ItpPoint &p)
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		set->x = p.x;
		set->y = p.y;
		
		this->pointSet = set;
	}
}

TpPoint::TpPoint(ItpPoint *p)
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

TpPoint::TpPoint(int32_t x, int32_t y)
{
	ItpPointSet *set = new ItpPointSet();
	
	if(set){
		set->x = x;
		set->y = y;
		
		this->pointSet = set;
	}
}

TpPoint::~TpPoint()
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	
	if(set){
		delete set;
	}
}

ItpPoint TpPoint::get()
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	ItpPoint point = {0, 0};
	
	if(set){
		point.x = set->x;
		point.y = set->y;
	}
	
	return point;
}

TpPoint TpPoint::operator = (TpPoint &p)
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	ItpPoint point = p.get();
	
	if(set){
		set->x = point.x;
		set->y = point.y;
	}
	
	return *this;
}

TpPoint TpPoint::operator = (ItpPoint &p)
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	
	if(set){
		set->x = p.x;
		set->y = p.y;
	}
	
	return *this;
}

bool TpPoint::operator == (TpPoint &p)
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	ItpPoint point = p.get();
	bool ret = false;
	
	if(set){
		ret = ((set->x == point.x) && (set->y == point.y));
	}
	
	return ret;
}

bool TpPoint::operator == (ItpPoint &p)
{
	ItpPointSet *set = (ItpPointSet*)this->pointSet;
	bool ret = false;
	
	if(set){
		ret = ((set->x == p.x) && (set->y == p.y));
	}
	
	return ret;
}

bool TpPoint::operator != (TpPoint &p)
{
	return !(*this == p);
}

bool TpPoint::operator != (ItpPoint &p)
{
	return !(*this == p);
}
