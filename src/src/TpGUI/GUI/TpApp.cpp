#include "TpApp.h"
#include "TpObjectFunction.hpp"
#include "TpApp_p.h"
#include "TpFontConfig.h"

TpApp::TpApp(int32_t argc, char *argv[], const TpString &deskStrKey)
    : TpCoreApp(argc, argv)
{
    TpCoreAppData *coreData = static_cast<TpCoreAppData *>(TpCoreApp::data_);
    if (coreData)
    {
        delete coreData;
        coreData = nullptr;
        TpCoreApp::data_ = nullptr;
    }

    TpAppData *appData = new TpAppData();

    appData->mainThreadId = std::this_thread::get_id();
    appData->running = true;
    appData->waitRun = false;

    // 确保应用单例运行
    bool ret = decideRunOnce(argv[0]);
    if (ret)
        std::exit(0);

    appData->clipboard = TpClipboard::Inst();
    appData->vScreen = nullptr;
    appData->message = new TpMessage();
    appData->eventType = TP_DIS_NONE;

    appData->appExecThread = new AppExec(appData);

    TpAutoObject::Inst()->autoFreeObject = true;

    appPtr = this;
    TpCoreApp::data_ = appData;

    // 初始化字体
    TpFontConfig::Inst();

    // APP创建，解析初始CSS样式
    TpString cssFilePath = parseThemeFile(appData->systemTheme);
    appData->cssParser_->parseCss(cssFilePath);

    // 绑定物理窗口；判断是否是桌面
    if (deskStrKey.compare("tinyPiX_DeskTop_0x43ef3dc14") == 0)
    {
        appData->isDesk = true;
        bindVScreen(appData, new TpFixScreen("tinyPiX_DeskTop_0x43ef3dc14"));
    }
    else
    {
        bindVScreen(appData, new TpFixScreen());
    }
}

TpApp::~TpApp()
{
    tvg::Initializer::term();

    TpAppData *set = static_cast<TpAppData *>(data_);

    if (!set)
        return;

    if (set->clipboard)
        delete set->clipboard;

    if (set->message)
        delete set->message;

    if (set->appExecThread)
        delete set->appExecThread;

    set->vReserveMap.clear();

    delete set;
    data_ = nullptr;
}

TpApp *TpApp::Inst()
{
    return dynamic_cast<TpApp *>(appPtr);
}

bool TpApp::run()
{
    TpAppData *appData = static_cast<TpAppData *>(TpCoreApp::data_);
    if (!appData)
        return appData->running;

    appData->waitRun = true;

    if (appData->vScreen == nullptr)
        return false;

    while (appData->running)
    {
        // 异步调用信号槽
        std::queue<std::function<void()>> cacheTaskList;

        {
            std::lock_guard<std::mutex> lock(appData->queueSlotMutex_);
            cacheTaskList = appData->slotTasks_;
            appData->slotTasks_ = std::queue<std::function<void()>>();
        }

        while (!cacheTaskList.empty())
        {
            auto task = cacheTaskList.front();
            cacheTaskList.pop();
            task();
        }

        // 异步刷新UI
        std::queue<UpdateCommand> cacheUpdateTaskList;
        {
            std::lock_guard<std::mutex> lock(appData->queueUpdateMutex_);
            cacheUpdateTaskList = appData->updateTasks_;
            appData->updateTasks_ = std::queue<UpdateCommand>();
        }
        DownUpdateCommand(cacheUpdateTaskList);

        TpTimer::sleep(16);
    }

    return appData->running;
}

TpClipboard *TpApp::clipboard()
{
    TpAppData *set = static_cast<TpAppData *>(data_);
    TpClipboard *clipboard = nullptr;

    if (set)
    {
        clipboard = set->clipboard;
    }

    return clipboard;
}

TpWidget *TpApp::mainWindow()
{
    TpAppData *set = static_cast<TpAppData *>(data_);
    return set->mainWindow;
}

bool TpApp::isDesktop()
{
    TpAppData *set = static_cast<TpAppData *>(data_);
    return set->isDesk;
}

void TpApp::setClipRect(const TpRect &rect)
{
    if (rect.isEmpty() || rect.isNull())
        return;

    TpAppData *appData = static_cast<TpAppData *>(data_);
    TpWidgetData *mainWindowData = static_cast<TpWidgetData *>(appData->mainWindow->objectSets());

    // 调整窗口大小和坐标
    uint32_t rW = 0, rH = 0;
    tinyPiX_wf_get_display_size(mainWindowData->agent, &rW, &rH);
    tinyPiX_wf_set_rect(mainWindowData->agent, rect.x(), rect.y(), rect.width(), rect.height());

    mainWindowData->offsetX = rect.x();
    mainWindowData->offsetY = rect.y();

    mainWindowData->absoluteRect.setX(rect.x());
    mainWindowData->absoluteRect.setY(rect.y());

    TpResizeEventData input;
    input.object = appData->mainWindow;
    input.nw = rect.width();
    input.nh = rect.height();
    input.question = TpResizeEvent::TP_NORMAL_CHANGE;
    TpResizeEvent event;
    bool ret = event.construct(&input);

    if (ret)
    {
        refreshCacheImage(mainWindowData);
        IssueObjEvent(appData->mainWindow, event, onResizeEvent, true);
    }
}

tpShared<TpCssParser> TpApp::cssParser()
{
    TpAppData *set = static_cast<TpAppData *>(data_);
    return set->cssParser_;
}

void TpApp::setStyle(const Tp::SystemTheme &style)
{
    TpAppData *set = static_cast<TpAppData *>(data_);

    if (set->systemTheme != style)
    {
        set->systemTheme = style;

        TpString cssFilePath = parseThemeFile(style);
        set->cssParser_->clearCss();
        set->cssParser_->parseCss(cssFilePath);

        // app run起来之后才下发主题切换事件，在run的时候已经解析过了
        // if (set->waitRun)
        //     sendThemeChangedEvent(set, style);
    }
}

Tp::SystemTheme TpApp::style()
{
    TpAppData *set = static_cast<TpAppData *>(data_);

    return set->systemTheme;
}

void TpApp::wakeUpVirtualKeyboard(TpWidget *object)
{
    if (!object)
        return;

    TpAppData *set = static_cast<TpAppData *>(data_);

    if (set->virtualKeyboard == nullptr)
        initVirtualKeyboard(set);

    set->curInputObj = object;
    set->virtualKeyboard->show();
}

void TpApp::dormantVirtualKeyboard()
{
    TpAppData *set = static_cast<TpAppData *>(data_);
    set->curInputObj = nullptr;
    set->virtualKeyboard->close();
}

bool TpApp::sendReturn(TpObject *object)
{
    TpAppData *set = static_cast<TpAppData *>(data_);
    bool returnAct = false;

    if (set)
    {
        returnAct = (object != nullptr);

        if (returnAct)
        {
            if (object->objectType() != Tp::TP_MAIN_WINDOW_OBJECT &&
                object->objectType() != Tp::TP_FIXSCREEN_OBJECT)
            {
                return false;
            }

            ITpUserEvent message;
            message.type = TP_RETURN_ACT;
            message.user_data0 = object;

            returnAct = set->message->sendWait(&message);
        }
    }

    return returnAct;
}

bool TpApp::sendActive(TpObject *object, bool actived)
{
    TpAppData *set = static_cast<TpAppData *>(data_);
    bool beActived = false;

    if (set)
    {
        beActived = (object != nullptr);

        if (beActived)
        {
            if (object->objectType() != Tp::TP_MAIN_WINDOW_OBJECT && object->objectType() != Tp::TP_FIXSCREEN_OBJECT)
            {
                return false;
            }

            ITpUserEvent message;
            message.type = TP_ACTIVE_ACT;

            message.user_data0 = object;
            message.user_code = actived;
            actived = set->message->sendWait(&message);
        }
    }

    return beActived;
}

bool TpApp::sendRegister(TpObject *object)
{
    TpCoreApp::sendRegister(object);

    if (object->objectType() == Tp::TP_FLOAT_OBJECT)
    {
        TpAppData *appData = static_cast<TpAppData *>(data_);

        TpWidget *floatScreenWidget = dynamic_cast<TpWidget *>(object);
        if (floatScreenWidget)
            appData->floatScreenList.emplace_back(floatScreenWidget);
    }

    return true;
}

bool TpApp::sendAbort(TpObject *object)
{
    TpAppData *set = static_cast<TpAppData *>(data_);
    bool abort = false;

    if (set)
    {
        abort = (object != nullptr);

        if (abort)
        {
            ITpUserEvent message;
            message.type = TP_ABORT_ACT;
            message.user_data0 = object;

            abort = set->message->sendWait(&message);
        }
    }

    return abort;
}

void TpApp::setDisableEventType(int32_t type)
{
    TpAppData *set = static_cast<TpAppData *>(data_);

    if (set)
    {
        set->eventType = type;
    }
}

ITpAppData *TpApp::appObjectSet()
{
    return static_cast<TpAppData *>(data_);
}

int32_t TpApp::disableEventType()
{
    TpAppData *set = static_cast<TpAppData *>(data_);
    int32_t type = 0;

    if (set)
    {
        type = set->eventType;
    }

    return type;
}

void TpApp::postUpdateEvent(TpWidget *updateObj, const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h, bool onlyBlit)
{
    if (!updateObj)
        return;

    TpAppData *set = static_cast<TpAppData *>(data_);

    // if (!set->running)
    // return;

    UpdateCommand updateCommandInfo;
    updateCommandInfo.updateObj = updateObj;
    updateCommandInfo.x = x;
    updateCommandInfo.y = y;
    updateCommandInfo.w = w;
    updateCommandInfo.h = h;
    updateCommandInfo.onlyBlit = onlyBlit;

    {
        std::lock_guard<std::mutex> lock(set->queueUpdateMutex_);
        set->updateTasks_.push(updateCommandInfo);
    }
}
