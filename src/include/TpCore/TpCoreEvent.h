#ifndef __TP_CORE_EVENT_H
#define __TP_CORE_EVENT_H

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ITpEventData);
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

    friend class TpMoveEvent;
    friend class TpResizeEvent;
    friend class TpFocusEvent;
    friend class TpLeaveEvent;
    friend class TpVisibleEvent;
    friend class TpPaintEvent;
    friend class TpActiveEvent;

public:
    /// @brief 事件类型枚举
    enum TpEventType
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
    TpEvent() : eventData_(nullptr) {};
    virtual ~TpEvent() {}

public:
    /// @brief 事件数据解析
    /// @param eventData 事件数据
    /// @return 解析构建结果
    virtual bool construct(ITpEventData *eventData) = 0;

    /// @brief 获取事件类型
    /// @return 类型枚举
    virtual TpEventType eventType() { return eventType_; };

protected:
    ITpEventData *eventData_;
    TpEventType eventType_;
};

#endif