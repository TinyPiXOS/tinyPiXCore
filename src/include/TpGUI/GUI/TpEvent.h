#ifndef __TP_VEVENT_H
#define __TP_VEVENT_H

#include "TpObject.h"
#include "event.h"
#include <functional>
#include <TpCoreEvent.h>

#define KEYBOARD_STRING_LEN 32

TP_DEF_VOID_TYPE_VAR(ItpSufaceData);

class TpPainter;
class TpRect;
class TpPoint;

class TpKeyboardEvent : public TpEvent
{
public:
    TpKeyboardEvent(TpEvent::TpEventType type);
    virtual ~TpKeyboardEvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    virtual uint8_t which();        // device index
    virtual bool state();           // press or released
    virtual int32_t scancode();     // scan code
    virtual int32_t virtualKey();   // virtual key
    virtual int32_t symbol();       ////combine mod key to translate key number
    virtual const char *shortCut(); ////define key string
    virtual KeyModeType keyMod();   // mode key, such as ctrl, alt, caps, num lock, etc..

    /// @brief 获取按键的是否是可显示字符
    /// @return 是返回true，否则返回false
    bool isPrintable();
};

/// @brief 鼠标事件；包括鼠标按下、释放、双击、长按、鼠标移动事件
class TpMouseEvent : public TpEvent
{
public:
    TpMouseEvent(TpEvent::TpEventType type);
    virtual ~TpMouseEvent();

public:
    /// @brief 构建数据；用户可忽略此函数
    /// @param eventData 数据指针
    /// @return 构建结果
    virtual bool construct(ITpEventData *eventData) override;

public:
    /// @brief device index
    /// @return
    virtual int32_t which();

    /// @brief 获取鼠标按键类型，左键、右键等
    /// @return 按键类型枚举
    virtual MouseEventType button();

    /// @brief 鼠标按键是否按下
    /// @return 按下为true，释放为false
    virtual bool state();

    /// @brief 获取鼠标在窗口内坐标
    /// @return 坐标
    virtual TpPoint pos();
    /// @brief 获取鼠标相对于屏幕的全局坐标
    /// @return 坐标
    virtual TpPoint globalPos();
};
typedef std::function<void(TpMouseEvent *)> TpMouseEventListenerFunc;

class TpWheelEvent : public TpEvent
{
public:
    TpWheelEvent();
    virtual ~TpWheelEvent();

public:
    /// @brief 构建数据；用户可忽略此函数
    /// @param eventData 数据指针
    /// @return 构建结果
    virtual bool construct(ITpEventData *eventData) override;

public:
    /// @brief 滚轮滚动值；正值为页面向上滚动。其它值为页面向下滚动
    /// @return 滚动值
    int32_t angleDelta() const { return angleDelta_; }

private:
    int32_t angleDelta_;
};

class TpFingerEvent : public TpEvent
{
public:
    enum
    {
        TOUCH_FINGER_NONE = -1,
        TOUCH_FINGER_UP,
        TOUCH_FINGER_DOWN,
        TOUCH_FINGER_MOTION,
    };

public:
    TpFingerEvent();
    virtual ~TpFingerEvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    virtual int32_t touchFingerType();
    virtual int32_t timestamp();
    virtual long long fingerID();
    virtual long long touchID();
    virtual float pressure();
    virtual int32_t X();  // event occurs, coordinate x
    virtual int32_t Y();  // event occurs, coordinate y
    virtual int32_t dx(); //+ left, - right
    virtual int32_t dy(); //+ down, - up
};

class TpDollAREvent : public TpEvent
{
public:
    enum
    {
        TOUCH_DOLLAR_NONE = -1,
        TOUCH_DOLLAR_GESTURE,
        TOUCH_DOLLAR_RECORD,
    };

public:
    TpDollAREvent();
    virtual ~TpDollAREvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    virtual int32_t dollarType();
    virtual int32_t timestamp();
    virtual long long touchID();
    virtual long long GestureID();
    virtual int32_t numFingers(); // multi fingers touch
    virtual int32_t X();          // event occurs, coordinate x
    virtual int32_t Y();          // event occurs, coordinate y
};

class TpMultiGestureEvent : public TpEvent
{
public:
    TpMultiGestureEvent();
    virtual ~TpMultiGestureEvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    virtual int32_t timestamp();
    virtual long long touchID();
    virtual float dtheta();
    virtual float ddist();
    virtual int32_t X();
    virtual int32_t Y();
    virtual uint16_t numfingers();
    virtual uint16_t padding();
};

class TpMoveEvent : public TpEvent
{
public:
    TpMoveEvent();
    virtual ~TpMoveEvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    virtual int32_t newX();
    virtual int32_t newY();
};

class TpResizeEvent : public TpEvent
{
public:
    enum
    {
        TP_UNKOWN_CHANGE = -1,
        TP_NORMAL_CHANGE,
        TP_RESOLUTION_CHANGE,
    };

public:
    TpResizeEvent();
    virtual ~TpResizeEvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    virtual int32_t question();

public:
    virtual int32_t nWidth();
    virtual int32_t nHeight();
};

class TpFocusEvent : public TpEvent
{
public:
    TpFocusEvent();
    virtual ~TpFocusEvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    virtual bool focused();
};

class TpLeaveEvent : public TpEvent
{
public:
    TpLeaveEvent();
    virtual ~TpLeaveEvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    /// @brief 鼠标是否悬停本窗口
    /// @return true进入，false离开
    virtual bool leave();
};

class TpVisibleEvent : public TpEvent
{
public:
    TpVisibleEvent();
    virtual ~TpVisibleEvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    virtual bool visible();
};

class TpPaintEvent : public TpEvent
{
public:
    TpPaintEvent();
    virtual ~TpPaintEvent();

public:
    virtual TpPainter *painter();          // must set offsetX and offsetY
    virtual TpRect updateRect(); // update rect
    virtual bool isCanDraw();

public:
    /// @brief 构建绘制事件数据；用户无需调用
    virtual bool construct(ITpEventData *inputData) override;
};

class TpActiveEvent : public TpEvent
{
public:
    TpActiveEvent();
    virtual ~TpActiveEvent();

public:
    virtual bool construct(ITpEventData *eventData);

public:
    virtual bool isActived();
};

/// @brief 主题切换事件；暂未实现
class TpThemeChangeEvent : public TpEvent
{
public:
    TpThemeChangeEvent();
    virtual ~TpThemeChangeEvent();

public:
    virtual bool construct(ITpEventData *eventData) override;
};
#endif
