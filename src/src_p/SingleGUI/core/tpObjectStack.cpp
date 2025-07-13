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
#include "tpObjectStack.h"
#include "tpScreen.h"
#include <list>
#include <mutex>
#include <functional>

typedef struct
{
	std::mutex gMutex;
	std::list<tpScreen *> objectStackList;
} ItpObjectStackSet;

tpObjectStack::tpObjectStack()
{
	ItpObjectStackSet *set = new ItpObjectStackSet();

	if (set)
	{
		this->objectStackSet = set;
	}
}

tpObjectStack::~tpObjectStack()
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;

	if (set)
	{
		set->objectStackList.clear();
		delete set;
	}
}

bool tpObjectStack::push(tpScreen *hook)
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;
	bool ret = false;

	if (set)
	{
		if (hook)
		{
			set->gMutex.lock();

			auto iter = std::find_if(set->objectStackList.begin(), set->objectStackList.end(), [hook](tpScreen *value)
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

bool tpObjectStack::pop()
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

bool tpObjectStack::remove(tpScreen *hook)
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;
	bool ret = false;

	if (set)
	{
		if (hook)
		{
			set->gMutex.lock();

			auto iter = std::find_if(set->objectStackList.begin(), set->objectStackList.end(), [hook](tpScreen *value)
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

void tpObjectStack::clear()
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;

	if (set)
	{
		set->gMutex.lock();
		set->objectStackList.clear();
		set->gMutex.unlock();
	}
}

void tpObjectStack::exit()
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;

	if (set)
	{
		set->gMutex.lock();

		std::list<tpScreen *>::iterator iter = set->objectStackList.begin();

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

tpScreen *tpObjectStack::top()
{
	ItpObjectStackSet *set = (ItpObjectStackSet *)this->objectStackSet;
	tpScreen *topHook = nullptr;

	if (set)
	{
		set->gMutex.lock();

		bool visible = true;
		auto iter = std::find_if(set->objectStackList.begin(), set->objectStackList.end(), [visible](tpScreen *value)
								 { return (value->visible() == visible); });

		if (iter != set->objectStackList.end())
		{
			topHook = *iter;
		}

		set->gMutex.unlock();
	}

	return topHook;
}

bool tpObjectStack::dispatch(ItpEvent *event)
{
	tpScreen *topHook = this->top();
	bool ret = false;

	if (topHook)
	{
		ret = topHook->dispatchEvent(event);
	}

	return ret;
}
