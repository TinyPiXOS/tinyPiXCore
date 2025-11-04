#ifndef __TP_SCREEN_PRIVATE_H
#define __TP_SCREEN_PRIVATE_H

#include "TpEvent.h"
#include "TpApp.h"
#include "TpDef.h"
#include "TpLayout.h"
#include "TpObjectStack.h"
#include "TpTimer.h"
#include "TpWidget.h"
#include "TpPainter.h"
#include <TpColors.h>
#include <TpRect.h>
#include <TpPoint.h>
#include <tinyPiXUtils.h>
#include <tinyPiXWF.h>
#include <cstring>
#include <mutex>
#include <thread>
#include "TpCDef.h"
#include "TpApp.h"

// 鼠标左键长按回调
static std::function<void(TpWidget *, ItpMouseSet)> longPressCallback = [](TpWidget *obj, ItpMouseSet mouseSet)
{
    // std::cout << " onLongPress ***********" << std::endl;

    ItpMouseSet longPressData = mouseSet;
    longPressData.type = TpEvent::EVENT_MOUSE_LONG_PRESS_TYPE;

    TpMouseEvent keyEvent;
    keyEvent.construct(&longPressData);

    IssueObjEvent(obj, keyEvent, onMouseLongPressEvent, obj->enabled());
};

static std::atomic<bool> longPressActive{false}; // 原子标志位
static std::unique_ptr<std::thread> pressThread; // 线程对象
static std::mutex pressThreadMutex;              // 保护线程状态

static inline void timer_delay(unsigned long long usec)
{
    struct timeval tv;
    tv.tv_sec = usec / 1000000;
    tv.tv_usec = usec % 1000000;
    int32_t err;
    do
    {
        err = select(0, NULL, NULL, NULL, &tv);
    } while (err < 0 && errno == EINTR);
}

static inline void generateParentList(TpObject *object, std::list<TpObject *> &list)
{
    if (object)
    {
        TpObject *parent = object->parent();
        TpObject *top = object->topObject();

        while (parent)
        {
            list.push_front(parent);

            if (parent == top)
            {
                break;
            }

            parent = parent->parent();
        }
    }
}

static inline void startLongPressCheck(TpWidget *object, const ItpMouseSet &mouseSet)
{
    std::lock_guard<std::mutex> lock(pressThreadMutex);
    if (pressThread || longPressActive)
        return;

    longPressActive = true;

    // 启动异步检测线程
    pressThread = std::unique_ptr<std::thread>(new std::thread([=]()
                                                               {
        const auto startTime = std::chrono::steady_clock::now();
        
        // 分段等待，每100ms检查一次状态
        while (longPressActive) 
        {
            // std::cout << " longPressActive " << longPressActive << std::endl;

            auto elapsed = std::chrono::steady_clock::now() - startTime;
            if (elapsed >= std::chrono::seconds(1)) 
            {
                // 触发长按回调
                if (longPressCallback) 
                    longPressCallback(object, mouseSet);
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        longPressActive = false; }));
}

static inline void stopLongPressCheck()
{
    std::lock_guard<std::mutex> lock(pressThreadMutex);
    longPressActive = false; // 通知线程退出

    if (pressThread && pressThread->joinable())
    {
        // 等待线程结束
        pressThread->join();
        pressThread.reset();
    }
}

static inline void broadMotion(TpObject *dragObject, TpObject *curMotionObject, std::list<TpObject *> &list, ItpEvent *events, TpWidget *pressObject)
{
    TpMouseEvent motionEvent;
    ItpMouseSet mInput;
    mInput.which = events->mouseMotionEvent.which;
    mInput.type = TpEvent::EVENT_MOUSE_MOVE_TYPE;

    std::list<TpObject *>::iterator iter = list.begin();

    TpWidget *childObj = static_cast<TpWidget *>(dragObject);
    if (childObj)
    {
        // motion, and state = true
        mInput.pos.setX(events->mouseMotionEvent.x - childObj->toScreen().x());
        mInput.pos.setY(events->mouseMotionEvent.y - childObj->toScreen().y());

        mInput.globalPos.setX(events->mouseMotionEvent.x);
        mInput.globalPos.setY(events->mouseMotionEvent.y);

        mInput.state = true;
        motionEvent.construct(&mInput);

        // 如果该对象安装了事件过滤器，先将事件传给事件过滤器
        // std::cout << "1111++++++++++++Move++++++++++" << mInput.globalPos.x << " , " << mInput.globalPos.y << std::endl;
        IssueObjEvent(childObj, motionEvent, onMouseMoveEvent, childObj->enabled());
    }

    TpWidget *childMotionObj = static_cast<TpWidget *>(curMotionObject);

    if (childMotionObj && curMotionObject != dragObject)
    {
        // motion, and dragObject is not null, then state = false; otherwise state = true
        mInput.pos.setX(events->mouseMotionEvent.x - childMotionObj->toScreen().x());
        mInput.pos.setY(events->mouseMotionEvent.y - childMotionObj->toScreen().y());

        mInput.globalPos.setX(events->mouseMotionEvent.x);
        mInput.globalPos.setY(events->mouseMotionEvent.y);

        mInput.state = dragObject ? false : events->mouseMotionEvent.state;
        motionEvent.construct(&mInput);

        // 如果该对象安装了事件过滤器，先将事件传给事件过滤器
        // std::cout << "222++++++++++++Move++++++++++" << mInput.globalPos.x << " , " << mInput.globalPos.y << std::endl;
        IssueObjEvent(childMotionObj, motionEvent, onMouseMoveEvent, childMotionObj->enabled());

        // 如果按下对象后鼠标移动，不再触发长按事件
        if (childMotionObj == pressObject)
        {
            stopLongPressCheck();
        }
    }

    list.clear();
}

static inline void broadMouseKey(TpObject *object, std::list<TpObject *> &list, ItpEvent *events, TpWidget *pressObject)
{
    ItpMouseSet mInput;
    mInput.which = events->mouseButtonEvent.which;
    mInput.button = events->mouseButtonEvent.button;
    mInput.state = events->mouseButtonEvent.state;
    mInput.type = mInput.state ? TpEvent::EVENT_MOUSE_PRESS_TYPE : TpEvent::EVENT_MOUSE_RELEASE_TYPE;

    TpWidget *childObj = static_cast<TpWidget *>(object);
    if (!childObj)
    {
        list.clear();
        return;
    }

    // mouse down and up
    mInput.pos.setX(events->mouseButtonEvent.x - childObj->toScreen().x());
    mInput.pos.setY(events->mouseButtonEvent.y - childObj->toScreen().y());

    mInput.globalPos.setX(events->mouseButtonEvent.x);
    mInput.globalPos.setY(events->mouseButtonEvent.y);

    // 滚轮事件
    if (mInput.button == BUTTON_WHEELUP || mInput.button == BUTTON_WHEELDOWN)
    {
        TpWheelEvent wheelEvent;
        mInput.type = TpEvent::EVENT_WHEEL_EVENT;
        wheelEvent.construct(&mInput);

        IssueObjEvent(childObj, wheelEvent, onWheelEvent, childObj->enabled());

        list.clear();
        return;
    }

    // 按键事件
    if (mInput.button == BUTTON_LEFT)
    {
        // 定义双击间隔时间（毫秒）
        const int32_t doubleClickInterval = 200;

        if (mInput.state == true)
        {
            // 记录当前点击时间,减10分钟，确保第一次进入时的判断正确
            static auto lastClickTime = std::chrono::high_resolution_clock::now() - std::chrono::minutes(10);

            auto now = std::chrono::high_resolution_clock::now();

            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime).count();

            // 是双击事件
            if (elapsed < doubleClickInterval)
            {
                // 如果是双击，触发双击事件
                mInput.type = TpEvent::EVENT_MOUSE_DOUBLE_CLICK_TYPE;
            }

            // 重置上一次点击时间
            lastClickTime = std::chrono::high_resolution_clock::now();
        }
    }

    TpMouseEvent keyEvent;
    keyEvent.construct(&mInput);

    if (mInput.type == TpEvent::EVENT_MOUSE_DOUBLE_CLICK_TYPE)
    {
        // 终止长按计算线程
        stopLongPressCheck();

        IssueObjEvent(childObj, keyEvent, onMouseDoubleClickEvent, childObj->enabled());
    }
    else
    {
        // 鼠标按下时直接触发事件，释放时需要判断，鼠标按下后是否滑动，滑动则不触发释放事件
        if (mInput.state)
        {
            IssueObjEvent(childObj, keyEvent, onMousePressEvent, childObj->enabled());

            // 计时，判断是否是长按
            startLongPressCheck(childObj, mInput);
        }
        else
        {
            // 终止长按计算线程
            stopLongPressCheck();

            // 无论鼠标在哪，都触发按下对象的释放事件
            if (pressObject)
            {
                IssueObjEvent(pressObject, keyEvent, onMouseRleaseEvent, pressObject->enabled());
            }
        }
    }

    list.clear();
}

static inline void broadFinger(TpObjectData *set, ItpFingerSet &input, TpObject *object, std::list<TpObject *> &list, ItpEvent *events)
{
    TpWidget *childObj = static_cast<TpWidget *>(object);
    if (!childObj)
        return;

    uint32_t rW = 0, rH = 0;
    tinyPiX_wf_get_display_size(set->agent, &rW, &rH);
    TpFingerEvent event;

    input.timestamp = events->fingerEvent.timestamp;
    input.fingerID = events->fingerEvent.fingerId;
    input.touchID = events->fingerEvent.touchId;
    input.pressure = events->fingerEvent.pressure;

    event.construct(&input);
    std::list<TpObject *>::iterator iter = list.begin();

    input.x = events->fingerEvent.x * rW - childObj->toScreen().x();
    input.y = events->fingerEvent.y * rH - childObj->toScreen().y();
    event.construct(&input);

    // 如果该对象安装了事件过滤器，先将事件传给事件过滤器
    IssueObjEvent(childObj, event, onFingerEvent, childObj->enabled());

    list.clear();
}

static inline void broaDollar(TpObjectData *set, ItpDollarSet &input, TpObject *object, std::list<TpObject *> &list, ItpEvent *events)
{
    TpWidget *childObj = static_cast<TpWidget *>(object);
    if (!childObj)
        return;

    uint32_t rW = 0, rH = 0;
    tinyPiX_wf_get_display_size(set->agent, &rW, &rH);
    TpDollAREvent event;

    input.timestamp = events->dollarEvent.timestamp;
    input.touchID = events->dollarEvent.touchid;
    input.GestureID = events->dollarEvent.gestureid;
    input.numFingers = events->dollarEvent.numfingers;

    event.construct(&input);

    std::list<TpObject *>::iterator iter = list.begin();

    input.x = events->dollarEvent.x * rW - childObj->toScreen().x();
    input.y = events->dollarEvent.y * rH - childObj->toScreen().y();
    event.construct(&input);

    // 如果该对象安装了事件过滤器，先将事件传给事件过滤器
    IssueObjEvent(childObj, event, onDollAREvent, childObj->enabled());
}

static inline void broadMultiGesture(TpObjectData *set, ItpMultiGestureSet &input, TpObject *object, std::list<TpObject *> &list, ItpEvent *events)
{
    TpWidget *childObj = static_cast<TpWidget *>(object);
    if (!childObj)
        return;

    uint32_t rW = 0, rH = 0;
    tinyPiX_wf_get_display_size(set->agent, &rW, &rH);
    TpMultiGestureEvent event;

    input.timestamp = events->gestrueEvent.timestamp;
    input.touchID = events->gestrueEvent.touchid;
    input.dtheta = events->gestrueEvent.dtheta;
    input.ddist = events->gestrueEvent.ddist;
    input.numfingers = events->gestrueEvent.numfingers;
    input.padding = events->gestrueEvent.padding;

    event.construct(&input);

    std::list<TpObject *>::iterator iter = list.begin();

    TpWidget *setCurChildObj = static_cast<TpWidget *>(set->tmp.curObject);
    if (setCurChildObj)
    {
        input.x = events->gestrueEvent.x * rW - setCurChildObj->toScreen().x();
        input.y = events->gestrueEvent.y * rH - setCurChildObj->toScreen().y();
        event.construct(&input);
    }

    // 如果该对象安装了事件过滤器，先将事件传给事件过滤器
    IssueObjEvent(childObj, event, onMultiGestureEvent, childObj->enabled());
}

static inline bool splitTouchMousePoint(ItpEvent *event, TpPoint *point)
{
    switch (event->type)
    {
    case TP_MOUSEMOTION:
    {
        point->setX(event->mouseMotionEvent.x);
        point->setY(event->mouseMotionEvent.y);
    }
    break;
    case TP_MOUSEBUTTONDOWN:
    case TP_MOUSEBUTTONUP:
    {
        point->setX(event->mouseButtonEvent.x);
        point->setY(event->mouseButtonEvent.y);
    }
    break;
    case TP_FINGERMOTION:
    case TP_FINGERUP:
    case TP_FINGERDOWN:
    {
        point->setX(event->fingerEvent.x);
        point->setY(event->fingerEvent.y);
    }
    break;
    case TP_DOLLARGESTURE:
    case TP_DOLLARRECORD:
    {
        point->setX(event->dollarEvent.x);
        point->setY(event->dollarEvent.y);
    }
    break;
    case TP_MULTIGESTURE:
    {
        point->setX(event->gestrueEvent.x);
        point->setY(event->gestrueEvent.y);
    }
    break;
    default:
    {
        return false;
    }
    }

    return true;
}

static inline void doTransUpdate(void *args)
{
    TpScreen *object = (TpScreen *)args;
    TpObjectData *set = (TpObjectData *)object->objectSets();
    int32_t x, y;
    uint32_t w, h;
    tinyPiX_wf_get_rect(set->agent, &x, &y, &w, &h);
    object->update(x, y, w, h, true);
}

static inline int32_t transferEvent(int32_t id, void *event, void *args)
{
    ItpEvent *events = (ItpEvent *)event;
    TpScreen *object = (TpScreen *)args;

    return object->dispatchEvent(events);
}

static inline int32_t transferFocus(int32_t id, int32_t focused, void *args)
{
    TpWidget *object = (TpWidget *)args;
    TpFocusEvent event;
    ItpObjectFocusSet input;
    input.object = object;
    input.focused = focused;
    event.construct(&input);

    return object->onFocusEvent(&event);
}

static inline int32_t transferLeave(int32_t id, int32_t leaved, int mouseX, int mouseY, void *args)
{
    TpWidget *object = (TpWidget *)args;
    TpLeaveEvent event;
    ItpObjectLeaveSet input;
    input.object = nullptr;
    input.leaved = leaved;
    event.construct(&input);

    if (leaved == false)
    {
        // notice cur object, leave out
        TpObjectData *set = (TpObjectData *)object->objectSets();
        if (set->tmp.curmotion != object)
        {
            // 如果鼠标坐标没有移出当前对象区域，不触发leve事件
            if (set->tmp.curmotion && (!set->tmp.curmotion->rect().contains(mouseX, mouseY)))
            {
                // leaveout

                // 如果该对象安装了事件过滤器，先将事件传给事件过滤器
                // std::cout << " LeaveEvent11111111111 " << std::endl;
                IssueObjEvent(set->tmp.curmotion, event, onLeaveEvent, set->tmp.curmotion->enabled());
            }
        }
    }

    // 如果该对象安装了事件过滤器，先将事件传给事件过滤器
    IssueObjEvent(object, event, onLeaveEvent, object->enabled());

    return true;
}

static inline int32_t transferResize(int32_t id, uint32_t nw, uint32_t nh, int32_t question, void *args) // only for resolution
{
    TpWidget *object = (TpWidget *)args;
    TpResizeEvent event;
    ItpObjectResizeSet input;
    input.object = object;
    input.nw = nw;
    input.nh = nh;
    input.question = TpResizeEvent::TP_RESOLUTION_CHANGE;
    event.construct(&input);

    TpObjectData *set = (TpObjectData *)object->objectSets();

#if 1
    set->absoluteRect.setRect(0, 0, nw, nh);
    set->logicalRect.setRect(0, 0, nw, nh);
#endif

    if (!set->reserveImage.isNull())
    {
        bool ret = (nw > 0 && nh > 0);
        if (ret)
        {
            set->cacheImage = set->reserveImage.scaled(nw, nh);
        }

        doTransUpdate(object);
    }

    IssueObjEvent(object, event, onResizeEvent, true);

    return true;
}

static inline int32_t transferVisible(int32_t id, int32_t visible, void *args)
{
    TpWidget *object = (TpWidget *)args;
    TpVisibleEvent event;
    ItpObjectVisibleSet input;
    input.object = object;
    input.visible = visible;
    event.construct(&input);

    TpObjectData *set = (TpObjectData *)object->objectSets();

    if (set)
    {
        set->visible = visible;
    }

    object->onVisibleEvent(&event);
    doTransUpdate(object);

    return true;
}

static inline int32_t transferMoved(int32_t id, int32_t nx, int32_t ny, int32_t question, void *args)
{
    TpWidget *object = (TpWidget *)args;
    TpMoveEvent event;
    ItpObjectMoveSet input;
    input.object = object;
    input.nx = nx;
    input.ny = ny;
    event.construct(&input);

    if (object->objectType() == Tp::TP_FLOAT_OBJECT)
    {
        TpObjectData *set = (TpObjectData *)object->objectSets();

        set->absoluteRect.setX(nx);
        set->absoluteRect.setY(ny);

        object->broadSetTop();
        object->onMoveEvent(&event);
    }

    return true;
}

static inline int32_t transferActive(int32_t id, int32_t actived, void *args)
{
    TpWidget *object = (TpWidget *)args;
    TpApp::Inst()->sendActive(object, actived);

    TpActiveEvent event;
    ItpObjectActiveSet input;
    input.object = object;
    input.actived = actived;
    event.construct(&input);

    object->onActiveEvent(&event);
    doTransUpdate(object);

    return actived;
}

static inline int32_t transferQuit(int32_t id, int32_t question, void *args)
{
    TpWidget *object = (TpWidget *)args;
    object->deleteLater();
    return 1;
}

static inline int32_t transferReturn(int32_t id, void *args)
{
    TpWidget *object = (TpWidget *)args;
    return ((TpScreen *)object)->returns();
}

static inline int32_t transferAppState(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require, void *args)
{
    TpWidget *object = (TpWidget *)args;
    return object->appChange(id, pid, visible, active, color, alpha, require);
}

#endif
