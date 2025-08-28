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
#include "TpCanvas.h"
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
        memset(set, 0, sizeof(ItpKeyboardSet));
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
        memset(set, 0, sizeof(ItpMouseSet));
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

ItpPoint TpMouseEvent::pos()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;
    if (!set)
        return ItpPoint();

    return set->pos;
}

ItpPoint TpMouseEvent::globalPos()
{
    ItpMouseSet *set = (ItpMouseSet *)TpEvent::TpEventSet;
    if (!set)
        return ItpPoint();

    return set->globalPos;
}

//--------------------------TpWheelEvent------------------------------/
TpWheelEvent::TpWheelEvent() : TpEvent()
{
    ItpMouseSet *set = new ItpMouseSet();

    if (set)
    {
        memset(set, 0, sizeof(ItpMouseSet));
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
        memset(set, 0, sizeof(ItpFingerSet));
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
        memset(set, 0, sizeof(ItpDollarSet));
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
        memset(set, 0, sizeof(ItpMultiGestureSet));
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

//--------------------------TpObjectMoveEvent------------------------------/
TpObjectMoveEvent::TpObjectMoveEvent() : TpEvent()
{
    ItpObjectMoveSet *set = new ItpObjectMoveSet();

    if (set)
    {
        memset(set, 0, sizeof(ItpObjectMoveSet));
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpObjectMoveEvent::~TpObjectMoveEvent()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpObjectMoveEvent::construct(ItpEventData *eventData)
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

TpEvent::ItpEventType TpObjectMoveEvent::eventType()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpObjectMoveEvent::object()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

int32_t TpObjectMoveEvent::newX()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;
    int32_t newX = TP_INVALIDATE_VALUE;

    if (set)
    {
        newX = set->nx;
    }

    return newX;
}

int32_t TpObjectMoveEvent::newY()
{
    ItpObjectMoveSet *set = (ItpObjectMoveSet *)TpEvent::TpEventSet;
    int32_t newY = TP_INVALIDATE_VALUE;

    if (set)
    {
        newY = set->ny;
    }

    return newY;
}

//--------------------------TpObjectResizeEvent------------------------------/
TpObjectResizeEvent::TpObjectResizeEvent() : TpEvent()
{
    ItpObjectResizeSet *set = new ItpObjectResizeSet(); // EVENT_OBJECT_RESIZE_TYPE

    if (set)
    {
        memset(set, 0, sizeof(ItpObjectResizeSet));
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpObjectResizeEvent::~TpObjectResizeEvent()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpObjectResizeEvent::construct(ItpEventData *eventData)
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

TpEvent::ItpEventType TpObjectResizeEvent::eventType()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

int32_t TpObjectResizeEvent::question()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    int32_t question = TP_UNKOWN_CHANGE;

    if (set)
    {
        question = set->question;
    }

    return question;
}

TpObject *TpObjectResizeEvent::object()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

int32_t TpObjectResizeEvent::nWidth()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    int32_t newWidth = TP_INVALIDATE_VALUE;

    if (set)
    {
        newWidth = set->nw;
    }

    return newWidth;
}

int32_t TpObjectResizeEvent::nHeight()
{
    ItpObjectResizeSet *set = (ItpObjectResizeSet *)TpEvent::TpEventSet;
    int32_t newHeight = TP_INVALIDATE_VALUE;

    if (set)
    {
        newHeight = set->nw;
    }

    return newHeight;
}

//--------------------------TpObjectFocusEvent------------------------------/
TpObjectFocusEvent::TpObjectFocusEvent() : TpEvent()
{
    ItpObjectFocusSet *set = new ItpObjectFocusSet(); // EVENT_OBJECT_FOCUS_TYPE

    if (set)
    {
        memset(set, 0, sizeof(ItpObjectFocusSet));
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpObjectFocusEvent::~TpObjectFocusEvent()
{
    ItpObjectFocusSet *set = (ItpObjectFocusSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpObjectFocusEvent::construct(ItpEventData *eventData)
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

TpEvent::ItpEventType TpObjectFocusEvent::eventType()
{
    ItpObjectFocusSet *set = (ItpObjectFocusSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpObjectFocusEvent::object()
{
    ItpObjectFocusSet *set = (ItpObjectFocusSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

bool TpObjectFocusEvent::focused()
{
    ItpObjectFocusSet *set = (ItpObjectFocusSet *)TpEvent::TpEventSet;
    bool focused = false;

    if (set)
    {
        focused = set->focused;
    }

    return focused;
}

//--------------------------TpObjectLeaveEvent------------------------------/
TpObjectLeaveEvent::TpObjectLeaveEvent() : TpEvent()
{
    ItpObjectLeaveSet *set = new ItpObjectLeaveSet(); // EVENT_OBJECT_LEAVE_TYPE

    if (set)
    {
        memset(set, 0, sizeof(ItpObjectLeaveSet));
        set->type = TpEvent::EVENT_OBJECT_LEAVE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpObjectLeaveEvent::~TpObjectLeaveEvent()
{
    ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpObjectLeaveEvent::construct(ItpEventData *eventData)
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

TpEvent::ItpEventType TpObjectLeaveEvent::eventType()
{
    ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpObjectLeaveEvent::object()
{
    ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

bool TpObjectLeaveEvent::leave()
{
    ItpObjectLeaveSet *set = (ItpObjectLeaveSet *)TpEvent::TpEventSet;
    bool leaved = false;

    if (set)
    {
        leaved = set->leaved;
    }

    return leaved;
}

//--------------------------TpObjectVisibleEvent------------------------------/
TpObjectVisibleEvent::TpObjectVisibleEvent() : TpEvent()
{
    ItpObjectVisibleSet *set = new ItpObjectVisibleSet(); // EVENT_OBJECT_VISIBLE_TYPE

    if (set)
    {
        memset(set, 0, sizeof(ItpObjectVisibleSet));
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpObjectVisibleEvent::~TpObjectVisibleEvent()
{
    ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpObjectVisibleEvent::construct(ItpEventData *eventData)
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

TpEvent::ItpEventType TpObjectVisibleEvent::eventType()
{
    ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpObjectVisibleEvent::object()
{
    ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

bool TpObjectVisibleEvent::visible()
{
    ItpObjectVisibleSet *set = (ItpObjectVisibleSet *)TpEvent::TpEventSet;
    bool visible = false;

    if (set)
    {
        visible = set->visible;
    }

    return visible;
}

//--------------------------TpObjectRotateEvent------------------------------/
TpObjectRotateEvent::TpObjectRotateEvent() : TpEvent()
{
    ItpObjectRotateSet *set = new ItpObjectRotateSet(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        memset(set, 0, sizeof(ItpObjectRotateSet));
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpObjectRotateEvent::~TpObjectRotateEvent()
{
    ItpObjectRotateSet *set = (ItpObjectRotateSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpObjectRotateEvent::construct(ItpEventData *eventData)
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

TpEvent::ItpEventType TpObjectRotateEvent::eventType()
{
    ItpObjectRotateSet *set = (ItpObjectRotateSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpObjectRotateEvent::object()
{
    ItpObjectRotateSet *set = (ItpObjectRotateSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

ItpRotateType TpObjectRotateEvent::rotate()
{
    ItpObjectRotateSet *set = (ItpObjectRotateSet *)TpEvent::TpEventSet;
    ItpRotateType rotate = TP_ROT_0;

    if (set)
    {
        rotate = set->rotate;
    }

    return rotate;
}

//--------------------------TpObjectPaintEvent------------------------------/
TpObjectPaintEvent::TpObjectPaintEvent() : TpEvent()
{
    ItpObjectPaintSet *set = new ItpObjectPaintSet(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        memset(set, 0, sizeof(ItpObjectPaintSet));
        set->type = TpEvent::EVENT_NONE_TYPE;
        set->canDraw = false;
        TpEvent::TpEventSet = set;
    }
}

TpObjectPaintEvent::~TpObjectPaintEvent()
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

bool TpObjectPaintEvent::construct(ItpEventData *eventData)
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

    ItpObjectType type = inputObjectChild->objectType();
    set->object = inputObjectChild;

    if (type == TP_FLOAT_OBJECT || type == TP_TOP_OBJECT)
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
    set->canvas = new TpCanvas(set->surface, set->offsetX, set->offsetY);

    if (set->canvas == nullptr)
    {
        set->surface = nullptr;
        return false;
    }

    TpRect clipRect = input->updateRect;
    ItpRect objectAbsRect = inputObjectChild->toScreen();
    TpRect absRect(objectAbsRect);

    set->canDraw = clipRect.intersect(absRect);

    if (set->canDraw == false)
    {
        return false;
    }

    set->updateRect = clipRect.get(); // input->updateRect;
    TpObject *top = input->object->topObject();

    if (top && top->objectType() == TP_FLOAT_OBJECT)
    {
        clipRect.setX0(clipRect.X0() - inputObjectChild->offsetX());
        clipRect.setY0(clipRect.Y0() - inputObjectChild->offsetY());
    }

    // std::cout << "clipRect 区域： " << input->object << " : " << clipRect.get().x << " , " << clipRect.get().y << " , "
    //           << clipRect.get().w << " , " << clipRect.get().h << std::endl;

    set->rect = inputObjectChild->rect();
    set->canvas->setClipRect(clipRect.get());
    set->type = EVENT_OBJECT_PAINT_TYPE;

    return true;
}

TpEvent::ItpEventType TpObjectPaintEvent::eventType()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpObjectPaintEvent::object()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

TpCanvas *TpObjectPaintEvent::canvas()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    TpCanvas *canvas = nullptr;

    if (set)
    {
        canvas = set->canvas;
    }

    return canvas;
}

tpShared<TpSurface> TpObjectPaintEvent::surface()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;

    if (set)
    {
        return set->surface;
    }

    return nullptr;
}

ItpSufaceData *TpObjectPaintEvent::itpSurface()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    ItpSufaceData *itpSurface = nullptr;

    if (set)
    {
        itpSurface = set->itpSurface;
    }

    return itpSurface;
}

int32_t TpObjectPaintEvent::offsetX()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    int32_t offsetX = 0;

    if (set)
    {
        offsetX = set->offsetX;
    }

    return offsetX;
}

int32_t TpObjectPaintEvent::offsetY()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    int32_t offsetY = 0;

    if (set)
    {
        offsetY = set->offsetY;
    }

    return offsetY;
}

ItpRect TpObjectPaintEvent::updateRect()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    ItpRect result = {0, 0, 0, 0};

    if (set)
    {
        result = set->updateRect;
    }

    return result;
}

ItpRect TpObjectPaintEvent::rect()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    ItpRect result = {0, 0, 0, 0};

    if (set)
    {
        result = set->rect;
    }

    return result;
}

ItpRect TpObjectPaintEvent::absRect()
{
    ItpObjectPaintSet *set = (ItpObjectPaintSet *)TpEvent::TpEventSet;
    ItpRect result;

    TpChildWidget *chiildObject = static_cast<TpChildWidget *>(set->object);

    if (!chiildObject)
        return result;

    result = chiildObject->toScreen();

    return result;
}

bool TpObjectPaintEvent::isCanDraw()
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
TpObjectActiveEvent::TpObjectActiveEvent() : TpEvent()
{
    ItpObjectActiveSet *set = new ItpObjectActiveSet(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        memset(set, 0, sizeof(TpObjectActiveEvent));
        set->type = TpEvent::EVENT_NONE_TYPE;
        TpEvent::TpEventSet = set;
    }
}

TpObjectActiveEvent::~TpObjectActiveEvent()
{
    ItpObjectActiveSet *set = (ItpObjectActiveSet *)TpEvent::TpEventSet;

    if (set)
    {
        delete set;
    }
}

bool TpObjectActiveEvent::construct(ItpEventData *eventData)
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

TpEvent::ItpEventType TpObjectActiveEvent::eventType()
{
    ItpObjectActiveSet *set = (ItpObjectActiveSet *)TpEvent::TpEventSet;
    TpEvent::ItpEventType type = TpEvent::EVENT_NONE_TYPE;

    if (set)
    {
        type = set->type;
    }

    return type;
}

TpObject *TpObjectActiveEvent::object()
{
    ItpObjectActiveSet *set = (ItpObjectActiveSet *)TpEvent::TpEventSet;
    TpObject *object = nullptr;

    if (set)
    {
        object = set->object;
    }

    return object;
}

bool TpObjectActiveEvent::isActived()
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
