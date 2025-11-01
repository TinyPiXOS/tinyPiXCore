#include "TpScreen.h"
#include "TpChildWidget_p.h"
#include "TpScreen_p.h"

TpScreen::TpScreen(const char *type, int32_t x, int32_t y, uint32_t w, uint32_t h)
    : TpWidget(nullptr)
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    if (!set)
        return;

    set->objectType = type;
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

    // tinyPiX_wf_set_visible(set->agent, true);
}

TpScreen::~TpScreen()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

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
    TpWidget::setVisible(visible);

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (!set)
        return;

    // if (set->visible != visible)
    {
        tinyPiX_wf_set_visible(set->agent, visible);
        set->visible = visible;
    }

    if (visible == false)
    {
        TpWidget *mainScreen = TpApp::Inst()->mainWindow();
        if (mainScreen)
        {
            mainScreen->update();
        }
    }

    update();
}

bool TpScreen::actived()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    bool actived = false;

    if (set)
    {
        actived = tinyPiX_wf_get_active(set->agent);
    }

    return actived;
}

void TpScreen::setText(const char *text)
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (set)
    {
        bool ret = tinyPiX_wf_set_title(set->agent, text);

        if (ret)
        {
            TpWidget::setText(text);
        }
    }
}

void TpScreen::setText(const TpString &text)
{
    this->setText(text.c_str());
}

void TpScreen::setRect(const TpRect &rect)
{
    setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void TpScreen::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    // TpMainWindow不可被调整大小
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (!set)
        return;

    tinyPiX_wf_set_rect(set->agent, x, y, w, h);

    set->offsetX = x;
    set->offsetY = y;

    TpWidget::setRect(x, y, w, h);
}

void TpScreen::setSize(const int32_t &width, const int32_t &height)
{
    // TpMainWindow不可被调整大小
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (!set)
        return;

    tinyPiX_wf_set_rect(set->agent, set->offsetX, set->offsetY, width, height);

    TpWidget::setSize(width, height);
}

void TpScreen::setWidth(const int32_t &width)
{
    // TpMainWindow不可被调整大小
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (!set)
        return;

    tinyPiX_wf_set_rect(set->agent, set->offsetX, set->offsetY, width, height());

    TpWidget::setWidth(width);
}

void TpScreen::setHeight(const int32_t &height)
{
    // TpMainWindow不可被调整大小
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (!set)
        return;

    tinyPiX_wf_set_rect(set->agent, set->offsetX, set->offsetY, width(), height);

    TpWidget::setHeight(height);
}

void TpScreen::move(int32_t x, int32_t y)
{
    // TpMainWindow不可被移动坐标
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    if (!set)
        return;

    Tp::TpObjectSysLayer layer = (Tp::TpObjectSysLayer)tinyPiX_wf_get_layer(set->agent);

    if (layer >= Tp::TP_WM_USE_FLOAT)
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

    update();
}

const TpPoint TpScreen::pos()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    if (!set)
        return TpPoint();

    return TpPoint(set->absoluteRect.x(), set->absoluteRect.y());
}

void TpScreen::setBeMoved(bool moved)
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (set)
    {
        Tp::TpObjectSysLayer layer = (Tp::TpObjectSysLayer)tinyPiX_wf_get_layer(set->agent);

        if (layer >= Tp::TP_WM_USE_FLOAT)
        {
            tinyPiX_wf_set_bemoved(set->agent, moved);
        }
    }
}

bool TpScreen::moved()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    bool moved = false;

    if (set)
    {
        moved = tinyPiX_wf_get_bemoved(set->agent);
    }

    return moved;
}

void TpScreen::setWindowOpacity(float opacity)
{
    TpWidget::setWindowOpacity(opacity);

    // TODU 屏蔽旧版本针对于dialog的透明度设置，测试新版接口无误后可删除此注释代码
    // TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    // tinyPiX_wf_set_alpha(set->agent, 255 * opacity);
}

void TpScreen::bringToTop()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (set)
    {
        tinyPiX_wf_bring_to_top(set->agent);
    }
}

void TpScreen::bringToBottom()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (set)
    {
        tinyPiX_wf_bring_to_bottom(set->agent);
    }
}

void TpScreen::update(int32_t x, int32_t y, int32_t w, int32_t h, bool onlyBlit)
{
    if (!visible())
        return;

    TpApp::Inst()->postUpdateEvent(this, x, y, w, h, onlyBlit);
}

void TpScreen::update(bool onlyBlit)
{
    update(this->toScreen().x(), this->toScreen().y(), this->width(), this->height(), onlyBlit);
}

Tp::TpObjectType TpScreen::objectType()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    Tp::TpObjectType type = Tp::TP_UNKOWN_OBJECT;

    if (set)
    {
        Tp::TpObjectSysLayer layer = (Tp::TpObjectSysLayer)tinyPiX_wf_get_layer(set->agent);

        switch (layer)
        {
        case Tp::TP_WM_DESK:
        case Tp::TP_WM_WIN:
        {
            type = Tp::TP_MAIN_WINDOW_OBJECT;
        }
        break;
        case Tp::TP_WM_USE_FLOAT:
        case Tp::TP_WM_SYS_FLOAT:
        {
            type = Tp::TP_FLOAT_OBJECT;
        }
        break;
        }
    }

    return type;
}

Tp::TpObjectSysLayer TpScreen::objectLayer()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    Tp::TpObjectSysLayer layer = Tp::TP_WM_NONE;

    if (set)
    {
        layer = (Tp::TpObjectSysLayer)tinyPiX_wf_get_layer(set->agent);
    }

    return layer;
}

int32_t TpScreen::objectSysID()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    int32_t id = TP_INVALIDATE_VALUE;

    if (set)
    {
        id = tinyPiX_wf_get_id(set->agent);
    }

    return id;
}

bool TpScreen::objectActive()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    bool actived = false;

    if (set)
    {
        actived = tinyPiX_wf_get_active(set->agent);
    }

    return actived;
}

void TpScreen::setParent(TpObject *parent)
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

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
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    bool exitOK = true;

    if (!set)
        return;

    ItpUserEvent message;

    switch (this->objectType())
    {
    case Tp::TP_FIXSCREEN_OBJECT:
    {
        message.type = TpApp::TP_ABORT_ACT;
        TpApp::Inst()->sendAbort(this);
    }
    break;
    case Tp::TP_MAIN_WINDOW_OBJECT:
    {
        message.type = TpApp::TP_DELETE_ACT;
        TpApp::Inst()->sendDelete(this);
    }
    break;
    case Tp::TP_FLOAT_OBJECT:
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
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    bool returns = true;

    if (set)
    {
        ItpUserEvent message;

        switch (this->objectType())
        {
        case Tp::TP_MAIN_WINDOW_OBJECT:
        case Tp::TP_FIXSCREEN_OBJECT:
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
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
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
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    uint32_t sWidth = 0;

    if (set)
    {
        tinyPiX_wf_get_display_size(set->agent, &sWidth, nullptr);
    }

    return sWidth;
}

int32_t TpScreen::screenHeight()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
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
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

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
                TpWidget *curParent = dynamic_cast<TpWidget *>(set->tmp.curmotion->parent());
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

                    curParent = dynamic_cast<TpWidget *>(curParent->parent());
                }
            }

            if (set->tmp.lstmotion)
            {
                lInput.object = set->tmp.lstmotion;
                lInput.leaved = false;
                leaveEvent.construct(&lInput);

                // 根据当前鼠标坐标判断是否也离开了上一个对象及父对象
                TpWidget *curParent = set->tmp.lstmotion;
                while (curParent)
                {
                    TpRect curParentRect = curParent->toScreen();

                    /*  如果当前鼠标也离开了上一个对象的父对象，则也触发上一个对象的父对象的leaveOut事件 */
                    if (!curParentRect.contains(point))
                    {
                        IssueObjEvent(curParent, leaveEvent, onLeaveEvent, curParent->enabled());
                    }

                    curParent = dynamic_cast<TpWidget *>(curParent->parent());
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

        // this->find(point);

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
