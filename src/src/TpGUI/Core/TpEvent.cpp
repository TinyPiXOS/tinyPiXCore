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
#include "TpEvent.h"
#include "TpSurface.h"
#include "TpPainter.h"
#include "TpRect.h"
#include "TpChildWidget.h"
#include <cstring>
#include "TpDef.h"

//--------------------------TpEvent------------------------------/
TpEvent::TpEvent() : TpEventSet(nullptr)
{
}

TpEvent::~TpEvent()
{
}

//--------------------------TpShowEvent------------------------------/

// TpShowEvent::TpShowEvent() : TpEvent()
// {
// }

// TpShowEvent::~TpShowEvent()
// {
// }

// bool TpShowEvent::construct(ItpEventData *eventData)
// {
// 	return true;
// }

// TpEvent::ItpEventType TpShowEvent::eventType()
// {
// 	return TpEvent::EVENT_WINDOW_SHOW_TYPE;
// }

//--------------------------TpKeyboardEvent------------------------------/
TpKeyboardEvent::TpKeyboardEvent() : TpEvent()
{
    ItpKeyboardSet *set = new ItpKeyboardSet();

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpKeyboardEvent::~TpKeyboardEvent()
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpKeyboardEvent::construct(ItpEventData *eventData)
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpKeyboardEvent::eventType()
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

uint8_t TpKeyboardEvent::which()
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;
    uint8_t which = TP_INVALIDATE_VALUE;

    if (set)
    {
        which = set->which;
    }

    return which;
}

bool TpKeyboardEvent::state()
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;
    bool state = false;

    if (set)
    {
        state = set->state;
    }

    return state;
}

int32_t TpKeyboardEvent::scancode()
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;
    int32_t scancode = TP_INVALIDATE_VALUE;

    if (set)
    {
        scancode = set->scancode;
    }

    return scancode;
}

int32_t TpKeyboardEvent::virtualKey()
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;
    int32_t virtualKey = TP_INVALIDATE_VALUE;

    if (set)
    {
        virtualKey = set->virtualKey;
    }

    return virtualKey;
}

int32_t TpKeyboardEvent::symbol()
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;
    int32_t symbol = TP_INVALIDATE_VALUE;

    if (set)
    {
        symbol = set->symbol;
    }

    return symbol;
}

const char *TpKeyboardEvent::shortCut()
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;
    const char *pShortCut = nullptr;

    if (set)
    {
        pShortCut = set->shortCut;
    }

    return pShortCut;
}

KeyModeType TpKeyboardEvent::keyMod()
{
    ItpKeyboardSet *set = (ItpKeyboardSet *)TpEvent::TpEventSet;
    KeyModeType mod = TP_KMOD_NONE;

    if (set)
    {
        mod = set->keyMod;
    }

    return mod;
}

bool TpKeyboardEvent::isPrintable()
{
    int32_t chechKey = scancode();

    if ((chechKey >= TP_SCANCODE_A && chechKey <= TP_SCANCODE_0) || chechKey == TP_SCANCODE_SPACE || chechKey == TP_SCANCODE_TAB || (chechKey >= TP_SCANCODE_MINUS && chechKey <= TP_SCANCODE_BACKSLASH) || (chechKey >= TP_SCANCODE_SEMICOLON && chechKey <= TP_SCANCODE_SLASH) || (chechKey >= TP_SCANCODE_KP_DIVIDE && chechKey <= TP_SCANCODE_KP_PLUS) || (chechKey >= TP_SCANCODE_KP_1 && chechKey <= TP_SCANCODE_KP_PERIOD))
        return true;

    return false;
}

//--------------------------TpMouseEvent------------------------------/
TpMouseEvent::TpMouseEvent() : TpEvent()
{
    ItpMouseSet *set = new ItpMouseSet();

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpMouseEvent::~TpMouseEvent()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
        set = nullptr;
        TpEvent::TpEventSet = nullptr;
    }
}

bool TpMouseEvent::construct(ItpEventData *eventData)
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;

    if (!set)
        return false;

    ItpMouseSet *pEventData = (ItpMouseSet *)eventData;

    if (!pEventData)
        return false;

    *set = *pEventData;
    // set->type = EVENT_MOUSE_KEY_TYPE;

    return true;
}

TpEvent::ItpEventType TpMouseEvent::eventType()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

int32_t TpMouseEvent::which()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;
    int32_t which = TP_INVALIDATE_VALUE;

    if (set)
    {
        which = set->which;
    }

    return which;
}

MouseEventType TpMouseEvent::button()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;
    MouseEventType button = BUTTON_INVALIDATE_VALUE;
    if (set)
    {
        button = set->button;
    }

    return button;
}

bool TpMouseEvent::state()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;
    bool state = false;

    if (set)
    {
        state = set->state;
    }

    return state;
}

TpPoint TpMouseEvent::pos()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;
    if (!set)
        return TpPoint();

    return set->pos;
}

TpPoint TpMouseEvent::globalPos()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;
    if (!set)
        return TpPoint();

    return set->globalPos;
}

//--------------------------TpWheelEvent------------------------------/
TpWheelEvent::TpWheelEvent() : TpEvent()
{
    ItpMouseSet *set = new ItpMouseSet();

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpWheelEvent::~TpWheelEvent()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
        set = nullptr;
        TpEvent::TpEventSet = nullptr;
    }
}

bool TpWheelEvent::construct(ItpEventData *eventData)
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpWheelEvent::eventType()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

//--------------------------TpFingerEvent------------------------------/
TpFingerEvent::TpFingerEvent() : TpEvent()
{
    ItpFingerSet *set = new ItpFingerSet();

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpFingerEvent::~TpFingerEvent()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpFingerEvent::construct(ItpEventData *eventData)
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpFingerEvent::eventType()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

int32_t TpFingerEvent::touchFingerType()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    int32_t type = TOUCH_FINGER_NONE;

    if (set)
    {
        type = set->touchFingerType;
    }

    return type;
}

int32_t TpFingerEvent::timestamp()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    int32_t timestamp = 0;

    if (set)
    {
        timestamp = set->timestamp;
    }

    return timestamp;
}

long long TpFingerEvent::fingerID()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    long long fingerId = TP_INVALIDATE_VALUE;

    if (set)
    {
        fingerId = set->fingerID;
    }

    return fingerId;
}

long long TpFingerEvent::touchID()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    long long touchID = TP_INVALIDATE_VALUE;

    if (set)
    {
        touchID = set->touchID;
    }

    return touchID;
}

int32_t TpFingerEvent::X()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    int32_t x = TP_INVALIDATE_VALUE;

    if (set)
    {
        x = set->x;
    }

    return x;
}

int32_t TpFingerEvent::Y()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    int32_t y = TP_INVALIDATE_VALUE;

    if (set)
    {
        y = set->y;
    }

    return y;
}

int32_t TpFingerEvent::dx()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    int32_t dx = TP_INVALIDATE_VALUE;

    if (set)
    {
        dx = set->dx;
    }

    return dx;
}

int32_t TpFingerEvent::dy()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    int32_t dy = TP_INVALIDATE_VALUE;

    if (set)
    {
        dy = set->dy;
    }

    return dy;
}

float TpFingerEvent::pressure()
{
    ItpFingerSet *set = (ItpFingerSet *)TpEvent::TpEventSet;
    float pressure = 0.0;

    if (set)
    {
        pressure = set->pressure;
    }

    return pressure;
}

//--------------------------TpDollAREvent------------------------------/

TpDollAREvent::TpDollAREvent() : TpEvent()
{
    ItpDollarSet *set = new ItpDollarSet();

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpDollAREvent::~TpDollAREvent()
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpDollAREvent::construct(ItpEventData *eventData)
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpDollAREvent::eventType()
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

int32_t TpDollAREvent::dollarType()
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;
    int32_t type = TP_INVALIDATE_VALUE;

    if (set)
    {
        type = set->dollarType;
    }

    return type;
}

int32_t TpDollAREvent::timestamp()
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;
    int32_t timestamp = 0;

    if (set)
    {
        timestamp = set->timestamp;
    }

    return timestamp;
}

long long TpDollAREvent::touchID()
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;
    long long touchID = TP_INVALIDATE_VALUE;

    if (set)
    {
        touchID = set->touchID;
    }

    return touchID;
}

long long TpDollAREvent::GestureID()
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;
    long long GestureID = TP_INVALIDATE_VALUE;

    if (set)
    {
        GestureID = set->GestureID;
    }

    return GestureID;
}

int32_t TpDollAREvent::numFingers()
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;
    int32_t numFingers = 0;

    if (set)
    {
        numFingers = set->numFingers;
    }

    return numFingers;
}

int32_t TpDollAREvent::X()
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;
    int32_t x = TP_INVALIDATE_VALUE;

    if (set)
    {
        x = set->x;
    }

    return x;
}

int32_t TpDollAREvent::Y()
{
    ItpDollarSet *set = (ItpDollarSet *)TpEvent::TpEventSet;
    int32_t y = TP_INVALIDATE_VALUE;

    if (set)
    {
        y = set->y;
    }

    return y;
}
//--------------------------TpMultiGestureEvent------------------------------/
TpMultiGestureEvent::TpMultiGestureEvent()
{
    ItpMultiGestureSet *set = new ItpMultiGestureSet();

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpMultiGestureEvent::~TpMultiGestureEvent()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpMultiGestureEvent::construct(ItpEventData *eventData)
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpMultiGestureEvent::eventType()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

int32_t TpMultiGestureEvent::timestamp()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;
    int32_t timestamp = 0;

    if (set)
    {
        timestamp = set->timestamp;
    }

    return timestamp;
}

long long TpMultiGestureEvent::touchID()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;
    long long touchID = TP_INVALIDATE_VALUE;

    if (set)
    {
        touchID = set->touchID;
    }

    return touchID;
}

float TpMultiGestureEvent::dtheta()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;
    float dtheta = 0.0;

    if (set)
    {
        dtheta = set->dtheta;
    }

    return dtheta;
}

float TpMultiGestureEvent::ddist()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;
    float ddist = 0.0;

    if (set)
    {
        ddist = set->ddist;
    }

    return ddist;
}

int32_t TpMultiGestureEvent::X()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;
    int32_t x = TP_INVALIDATE_VALUE;

    if (set)
    {
        x = set->x;
    }

    return x;
}

int32_t TpMultiGestureEvent::Y()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;
    int32_t y = TP_INVALIDATE_VALUE;

    if (set)
    {
        y = set->y;
    }

    return y;
}

uint16_t TpMultiGestureEvent::numfingers()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;
    uint16_t numfingers = 0;

    if (set)
    {
        numfingers = set->numfingers;
    }

    return numfingers;
}

uint16_t TpMultiGestureEvent::padding()
{
    ItpMultiGestureSet *set = (ItpMultiGestureSet *)TpEvent::TpEventSet;
    uint16_t padding = 0;

    if (set)
    {
        padding = set->padding;
    }

    return padding;
}

//--------------------------TpMoveEvent------------------------------/
TpMoveEvent::TpMoveEvent() : TpEvent()
{
    ItpObjectMoveSet *set = new ItpObjectMoveSet();

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpMoveEvent::~TpMoveEvent()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpMoveEvent::construct(ItpEventData *eventData)
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpMoveEvent::eventType()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpMoveEvent::object()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

int32_t TpMoveEvent::newX()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;
    int32_t newX = TP_INVALIDATE_VALUE;

    if (set)
    {
        newX = set->nx;
    }

    return newX;
}

int32_t TpMoveEvent::newY()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;
    int32_t newY = TP_INVALIDATE_VALUE;

    if (set)
    {
        newY = set->ny;
    }

    return newY;
}

//--------------------------TpResizeEvent------------------------------/
TpResizeEvent::TpResizeEvent() : TpEvent()
{
    ItpObjectResizeSet *set = new ItpObjectResizeSet(); // EVENT_OBJECT_RESIZE_TYPE

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpResizeEvent::~TpResizeEvent()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpResizeEvent::construct(ItpEventData *eventData)
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpResizeEvent::eventType()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

int32_t TpResizeEvent::question()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    int32_t question = TP_UNKOWN_CHANGE;

    if (set)
    {
        question = set->question;
    }

    return question;
}

TpObject *TpResizeEvent::object()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

int32_t TpResizeEvent::nWidth()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    int32_t newWidth = TP_INVALIDATE_VALUE;

    if (set)
    {
        newWidth = set->nw;
    }

    return newWidth;
}

int32_t TpResizeEvent::nHeight()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    int32_t newHeight = TP_INVALIDATE_VALUE;

    if (set)
    {
        newHeight = set->nw;
    }

    return newHeight;
}

//--------------------------TpFocusEvent------------------------------/
TpFocusEvent::TpFocusEvent() : TpEvent()
{
    ItpObjectFocusSet *set = new ItpObjectFocusSet(); // EVENT_OBJECT_FOCUS_TYPE

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpFocusEvent::~TpFocusEvent()
{
    ItpObjectFocusSet *set = (ItpObjectFocusSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpFocusEvent::construct(ItpEventData *eventData)
{
    ItpObjectFocusSet *set = (ItpObjectFocusSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpFocusEvent::eventType()
{
    ItpObjectFocusSet *set = (ItpObjectFocusSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpFocusEvent::object()
{
    ItpObjectFocusSet *set = (ItpObjectFocusSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

bool TpFocusEvent::focused()
{
    ItpObjectFocusSet *set = (ItpObjectFocusSet *)TpEvent::TpEventSet;
    bool focused = false;

    if (set)
    {
        focused = set->focused;
    }

    return focused;
}

//--------------------------TpLeaveEvent------------------------------/
TpLeaveEvent::TpLeaveEvent() : TpEvent()
{
    ItpObjectLeaveSet *set = new ItpObjectLeaveSet(); // EVENT_OBJECT_LEAVE_TYPE

    if (set)
    {
        set->type = TpEvent::EVENT_OBJECT_LEAVE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpLeaveEvent::~TpLeaveEvent()
{
    ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpLeaveEvent::construct(ItpEventData *eventData)
{
    ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpLeaveEvent::eventType()
{
    ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpLeaveEvent::object()
{
    ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

bool TpLeaveEvent::leave()
{
    ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)TpEvent::TpEventSet;
    bool leaved = false;

    if (set)
    {
        leaved = set->leaved;
    }

    return leaved;
}

//--------------------------TpVisibleEvent------------------------------/
TpVisibleEvent::TpVisibleEvent() : TpEvent()
{
    ItpObjectVisibleSet *set = new ItpObjectVisibleSet(); // EVENT_OBJECT_VISIBLE_TYPE

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpVisibleEvent::~TpVisibleEvent()
{
    ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpVisibleEvent::construct(ItpEventData *eventData)
{
    ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpVisibleEvent::eventType()
{
    ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpVisibleEvent::object()
{
    ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

bool TpVisibleEvent::visible()
{
    ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)TpEvent::TpEventSet;
    bool visible = false;

    if (set)
    {
        visible = set->visible;
    }

    return visible;
}

//--------------------------TpRotateEvent------------------------------/
TpRotateEvent::TpRotateEvent() : TpEvent()
{
    ItpObjectRotateSet *set = new ItpObjectRotateSet(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpRotateEvent::~TpRotateEvent()
{
    ItpObjectRotateSet *set = (ItpObjectRotateSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpRotateEvent::construct(ItpEventData *eventData)
{
    ItpObjectRotateSet *set = (ItpObjectRotateSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpRotateEvent::eventType()
{
    ItpObjectRotateSet *set = (ItpObjectRotateSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpRotateEvent::object()
{
    ItpObjectRotateSet *set = (ItpObjectRotateSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

ItpRotateType TpRotateEvent::rotate()
{
    ItpObjectRotateSet *set = (ItpObjectRotateSet *)TpEvent::TpEventSet;
    ItpRotateType rotate = TP_ROT_0;

    if (set)
    {
        rotate = set->rotate;
    }

    return rotate;
}

//--------------------------TpPaintEvent------------------------------/
TpPaintEvent::TpPaintEvent() : TpEvent()
{
    ItpObjectPaintSet *set = new ItpObjectPaintSet(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        set->canDraw = false;
        TpEvent::TpEventSet = set;
    }
}

TpPaintEvent::~TpPaintEvent()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;

    if (set)
    {
        if (set->canvas)
        {
            delete set->canvas;
        }

        delete set;
    }
}

bool TpPaintEvent::construct(ItpEventData *eventData)
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;

    if (!set)
        return false;

    set->canDraw = false;

    ItpObjectPaintInput *input = (ItpObjectPaintInput *)eventData;
    if (!input)
        return false;

    TpChildWidget *inputObjectChild = static_cast<TpChildWidget *>(input->object);
    if (!inputObjectChild)
        return false;

    Tp::ItpObjectType type = inputObjectChild->objectType();
    set->object = inputObjectChild;

    if (type == Tp::TP_FLOAT_OBJECT || type == Tp::TP_TOP_OBJECT)
    {
        set->offsetX = 0;
        set->offsetY = 0;
    }
    else
    {
        set->offsetX = inputObjectChild->toScreen().x() - inputObjectChild->offsetX();
        set->offsetY = inputObjectChild->toScreen().y() - inputObjectChild->offsetY();
    }

    set->surface = input->surface;
    set->canvas = new TpPainter(set->surface, set->offsetX, set->offsetY, inputObjectChild->width(), inputObjectChild->height());

    if (set->canvas == nullptr)
    {
        set->surface = nullptr;
        return false;
    }

    TpRect clipRect = input->updateRect;
    TpRect objectAbsRect = inputObjectChild->toScreen();
    TpRect absRect(objectAbsRect);

    set->canDraw = clipRect.intersect(absRect);

    if (set->canDraw == false)
    {
        return false;
    }

    set->updateRect = clipRect;
    TpObject *top = input->object->topObject();

    if (top && top->objectType() == Tp::TP_FLOAT_OBJECT)
    {
        clipRect.setX(clipRect.x() - inputObjectChild->offsetX());
        clipRect.setY(clipRect.y() - inputObjectChild->offsetY());
    }

    // std::cout << "clipRect 区域： " << input->object << " : " << clipRect.get().x << " , " << clipRect.get().y << " , "
    //           << clipRect.get().w << " , " << clipRect.get().h << std::endl;

    set->rect = inputObjectChild->rect();
    set->canvas->setClipRect(clipRect);
    set->type = EVENT_OBJECT_PAINT_TYPE;

    return true;
}

TpEvent::ItpEventType TpPaintEvent::eventType()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpPaintEvent::object()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

TpPainter *TpPaintEvent::painter()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    TpPainter *canvas = nullptr;

    if (set)
    {
        canvas = set->canvas;
    }

    return canvas;
}

tpShared<TpSurface> TpPaintEvent::surface()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;

    if (set)
    {
        return set->surface;
    }

    return nullptr;
}

ItpSufaceData *TpPaintEvent::itpSurface()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    ItpSufaceData *itpSurface = nullptr;

    if (set)
    {
        itpSurface = set->itpSurface;
    }

    return itpSurface;
}

int32_t TpPaintEvent::offsetX()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    int32_t offsetX = 0;

    if (set)
    {
        offsetX = set->offsetX;
    }

    return offsetX;
}

int32_t TpPaintEvent::offsetY()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    int32_t offsetY = 0;

    if (set)
    {
        offsetY = set->offsetY;
    }

    return offsetY;
}

TpRect TpPaintEvent::updateRect()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    TpRect result = {0, 0, 0, 0};

    if (set)
    {
        result = set->updateRect;
    }

    return result;
}

TpRect TpPaintEvent::rect()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    TpRect result = {0, 0, 0, 0};

    if (set)
    {
        result = set->rect;
    }

    return result;
}

TpRect TpPaintEvent::absRect()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    TpRect result;

    TpChildWidget *chiildObject = static_cast<TpChildWidget *>(set->object);

    if (!chiildObject)
        return result;

    result = chiildObject->toScreen();

    return result;
}

bool TpPaintEvent::isCanDraw()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    bool canDraw = false;

    if (set)
    {
        canDraw = set->canDraw;
    }

    return canDraw;
}

//--------------------------TpObjectActivedEvent------------------------------/
TpActiveEvent::TpActiveEvent() : TpEvent()
{
    ItpObjectActiveSet *set = new ItpObjectActiveSet(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpActiveEvent::~TpActiveEvent()
{
    ItpObjectActiveSet *set = (ItpObjectActiveSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpActiveEvent::construct(ItpEventData *eventData)
{
    ItpObjectActiveSet *set = (ItpObjectActiveSet *)TpEvent::TpEventSet;

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

TpEvent::ItpEventType TpActiveEvent::eventType()
{
    ItpObjectActiveSet *set = (ItpObjectActiveSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpActiveEvent::object()
{
    ItpObjectActiveSet *set = (ItpObjectActiveSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

bool TpActiveEvent::isActived()
{
    ItpObjectActiveSet *set = (ItpObjectActiveSet *)TpEvent::TpEventSet;
    bool actived = false;

    if (set)
    {
        actived = set->actived;
    }

    return actived;
}

TpThemeChangeEvent::TpThemeChangeEvent()
{
}

TpThemeChangeEvent::~TpThemeChangeEvent()
{
}

bool TpThemeChangeEvent::construct(ItpEventData *eventData)
{
    return true;
}

TpEvent::ItpEventType TpThemeChangeEvent::eventType()
{
    return EVENT_THEME_CHANGE_TYPE;
}
