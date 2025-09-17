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
#include "TpObjectStack.h"
#include "TpScreen.h"
#include <list>
#include <mutex>
#include <functional>

typedef struct
{
	std::mutex gMutex;
	std::list<TpScreen *> objectStackList;
} ItpObjectStackSet;

TpObjectStack::TpObjectStack()
{
	ItpObjectStackSet *set = new ItpObjectStackSet();

	if (set)
	{
		this->objectStackSet = set;
	}
}

TpObjectStack::~TpObjectStack()
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;

	if (set)
	{
		set->objectStackList.clear();
		delete set;
	}
}

bool TpObjectStack::push(TpScreen *hook)
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;
	bool ret = false;

	if (set)
	{
		if (hook)
		{
			set->gMutex.lock();

			auto iter = std::find_if(set->objectStackList.begin(), set->objectStackList.end(), [hook](TpScreen *value)
									 { return (hook == value); });

			if (iter != set->objectStackList.end())
			{
				set->gMutex.unlock();
				return false;
			}

			set->objectStackList.push_front(hook);
			set->gMutex.unlock();

			ret = true;
		}
	}

	return ret;
}

bool TpObjectStack::pop()
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;
	bool ret = false;

	if (set)
	{
		set->gMutex.lock();
		set->objectStackList.pop_front();
		set->gMutex.unlock();

		ret = true;
	}

	return ret;
}

bool TpObjectStack::remove(TpScreen *hook)
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;
	bool ret = false;

	if (set)
	{
		if (hook)
		{
			set->gMutex.lock();

			auto iter = std::find_if(set->objectStackList.begin(), set->objectStackList.end(), [hook](TpScreen *value)
									 { return (hook == value); });

			if (iter != set->objectStackList.end())
			{
				set->objectStackList.erase(iter);
			}

			set->gMutex.unlock();

			ret = true;
		}
	}

	return ret;
}

void TpObjectStack::clear()
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;

	if (set)
	{
		set->gMutex.lock();
		set->objectStackList.clear();
		set->gMutex.unlock();
	}
}

void TpObjectStack::exit()
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;

	if (set)
	{
		set->gMutex.lock();

		std::list<TpScreen *>::iterator iter = set->objectStackList.begin();

		for (; iter != set->objectStackList.end(); iter++)
		{
			if ((*iter)->parent())
			{
				(*iter)->setVisible(false);
				(*iter)->update();
			}
		}

		set->gMutex.unlock();
	}
}

TpScreen *TpObjectStack::top()
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;
	TpScreen *topHook = nullptr;

	if (set)
	{
		set->gMutex.lock();

		bool visible = true;
		auto iter = std::find_if(set->objectStackList.begin(), set->objectStackList.end(), [visible](TpScreen *value)
								 { return (value->visible() == visible); });

		if (iter != set->objectStackList.end())
		{
			topHook = *iter;
		}

		set->gMutex.unlock();
	}

	return topHook;
}

bool TpObjectStack::dispatch(ItpEvent *event)
{
	TpScreen *topHook = this->top();
	bool ret = false;

	if (topHook)
	{
		ret = topHook->dispatchEvent(event);
	}

	return ret;
}
