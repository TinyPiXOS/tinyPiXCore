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
#include "tpEvent.h"
#include "tpSurface.h"
#include "tpCanvas.h"
#include "tpRect.h"
#include "tpChildWidget.h"
#include <cstring>
#include "tpDef.h"

//--------------------------tpEvent------------------------------/
tpEvent::tpEvent() : tpEventSet(nullptr)
{
}

tpEvent::~tpEvent()
{
}

//--------------------------tpShowEvent------------------------------/

// tpShowEvent::tpShowEvent() : tpEvent()
// {
// }

// tpShowEvent::~tpShowEvent()
// {
// }

// bool tpShowEvent::construct(ItpEventData *eventData)
// {
// 	return true;
// }

// tpEvent::ItpEventType tpShowEvent::eventType()
// {
// 	return tpEvent::EVENT_WINDOW_SHOW_TYPE;
// }

//--------------------------tpKeyboardEvent------------------------------/
tpKeyboardEvent::tpKeyboardEvent() : tpEvent()
{
	ItpKeyboardSet *set = new ItpKeyboardSet();

	if (set)
	{
		memset(set, 0, sizeof(ItpKeyboardSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpKeyboardEvent::~tpKeyboardEvent()
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpKeyboardEvent::construct(ItpEventData *eventData)
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpKeyboardSet *pEventData = (ItpKeyboardSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			return true;
		}
	}

	return false;
}

tpEvent::ItpEventType tpKeyboardEvent::eventType()
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

uint8_t tpKeyboardEvent::which()
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;
	uint8_t which = TP_INVALIDATE_VALUE;

	if (set)
	{
		which = set->which;
	}

	return which;
}

bool tpKeyboardEvent::state()
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;
	bool state = false;

	if (set)
	{
		state = set->state;
	}

	return state;
}

int32_t tpKeyboardEvent::scancode()
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;
	int32_t scancode = TP_INVALIDATE_VALUE;

	if (set)
	{
		scancode = set->scancode;
	}

	return scancode;
}

int32_t tpKeyboardEvent::virtualKey()
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;
	int32_t virtualKey = TP_INVALIDATE_VALUE;

	if (set)
	{
		virtualKey = set->virtualKey;
	}

	return virtualKey;
}

int32_t tpKeyboardEvent::symbol()
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;
	int32_t symbol = TP_INVALIDATE_VALUE;

	if (set)
	{
		symbol = set->symbol;
	}

	return symbol;
}

const char *tpKeyboardEvent::shortCut()
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;
	const char *pShortCut = nullptr;

	if (set)
	{
		pShortCut = set->shortCut;
	}

	return pShortCut;
}

KeyModeType tpKeyboardEvent::keyMod()
{
	ItpKeyboardSet *set = (ItpKeyboardSet *)tpEvent::tpEventSet;
	KeyModeType mod = TP_KMOD_NONE;

	if (set)
	{
		mod = set->keyMod;
	}

	return mod;
}

bool tpKeyboardEvent::isPrintable()
{
	int32_t chechKey = scancode();

	if ((chechKey >= TP_SCANCODE_A && chechKey <= TP_SCANCODE_0) || chechKey == TP_SCANCODE_SPACE || chechKey == TP_SCANCODE_TAB || (chechKey >= TP_SCANCODE_MINUS && chechKey <= TP_SCANCODE_BACKSLASH) || (chechKey >= TP_SCANCODE_SEMICOLON && chechKey <= TP_SCANCODE_SLASH) || (chechKey >= TP_SCANCODE_KP_DIVIDE && chechKey <= TP_SCANCODE_KP_PLUS) || (chechKey >= TP_SCANCODE_KP_1 && chechKey <= TP_SCANCODE_KP_PERIOD))
		return true;

	return false;
}

//--------------------------tpMouseEvent------------------------------/
tpMouseEvent::tpMouseEvent() : tpEvent()
{
	ItpMouseSet *set = new ItpMouseSet();

	if (set)
	{
		memset(set, 0, sizeof(ItpMouseSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpMouseEvent::~tpMouseEvent()
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
		set = nullptr;
		tpEvent::tpEventSet = nullptr;
	}
}

bool tpMouseEvent::construct(ItpEventData *eventData)
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;

	if (!set)
		return false;

	ItpMouseSet *pEventData = (ItpMouseSet *)eventData;

	if (!pEventData)
		return false;

	*set = *pEventData;
	// set->type = EVENT_MOUSE_KEY_TYPE;

	return true;
}

tpEvent::ItpEventType tpMouseEvent::eventType()
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

int32_t tpMouseEvent::which()
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;
	int32_t which = TP_INVALIDATE_VALUE;

	if (set)
	{
		which = set->which;
	}

	return which;
}

MouseEventType tpMouseEvent::button()
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;
	MouseEventType button = BUTTON_INVALIDATE_VALUE;
	if (set)
	{
		button = set->button;
	}

	return button;
}

bool tpMouseEvent::state()
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;
	bool state = false;

	if (set)
	{
		state = set->state;
	}

	return state;
}

ItpPoint tpMouseEvent::pos()
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;
	if (!set)
		return ItpPoint();

	return set->pos;
}

ItpPoint tpMouseEvent::globalPos()
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;
	if (!set)
		return ItpPoint();

	return set->globalPos;
}

//--------------------------tpWheelEvent------------------------------/
tpWheelEvent::tpWheelEvent() : tpEvent()
{
	ItpMouseSet *set = new ItpMouseSet();

	if (set)
	{
		memset(set, 0, sizeof(ItpMouseSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpWheelEvent::~tpWheelEvent()
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
		set = nullptr;
		tpEvent::tpEventSet = nullptr;
	}
}

bool tpWheelEvent::construct(ItpEventData *eventData)
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;

	if (!set)
		return false;

	ItpMouseSet *pEventData = (ItpMouseSet *)eventData;

	if (!pEventData)
		return false;

	*set = *pEventData;
	set->type = EVENT_WHEEL_EVENT;

	if (pEventData->button == BUTTON_WHEELUP)
		angleDelta_ = 10;
	else
		angleDelta_ = -10;

	return true;
}

tpEvent::ItpEventType tpWheelEvent::eventType()
{
	ItpMouseSet *set = (ItpMouseSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

//--------------------------tpFingerEvent------------------------------/
tpFingerEvent::tpFingerEvent() : tpEvent()
{
	ItpFingerSet *set = new ItpFingerSet();

	if (set)
	{
		memset(set, 0, sizeof(ItpFingerSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpFingerEvent::~tpFingerEvent()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpFingerEvent::construct(ItpEventData *eventData)
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpFingerSet *pEventData = (ItpFingerSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			set->type = EVENT_FINGER_TYPE;

			return true;
		}
	}

	return false;
}

tpEvent::ItpEventType tpFingerEvent::eventType()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

int32_t tpFingerEvent::touchFingerType()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	int32_t type = TOUCH_FINGER_NONE;

	if (set)
	{
		type = set->touchFingerType;
	}

	return type;
}

int32_t tpFingerEvent::timestamp()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	int32_t timestamp = 0;

	if (set)
	{
		timestamp = set->timestamp;
	}

	return timestamp;
}

long long tpFingerEvent::fingerID()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	long long fingerId = TP_INVALIDATE_VALUE;

	if (set)
	{
		fingerId = set->fingerID;
	}

	return fingerId;
}

long long tpFingerEvent::touchID()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	long long touchID = TP_INVALIDATE_VALUE;

	if (set)
	{
		touchID = set->touchID;
	}

	return touchID;
}

int32_t tpFingerEvent::X()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	int32_t x = TP_INVALIDATE_VALUE;

	if (set)
	{
		x = set->x;
	}

	return x;
}

int32_t tpFingerEvent::Y()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	int32_t y = TP_INVALIDATE_VALUE;

	if (set)
	{
		y = set->y;
	}

	return y;
}

int32_t tpFingerEvent::dx()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	int32_t dx = TP_INVALIDATE_VALUE;

	if (set)
	{
		dx = set->dx;
	}

	return dx;
}

int32_t tpFingerEvent::dy()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	int32_t dy = TP_INVALIDATE_VALUE;

	if (set)
	{
		dy = set->dy;
	}

	return dy;
}

float tpFingerEvent::pressure()
{
	ItpFingerSet *set = (ItpFingerSet *)tpEvent::tpEventSet;
	float pressure = 0.0;

	if (set)
	{
		pressure = set->pressure;
	}

	return pressure;
}

//--------------------------tpDollAREvent------------------------------/

tpDollAREvent::tpDollAREvent() : tpEvent()
{
	ItpDollarSet *set = new ItpDollarSet();

	if (set)
	{
		memset(set, 0, sizeof(ItpDollarSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpDollAREvent::~tpDollAREvent()
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpDollAREvent::construct(ItpEventData *eventData)
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpDollarSet *pEventData = (ItpDollarSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			set->type = EVENT_DOLLAR_TYPE;

			return true;
		}
	}

	return false;
}

tpEvent::ItpEventType tpDollAREvent::eventType()
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

int32_t tpDollAREvent::dollarType()
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;
	int32_t type = TP_INVALIDATE_VALUE;

	if (set)
	{
		type = set->dollarType;
	}

	return type;
}

int32_t tpDollAREvent::timestamp()
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;
	int32_t timestamp = 0;

	if (set)
	{
		timestamp = set->timestamp;
	}

	return timestamp;
}

long long tpDollAREvent::touchID()
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;
	long long touchID = TP_INVALIDATE_VALUE;

	if (set)
	{
		touchID = set->touchID;
	}

	return touchID;
}

long long tpDollAREvent::GestureID()
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;
	long long GestureID = TP_INVALIDATE_VALUE;

	if (set)
	{
		GestureID = set->GestureID;
	}

	return GestureID;
}

int32_t tpDollAREvent::numFingers()
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;
	int32_t numFingers = 0;

	if (set)
	{
		numFingers = set->numFingers;
	}

	return numFingers;
}

int32_t tpDollAREvent::X()
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;
	int32_t x = TP_INVALIDATE_VALUE;

	if (set)
	{
		x = set->x;
	}

	return x;
}

int32_t tpDollAREvent::Y()
{
	ItpDollarSet *set = (ItpDollarSet *)tpEvent::tpEventSet;
	int32_t y = TP_INVALIDATE_VALUE;

	if (set)
	{
		y = set->y;
	}

	return y;
}
//--------------------------tpMultiGestureEvent------------------------------/
tpMultiGestureEvent::tpMultiGestureEvent()
{
	ItpMultiGestureSet *set = new ItpMultiGestureSet();

	if (set)
	{
		memset(set, 0, sizeof(ItpMultiGestureSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpMultiGestureEvent::~tpMultiGestureEvent()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpMultiGestureEvent::construct(ItpEventData *eventData)
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpMultiGestureSet *pEventData = (ItpMultiGestureSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			set->type = EVENT_DOLLAR_TYPE;

			return true;
		}
	}
	return false;
}

tpEvent::ItpEventType tpMultiGestureEvent::eventType()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

int32_t tpMultiGestureEvent::timestamp()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;
	int32_t timestamp = 0;

	if (set)
	{
		timestamp = set->timestamp;
	}

	return timestamp;
}

long long tpMultiGestureEvent::touchID()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;
	long long touchID = TP_INVALIDATE_VALUE;

	if (set)
	{
		touchID = set->touchID;
	}

	return touchID;
}

float tpMultiGestureEvent::dtheta()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;
	float dtheta = 0.0;

	if (set)
	{
		dtheta = set->dtheta;
	}

	return dtheta;
}

float tpMultiGestureEvent::ddist()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;
	float ddist = 0.0;

	if (set)
	{
		ddist = set->ddist;
	}

	return ddist;
}

int32_t tpMultiGestureEvent::X()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;
	int32_t x = TP_INVALIDATE_VALUE;

	if (set)
	{
		x = set->x;
	}

	return x;
}

int32_t tpMultiGestureEvent::Y()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;
	int32_t y = TP_INVALIDATE_VALUE;

	if (set)
	{
		y = set->y;
	}

	return y;
}

uint16_t tpMultiGestureEvent::numfingers()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;
	uint16_t numfingers = 0;

	if (set)
	{
		numfingers = set->numfingers;
	}

	return numfingers;
}

uint16_t tpMultiGestureEvent::padding()
{
	ItpMultiGestureSet *set = (ItpMultiGestureSet *)tpEvent::tpEventSet;
	uint16_t padding = 0;

	if (set)
	{
		padding = set->padding;
	}

	return padding;
}

//--------------------------tpObjectMoveEvent------------------------------/
tpObjectMoveEvent::tpObjectMoveEvent() : tpEvent()
{
	ItpObjectMoveSet *set = new ItpObjectMoveSet();

	if (set)
	{
		memset(set, 0, sizeof(ItpObjectMoveSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpObjectMoveEvent::~tpObjectMoveEvent()
{
	ItpObjectMoveSet *set = (ItpObjectMoveSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpObjectMoveEvent::construct(ItpEventData *eventData)
{
	ItpObjectMoveSet *set = (ItpObjectMoveSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpObjectMoveSet *pEventData = (ItpObjectMoveSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			set->type = EVENT_OBJECT_MOVE_TYPE;

			return true;
		}
	}

	return false;
}

tpEvent::ItpEventType tpObjectMoveEvent::eventType()
{
	ItpObjectMoveSet *set = (ItpObjectMoveSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

tpObject *tpObjectMoveEvent::object()
{
	ItpObjectMoveSet *set = (ItpObjectMoveSet *)tpEvent::tpEventSet;
	tpObject *object = nullptr;

	if (set)
	{
		object = set->object;
	}

	return object;
}

int32_t tpObjectMoveEvent::newX()
{
	ItpObjectMoveSet *set = (ItpObjectMoveSet *)tpEvent::tpEventSet;
	int32_t newX = TP_INVALIDATE_VALUE;

	if (set)
	{
		newX = set->nx;
	}

	return newX;
}

int32_t tpObjectMoveEvent::newY()
{
	ItpObjectMoveSet *set = (ItpObjectMoveSet *)tpEvent::tpEventSet;
	int32_t newY = TP_INVALIDATE_VALUE;

	if (set)
	{
		newY = set->ny;
	}

	return newY;
}

//--------------------------tpObjectResizeEvent------------------------------/
tpObjectResizeEvent::tpObjectResizeEvent() : tpEvent()
{
	ItpObjectResizeSet *set = new ItpObjectResizeSet(); // EVENT_OBJECT_RESIZE_TYPE

	if (set)
	{
		memset(set, 0, sizeof(ItpObjectResizeSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpObjectResizeEvent::~tpObjectResizeEvent()
{
	ItpObjectResizeSet *set = (ItpObjectResizeSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpObjectResizeEvent::construct(ItpEventData *eventData)
{
	ItpObjectResizeSet *set = (ItpObjectResizeSet *)tpEvent::tpEventSet;

	if (!set)
		return false;

	ItpObjectResizeSet *pEventData = (ItpObjectResizeSet *)eventData;
	if (pEventData)
	{
		*set = *pEventData;
		set->type = EVENT_OBJECT_RESIZE_TYPE;

		return true;
	}

	return false;
}

tpEvent::ItpEventType tpObjectResizeEvent::eventType()
{
	ItpObjectResizeSet *set = (ItpObjectResizeSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

int32_t tpObjectResizeEvent::question()
{
	ItpObjectResizeSet *set = (ItpObjectResizeSet *)tpEvent::tpEventSet;
	int32_t question = TP_UNKOWN_CHANGE;

	if (set)
	{
		question = set->question;
	}

	return question;
}

tpObject *tpObjectResizeEvent::object()
{
	ItpObjectResizeSet *set = (ItpObjectResizeSet *)tpEvent::tpEventSet;
	tpObject *object = nullptr;

	if (set)
	{
		object = set->object;
	}

	return object;
}

int32_t tpObjectResizeEvent::nWidth()
{
	ItpObjectResizeSet *set = (ItpObjectResizeSet *)tpEvent::tpEventSet;
	int32_t newWidth = TP_INVALIDATE_VALUE;

	if (set)
	{
		newWidth = set->nw;
	}

	return newWidth;
}

int32_t tpObjectResizeEvent::nHeight()
{
	ItpObjectResizeSet *set = (ItpObjectResizeSet *)tpEvent::tpEventSet;
	int32_t newHeight = TP_INVALIDATE_VALUE;

	if (set)
	{
		newHeight = set->nw;
	}

	return newHeight;
}

//--------------------------tpObjectFocusEvent------------------------------/
tpObjectFocusEvent::tpObjectFocusEvent() : tpEvent()
{
	ItpObjectFocusSet *set = new ItpObjectFocusSet(); // EVENT_OBJECT_FOCUS_TYPE

	if (set)
	{
		memset(set, 0, sizeof(ItpObjectFocusSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpObjectFocusEvent::~tpObjectFocusEvent()
{
	ItpObjectFocusSet *set = (ItpObjectFocusSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpObjectFocusEvent::construct(ItpEventData *eventData)
{
	ItpObjectFocusSet *set = (ItpObjectFocusSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpObjectFocusSet *pEventData = (ItpObjectFocusSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			set->type = EVENT_OBJECT_FOCUS_TYPE;

			return true;
		}
	}

	return false;
}

tpEvent::ItpEventType tpObjectFocusEvent::eventType()
{
	ItpObjectFocusSet *set = (ItpObjectFocusSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

tpObject *tpObjectFocusEvent::object()
{
	ItpObjectFocusSet *set = (ItpObjectFocusSet *)tpEvent::tpEventSet;
	tpObject *object = nullptr;

	if (set)
	{
		object = set->object;
	}

	return object;
}

bool tpObjectFocusEvent::focused()
{
	ItpObjectFocusSet *set = (ItpObjectFocusSet *)tpEvent::tpEventSet;
	bool focused = false;

	if (set)
	{
		focused = set->focused;
	}

	return focused;
}

//--------------------------tpObjectLeaveEvent------------------------------/
tpObjectLeaveEvent::tpObjectLeaveEvent() : tpEvent()
{
	ItpObjectLeaveSet *set = new ItpObjectLeaveSet(); // EVENT_OBJECT_LEAVE_TYPE

	if (set)
	{
		memset(set, 0, sizeof(ItpObjectLeaveSet));
		set->type = tpEvent::EVENT_OBJECT_LEAVE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpObjectLeaveEvent::~tpObjectLeaveEvent()
{
	ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpObjectLeaveEvent::construct(ItpEventData *eventData)
{
	ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpObjectLeaveSet *pEventData = (ItpObjectLeaveSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			set->type = EVENT_OBJECT_LEAVE_TYPE;

			return true;
		}
	}

	return false;
}

tpEvent::ItpEventType tpObjectLeaveEvent::eventType()
{
	ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

tpObject *tpObjectLeaveEvent::object()
{
	ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)tpEvent::tpEventSet;
	tpObject *object = nullptr;

	if (set)
	{
		object = set->object;
	}

	return object;
}

bool tpObjectLeaveEvent::leave()
{
	ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)tpEvent::tpEventSet;
	bool leaved = false;

	if (set)
	{
		leaved = set->leaved;
	}

	return leaved;
}

//--------------------------tpObjectVisibleEvent------------------------------/
tpObjectVisibleEvent::tpObjectVisibleEvent() : tpEvent()
{
	ItpObjectVisibleSet *set = new ItpObjectVisibleSet(); // EVENT_OBJECT_VISIBLE_TYPE

	if (set)
	{
		memset(set, 0, sizeof(ItpObjectVisibleSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpObjectVisibleEvent::~tpObjectVisibleEvent()
{
	ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpObjectVisibleEvent::construct(ItpEventData *eventData)
{
	ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpObjectVisibleSet *pEventData = (ItpObjectVisibleSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			set->type = EVENT_OBJECT_VISIBLE_TYPE;

			return true;
		}
	}

	return false;
}

tpEvent::ItpEventType tpObjectVisibleEvent::eventType()
{
	ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

tpObject *tpObjectVisibleEvent::object()
{
	ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)tpEvent::tpEventSet;
	tpObject *object = nullptr;

	if (set)
	{
		object = set->object;
	}

	return object;
}

bool tpObjectVisibleEvent::visible()
{
	ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)tpEvent::tpEventSet;
	bool visible = false;

	if (set)
	{
		visible = set->visible;
	}

	return visible;
}

//--------------------------tpObjectRotateEvent------------------------------/
tpObjectRotateEvent::tpObjectRotateEvent() : tpEvent()
{
	ItpObjectRotateSet *set = new ItpObjectRotateSet(); // EVENT_OBJECT_ROTATE_TYPE

	if (set)
	{
		memset(set, 0, sizeof(ItpObjectRotateSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpObjectRotateEvent::~tpObjectRotateEvent()
{
	ItpObjectRotateSet *set = (ItpObjectRotateSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpObjectRotateEvent::construct(ItpEventData *eventData)
{
	ItpObjectRotateSet *set = (ItpObjectRotateSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpObjectRotateSet *pEventData = (ItpObjectRotateSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			set->type = EVENT_OBJECT_ROTATE_TYPE;

			return true;
		}
	}

	return false;
}

tpEvent::ItpEventType tpObjectRotateEvent::eventType()
{
	ItpObjectRotateSet *set = (ItpObjectRotateSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

tpObject *tpObjectRotateEvent::object()
{
	ItpObjectRotateSet *set = (ItpObjectRotateSet *)tpEvent::tpEventSet;
	tpObject *object = nullptr;

	if (set)
	{
		object = set->object;
	}

	return object;
}

ItpRotateType tpObjectRotateEvent::rotate()
{
	ItpObjectRotateSet *set = (ItpObjectRotateSet *)tpEvent::tpEventSet;
	ItpRotateType rotate = TP_ROT_0;

	if (set)
	{
		rotate = set->rotate;
	}

	return rotate;
}

//--------------------------tpObjectPaintEvent------------------------------/
tpObjectPaintEvent::tpObjectPaintEvent() : tpEvent()
{
	ItpObjectPaintSet *set = new ItpObjectPaintSet(); // EVENT_OBJECT_ROTATE_TYPE

	if (set)
	{
		memset(set, 0, sizeof(ItpObjectPaintSet));
		set->type = tpEvent::EVENT_NONE_TYPE;
		set->canDraw = false;
		tpEvent::tpEventSet = set;
	}
}

tpObjectPaintEvent::~tpObjectPaintEvent()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;

	if (set)
	{
		if (set->canvas)
		{
			delete set->canvas;
		}

		delete set;
	}
}

bool tpObjectPaintEvent::construct(ItpEventData *eventData)
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;

	if (!set)
		return false;

	set->canDraw = false;

	ItpObjectPaintInput *input = (ItpObjectPaintInput *)eventData;
	if (!input)
		return false;

	tpChildWidget *inputObjectChild = static_cast<tpChildWidget *>(input->object);
	if (!inputObjectChild)
		return false;

	ItpObjectType type = inputObjectChild->objectType();
	set->object = inputObjectChild;

	if (type == TP_FLOAT_OBJECT ||
		type == TP_TOP_OBJECT)
	{
		set->offsetX = 0;
		set->offsetY = 0;
	}
	else
	{
		set->offsetX = inputObjectChild->toScreen().x - inputObjectChild->offsetX();
		set->offsetY = inputObjectChild->toScreen().y - inputObjectChild->offsetY();
	}

	set->surface = input->surface;
	set->canvas = new tpCanvas(set->surface, set->offsetX, set->offsetY);

	if (set->canvas == nullptr)
	{
		delete set->surface;
		set->surface = nullptr;
		return false;
	}

	tpRect clipRect = input->updateRect;
	ItpRect objectAbsRect = inputObjectChild->toScreen();
	tpRect absRect(objectAbsRect);

	set->canDraw = clipRect.intersect(absRect);

	if (set->canDraw == false)
	{
		return false;
	}

	set->updateRect = clipRect.get(); // input->updateRect;
	tpObject *top = input->object->topObject();

	if (top &&
		top->objectType() == TP_FLOAT_OBJECT)
	{
		clipRect.setX0(clipRect.X0() - inputObjectChild->offsetX());
		clipRect.setY0(clipRect.Y0() - inputObjectChild->offsetY());
	}

	set->rect = inputObjectChild->rect();
	set->canvas->setClipRect(clipRect);
	set->type = EVENT_OBJECT_PAINT_TYPE;

	return true;
}

tpEvent::ItpEventType tpObjectPaintEvent::eventType()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

tpObject *tpObjectPaintEvent::object()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	tpObject *object = nullptr;

	if (set)
	{
		object = set->object;
	}

	return object;
}

tpCanvas *tpObjectPaintEvent::canvas()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	tpCanvas *canvas = nullptr;

	if (set)
	{
		canvas = set->canvas;
	}

	return canvas;
}

tpSurface *tpObjectPaintEvent::surface()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	tpSurface *surface = nullptr;

	if (set)
	{
		surface = set->surface;
	}

	return surface;
}

ItpSufaceData *tpObjectPaintEvent::itpSurface()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	ItpSufaceData *itpSurface = nullptr;

	if (set)
	{
		itpSurface = set->itpSurface;
	}

	return itpSurface;
}

int32_t tpObjectPaintEvent::offsetX()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	int32_t offsetX = 0;

	if (set)
	{
		offsetX = set->offsetX;
	}

	return offsetX;
}

int32_t tpObjectPaintEvent::offsetY()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	int32_t offsetY = 0;

	if (set)
	{
		offsetY = set->offsetY;
	}

	return offsetY;
}

ItpRect tpObjectPaintEvent::updateRect()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	ItpRect result = {0, 0, 0, 0};

	if (set)
	{
		result = set->updateRect;
	}

	return result;
}

ItpRect tpObjectPaintEvent::rect()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	ItpRect result = {0, 0, 0, 0};

	if (set)
	{
		result = set->rect;
	}

	return result;
}

ItpRect tpObjectPaintEvent::absRect()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	ItpRect result;

	tpChildWidget *chiildObject = static_cast<tpChildWidget *>(set->object);

	if (!chiildObject)
		return result;

	result = chiildObject->toScreen();

	return result;
}

bool tpObjectPaintEvent::isCanDraw()
{
	ItpObjectPaintSet *set = (ItpObjectPaintSet *)tpEvent::tpEventSet;
	bool canDraw = false;

	if (set)
	{
		canDraw = set->canDraw;
	}

	return canDraw;
}

//--------------------------tpObjectActivedEvent------------------------------/
tpObjectActiveEvent::tpObjectActiveEvent() : tpEvent()
{
	ItpObjectActiveSet *set = new ItpObjectActiveSet(); // EVENT_OBJECT_ROTATE_TYPE

	if (set)
	{
		memset(set, 0, sizeof(tpObjectActiveEvent));
		set->type = tpEvent::EVENT_NONE_TYPE;
		tpEvent::tpEventSet = set;
	}
}

tpObjectActiveEvent::~tpObjectActiveEvent()
{
	ItpObjectActiveSet *set = (ItpObjectActiveSet *)tpEvent::tpEventSet;

	if (set)
	{
		delete set;
	}
}

bool tpObjectActiveEvent::construct(ItpEventData *eventData)
{
	ItpObjectActiveSet *set = (ItpObjectActiveSet *)tpEvent::tpEventSet;

	if (set)
	{
		ItpObjectActiveSet *pEventData = (ItpObjectActiveSet *)eventData;

		if (pEventData)
		{
			*set = *pEventData;
			set->type = EVENT_OBJECT_ACTIVE_TYPE;

			return true;
		}
	}

	return false;
}

tpEvent::ItpEventType tpObjectActiveEvent::eventType()
{
	ItpObjectActiveSet *set = (ItpObjectActiveSet *)tpEvent::tpEventSet;
	tpEvent::ItpEventType type = tpEvent::EVENT_NONE_TYPE;

	if (set)
	{
		type = set->type;
	}

	return type;
}

tpObject *tpObjectActiveEvent::object()
{
	ItpObjectActiveSet *set = (ItpObjectActiveSet *)tpEvent::tpEventSet;
	tpObject *object = nullptr;

	if (set)
	{
		object = set->object;
	}

	return object;
}

bool tpObjectActiveEvent::isActived()
{
	ItpObjectActiveSet *set = (ItpObjectActiveSet *)tpEvent::tpEventSet;
	bool actived = false;

	if (set)
	{
		actived = set->actived;
	}

	return actived;
}

tpThemeChangeEvent::tpThemeChangeEvent()
{
}

tpThemeChangeEvent::~tpThemeChangeEvent()
{
}

bool tpThemeChangeEvent::construct(ItpEventData *eventData)
{
	return true;
}

tpEvent::ItpEventType tpThemeChangeEvent::eventType()
{
	return EVENT_THEME_CHANGE_TYPE;
}
