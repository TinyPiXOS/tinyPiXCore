#include "TpScreen.h"
#include "TpEvent.h"
#include "TpApp.h"
#include "TpDef.h"
#include "TpLayout.h"
#include "TpObjectStack.h"
#include "TpTimer.h"
#include "TpChildWidget.h"
#include <TpCanvas.h>
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

// 记录鼠标按下时的对象，最后鼠标无论在哪释放，都触发按下对象的release
// static TpChildWidget *pressObject = nullptr;

// 鼠标左键长按回调
static std::function<void(TpChildWidget *, ItpMouseSet)> longPressCallback = [](TpChildWidget *obj, ItpMouseSet mouseSet)
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

static inline void startLongPressCheck(TpChildWidget *object, const ItpMouseSet &mouseSet)
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

static inline void broadMotion(TpObject *dragObject, TpObject *curMotionObject, std::list<TpObject *> &list, ItpEvent *events, TpChildWidget *pressObject)
{
    TpMouseEvent motionEvent;
    ItpMouseSet mInput;
    mInput.which = events->mouseMotionEvent.which;
    mInput.type = TpEvent::EVENT_MOUSE_MOVE_TYPE;

    std::list<TpObject *>::iterator iter = list.begin();

    TpChildWidget *childObj = static_cast<TpChildWidget *>(dragObject);
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

    TpChildWidget *childMotionObj = static_cast<TpChildWidget *>(curMotionObject);

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

static inline void broadMouseKey(TpObject *object, std::list<TpObject *> &list, ItpEvent *events, TpChildWidget *pressObject)
{
    ItpMouseSet mInput;
    mInput.which = events->mouseButtonEvent.which;
    mInput.button = events->mouseButtonEvent.button;
    mInput.state = events->mouseButtonEvent.state;
    mInput.type = mInput.state ? TpEvent::EVENT_MOUSE_PRESS_TYPE : TpEvent::EVENT_MOUSE_RELEASE_TYPE;

    TpChildWidget *childObj = static_cast<TpChildWidget *>(object);
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

static inline void broadFinger(ItpObjectSet *set, ItpFingerSet &input, TpObject *object, std::list<TpObject *> &list, ItpEvent *events)
{
    TpChildWidget *childObj = static_cast<TpChildWidget *>(object);
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

static inline void broaDollar(ItpObjectSet *set, ItpDollarSet &input, TpObject *object, std::list<TpObject *> &list, ItpEvent *events)
{
    TpChildWidget *childObj = static_cast<TpChildWidget *>(object);
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

static inline void broadMultiGesture(ItpObjectSet *set, ItpMultiGestureSet &input, TpObject *object, std::list<TpObject *> &list, ItpEvent *events)
{
    TpChildWidget *childObj = static_cast<TpChildWidget *>(object);
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

    TpChildWidget *setCurChildObj = static_cast<TpChildWidget *>(set->tmp.curObject);
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
    ItpObjectSet *set = (ItpObjectSet *)object->objectSets();
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
    TpChildWidget *object = (TpChildWidget *)args;
    TpFocusEvent event;
    ItpObjectFocusSet input;
    input.object = object;
    input.focused = focused;
    event.construct(&input);

    return object->onFocusEvent(&event);
}

static inline int32_t transferLeave(int32_t id, int32_t leaved, int mouseX, int mouseY, void *args)
{
    TpChildWidget *object = (TpChildWidget *)args;
    TpLeaveEvent event;
    ItpObjectLeaveSet input;
    input.object = nullptr;
    input.leaved = leaved;
    event.construct(&input);

    if (leaved == false)
    {
        // notice cur object, leave out
        ItpObjectSet *set = (ItpObjectSet *)object->objectSets();
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
    TpChildWidget *object = (TpChildWidget *)args;
    TpResizeEvent event;
    ItpObjectResizeSet input;
    input.object = object;
    input.nw = nw;
    input.nh = nh;
    input.question = TpResizeEvent::TP_RESOLUTION_CHANGE;
    event.construct(&input);

    ItpObjectSet *set = (ItpObjectSet *)object->objectSets();

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

            // if (set->cacheImage)
            // {
            // 	set->cacheImage->setAlpha(set->reserveImage->alpha());
            // }
        }

        doTransUpdate(object);
    }

    IssueObjEvent(object, event, onResizeEvent, true);

    return true;
}

static inline int32_t transferVisible(int32_t id, int32_t visible, void *args)
{
    TpChildWidget *object = (TpChildWidget *)args;
    TpVisibleEvent event;
    ItpObjectVisibleSet input;
    input.object = object;
    input.visible = visible;
    event.construct(&input);

    ItpObjectSet *set = (ItpObjectSet *)object->objectSets();

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
    TpChildWidget *object = (TpChildWidget *)args;
    TpMoveEvent event;
    ItpObjectMoveSet input;
    input.object = object;
    input.nx = nx;
    input.ny = ny;
    event.construct(&input);

    if (object->objectType() == TP_FLOAT_OBJECT)
    {
        ItpObjectSet *set = (ItpObjectSet *)object->objectSets();

        set->absoluteRect.setX(nx);
        set->absoluteRect.setY(ny);

        object->broadSetTop();
        object->onMoveEvent(&event);
    }

    return true;
}

static inline int32_t transferActive(int32_t id, int32_t actived, void *args)
{
    TpChildWidget *object = (TpChildWidget *)args;
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
    TpChildWidget *object = (TpChildWidget *)args;
    object->deleteLater();
    return 1;
}

static inline int32_t transferReturn(int32_t id, void *args)
{
    TpChildWidget *object = (TpChildWidget *)args;
    return ((TpScreen *)object)->returns();
}

static inline int32_t transferAppState(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require, void *args)
{
    TpChildWidget *object = (TpChildWidget *)args;
    return object->appChange(id, pid, visible, active, color, alpha, require);
}

TpScreen::TpScreen(const char *type, int32_t x, int32_t y, uint32_t w, uint32_t h)
    : TpChildWidget(nullptr)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    if (!set)
        return;

    set->agent = tinyPiX_wf_create(type, x, y, w, h);

    if (set->agent == nullptr)
    {
        this->close();
    }
    else
    {
        tinyPiX_wf_args_assign(set->agent, this);

        tinyPiX_wf_event_assign(set->agent, transferEvent);
        tinyPiX_wf_focus_assign(set->agent, transferFocus);
        tinyPiX_wf_leave_assign(set->agent, transferLeave);
        tinyPiX_wf_resize_assign(set->agent, transferResize);
        tinyPiX_wf_visible_assign(set->agent, transferVisible);
        tinyPiX_wf_moved_assign(set->agent, transferMoved);
        tinyPiX_wf_actived_assign(set->agent, transferActive);
        tinyPiX_wf_quit_assign(set->agent, transferQuit);
        tinyPiX_wf_return_assign(set->agent, transferReturn);
        tinyPiX_wf_app_assign(set->agent, transferAppState);

        set->top = this;
        tinyPiX_wf_get_rect(set->agent, &x, &y, &w, &h);

        set->offsetX = x;
        set->offsetY = y;

        set->absoluteRect.setRect(x, y, w, h);
        set->logicalRect.setRect(0, 0, w, h);

        if (set->top)
        {
            this->broadSetTop();
        }
    }

    tinyPiX_wf_set_visible(set->agent, true);
}

TpScreen::~TpScreen()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (set)
    {
        if (set->agent)
        {
            tinyPiX_wf_free(set->agent);
            set->agent = nullptr;
        }
    }
}

void TpScreen::setVisible(bool visible)
{
    TpChildWidget::setVisible(visible);

    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (!set)
        return;

    // if (set->visible != visible)
    {
        tinyPiX_wf_set_visible(set->agent, visible);
        set->visible = visible;
    }

    if (visible == false)
    {
        TpChildWidget *fixedScreen = TpApp::Inst()->vScreen();
        if (fixedScreen)
        {
            fixedScreen->update();
        }
    }

    update();
}

bool TpScreen::actived()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    bool actived = false;

    if (set)
    {
        actived = tinyPiX_wf_get_active(set->agent);
    }

    return actived;
}

void TpScreen::setText(const char *text)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (set)
    {
        bool ret = tinyPiX_wf_set_title(set->agent, text);

        if (ret)
        {
            TpChildWidget::setText(text);
        }
    }
}

void TpScreen::setText(const TpString &text)
{
    this->setText(text.c_str());
}

void TpScreen::setRect(const int32_t &x, const int32_t &y, const uint32_t &w, const uint32_t &h)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (!set)
        return;

    tinyPiX_wf_set_rect(set->agent, x, y, w, h);

    set->offsetX = x;
    set->offsetY = y;

    TpChildWidget::setRect(x, y, w, h);
}

void TpScreen::setSize(const uint32_t &width, const uint32_t &height)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (!set)
        return;

    tinyPiX_wf_set_rect(set->agent, set->offsetX, set->offsetY, width, height);

    TpChildWidget::setSize(width, height);
}

void TpScreen::setWidth(const uint32_t &width)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (!set)
        return;

    tinyPiX_wf_set_rect(set->agent, set->offsetX, set->offsetY, width, height());

    TpChildWidget::setWidth(width);
}

void TpScreen::setHeight(const uint32_t &height)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (!set)
        return;

    tinyPiX_wf_set_rect(set->agent, set->offsetX, set->offsetY, width(), height);

    TpChildWidget::setHeight(height);
}

void TpScreen::move(int32_t x, int32_t y)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (set)
    {
        ItpObjectSysLayer layer = (ItpObjectSysLayer)tinyPiX_wf_get_layer(set->agent);

        if (layer >= TP_WM_USE_FLOAT)
        {
            int32_t ox = 0, oy = 0;

            tinyPiX_wf_get_rect(set->agent, &ox, &oy, nullptr, nullptr);
            tinyPiX_wf_set_position(set->agent, x, y);

            set->offsetX = x;
            set->offsetY = y;

            set->logicalRect.setX(0);
            set->logicalRect.setY(0);

            set->absoluteRect.setX(x);
            set->absoluteRect.setY(y);

            this->broadSetTop();
        }
    }
}

void TpScreen::setBeMoved(bool moved)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (set)
    {
        ItpObjectSysLayer layer = (ItpObjectSysLayer)tinyPiX_wf_get_layer(set->agent);

        if (layer >= TP_WM_USE_FLOAT)
        {
            tinyPiX_wf_set_bemoved(set->agent, moved);
        }
    }
}

bool TpScreen::moved()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    bool moved = false;

    if (set)
    {
        moved = tinyPiX_wf_get_bemoved(set->agent);
    }

    return moved;
}

void TpScreen::setAlpha(const uint8_t &alpha)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (set)
    {
        tinyPiX_wf_set_alpha(set->agent, alpha);
        set->alpha = alpha;
    }
}

void TpScreen::bringToTop()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (set)
    {
        tinyPiX_wf_bring_to_top(set->agent);
    }
}

void TpScreen::bringToBottom()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (set)
    {
        tinyPiX_wf_bring_to_bottom(set->agent);
    }
}

void TpScreen::update(int32_t x, int32_t y, int32_t w, int32_t h, bool onlyBlit)
{
    TpApp::Inst()->postUpdateEvent(this, x, y, w, h, onlyBlit);
}

void TpScreen::update(bool onlyBlit)
{
    update(this->toScreen().x(), this->toScreen().y(), this->width(), this->height(), onlyBlit);
}

ItpObjectType TpScreen::objectType()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    ItpObjectType type = TP_UNKOWN_OBJECT;

    if (set)
    {
        ItpObjectSysLayer layer = (ItpObjectSysLayer)tinyPiX_wf_get_layer(set->agent);

        switch (layer)
        {
        case TP_WM_DESK:
        case TP_WM_WIN:
        {
            type = TP_TOP_OBJECT;
        }
        break;
        case TP_WM_USE_FLOAT:
        case TP_WM_SYS_FLOAT:
        {
            type = TP_FLOAT_OBJECT;
        }
        break;
        }
    }

    return type;
}

ItpObjectSysLayer TpScreen::objectLayer()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    ItpObjectSysLayer layer = TP_WM_NONE;

    if (set)
    {
        layer = (ItpObjectSysLayer)tinyPiX_wf_get_layer(set->agent);
    }

    return layer;
}

int32_t TpScreen::objectSysID()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    int32_t id = TP_INVALIDATE_VALUE;

    if (set)
    {
        id = tinyPiX_wf_get_id(set->agent);
    }

    return id;
}

bool TpScreen::objectActive()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    bool actived = false;

    if (set)
    {
        actived = tinyPiX_wf_get_active(set->agent);
    }

    return actived;
}

void TpScreen::setParent(TpObject *parent)
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    if (set)
    {
        set->parent = nullptr;
    }
}

TpObject *TpScreen::parent()
{
    return nullptr;
}

TpObject *TpScreen::topObject()
{
    return this;
}

void TpScreen::deleteLater()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    bool exitOK = true;

    if (!set)
        return;

    ItpUserEvent message;

    switch (this->objectType())
    {
    case TP_TOP_OBJECT:
    {
        message.type = TpApp::TP_ABORT_ACT;
        TpApp::Inst()->sendAbort(this);
    }
    break;
    case TP_FLOAT_OBJECT:
    {
        message.type = TpApp::TP_DELETE_ACT;
        TpApp::Inst()->sendDelete(this);
    }
    break;
    default:
        exitOK = false;
    }
}

bool TpScreen::returns()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    bool returns = true;

    if (set)
    {
        ItpUserEvent message;

        switch (this->objectType())
        {
        case TP_TOP_OBJECT:
        {
            message.type = TpApp::TP_RETURN_ACT;
            TpApp::Inst()->sendReturn(this);
        }
        break;
        default:
            returns = false;
        }
    }

    return returns;
}

TpSize TpScreen::screenSize()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    uint32_t sWidth = 0;
    uint32_t sHeight = 0;

    if (set)
    {
        tinyPiX_wf_get_display_size(set->agent, &sWidth, &sHeight);
    }

    return TpSize(sWidth, sHeight);
}

int32_t TpScreen::screenWidth()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    uint32_t sWidth = 0;

    if (set)
    {
        tinyPiX_wf_get_display_size(set->agent, &sWidth, nullptr);
    }

    return sWidth;
}

int32_t TpScreen::screenHeight()
{
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();
    uint32_t sHeight = 0;

    if (set)
    {
        tinyPiX_wf_get_display_size(set->agent, nullptr, &sHeight);
    }

    return sHeight;
}

int32_t TpScreen::dispatchEvent(void *events)
{
    ItpEvent *eventPtr = (ItpEvent *)events;

    TpPoint point;
    ItpObjectSet *set = (ItpObjectSet *)TpObject::objectSets();

    bool ret = false;

    int32_t eventMask = TpApp::Inst()->disableEventType();
    ret = splitTouchMousePoint(eventPtr, &point);

    if (ret == false)
    {
        switch (eventPtr->type)
        {
        case TP_KEYDOWN:
        case TP_KEYUP:
        {
            if ((eventMask & TpApp::TP_DIS_KEYBOARD) == TpApp::TP_DIS_KEYBOARD)
            {
                return false;
            }

            if (set->tmp.curfocus && set->tmp.curfocus->enabled())
            {
                TpKeyboardEvent event;
                ItpKeyboardSet input;
                input.which = eventPtr->keyboardEvent.which;
                input.state = eventPtr->keyboardEvent.state;
                input.scancode = eventPtr->keyboardEvent.keysym.scancode;
                input.virtualKey = eventPtr->keyboardEvent.keysym.virtualkey;
                input.symbol = eventPtr->keyboardEvent.keysym.symbol;
                input.keyMod = eventPtr->keyboardEvent.keysym.keymod;
                memset(input.shortCut, 0, strlen((char *)eventPtr->keyboardEvent.keysym.shortcut));
                memcpy(input.shortCut, eventPtr->keyboardEvent.keysym.shortcut, strlen((char *)eventPtr->keyboardEvent.keysym.shortcut));

                input.type = input.state ? TpEvent::EVENT_KEYBOARD_PRESS_TYPE : TpEvent::EVENT_KEYBOARD_RELEASE_TYPE;

                event.construct(&input);

                if (input.state)
                {
                    IssueObjEvent(set->tmp.curfocus, event, onKeyPressEvent, set->tmp.curfocus->enabled());
                }
                else
                {
                    IssueObjEvent(set->tmp.curfocus, event, onKeyReleaseEvent, set->tmp.curfocus->enabled());
                }
            }
        }
        break;
        }

        return true;
    }

    set->tmp.curObject = this->find(point);

    switch (eventPtr->type)
    {
    case TP_MOUSEMOTION:
    {
        // std::cout << "TP_MOUSEMOTION " << std::endl;
        if ((eventMask & TpApp::TP_DIS_MOTION) == TpApp::TP_DIS_MOTION)
        {
            return false;
        }

        std::list<TpObject *> motionList;

        set->tmp.curmotion = set->tmp.curObject;
        TpLeaveEvent leaveEvent;
        ItpObjectLeaveSet lInput;

        if (set->tmp.curmotion != set->tmp.lstmotion)
        {
            if (set->tmp.curmotion)
            {
                lInput.object = set->tmp.lstmotion;
                lInput.leaved = true;
                leaveEvent.construct(&lInput);

                // 如果lstmotion为空，则必然要触发当前进入的对象的leaveIn事件
                // 但是如果鼠标上一针坐标已经在当前对象了，就不需要重复触发
                if (set->tmp.lstmotion)
                {
                    TpRect curMotionRect = set->tmp.curmotion->toScreen();
                    if (!curMotionRect.contains(set->tmp.lastPoint))
                    {
                        IssueObjEvent(set->tmp.curmotion, leaveEvent, onLeaveEvent, set->tmp.curmotion->enabled());
                    }
                }
                else
                {
                    IssueObjEvent(set->tmp.curmotion, leaveEvent, onLeaveEvent, set->tmp.curmotion->enabled());
                }

                // 判断是否也进入了当前对象的父对象
                TpChildWidget *curParent = dynamic_cast<TpChildWidget *>(set->tmp.curmotion->parent());
                while (curParent)
                {
                    TpRect curParentRect = curParent->toScreen();

                    /*  如果上一个对象为空，说明是程序刚启动，第一次进入，直接触发所有的leaveIn即可
                        如果不为空，则需要判断，如果上一个鼠标坐标不在该窗口，当前坐标在该窗口则触发leaveIn，否则不触发*/
                    if (set->tmp.lstmotion)
                    {
                        if (curParentRect.contains(point) && !curParentRect.contains(set->tmp.lastPoint))
                        {
                            IssueObjEvent(curParent, leaveEvent, onLeaveEvent, curParent->enabled());
                        }
                    }
                    else
                    {
                        IssueObjEvent(curParent, leaveEvent, onLeaveEvent, curParent->enabled());
                    }

                    curParent = dynamic_cast<TpChildWidget *>(curParent->parent());
                }
            }

            if (set->tmp.lstmotion)
            {
                lInput.object = set->tmp.lstmotion;
                lInput.leaved = false;
                leaveEvent.construct(&lInput);

                // 根据当前鼠标坐标判断是否也离开了上一个对象及父对象
                TpChildWidget *curParent = set->tmp.lstmotion;
                while (curParent)
                {
                    TpRect curParentRect = curParent->toScreen();

                    /*  如果当前鼠标也离开了上一个对象的父对象，则也触发上一个对象的父对象的leaveOut事件 */
                    if (!curParentRect.contains(point))
                    {
                        IssueObjEvent(curParent, leaveEvent, onLeaveEvent, curParent->enabled());
                    }

                    curParent = dynamic_cast<TpChildWidget *>(curParent->parent());
                }
            }

            set->tmp.lastPoint = point;
            set->tmp.lstmotion = set->tmp.curmotion;
        }

        generateParentList(set->tmp.curmotion, motionList);

        broadMotion(set->mousePressObject, set->tmp.curmotion, motionList, eventPtr, set->mousePressObject);
        // broadMotion(set->tmp.dragObject, set->tmp.curmotion, motionList, eventPtr);

        // 鼠标移动取消长按事件
        stopLongPressCheck();
        // std::cout << " this Ptr " << this << std::endl;
    }
    break;
    case TP_MOUSEBUTTONDOWN:
    case TP_MOUSEBUTTONUP:
    {
        if ((eventMask & TpApp::TP_DIS_MOUSE) == TpApp::TP_DIS_MOUSE)
        {
            return false;
        }

        this->find(point);

        std::list<TpObject *> keyList;
        set->tmp.curfocus = set->tmp.curObject;
        TpFocusEvent focusEvent;
        ItpObjectFocusSet fInput;

        if (set->tmp.curfocus != set->tmp.lstfocus)
        {
            if (set->tmp.curfocus && set->tmp.curfocus->enabled())
            {
                // obtain focus
                fInput.object = set->tmp.lstfocus;
                fInput.focused = true;
                focusEvent.construct(&fInput);

                IssueObjEvent(set->tmp.curfocus, focusEvent, onFocusEvent, set->tmp.curfocus->enabled());
            }

            if (set->tmp.lstfocus && set->tmp.lstfocus->enabled())
            {
                // lost focus
                fInput.object = set->tmp.curfocus;
                fInput.focused = false;
                focusEvent.construct(&fInput);

                IssueObjEvent(set->tmp.lstfocus, focusEvent, onFocusEvent, set->tmp.lstfocus->enabled());
            }

            set->tmp.lstfocus = set->tmp.curfocus;
        }

        generateParentList(set->tmp.curmotion, keyList);
        broadMouseKey(set->tmp.curfocus, keyList, eventPtr, set->mousePressObject);

        set->mousePressObject = eventPtr->mouseButtonEvent.state ? set->tmp.curfocus : nullptr;

        // set->tmp.dragObject = (eventPtr->type == TP_MOUSEBUTTONDOWN) ? set->tmp.curObject : nullptr;
        // std::cout << " set->tmp.dragObject " << set->tmp.dragObject << std::endl;
    }
    break;
    case TP_FINGERDOWN:
    case TP_FINGERUP:
    case TP_FINGERMOTION:
    {
        if ((eventMask & TpApp::TP_DIS_FINGER) == TpApp::TP_DIS_FINGER)
        {
            return false;
        }

        // don't know how to do
        std::list<TpObject *> fingerList;
        ItpFingerSet input;

        switch (eventPtr->type)
        {
        case TP_FINGERDOWN:
        {
            input.touchFingerType = TpFingerEvent::TOUCH_FINGER_DOWN;
        }
        break;
        case TP_FINGERUP:
        {
            input.touchFingerType = TpFingerEvent::TOUCH_FINGER_UP;
        }
        break;
        case TP_FINGERMOTION:
        {
            input.touchFingerType = TpFingerEvent::TOUCH_FINGER_MOTION;
        }
        break;
        }

        generateParentList(set->tmp.curObject, fingerList);
        broadFinger(set, input, set->tmp.curObject, fingerList, eventPtr);
    }
    break;
    case TP_DOLLARGESTURE:
    case TP_DOLLARRECORD:
    {
        if ((eventMask & TpApp::TP_DIS_DOLLAR) == TpApp::TP_DIS_DOLLAR)
        {
            return false;
        }

        // don't know how to do
        if (set->tmp.curObject == nullptr)
        {
            return false;
        }
        std::list<TpObject *> dollarList;
        ItpDollarSet input;
        switch (eventPtr->type)
        {
        case TP_DOLLARGESTURE:
        {
            input.dollarType = TpDollAREvent::TOUCH_DOLLAR_GESTURE;
        }
        break;
        case TP_DOLLARRECORD:
        {
            input.dollarType = TpDollAREvent::TOUCH_DOLLAR_RECORD;
        }
        break;
        }

        generateParentList(set->tmp.curObject, dollarList);
        broaDollar(set, input, set->tmp.curObject, dollarList, eventPtr);
    }
    break;
    case TP_MULTIGESTURE:
    {
        if ((eventMask & TpApp::TP_DIS_GESTURE) == TpApp::TP_DIS_GESTURE)
        {
            return false;
        }

        // don't know how to do
        if (set->tmp.curObject == nullptr)
        {
            return false;
        }

        std::list<TpObject *> multiList;
        ItpMultiGestureSet input;

        generateParentList(set->tmp.curObject, multiList);
        broadMultiGesture(set, input, set->tmp.curObject, multiList, eventPtr);
    }
    break;
    default:
        return false;
    }

    return true;
}
