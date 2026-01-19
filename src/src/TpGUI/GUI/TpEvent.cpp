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
    TpKeyboardEventData *set = new TpKeyboardEventData();
    TpEvent::eventData_ = set;
    TpEvent::eventType_ = type;
}

TpKeyboardEvent::~TpKeyboardEvent()
{
    TpKeyboardEventData *set = (TpKeyboardEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
        set = nullptr;
        TpEvent::eventData_ = nullptr;
    }
}

bool TpKeyboardEvent::construct(ITpEventData *eventData)
{
    TpKeyboardEventData *set = (TpKeyboardEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpKeyboardEventData *pEventData = (TpKeyboardEventData *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

uint8_t TpKeyboardEvent::which()
{
    TpKeyboardEventData *set = (TpKeyboardEventData *)TpEvent::eventData_;
    uint8_t which = TP_INVALIDATE_VALUE;

    if (set)
    {
        which = set->which;
    }

    return which;
}

bool TpKeyboardEvent::state()
{
    TpKeyboardEventData *set = (TpKeyboardEventData *)TpEvent::eventData_;
    bool state = false;

    if (set)
    {
        state = set->state;
    }

    return state;
}

int32_t TpKeyboardEvent::scancode()
{
    TpKeyboardEventData *set = (TpKeyboardEventData *)TpEvent::eventData_;
    int32_t scancode = TP_INVALIDATE_VALUE;

    if (set)
    {
        scancode = set->scancode;
    }

    return scancode;
}

int32_t TpKeyboardEvent::virtualKey()
{
    TpKeyboardEventData *set = (TpKeyboardEventData *)TpEvent::eventData_;
    int32_t virtualKey = TP_INVALIDATE_VALUE;

    if (set)
    {
        virtualKey = set->virtualKey;
    }

    return virtualKey;
}

int32_t TpKeyboardEvent::symbol()
{
    TpKeyboardEventData *set = (TpKeyboardEventData *)TpEvent::eventData_;
    int32_t symbol = TP_INVALIDATE_VALUE;

    if (set)
    {
        symbol = set->symbol;
    }

    return symbol;
}

const char *TpKeyboardEvent::shortCut()
{
    TpKeyboardEventData *set = (TpKeyboardEventData *)TpEvent::eventData_;
    const char *pShortCut = nullptr;

    if (set)
    {
        pShortCut = set->shortCut;
    }

    return pShortCut;
}

KeyModeType TpKeyboardEvent::keyMod()
{
    TpKeyboardEventData *set = (TpKeyboardEventData *)TpEvent::eventData_;
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
    TpMouseEventData *set = new TpMouseEventData();

    TpEvent::eventType_ = type;
    TpEvent::eventData_ = set;
}

TpMouseEvent::~TpMouseEvent()
{
    TpMouseEventData *set = (TpMouseEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
        set = nullptr;
        TpEvent::eventData_ = nullptr;
    }
}

bool TpMouseEvent::construct(ITpEventData *eventData)
{
    TpMouseEventData *set = (TpMouseEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpMouseEventData *pEventData = (TpMouseEventData *)eventData;

    if (!pEventData)
        return false;

    *set = *pEventData;

    return true;
}

int32_t TpMouseEvent::which()
{
    TpMouseEventData *set = (TpMouseEventData *)TpEvent::eventData_;
    int32_t which = TP_INVALIDATE_VALUE;

    if (set)
    {
        which = set->which;
    }

    return which;
}

MouseEventType TpMouseEvent::button()
{
    TpMouseEventData *set = (TpMouseEventData *)TpEvent::eventData_;
    MouseEventType button = BUTTON_INVALIDATE_VALUE;
    if (set)
    {
        button = set->button;
    }

    return button;
}

bool TpMouseEvent::state()
{
    TpMouseEventData *set = (TpMouseEventData *)TpEvent::eventData_;
    bool state = false;

    if (set)
    {
        state = set->state;
    }

    return state;
}

TpPoint TpMouseEvent::pos()
{
    TpMouseEventData *set = (TpMouseEventData *)TpEvent::eventData_;
    if (!set)
        return TpPoint();

    return set->pos;
}

TpPoint TpMouseEvent::globalPos()
{
    TpMouseEventData *set = (TpMouseEventData *)TpEvent::eventData_;
    if (!set)
        return TpPoint();

    return set->globalPos;
}

//--------------------------TpWheelEvent------------------------------/
TpWheelEvent::TpWheelEvent() : TpEvent()
{
    TpMouseEventData *set = new TpMouseEventData();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_WHEEL_EVENT;
        TpEvent::eventData_ = set;
    }
}

TpWheelEvent::~TpWheelEvent()
{
    TpMouseEventData *set = (TpMouseEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
        set = nullptr;
        TpEvent::eventData_ = nullptr;
    }
}

bool TpWheelEvent::construct(ITpEventData *eventData)
{
    TpMouseEventData *set = (TpMouseEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpMouseEventData *pEventData = (TpMouseEventData *)eventData;

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
    TpFingerEventData *set = new TpFingerEventData();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_FINGER_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpFingerEvent::~TpFingerEvent()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpFingerEvent::construct(ITpEventData *eventData)
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpFingerEventData *pEventData = (TpFingerEventData *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpFingerEvent::touchFingerType()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;
    int32_t type = TOUCH_FINGER_NONE;

    if (set)
    {
        type = set->touchFingerType;
    }

    return type;
}

int32_t TpFingerEvent::timestamp()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;
    int32_t timestamp = 0;

    if (set)
    {
        timestamp = set->timestamp;
    }

    return timestamp;
}

long long TpFingerEvent::fingerID()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;
    long long fingerId = TP_INVALIDATE_VALUE;

    if (set)
    {
        fingerId = set->fingerID;
    }

    return fingerId;
}

long long TpFingerEvent::touchID()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;
    long long touchID = TP_INVALIDATE_VALUE;

    if (set)
    {
        touchID = set->touchID;
    }

    return touchID;
}

int32_t TpFingerEvent::X()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;
    int32_t x = TP_INVALIDATE_VALUE;

    if (set)
    {
        x = set->x;
    }

    return x;
}

int32_t TpFingerEvent::Y()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;
    int32_t y = TP_INVALIDATE_VALUE;

    if (set)
    {
        y = set->y;
    }

    return y;
}

int32_t TpFingerEvent::dx()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;
    int32_t dx = TP_INVALIDATE_VALUE;

    if (set)
    {
        dx = set->dx;
    }

    return dx;
}

int32_t TpFingerEvent::dy()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;
    int32_t dy = TP_INVALIDATE_VALUE;

    if (set)
    {
        dy = set->dy;
    }

    return dy;
}

float TpFingerEvent::pressure()
{
    TpFingerEventData *set = (TpFingerEventData *)TpEvent::eventData_;
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
    TpDollAREventData *set = new TpDollAREventData();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_DOLLAR_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpDollAREvent::~TpDollAREvent()
{
    TpDollAREventData *set = (TpDollAREventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpDollAREvent::construct(ITpEventData *eventData)
{
    TpDollAREventData *set = (TpDollAREventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpDollAREventData *pEventData = (TpDollAREventData *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpDollAREvent::dollarType()
{
    TpDollAREventData *set = (TpDollAREventData *)TpEvent::eventData_;
    int32_t type = TP_INVALIDATE_VALUE;

    if (set)
    {
        type = set->dollarType;
    }

    return type;
}

int32_t TpDollAREvent::timestamp()
{
    TpDollAREventData *set = (TpDollAREventData *)TpEvent::eventData_;
    int32_t timestamp = 0;

    if (set)
    {
        timestamp = set->timestamp;
    }

    return timestamp;
}

long long TpDollAREvent::touchID()
{
    TpDollAREventData *set = (TpDollAREventData *)TpEvent::eventData_;
    long long touchID = TP_INVALIDATE_VALUE;

    if (set)
    {
        touchID = set->touchID;
    }

    return touchID;
}

long long TpDollAREvent::GestureID()
{
    TpDollAREventData *set = (TpDollAREventData *)TpEvent::eventData_;
    long long GestureID = TP_INVALIDATE_VALUE;

    if (set)
    {
        GestureID = set->GestureID;
    }

    return GestureID;
}

int32_t TpDollAREvent::numFingers()
{
    TpDollAREventData *set = (TpDollAREventData *)TpEvent::eventData_;
    int32_t numFingers = 0;

    if (set)
    {
        numFingers = set->numFingers;
    }

    return numFingers;
}

int32_t TpDollAREvent::X()
{
    TpDollAREventData *set = (TpDollAREventData *)TpEvent::eventData_;
    int32_t x = TP_INVALIDATE_VALUE;

    if (set)
    {
        x = set->x;
    }

    return x;
}

int32_t TpDollAREvent::Y()
{
    TpDollAREventData *set = (TpDollAREventData *)TpEvent::eventData_;
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
    TpMultiGestureEventData *set = new TpMultiGestureEventData();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_DOLLAR_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpMultiGestureEvent::~TpMultiGestureEvent()
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpMultiGestureEvent::construct(ITpEventData *eventData)
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpMultiGestureEventData *pEventData = (TpMultiGestureEventData *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpMultiGestureEvent::timestamp()
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;
    int32_t timestamp = 0;

    if (set)
    {
        timestamp = set->timestamp;
    }

    return timestamp;
}

long long TpMultiGestureEvent::touchID()
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;
    long long touchID = TP_INVALIDATE_VALUE;

    if (set)
    {
        touchID = set->touchID;
    }

    return touchID;
}

float TpMultiGestureEvent::dtheta()
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;
    float dtheta = 0.0;

    if (set)
    {
        dtheta = set->dtheta;
    }

    return dtheta;
}

float TpMultiGestureEvent::ddist()
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;
    float ddist = 0.0;

    if (set)
    {
        ddist = set->ddist;
    }

    return ddist;
}

int32_t TpMultiGestureEvent::X()
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;
    int32_t x = TP_INVALIDATE_VALUE;

    if (set)
    {
        x = set->x;
    }

    return x;
}

int32_t TpMultiGestureEvent::Y()
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;
    int32_t y = TP_INVALIDATE_VALUE;

    if (set)
    {
        y = set->y;
    }

    return y;
}

uint16_t TpMultiGestureEvent::numfingers()
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;
    uint16_t numfingers = 0;

    if (set)
    {
        numfingers = set->numfingers;
    }

    return numfingers;
}

uint16_t TpMultiGestureEvent::padding()
{
    TpMultiGestureEventData *set = (TpMultiGestureEventData *)TpEvent::eventData_;
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
    TpMoveEventData *set = new TpMoveEventData();

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_OBJECT_MOVE_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpMoveEvent::~TpMoveEvent()
{
    TpMoveEventData *set = (TpMoveEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpMoveEvent::construct(ITpEventData *eventData)
{
    TpMoveEventData *set = (TpMoveEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpMoveEventData *pEventData = (TpMoveEventData *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpMoveEvent::newX()
{
    TpMoveEventData *set = (TpMoveEventData *)TpEvent::eventData_;
    int32_t newX = TP_INVALIDATE_VALUE;

    if (set)
    {
        newX = set->nx;
    }

    return newX;
}

int32_t TpMoveEvent::newY()
{
    TpMoveEventData *set = (TpMoveEventData *)TpEvent::eventData_;
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
    TpResizeEventData *set = new TpResizeEventData(); // EVENT_OBJECT_RESIZE_TYPE
    TpEvent::eventType_ = TpEvent::EVENT_OBJECT_RESIZE_TYPE;
    TpEvent::eventData_ = set;
}

TpResizeEvent::~TpResizeEvent()
{
    TpResizeEventData *set = (TpResizeEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpResizeEvent::construct(ITpEventData *eventData)
{
    TpResizeEventData *set = (TpResizeEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpResizeEventData *pEventData = (TpResizeEventData *)eventData;
    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

int32_t TpResizeEvent::question()
{
    TpResizeEventData *set = (TpResizeEventData *)TpEvent::eventData_;
    int32_t question = TP_UNKOWN_CHANGE;

    if (set)
    {
        question = set->question;
    }

    return question;
}

int32_t TpResizeEvent::nWidth()
{
    TpResizeEventData *set = (TpResizeEventData *)TpEvent::eventData_;
    int32_t newWidth = TP_INVALIDATE_VALUE;

    if (set)
    {
        newWidth = set->nw;
    }

    return newWidth;
}

int32_t TpResizeEvent::nHeight()
{
    TpResizeEventData *set = (TpResizeEventData *)TpEvent::eventData_;
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
    TpFocusEventData *set = new TpFocusEventData(); // EVENT_OBJECT_FOCUS_TYPE

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_OBJECT_FOCUS_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpFocusEvent::~TpFocusEvent()
{
    TpFocusEventData *set = (TpFocusEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpFocusEvent::construct(ITpEventData *eventData)
{
    TpFocusEventData *set = (TpFocusEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpFocusEventData *pEventData = (TpFocusEventData *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

bool TpFocusEvent::focused()
{
    TpFocusEventData *set = (TpFocusEventData *)TpEvent::eventData_;
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
    TpLeaveEventData *set = new TpLeaveEventData(); // EVENT_OBJECT_LEAVE_TYPE
    TpEvent::eventType_ = TpEvent::EVENT_OBJECT_LEAVE_TYPE;
    TpEvent::eventData_ = set;
}

TpLeaveEvent::~TpLeaveEvent()
{
    TpLeaveEventData *set = (TpLeaveEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpLeaveEvent::construct(ITpEventData *eventData)
{
    TpLeaveEventData *set = (TpLeaveEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpLeaveEventData *pEventData = (TpLeaveEventData *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

bool TpLeaveEvent::leave()
{
    TpLeaveEventData *set = (TpLeaveEventData *)TpEvent::eventData_;
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
    TpVisibleEventData *set = new TpVisibleEventData(); // EVENT_OBJECT_VISIBLE_TYPE
    TpEvent::eventType_ = TpEvent::EVENT_OBJECT_VISIBLE_TYPE;
    TpEvent::eventData_ = set;
}

TpVisibleEvent::~TpVisibleEvent()
{
    TpVisibleEventData *set = (TpVisibleEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpVisibleEvent::construct(ITpEventData *eventData)
{
    TpVisibleEventData *set = (TpVisibleEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpVisibleEventData *pEventData = (TpVisibleEventData *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

bool TpVisibleEvent::visible()
{
    TpVisibleEventData *set = (TpVisibleEventData *)TpEvent::eventData_;
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
    TpPaintEventData *set = new TpPaintEventData(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        set->canDraw = false;
        TpEvent::eventType_ = TpEvent::EVENT_OBJECT_PAINT_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpPaintEvent::~TpPaintEvent()
{
    TpPaintEventData *set = (TpPaintEventData *)TpEvent::eventData_;

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
    TpPaintEventData *set = (TpPaintEventData *)TpEvent::eventData_;
    return set->painter;
}

TpRect TpPaintEvent::updateRect()
{
    TpPaintEventData *set = (TpPaintEventData *)TpEvent::eventData_;
    return set->updateRect;
}

bool TpPaintEvent::isCanDraw()
{
    TpPaintEventData *set = (TpPaintEventData *)TpEvent::eventData_;
    return set->canDraw;
}

bool TpPaintEvent::construct(ITpEventData *inputData)
{
    TpPaintEventData *eventData = static_cast<TpPaintEventData *>(TpEvent::eventData_);
    if (!eventData)
        return false;
    eventData->canDraw = false;

    TpPaintEventInput *input = static_cast<TpPaintEventInput *>(inputData);
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
    TpActiveEventData *set = new TpActiveEventData(); // EVENT_OBJECT_ROTATE_TYPE

    if (set)
    {
        TpEvent::eventType_ = TpEvent::EVENT_OBJECT_ACTIVE_TYPE;
        TpEvent::eventData_ = set;
    }
}

TpActiveEvent::~TpActiveEvent()
{
    TpActiveEventData *set = (TpActiveEventData *)TpEvent::eventData_;

    if (set)
    {
        delete set;
    }
}

bool TpActiveEvent::construct(ITpEventData *eventData)
{
    TpActiveEventData *set = (TpActiveEventData *)TpEvent::eventData_;

    if (!set)
        return false;

    TpActiveEventData *pEventData = (TpActiveEventData *)eventData;

    if (pEventData)
    {
        *set = *pEventData;
        return true;
    }

    return false;
}

bool TpActiveEvent::isActived()
{
    TpActiveEventData *set = (TpActiveEventData *)TpEvent::eventData_;
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
