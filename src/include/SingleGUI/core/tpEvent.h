#ifndef __TP_VEVENT_H
#define __TP_VEVENT_H

#include "tpObject.h"
#include "event.h"
#include <functional>

#define KEYBOARD_STRING_LEN 32

TP_DEF_VOID_TYPE_VAR(ItpEventData);
TP_DEF_VOID_TYPE_VAR(ItpSufaceData);

class tpCanvas;
class tpSurface;
class tpRect;

class tpEvent
{
public:
	friend class tpShowEvent;
	friend class tpKeyboardEvent;
	friend class tpMouseEvent;
	friend class tpWheelEvent;
	friend class tpFingerEvent;
	friend class tpDollAREvent;
	friend class tpMultiGestureEvent;

	friend class tpObjectMoveEvent;
	friend class tpObjectResizeEvent;
	friend class tpObjectFocusEvent;
	friend class tpObjectLeaveEvent;
	friend class tpObjectVisibleEvent;
	friend class tpObjectRotateEvent;
	friend class tpObjectPaintEvent;
	friend class tpObjectActiveEvent;

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
	tpEvent();
	virtual ~tpEvent();

public:
	/// @brief 事件数据解析
	/// @param eventData 事件数据
	/// @return 解析构建结果
	virtual bool construct(ItpEventData *eventData) = 0;

	/// @brief 获取事件类型
	/// @return 类型枚举
	virtual ItpEventType eventType() = 0;

protected:
	ItpEventData *tpEventSet;
};

/// @brief 窗口显示事件；暂未实现
// class tpShowEvent : public tpEvent
// {
// public:
// 	tpShowEvent();
// 	virtual ~tpShowEvent();

// public:
// 	virtual bool construct(ItpEventData *eventData);
// 	virtual ItpEventType eventType();
// };

class tpKeyboardEvent : public tpEvent
{
public:
	tpKeyboardEvent();
	virtual ~tpKeyboardEvent();

public:
	virtual bool construct(ItpEventData *eventData);
	virtual ItpEventType eventType();

public:
	virtual uint8_t which();		// device index
	virtual bool state();			// press or released
	virtual int32_t scancode();		// scan code
	virtual int32_t virtualKey();	// virtual key
	virtual int32_t symbol();		////combine mod key to translate key number
	virtual const char *shortCut(); ////define key string
	virtual KeyModeType keyMod();	// mode key, such as ctrl, alt, caps, num lock, etc..

	/// @brief 获取按键的是否是可显示字符
	/// @return 是返回true，否则返回false
	bool isPrintable();
};

/// @brief 鼠标事件；包括鼠标按下、释放、双击、长按、鼠标移动事件
class tpMouseEvent : public tpEvent
{
public:
	tpMouseEvent();
	virtual ~tpMouseEvent();

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
typedef std::function<void(tpMouseEvent *)> tpMouseEventListenerFunc;

class tpWheelEvent : public tpEvent
{
public:
	tpWheelEvent();
	virtual ~tpWheelEvent();

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

class tpFingerEvent : public tpEvent
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
	tpFingerEvent();
	virtual ~tpFingerEvent();

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

class tpDollAREvent : public tpEvent
{
public:
	enum
	{
		TOUCH_DOLLAR_NONE = -1,
		TOUCH_DOLLAR_GESTURE,
		TOUCH_DOLLAR_RECORD,
	};

public:
	tpDollAREvent();
	virtual ~tpDollAREvent();

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
	virtual int32_t X();		  // event occurs, coordinate x
	virtual int32_t Y();		  // event occurs, coordinate y
};

class tpMultiGestureEvent : public tpEvent
{
public:
	tpMultiGestureEvent();
	virtual ~tpMultiGestureEvent();

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

class tpObjectMoveEvent : public tpEvent
{
public:
	tpObjectMoveEvent();
	virtual ~tpObjectMoveEvent();

public:
	virtual bool construct(ItpEventData *eventData);

public:
	virtual ItpEventType eventType();

public:
	virtual tpObject *object();

public:
	virtual int32_t newX();
	virtual int32_t newY();
};

class tpObjectResizeEvent : public tpEvent
{
public:
	enum
	{
		TP_UNKOWN_CHANGE = -1,
		TP_NORMAL_CHANGE,
		TP_RESOLUTION_CHANGE,
	};

public:
	tpObjectResizeEvent();
	virtual ~tpObjectResizeEvent();

public:
	virtual bool construct(ItpEventData *eventData);

public:
	virtual ItpEventType eventType();
	virtual int32_t question();

public:
	virtual tpObject *object();

public:
	virtual int32_t nWidth();
	virtual int32_t nHeight();
};

class tpObjectFocusEvent : public tpEvent
{
public:
	tpObjectFocusEvent();
	virtual ~tpObjectFocusEvent();

public:
	virtual bool construct(ItpEventData *eventData);

public:
	virtual ItpEventType eventType();

public:
	virtual tpObject *object();

public:
	virtual bool focused();
};

class tpObjectLeaveEvent : public tpEvent
{
public:
	tpObjectLeaveEvent();
	virtual ~tpObjectLeaveEvent();

public:
	virtual bool construct(ItpEventData *eventData);

public:
	virtual ItpEventType eventType();

public:
	virtual tpObject *object();

public:
	/// @brief 鼠标是否悬停本窗口
	/// @return true进入，false离开
	virtual bool leave();
};

class tpObjectVisibleEvent : public tpEvent
{
public:
	tpObjectVisibleEvent();
	virtual ~tpObjectVisibleEvent();

public:
	virtual bool construct(ItpEventData *eventData);

public:
	virtual ItpEventType eventType();

public:
	virtual tpObject *object();

public:
	virtual bool visible();
};

class tpObjectRotateEvent : public tpEvent
{
public:
	tpObjectRotateEvent();
	virtual ~tpObjectRotateEvent();

public:
	virtual bool construct(ItpEventData *eventData);

public:
	virtual ItpEventType eventType();

public:
	virtual tpObject *object();

public:
	virtual ItpRotateType rotate();
};

class tpObjectPaintEvent : public tpEvent
{
public:
	tpObjectPaintEvent();
	virtual ~tpObjectPaintEvent();

public:
	virtual bool construct(ItpEventData *eventData);

public:
	virtual ItpEventType eventType();

public:
	virtual tpObject *object();

public:
	virtual tpCanvas *canvas();	  // must set offsetX and offsetY
	virtual tpSurface *surface(); // must set clipRect
	virtual ItpSufaceData *itpSurface();

public:
	virtual int32_t offsetX();
	virtual int32_t offsetY();

public:
	virtual ItpRect updateRect(); // update rect
	virtual ItpRect rect();		  // object logical rect, use this to canvas
	virtual ItpRect absRect();	  // object absolute rect, use this to canvas, not object absrect
public:
	virtual bool isCanDraw();
};

class tpObjectActiveEvent : public tpEvent
{
public:
	tpObjectActiveEvent();
	virtual ~tpObjectActiveEvent();

public:
	virtual bool construct(ItpEventData *eventData);

public:
	virtual ItpEventType eventType();

public:
	virtual tpObject *object();

public:
	virtual bool isActived();
};

class tpThemeChangeEvent : public tpEvent
{
public:
	tpThemeChangeEvent();
	virtual ~tpThemeChangeEvent();

public:
	virtual bool construct(ItpEventData *eventData) override;

	virtual tpEvent::ItpEventType eventType() override;
};
#endif
