#ifndef __TP_VEVENT_H
#define __TP_VEVENT_H

#include "TpObject.h"
#include "event.h"
#include <functional>

#define KEYBOARD_STRING_LEN 32

TP_DEF_VOID_TYPE_VAR(ItpEventData);
TP_DEF_VOID_TYPE_VAR(ItpSufaceData);

class TpCanvas;
class TpSurface;
class TpRect;

class TpEvent
{
public:
    friend class TpShowEvent;
    friend class TpKeyboardEvent;
    friend class TpMouseEvent;
    friend class TpWheelEvent;
    friend class TpFingerEvent;
    friend class TpDollAREvent;
    friend class TpMultiGestureEvent;

    friend class TpObjectMoveEvent;
    friend class TpObjectResizeEvent;
    friend class TpObjectFocusEvent;
    friend class TpObjectLeaveEvent;
    friend class TpObjectVisibleEvent;
    friend class TpObjectRotateEvent;
    friend class TpObjectPaintEvent;
    friend class TpObjectActiveEvent;

public:
    /// @brief 事件类型枚举
    enum ItpEventType
    {
        /// @brief 无类型
        EVENT_NONE_TYPE = -1,

        /// @brief 窗口显示事件
        EVENT_WINDOW_SHOW_TYPE,

        /// @brief 键盘按下事件
        EVENT_KEYBOARD_PRESS_TYPE,
        /// @brief 键盘释放事件
        EVENT_KEYBOARD_RELEASE_TYPE,

        /// @brief 鼠标按下事件
        EVENT_MOUSE_PRESS_TYPE,
        /// @brief 鼠标释放
        EVENT_MOUSE_RELEASE_TYPE,
        /// @brief 鼠标双击
        EVENT_MOUSE_DOUBLE_CLICK_TYPE,
        /// @brief 鼠标移动
        EVENT_MOUSE_MOVE_TYPE,
        /// @brief 鼠标长按
        EVENT_MOUSE_LONG_PRESS_TYPE,

        /// @brief 滚轮事件
        EVENT_WHEEL_EVENT,

        EVENT_FINGER_TYPE,
        EVENT_DOLLAR_TYPE,
        EVENT_MULTIGESTURE_TYPE,

        EVENT_OBJECT_MOVE_TYPE,
        /// @brief 窗口大小变化事件
        EVENT_OBJECT_RESIZE_TYPE,
        EVENT_OBJECT_FOCUS_TYPE,
        EVENT_OBJECT_LEAVE_TYPE,
        EVENT_OBJECT_VISIBLE_TYPE,
        EVENT_OBJECT_ROTATE_TYPE,
        EVENT_OBJECT_PAINT_TYPE,
        EVENT_OBJECT_ACTIVE_TYPE,

        EVENT_THEME_CHANGE_TYPE,
    };

public:
    TpEvent();
    virtual ~TpEvent();

public:
    /// @brief 事件数据解析
    /// @param eventData 事件数据
    /// @return 解析构建结果
    virtual bool construct(ItpEventData *eventData) = 0;

    /// @brief 获取事件类型
    /// @return 类型枚举
    virtual ItpEventType eventType() = 0;

protected:
    ItpEventData *TpEventSet;
};

/// @brief 窗口显示事件；暂未实现
// class TpShowEvent : public TpEvent
// {
// public:
// 	TpShowEvent();
// 	virtual ~TpShowEvent();

// public:
// 	virtual bool construct(ItpEventData *eventData);
// 	virtual ItpEventType eventType();
// };

class TpKeyboardEvent : public TpEvent
{
public:
    TpKeyboardEvent();
    virtual ~TpKeyboardEvent();

public:
    virtual bool construct(ItpEventData *eventData);
    virtual ItpEventType eventType();

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
    TpMouseEvent();
    virtual ~TpMouseEvent();

public:
    /// @brief 构建数据；用户可忽略此函数
    /// @param eventData 数据指针
    /// @return 构建结果
    virtual bool construct(ItpEventData *eventData) override;

    /// @brief 获取事件类型
    /// @return 类型枚举
    virtual ItpEventType eventType() override;

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
    virtual ItpPoint pos();
    /// @brief 获取鼠标相对于屏幕的全局坐标
    /// @return 坐标
    virtual ItpPoint globalPos();
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
    virtual bool construct(ItpEventData *eventData) override;

    /// @brief 获取事件类型
    /// @return 类型枚举
    virtual ItpEventType eventType() override;

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
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

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
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

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
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

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

class TpObjectMoveEvent : public TpEvent
{
public:
    TpObjectMoveEvent();
    virtual ~TpObjectMoveEvent();

public:
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

public:
    virtual TpObject *object();

public:
    virtual int32_t newX();
    virtual int32_t newY();
};

class TpObjectResizeEvent : public TpEvent
{
public:
    enum
    {
        TP_UNKOWN_CHANGE = -1,
        TP_NORMAL_CHANGE,
        TP_RESOLUTION_CHANGE,
    };

public:
    TpObjectResizeEvent();
    virtual ~TpObjectResizeEvent();

public:
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();
    virtual int32_t question();

public:
    virtual TpObject *object();

public:
    virtual int32_t nWidth();
    virtual int32_t nHeight();
};

class TpObjectFocusEvent : public TpEvent
{
public:
    TpObjectFocusEvent();
    virtual ~TpObjectFocusEvent();

public:
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

public:
    virtual TpObject *object();

public:
    virtual bool focused();
};

class TpObjectLeaveEvent : public TpEvent
{
public:
    TpObjectLeaveEvent();
    virtual ~TpObjectLeaveEvent();

public:
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

public:
    virtual TpObject *object();

public:
    /// @brief 鼠标是否悬停本窗口
    /// @return true进入，false离开
    virtual bool leave();
};

class TpObjectVisibleEvent : public TpEvent
{
public:
    TpObjectVisibleEvent();
    virtual ~TpObjectVisibleEvent();

public:
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

public:
    virtual TpObject *object();

public:
    virtual bool visible();
};

class TpObjectRotateEvent : public TpEvent
{
public:
    TpObjectRotateEvent();
    virtual ~TpObjectRotateEvent();

public:
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

public:
    virtual TpObject *object();

public:
    virtual ItpRotateType rotate();
};

class TpObjectPaintEvent : public TpEvent
{
public:
    TpObjectPaintEvent();
    virtual ~TpObjectPaintEvent();

public:
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

public:
    virtual TpObject *object();

public:
    virtual TpCanvas *canvas();            // must set offsetX and offsetY
    virtual tpShared<TpSurface> surface(); // must set clipRect
    virtual ItpSufaceData *itpSurface();

public:
    virtual int32_t offsetX();
    virtual int32_t offsetY();

public:
    virtual ItpRect updateRect(); // update rect
    virtual ItpRect rect();       // object logical rect, use this to canvas
    virtual ItpRect absRect();    // object absolute rect, use this to canvas, not object absrect
public:
    virtual bool isCanDraw();
};

class TpObjectActiveEvent : public TpEvent
{
public:
    TpObjectActiveEvent();
    virtual ~TpObjectActiveEvent();

public:
    virtual bool construct(ItpEventData *eventData);

public:
    virtual ItpEventType eventType();

public:
    virtual TpObject *object();

public:
    virtual bool isActived();
};

class TpThemeChangeEvent : public TpEvent
{
public:
    TpThemeChangeEvent();
    virtual ~TpThemeChangeEvent();

public:
    virtual bool construct(ItpEventData *eventData) override;

    virtual TpEvent::ItpEventType eventType() override;
};
#endif
