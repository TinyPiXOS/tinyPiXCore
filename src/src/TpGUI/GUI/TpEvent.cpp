#include <TpEvent.h>
#include <TpSurface.h>
#include <TpPainter.h>
#include <TpRect.h>
#include <TpWidget.h>
#include <TpDef.h>
#include <TpPoint.h>
#include <TpString.h>

//--------------------------TpKeyboardEvent------------------------------/
TpKeyboardEvent::TpKeyboardEvent(TpEvent::TpEventType type) : TpEvent()
{
    ITpKeyboardSet *set = new ITpKeyboardSet();
    TpEvent::eventData_ = set;
    TpEvent::eventType_ = type;
}

TpKeyboardEvent::~TpKeyboardEvent()
{
    ITpKeyboardSet *set = (ITpKeyboardSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
        set = nullptr;
        TpEvent::eventData_ = nullptr;
    }
}

bool TpKeyboardEvent::construct(ITpEventData *eventData)
{
    ITpKeyboardSet *set = (ITpKeyboardSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpKeyboardSet *pEventData = (ITpKeyboardSet *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

uint8_t TpKeyboardEvent::which()
{
    ITpKeyboardSet *set = (ITpKeyboardSet *)TpEvent::eventData_;
    uint8_t which = TP_INVALIDATE_VALUE;

    if (set)
    {
        which = set->which;
    }

    return which;
}

bool TpKeyboardEvent::state()
{
    ITpKeyboardSet *set = (ITpKeyboardSet *)TpEvent::eventData_;
    bool state = false;

    if (set)
    {
        state = set->state;
    }

    return state;
}

int32_t TpKeyboardEvent::scancode()
{
    ITpKeyboardSet *set = (ITpKeyboardSet *)TpEvent::eventData_;
    int32_t scancode = TP_INVALIDATE_VALUE;

    if (set)
    {
        scancode = set->scancode;
    }

    return scancode;
}

int32_t TpKeyboardEvent::virtualKey()
{
    ITpKeyboardSet *set = (ITpKeyboardSet *)TpEvent::eventData_;
    int32_t virtualKey = TP_INVALIDATE_VALUE;

    if (set)
    {
        virtualKey = set->virtualKey;
    }

    return virtualKey;
}

int32_t TpKeyboardEvent::symbol()
{
    ITpKeyboardSet *set = (ITpKeyboardSet *)TpEvent::eventData_;
    int32_t symbol = TP_INVALIDATE_VALUE;

    if (set)
    {
        symbol = set->symbol;
    }

    return symbol;
}

const char *TpKeyboardEvent::shortCut()
{
    ITpKeyboardSet *set = (ITpKeyboardSet *)TpEvent::eventData_;
    const char *pShortCut = nullptr;

    if (set)
    {
        pShortCut = set->shortCut;
    }

    return pShortCut;
}

KeyModeType TpKeyboardEvent::keyMod()
{
    ITpKeyboardSet *set = (ITpKeyboardSet *)TpEvent::eventData_;
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
TpMouseEvent::TpMouseEvent(TpEvent::TpEventType type) : TpEvent()
{
    ITpMouseSet *set = new ITpMouseSet();

    TpEvent::eventType_ = type;
    TpEvent::eventData_ = set;
}

TpMouseEvent::~TpMouseEvent()
{
    ITpMouseSet *set = (ITpMouseSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
        set = nullptr;
        TpEvent::eventData_ = nullptr;
    }
}

bool TpMouseEvent::construct(ITpEventData *eventData)
{
    ITpMouseSet *set = (ITpMouseSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpMouseSet *pEventData = (ITpMouseSet *)eventData;

    if (!pEventData)
        return false;

    *set = *pEventData;

    return true;
}

int32_t TpMouseEvent::which()
{
    ITpMouseSet *set = (ITpMouseSet *)TpEvent::eventData_;
    int32_t which = TP_INVALIDATE_VALUE;

    if (set)
    {
        which = set->which;
    }

    return which;
}

MouseEventType TpMouseEvent::button()
{
    ITpMouseSet *set = (ITpMouseSet *)TpEvent::eventData_;
    MouseEventType button = BUTTON_INVALIDATE_VALUE;
    if (set)
    {
        button = set->button;
    }

    return button;
}

bool TpMouseEvent::state()
{
    ITpMouseSet *set = (ITpMouseSet *)TpEvent::eventData_;
    bool state = false;

    if (set)
    {
        state = set->state;
    }

    return state;
}

TpPoint TpMouseEvent::pos()
{
    ITpMouseSet *set = (ITpMouseSet *)TpEvent::eventData_;
    if (!set)
        return TpPoint();

    return set->pos;
}

TpPoint TpMouseEvent::globalPos()
{
    ITpMouseSet *set = (ITpMouseSet *)TpEvent::eventData_;
    if (!set)
        return TpPoint();

    return set->globalPos;
}

//--------------------------TpWheelEvent------------------------------/
TpWheelEvent::TpWheelEvent() : TpEvent()
{
    ITpMouseSet *set = new ITpMouseSet();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_WHEEL_EVENT;
        TpEvent::eventData_ = set;
    }
}

TpWheelEvent::~TpWheelEvent()
{
    ITpMouseSet *set = (ITpMouseSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
        set = nullptr;
        TpEvent::eventData_ = nullptr;
    }
}

bool TpWheelEvent::construct(ITpEventData *eventData)
{
    ITpMouseSet *set = (ITpMouseSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpMouseSet *pEventData = (ITpMouseSet *)eventData;

    if (!pEventData)
        return false;

    *set = *pEventData;

    if (pEventData->button == BUTTON_WHEELUP)
        angleDelta_ = 10;
    else
        angleDelta_ = -10;

    return true;
}

//--------------------------TpFingerEvent------------------------------/
TpFingerEvent::TpFingerEvent() : TpEvent()
{
    ITpFingerSet *set = new ITpFingerSet();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_FINGER_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpFingerEvent::~TpFingerEvent()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpFingerEvent::construct(ITpEventData *eventData)
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpFingerSet *pEventData = (ITpFingerSet *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpFingerEvent::touchFingerType()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;
    int32_t type = TOUCH_FINGER_NONE;

    if (set)
    {
        type = set->touchFingerType;
    }

    return type;
}

int32_t TpFingerEvent::timestamp()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;
    int32_t timestamp = 0;

    if (set)
    {
        timestamp = set->timestamp;
    }

    return timestamp;
}

long long TpFingerEvent::fingerID()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;
    long long fingerId = TP_INVALIDATE_VALUE;

    if (set)
    {
        fingerId = set->fingerID;
    }

    return fingerId;
}

long long TpFingerEvent::touchID()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;
    long long touchID = TP_INVALIDATE_VALUE;

    if (set)
    {
        touchID = set->touchID;
    }

    return touchID;
}

int32_t TpFingerEvent::X()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;
    int32_t x = TP_INVALIDATE_VALUE;

    if (set)
    {
        x = set->x;
    }

    return x;
}

int32_t TpFingerEvent::Y()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;
    int32_t y = TP_INVALIDATE_VALUE;

    if (set)
    {
        y = set->y;
    }

    return y;
}

int32_t TpFingerEvent::dx()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;
    int32_t dx = TP_INVALIDATE_VALUE;

    if (set)
    {
        dx = set->dx;
    }

    return dx;
}

int32_t TpFingerEvent::dy()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;
    int32_t dy = TP_INVALIDATE_VALUE;

    if (set)
    {
        dy = set->dy;
    }

    return dy;
}

float TpFingerEvent::pressure()
{
    ITpFingerSet *set = (ITpFingerSet *)TpEvent::eventData_;
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
    ITpDollarSet *set = new ITpDollarSet();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_DOLLAR_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpDollAREvent::~TpDollAREvent()
{
    ITpDollarSet *set = (ITpDollarSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpDollAREvent::construct(ITpEventData *eventData)
{
    ITpDollarSet *set = (ITpDollarSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpDollarSet *pEventData = (ITpDollarSet *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpDollAREvent::dollarType()
{
    ITpDollarSet *set = (ITpDollarSet *)TpEvent::eventData_;
    int32_t type = TP_INVALIDATE_VALUE;

    if (set)
    {
        type = set->dollarType;
    }

    return type;
}

int32_t TpDollAREvent::timestamp()
{
    ITpDollarSet *set = (ITpDollarSet *)TpEvent::eventData_;
    int32_t timestamp = 0;

    if (set)
    {
        timestamp = set->timestamp;
    }

    return timestamp;
}

long long TpDollAREvent::touchID()
{
    ITpDollarSet *set = (ITpDollarSet *)TpEvent::eventData_;
    long long touchID = TP_INVALIDATE_VALUE;

    if (set)
    {
        touchID = set->touchID;
    }

    return touchID;
}

long long TpDollAREvent::GestureID()
{
    ITpDollarSet *set = (ITpDollarSet *)TpEvent::eventData_;
    long long GestureID = TP_INVALIDATE_VALUE;

    if (set)
    {
        GestureID = set->GestureID;
    }

    return GestureID;
}

int32_t TpDollAREvent::numFingers()
{
    ITpDollarSet *set = (ITpDollarSet *)TpEvent::eventData_;
    int32_t numFingers = 0;

    if (set)
    {
        numFingers = set->numFingers;
    }

    return numFingers;
}

int32_t TpDollAREvent::X()
{
    ITpDollarSet *set = (ITpDollarSet *)TpEvent::eventData_;
    int32_t x = TP_INVALIDATE_VALUE;

    if (set)
    {
        x = set->x;
    }

    return x;
}

int32_t TpDollAREvent::Y()
{
    ITpDollarSet *set = (ITpDollarSet *)TpEvent::eventData_;
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
    ITpMultiGestureSet *set = new ITpMultiGestureSet();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_DOLLAR_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpMultiGestureEvent::~TpMultiGestureEvent()
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpMultiGestureEvent::construct(ITpEventData *eventData)
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpMultiGestureSet *pEventData = (ITpMultiGestureSet *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpMultiGestureEvent::timestamp()
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;
    int32_t timestamp = 0;

    if (set)
    {
        timestamp = set->timestamp;
    }

    return timestamp;
}

long long TpMultiGestureEvent::touchID()
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;
    long long touchID = TP_INVALIDATE_VALUE;

    if (set)
    {
        touchID = set->touchID;
    }

    return touchID;
}

float TpMultiGestureEvent::dtheta()
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;
    float dtheta = 0.0;

    if (set)
    {
        dtheta = set->dtheta;
    }

    return dtheta;
}

float TpMultiGestureEvent::ddist()
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;
    float ddist = 0.0;

    if (set)
    {
        ddist = set->ddist;
    }

    return ddist;
}

int32_t TpMultiGestureEvent::X()
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;
    int32_t x = TP_INVALIDATE_VALUE;

    if (set)
    {
        x = set->x;
    }

    return x;
}

int32_t TpMultiGestureEvent::Y()
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;
    int32_t y = TP_INVALIDATE_VALUE;

    if (set)
    {
        y = set->y;
    }

    return y;
}

uint16_t TpMultiGestureEvent::numfingers()
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;
    uint16_t numfingers = 0;

    if (set)
    {
        numfingers = set->numfingers;
    }

    return numfingers;
}

uint16_t TpMultiGestureEvent::padding()
{
    ITpMultiGestureSet *set = (ITpMultiGestureSet *)TpEvent::eventData_;
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
    ITpObjectMoveSet *set = new ITpObjectMoveSet();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_OBJECT_MOVE_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpMoveEvent::~TpMoveEvent()
{
    ITpObjectMoveSet *set = (ITpObjectMoveSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpMoveEvent::construct(ITpEventData *eventData)
{
    ITpObjectMoveSet *set = (ITpObjectMoveSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpObjectMoveSet *pEventData = (ITpObjectMoveSet *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpMoveEvent::newX()
{
    ITpObjectMoveSet *set = (ITpObjectMoveSet *)TpEvent::eventData_;
    int32_t newX = TP_INVALIDATE_VALUE;

    if (set)
    {
        newX = set->nx;
    }

    return newX;
}

int32_t TpMoveEvent::newY()
{
    ITpObjectMoveSet *set = (ITpObjectMoveSet *)TpEvent::eventData_;
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
    ITpObjectResizeSet *set = new ITpObjectResizeSet(); // EVENT_OBJECT_RESIZE_TYPE
    TpEvent::eventType_ = TpEvent::EVENT_OBJECT_RESIZE_TYPE;
    TpEvent::eventData_ = set;
}

TpResizeEvent::~TpResizeEvent()
{
    ITpObjectResizeSet *set = (ITpObjectResizeSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpResizeEvent::construct(ITpEventData *eventData)
{
    ITpObjectResizeSet *set = (ITpObjectResizeSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpObjectResizeSet *pEventData = (ITpObjectResizeSet *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpResizeEvent::question()
{
    ITpObjectResizeSet *set = (ITpObjectResizeSet *)TpEvent::eventData_;
    int32_t question = TP_UNKOWN_CHANGE;

    if (set)
    {
        question = set->question;
    }

    return question;
}

int32_t TpResizeEvent::nWidth()
{
    ITpObjectResizeSet *set = (ITpObjectResizeSet *)TpEvent::eventData_;
    int32_t newWidth = TP_INVALIDATE_VALUE;

    if (set)
    {
        newWidth = set->nw;
    }

    return newWidth;
}

int32_t TpResizeEvent::nHeight()
{
    ITpObjectResizeSet *set = (ITpObjectResizeSet *)TpEvent::eventData_;
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
    ITpObjectFocusSet *set = new ITpObjectFocusSet(); // EVENT_OBJECT_FOCUS_TYPE

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_OBJECT_FOCUS_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpFocusEvent::~TpFocusEvent()
{
    ITpObjectFocusSet *set = (ITpObjectFocusSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpFocusEvent::construct(ITpEventData *eventData)
{
    ITpObjectFocusSet *set = (ITpObjectFocusSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpObjectFocusSet *pEventData = (ITpObjectFocusSet *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

bool TpFocusEvent::focused()
{
    ITpObjectFocusSet *set = (ITpObjectFocusSet *)TpEvent::eventData_;
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
    ITpObjectLeaveSet *set = new ITpObjectLeaveSet(); // EVENT_OBJECT_LEAVE_TYPE
    TpEvent::eventType_ = TpEvent::EVENT_OBJECT_LEAVE_TYPE;
    TpEvent::eventData_ = set;
}

TpLeaveEvent::~TpLeaveEvent()
{
    ITpObjectLeaveSet *set = (ITpObjectLeaveSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpLeaveEvent::construct(ITpEventData *eventData)
{
    ITpObjectLeaveSet *set = (ITpObjectLeaveSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpObjectLeaveSet *pEventData = (ITpObjectLeaveSet *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

bool TpLeaveEvent::leave()
{
    ITpObjectLeaveSet *set = (ITpObjectLeaveSet *)TpEvent::eventData_;
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
    ITpObjectVisibleSet *set = new ITpObjectVisibleSet(); // EVENT_OBJECT_VISIBLE_TYPE
    TpEvent::eventType_ = TpEvent::EVENT_OBJECT_VISIBLE_TYPE;
    TpEvent::eventData_ = set;
}

TpVisibleEvent::~TpVisibleEvent()
{
    ITpObjectVisibleSet *set = (ITpObjectVisibleSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpVisibleEvent::construct(ITpEventData *eventData)
{
    ITpObjectVisibleSet *set = (ITpObjectVisibleSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpObjectVisibleSet *pEventData = (ITpObjectVisibleSet *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

bool TpVisibleEvent::visible()
{
    ITpObjectVisibleSet *set = (ITpObjectVisibleSet *)TpEvent::eventData_;
    bool visible = false;

    if (set)
    {
        visible = set->visible;
    }

    return visible;
}

//--------------------------TpPaintEvent------------------------------/
TpPaintEvent::TpPaintEvent() : TpEvent()
{
    ITpObjectPaintSet *set = new ITpObjectPaintSet(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        set->canDraw = false;
        TpEvent::eventType_ = TpEvent::EVENT_OBJECT_PAINT_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpPaintEvent::~TpPaintEvent()
{
    ITpObjectPaintSet *set = (ITpObjectPaintSet *)TpEvent::eventData_;

    if (set)
    {
        if (set->painter)
        {
            delete set->painter;
        }

        delete set;
    }
}

TpPainter *TpPaintEvent::painter()
{
    ITpObjectPaintSet *set = (ITpObjectPaintSet *)TpEvent::eventData_;
    return set->painter;
}

TpRect TpPaintEvent::updateRect()
{
    ITpObjectPaintSet *set = (ITpObjectPaintSet *)TpEvent::eventData_;
    return set->updateRect;
}

bool TpPaintEvent::isCanDraw()
{
    ITpObjectPaintSet *set = (ITpObjectPaintSet *)TpEvent::eventData_;
    return set->canDraw;
}

bool TpPaintEvent::construct(ITpEventData *inputData)
{
    ITpObjectPaintSet *eventData = static_cast<ITpObjectPaintSet *>(TpEvent::eventData_);
    if (!eventData)
        return false;
    eventData->canDraw = false;

    ITpObjectPaintInput *input = static_cast<ITpObjectPaintInput *>(inputData);
    if (!input)
        return false;

    TpWidget *inputObj = static_cast<TpWidget *>(input->object);
    if (!inputObj)
        return false;

    eventData->object = inputObj;

    eventData->painter = new TpPainter(inputObj);
    if (eventData->painter == nullptr)
        return false;

    // 限制绘制区域;如果父窗口比自己大，则使用自己的尺寸，如果父窗口比自己小，则使用父窗口的
    TpWidget *inputParentWidget = dynamic_cast<TpWidget *>(inputObj->parent());

    eventData->canDraw = true;
    if (inputParentWidget)
    {
        eventData->canDraw = (inputObj->width() > 0) && (inputObj->height() > 0);
        if (!eventData->canDraw)
            return false;
    }

    eventData->updateRect = input->updateRect;

    return true;
}

//--------------------------TpObjectActivedEvent------------------------------/
TpActiveEvent::TpActiveEvent() : TpEvent()
{
    ITpObjectActiveSet *set = new ITpObjectActiveSet(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_OBJECT_ACTIVE_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpActiveEvent::~TpActiveEvent()
{
    ITpObjectActiveSet *set = (ITpObjectActiveSet *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpActiveEvent::construct(ITpEventData *eventData)
{
    ITpObjectActiveSet *set = (ITpObjectActiveSet *)TpEvent::eventData_;

    if (!set)
        return false;

    ITpObjectActiveSet *pEventData = (ITpObjectActiveSet *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

bool TpActiveEvent::isActived()
{
    ITpObjectActiveSet *set = (ITpObjectActiveSet *)TpEvent::eventData_;
    bool actived = false;

    if (set)
    {
        actived = set->actived;
    }

    return actived;
}

TpThemeChangeEvent::TpThemeChangeEvent()
{
    TpEvent::eventType_ = TpEvent::EVENT_THEME_CHANGE_TYPE;
}

TpThemeChangeEvent::~TpThemeChangeEvent()
{
}

bool TpThemeChangeEvent::construct(ITpEventData *eventData)
{
    return true;
}
