#include "TpScreen.h"
#include "TpScreen_p.h"

TpScreen::TpScreen(const char *type, int32_t x, int32_t y, uint32_t w, uint32_t h)
    : TpWidget(nullptr)
{
    TpScreenData *screenData = new TpScreenData();

    // 移除父类的数据；创建widget的指针
    TpWidgetData *objData = static_cast<TpWidgetData *>(TpObject::data_);
    if (objData)
    {
        delete objData;
        objData = nullptr;
        TpObject::data_ = nullptr;
    }
    TpObject::data_ = screenData;

    TpWidget::initTpData();

    screenData->objectType = type;
    screenData->agent = tinyPiX_wf_create(type, x, y, w, h);

    if (screenData->agent == nullptr)
    {
        this->close();
    }
    else
    {
        // 获取物理尺寸
        uint32_t rW, rH;
        tinyPiX_wf_get_display_size(screenData->agent, &rW, &rH);
        screenData->displaySize.setWidth(rW);
        screenData->displaySize.setHeight(rH);

        tinyPiX_wf_args_assign(screenData->agent, this);

        tinyPiX_wf_event_assign(screenData->agent, transferEvent);
        tinyPiX_wf_focus_assign(screenData->agent, transferFocus);
        tinyPiX_wf_leave_assign(screenData->agent, transferLeave);
        tinyPiX_wf_resize_assign(screenData->agent, transferResize);
        tinyPiX_wf_visible_assign(screenData->agent, transferVisible);
        tinyPiX_wf_moved_assign(screenData->agent, transferMoved);
        tinyPiX_wf_actived_assign(screenData->agent, transferActive);
        tinyPiX_wf_quit_assign(screenData->agent, transferQuit);
        tinyPiX_wf_return_assign(screenData->agent, transferReturn);
        tinyPiX_wf_app_assign(screenData->agent, transferAppState);

        screenData->top = this;
        tinyPiX_wf_get_rect(screenData->agent, &x, &y, &w, &h);

        screenData->offsetX = x;
        screenData->offsetY = y;

        screenData->absoluteRect.setRect(x, y, w, h);
        screenData->logicalRect.setRect(0, 0, w, h);

        if (screenData->top)
        {
            this->broadSetTop();
        }

        screenData->swCanvas = tvg::SwCanvas::gen();
        screenData->swCanvas->push(screenData->tvgScene);
    }
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

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    // if (set->visible != visible)
    {
        tinyPiX_wf_set_visible(widgetData->agent, visible);
        widgetData->visible = visible;
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

void TpScreen::setRect(const TpRect &rect)
{
    setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void TpScreen::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    // TpMainWindow不可被调整大小
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    tinyPiX_wf_set_rect(widgetData->agent, x, y, w, h);

    widgetData->offsetX = x;
    widgetData->offsetY = y;

    TpWidget::setRect(x, y, w, h);
}

void TpScreen::setSize(const int32_t &width, const int32_t &height)
{
    // TpMainWindow不可被调整大小
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    tinyPiX_wf_set_rect(widgetData->agent, widgetData->offsetX, widgetData->offsetY, width, height);

    TpWidget::setSize(width, height);

    // dialog尺寸变化时需全局刷新保证无残留
    update(0, 0, widgetData->displaySize.width(), widgetData->displaySize.height());
}

void TpScreen::setWidth(const int32_t &width)
{
    // TpMainWindow不可被调整大小
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    tinyPiX_wf_set_rect(widgetData->agent, widgetData->offsetX, widgetData->offsetY, width, height());

    TpWidget::setWidth(width);

    // dialog尺寸变化时需全局刷新保证无残留
    update(0, 0, widgetData->displaySize.width(), widgetData->displaySize.height());
}

void TpScreen::setHeight(const int32_t &height)
{
    // TpMainWindow不可被调整大小
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    tinyPiX_wf_set_rect(widgetData->agent, widgetData->offsetX, widgetData->offsetY, width(), height);

    TpWidget::setHeight(height);

    // dialog尺寸变化时需全局刷新保证无残留
    update(0, 0, widgetData->displaySize.width(), widgetData->displaySize.height());
}

void TpScreen::move(int32_t x, int32_t y)
{
    // TpMainWindow不可被移动坐标
    if (pluginType().compare(TO_STRING(TpMainWindow)) == 0)
        return;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    Tp::TpObjectSysLayer layer = (Tp::TpObjectSysLayer)tinyPiX_wf_get_layer(widgetData->agent);

    if (layer >= Tp::TP_WM_USE_FLOAT)
    {
        // int32_t ox = 0, oy = 0;

        // tinyPiX_wf_get_rect(widgetData->agent, &ox, &oy, nullptr, nullptr);
        tinyPiX_wf_set_position(widgetData->agent, x, y);

        widgetData->offsetX = x;
        widgetData->offsetY = y;

        widgetData->logicalRect.setX(0);
        widgetData->logicalRect.setY(0);

        widgetData->absoluteRect.setX(x);
        widgetData->absoluteRect.setY(y);

        this->broadSetTop();
    }

    // dialog移动时需全局刷新保证无残留
    update(0, 0, widgetData->displaySize.width(), widgetData->displaySize.height());
}

const TpPoint TpScreen::pos()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpPoint();

    return TpPoint(widgetData->absoluteRect.x(), widgetData->absoluteRect.y());
}

void TpScreen::setBeMoved(bool moved)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    Tp::TpObjectSysLayer layer = (Tp::TpObjectSysLayer)tinyPiX_wf_get_layer(widgetData->agent);

    if (layer >= Tp::TP_WM_USE_FLOAT)
    {
        tinyPiX_wf_set_bemoved(widgetData->agent, moved);
    }
}

bool TpScreen::moved()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return false;

    tinyPiX_wf_get_bemoved(widgetData->agent);
    return true;
}

void TpScreen::bringToTop()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    tinyPiX_wf_bring_to_top(widgetData->agent);
}

void TpScreen::bringToBottom()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    tinyPiX_wf_bring_to_bottom(widgetData->agent);
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

bool TpScreen::onResizeEvent(TpResizeEvent *event)
{
    TpScreenData *screenData = static_cast<TpScreenData *>(TpObject::data_);
    if (!screenData)
        return true;

    // 初始化canvas与surface的绑定；一个screen一个surface
    // 遍历this的所有子节点，所有子节点查询一下top
    // if (!screenData->wmSurface)
    {
        TpWidget *topScreenWidget = dynamic_cast<TpWidget *>(screenData->top);
        if (screenData->top && topScreenWidget)
        {
            TpObjectData *topData = static_cast<TpObjectData *>(topScreenWidget->objectSets());

            // 根据新的top指针，解析surface信息
            IPiWFSurface *surface_t = tinyPiX_wf_get_surface(topData->agent);

            TpRect topScreenRect = topScreenWidget->toScreen();

            // 目前为全局刷新，所以在此处初始化一次surface即可
            // 修改为局部刷新时，需要在resize时，重新设置surface的rect TODO
            screenData->wmSurface = tpMakeShared<TpSurface>(surface_t, topScreenRect);

            int32_t surfaceWidth = screenData->wmSurface->width();
            int32_t surfaceHeight = screenData->wmSurface->height();

            screenData->swCanvas->target((uint32_t *)screenData->wmSurface->matrix(), surfaceWidth, surfaceWidth, surfaceHeight, tvg::ColorSpace::ARGB8888);
        }
    }

    TpWidget::onResizeEvent(event);

    return true;
}

Tp::TpObjectType TpScreen::objectType()
{
    Tp::TpObjectType type = Tp::TP_UNKOWN_OBJECT;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return type;

    Tp::TpObjectSysLayer layer = (Tp::TpObjectSysLayer)tinyPiX_wf_get_layer(widgetData->agent);

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

    return type;
}

Tp::TpObjectSysLayer TpScreen::objectLayer()
{
    Tp::TpObjectSysLayer layer = Tp::TP_WM_NONE;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return layer;

    layer = (Tp::TpObjectSysLayer)tinyPiX_wf_get_layer(widgetData->agent);

    return layer;
}

int32_t TpScreen::objectSysID()
{
    int32_t id = TP_INVALIDATE_VALUE;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return id;

    id = tinyPiX_wf_get_id(widgetData->agent);

    return id;
}

bool TpScreen::objectActive()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return false;

    bool actived = tinyPiX_wf_get_active(widgetData->agent);
    return actived;
}

void TpScreen::setParent(TpObject *parent)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;
    widgetData->parent = nullptr;
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
    ITpUserEvent message;

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
        break;
    }
}

bool TpScreen::returns()
{
    bool returns = true;

    ITpUserEvent message;

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

    return returns;
}

int32_t TpScreen::dispatchEvent(void *events)
{
    ITpEvent *eventPtr = (ITpEvent *)events;

    TpPoint point;
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);

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

            if (widgetData->tmp.curfocus && widgetData->tmp.curfocus->enabled())
            {
                TpKeyboardEventData input;
                input.which = eventPtr->keyboardEvent.which;
                input.state = eventPtr->keyboardEvent.state;
                input.scancode = eventPtr->keyboardEvent.keysym.scancode;
                input.virtualKey = eventPtr->keyboardEvent.keysym.virtualkey;
                input.symbol = eventPtr->keyboardEvent.keysym.symbol;
                input.keyMod = eventPtr->keyboardEvent.keysym.keymod;
                memset(input.shortCut, 0, strlen((char *)eventPtr->keyboardEvent.keysym.shortcut));
                memcpy(input.shortCut, eventPtr->keyboardEvent.keysym.shortcut, strlen((char *)eventPtr->keyboardEvent.keysym.shortcut));

                TpKeyboardEvent event(input.state ? TpEvent::EVENT_KEYBOARD_PRESS_TYPE : TpEvent::EVENT_KEYBOARD_RELEASE_TYPE);
                event.construct(&input);

                if (input.state)
                {
                    IssueObjEvent(widgetData->tmp.curfocus, event, onKeyPressEvent, widgetData->tmp.curfocus->enabled());
                }
                else
                {
                    IssueObjEvent(widgetData->tmp.curfocus, event, onKeyReleaseEvent, widgetData->tmp.curfocus->enabled());
                }
            }
        }
        break;
        }

        return true;
    }

    widgetData->tmp.curObject = this->find(point);

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

        widgetData->tmp.curmotion = widgetData->tmp.curObject;
        TpLeaveEvent leaveEvent;
        TpLeaveEventData lInput;

        if (widgetData->tmp.curmotion != widgetData->tmp.lstmotion)
        {
            if (widgetData->tmp.curmotion)
            {
                lInput.object = widgetData->tmp.lstmotion;
                lInput.leaved = true;
                leaveEvent.construct(&lInput);

                // 如果lstmotion为空，则必然要触发当前进入的对象的leaveIn事件
                // 但是如果鼠标上一针坐标已经在当前对象了，就不需要重复触发
                if (widgetData->tmp.lstmotion)
                {
                    TpRect curMotionRect = widgetData->tmp.curmotion->toScreen();
                    if (!curMotionRect.contains(widgetData->tmp.lastPoint))
                    {
                        IssueObjEvent(widgetData->tmp.curmotion, leaveEvent, onLeaveEvent, widgetData->tmp.curmotion->enabled());
                    }
                }
                else
                {
                    IssueObjEvent(widgetData->tmp.curmotion, leaveEvent, onLeaveEvent, widgetData->tmp.curmotion->enabled());
                }

                // 判断是否也进入了当前对象的父对象
                TpWidget *curParent = dynamic_cast<TpWidget *>(widgetData->tmp.curmotion->parent());
                while (curParent)
                {
                    TpRect curParentRect = curParent->toScreen();

                    /*  如果上一个对象为空，说明是程序刚启动，第一次进入，直接触发所有的leaveIn即可
                        如果不为空，则需要判断，如果上一个鼠标坐标不在该窗口，当前坐标在该窗口则触发leaveIn，否则不触发*/
                    if (widgetData->tmp.lstmotion)
                    {
                        if (curParentRect.contains(point) && !curParentRect.contains(widgetData->tmp.lastPoint))
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

            if (widgetData->tmp.lstmotion)
            {
                lInput.object = widgetData->tmp.lstmotion;
                lInput.leaved = false;
                leaveEvent.construct(&lInput);

                // 根据当前鼠标坐标判断是否也离开了上一个对象及父对象
                TpWidget *curParent = widgetData->tmp.lstmotion;
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

            widgetData->tmp.lastPoint = point;
            widgetData->tmp.lstmotion = widgetData->tmp.curmotion;
        }

        generateParentList(widgetData->tmp.curmotion, motionList);

        broadMotion(widgetData->mousePressObject, widgetData->tmp.curmotion, motionList, eventPtr, widgetData->mousePressObject);
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
        widgetData->tmp.curfocus = widgetData->tmp.curObject;
        TpFocusEvent focusEvent;
        TpFocusEventData fInput;

        if (widgetData->tmp.curfocus != widgetData->tmp.lstfocus)
        {
            if (widgetData->tmp.curfocus && widgetData->tmp.curfocus->enabled())
            {
                // obtain focus
                fInput.object = widgetData->tmp.lstfocus;
                fInput.focused = true;
                focusEvent.construct(&fInput);

                IssueObjEvent(widgetData->tmp.curfocus, focusEvent, onFocusEvent, widgetData->tmp.curfocus->enabled());
            }

            if (widgetData->tmp.lstfocus && widgetData->tmp.lstfocus->enabled())
            {
                // lost focus
                fInput.object = widgetData->tmp.curfocus;
                fInput.focused = false;
                focusEvent.construct(&fInput);

                IssueObjEvent(widgetData->tmp.lstfocus, focusEvent, onFocusEvent, widgetData->tmp.lstfocus->enabled());
            }

            widgetData->tmp.lstfocus = widgetData->tmp.curfocus;
        }

        generateParentList(widgetData->tmp.curmotion, keyList);
        broadMouseKey(widgetData->tmp.curfocus, keyList, eventPtr, widgetData->mousePressObject);

        widgetData->mousePressObject = eventPtr->mouseButtonEvent.state ? widgetData->tmp.curfocus : nullptr;
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
        TpFingerEventData input;

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

        generateParentList(widgetData->tmp.curObject, fingerList);
        broadFinger(widgetData, input, widgetData->tmp.curObject, fingerList, eventPtr);
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
        if (widgetData->tmp.curObject == nullptr)
        {
            return false;
        }
        std::list<TpObject *> dollarList;
        TpDollAREventData input;
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

        generateParentList(widgetData->tmp.curObject, dollarList);
        broaDollar(widgetData, input, widgetData->tmp.curObject, dollarList, eventPtr);
    }
    break;
    case TP_MULTIGESTURE:
    {
        if ((eventMask & TpApp::TP_DIS_GESTURE) == TpApp::TP_DIS_GESTURE)
        {
            return false;
        }

        // don't know how to do
        if (widgetData->tmp.curObject == nullptr)
        {
            return false;
        }

        std::list<TpObject *> multiList;
        TpMultiGestureEventData input;

        generateParentList(widgetData->tmp.curObject, multiList);
        broadMultiGesture(widgetData, input, widgetData->tmp.curObject, multiList, eventPtr);
    }
    break;
    default:
        return false;
    }

    return true;
}

void *TpScreen::canvasPtr()
{
    TpScreenData *screenData = static_cast<TpScreenData *>(data_);
    if (!screenData->swCanvas)
    {
        screenData->swCanvas = tvg::SwCanvas::gen();
    }
    return screenData->swCanvas;
}
