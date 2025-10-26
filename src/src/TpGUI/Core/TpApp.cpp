#include "TpApp.h"
#include "TpApp_p.h"

TpApp::TpApp(int32_t argc, char *argv[])
{
    // // 根据CPU核心数；分配绘图引擎线程数
    uint32_t cores = std::thread::hardware_concurrency();
    tvg::Initializer::init(cores / 2);

    TpAppData *set = new TpAppData();

    bool ret = decideRunOnce(argv[0]);

    if (ret)
    {
        std::exit(0);
    }

    set->mainThreadId = std::this_thread::get_id();

    set->clipboard = TpClipboard::Inst();
    set->vScreen = nullptr;
    set->message = new TpMessage();

    set->pInfo.id = TP_INVALIDATE_VALUE;
    set->pInfo.pid = TP_INVALIDATE_VALUE;
    set->eventType = TP_DIS_NONE;

    set->running = true;
    set->waitRun = false;

    set->thread = new appExe(this);

    memset(set->pInfo.process, 0, PROCESS_MAX_NAME_LENGTH);
    parseArgs(set, argc, argv);

    TpAutoObject::Inst()->autoFreeObject = true;

    if (appInst)
    {
        std::cout << "detects app instance more once!, exit......" << std::endl;
        std::exit(0);
    }

    appInst = this;
    this->data_ = set;

    // APP创建，解析初始CSS样式
    TpString cssFilePath = parseThemeFile(set->systemTheme);
    set->cssParser_->parseCss(cssFilePath);

    // 尝试读取桌面信息；如果没有桌面则读取失败
    try
    {
        // 记录桌面信息要根据topbar的数据，决定应用所有窗体的xy偏移量，和真实可用宽高
        TpShareMemory deskTopBarshare("DeskTopBarConfig", 1024, false);
        if (deskTopBarshare.isMapped())
        {
            if (deskTopBarshare.readData(&set->desktopBarInfo_, sizeof(set->desktopBarInfo_)))
            {
                std::cout << "桌面信息：" << set->desktopBarInfo_.topBarWidth << " , " << set->desktopBarInfo_.topBarHeight
                          << " , " << set->desktopBarInfo_.topBarisVislble << std::endl;
            }
        }
    }
    catch (const std::exception &e)
    {
        // 共享内存不存在;无桌面模式
    }
}

TpApp::~TpApp()
{
    tvg::Initializer::term();

    TpAppData *set = (TpAppData *)this->data_;

    if (set)
    {
        if (set->clipboard)
        {
            delete set->clipboard;
        }

        if (set->message)
        {
            delete set->message;
        }

        if (set->thread)
        {
            delete set->thread;
        }

        set->vReserveMap.clear();

        delete set;
    }
}

TpApp *TpApp::Inst()
{
    return appInst;
}

bool TpApp::bindVScreen(TpScreen *object)
{
    TpAppData *set = static_cast<TpAppData *>(this->data_);
    bool ret = false;

    if (!set)
        return false;

    if (!object)
        return false;

    // if (object->objectType() != Tp::TP_TOP_OBJECT)
    // {
    //     std::cout << "bind screen type error !" << std::endl;
    //     return false;
    // }

    if (set->vScreen)
    {
        std::cout << "bind screen only once !" << std::endl;
        return false;
    }

    ret = (set->vScreen != object);

    if (ret)
    {
        set->gMutex.lock();
        set->vScreen = object;
        set->gMutex.unlock();
    }

    set->thread->start();

    return ret;
}

bool TpApp::run()
{
    TpAppData *set = (TpAppData *)this->data_;

    if (!set)
        return set->running;

    set->waitRun = true;

    if (set->vScreen == nullptr)
        return false;

    while (set->running)
    {
        // 异步调用信号槽
        std::queue<std::function<void()>> cacheTaskList;

        {
            std::lock_guard<std::mutex> lock(set->queueSlotMutex_);
            cacheTaskList = set->slotTasks_;
            set->slotTasks_ = std::queue<std::function<void()>>();
        }

        // std::cout << "执行槽函数前  "  << cacheTaskList.size()  << std::endl;

        while (!cacheTaskList.empty())
        {
            auto task = cacheTaskList.front();
            cacheTaskList.pop();
            // lock.unlock();
            task();
            // lock.lock();
        }

        // std::cout << "执行槽函数后 "  << std::endl;

        // 异步刷新UI
        std::queue<UpdateCommand> cacheUpdateTaskList;
        {
            std::lock_guard<std::mutex> lock(set->queueUpdateMutex_);
            cacheUpdateTaskList = set->updateTasks_;
            set->updateTasks_ = std::queue<UpdateCommand>();
        }
        DownUpdateCommand(cacheUpdateTaskList);

        // set->vScreen->update();
        // for (const auto &dia : set->floatScreenList)
        // {
        //     dia->update();
        // }

        TpTimer::sleep(16);
    }

    return set->running;
}

TpClipboard *TpApp::clipboard()
{
    TpAppData *set = (TpAppData *)this->data_;
    TpClipboard *clipboard = nullptr;

    if (set)
    {
        clipboard = set->clipboard;
    }

    return clipboard;
}

TpScreen *TpApp::vScreen()
{
    TpAppData *set = (TpAppData *)this->data_;
    TpScreen *vScreen = nullptr;

    if (set)
    {
        vScreen = set->vScreen;
    }

    return vScreen;
}

tpShared<TpCssParser> TpApp::cssParser()
{
    TpAppData *set = static_cast<TpAppData *>(this->data_);
    return set->cssParser_;
}

void TpApp::setStyle(const Tp::SystemTheme &style)
{
    TpAppData *set = (TpAppData *)this->data_;

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
    TpAppData *set = (TpAppData *)this->data_;

    return set->systemTheme;
}

TpImage TpApp::grabWindow()
{
    // TpAppData *set = (TpAppData *)this->data_;

    // tinyPiX_sys_capture_screen();
    return TpImage();
}

void TpApp::wakeUpVirtualKeyboard(TpWidget *object)
{
    if (!object)
        return;

    TpAppData *set = (TpAppData *)this->data_;

    if (set->virtualKeyboard == nullptr)
        initVirtualKeyboard(set);

    set->curInputObj = object;
    set->virtualKeyboard->show();
}

void TpApp::dormantVirtualKeyboard()
{
    TpAppData *set = (TpAppData *)this->data_;
    set->curInputObj = nullptr;
    set->virtualKeyboard->close();
}

bool TpApp::isExistObject(TpObject *object, bool autoRemove)
{
    TpAppData *set = (TpAppData *)this->data_;
    bool ret = false;

    if (object == nullptr)
    {
        return false;
    }

    if (set)
    {
        set->gMutex.lock();
        std::list<TpObject *> *curList = &set->objectList;

        auto iter = std::find_if(curList->begin(), curList->end(), [object](const TpObject *obj)
                                 { return (object == obj); });

        if (iter != curList->end())
        {
            if (autoRemove)
            {
                curList->erase(iter);
            }
            ret = true;
        }

        set->gMutex.unlock();
    }

    return ret;
}

bool TpApp::isMainThread()
{
    TpAppData *set = (TpAppData *)this->data_;
    return std::this_thread::get_id() == set->mainThreadId;
}

bool TpApp::sendRegister(TpObject *object)
{
    TpAppData *set = (TpAppData *)this->data_;
    bool registerObject = false;

    if (!set)
        return registerObject;

    if (object == nullptr)
        return registerObject;

    if (object->objectType() == Tp::TP_FLOAT_OBJECT)
    {
        TpWidget *floatScreenWidget = dynamic_cast<TpWidget *>(object);
        if (floatScreenWidget)
            set->floatScreenList.emplace_back(floatScreenWidget);
    }

    ItpUserEvent message;
    message.type = TP_REGISTER_ACT;
    message.user_data0 = object;

    registerObject = set->message->sendWait(&message);

    return registerObject;
}

bool TpApp::sendDelete(TpObject *object)
{
    if (!object)
        return false;

    TpAppData *set = static_cast<TpAppData *>(this->data_);
    bool deleteObject = false;

    if (!set)
        return false;

    ItpUserEvent message;
    message.type = TP_DELETE_ACT;
    message.user_data0 = object;

    bool sendRes = set->message->sendWait(&message);

    return sendRes;
}

bool TpApp::sendReturn(TpObject *object)
{
    TpAppData *set = (TpAppData *)this->data_;
    bool returnAct = false;

    if (set)
    {
        returnAct = (object != nullptr);

        if (returnAct)
        {
            if (object->objectType() != Tp::TP_TOP_OBJECT)
            {
                return false;
            }

            ItpUserEvent message;
            message.type = TP_RETURN_ACT;
            message.user_data0 = object;

            returnAct = set->message->sendWait(&message);
        }
    }

    return returnAct;
}

bool TpApp::sendActive(TpObject *object, bool actived)
{
    TpAppData *set = (TpAppData *)this->data_;
    bool beActived = false;

    if (set)
    {
        beActived = (object != nullptr);

        if (beActived)
        {
            if (object->objectType() != Tp::TP_TOP_OBJECT)
            {
                return false;
            }

            ItpUserEvent message;
            message.type = TP_ACTIVE_ACT;

            message.user_data0 = object;
            message.user_code = actived;
            actived = set->message->sendWait(&message);
        }
    }

    return beActived;
}

bool TpApp::sendAbort(TpObject *object)
{
    TpAppData *set = (TpAppData *)this->data_;
    bool abort = false;

    if (set)
    {
        abort = (object != nullptr);

        if (abort)
        {
            ItpUserEvent message;
            message.type = TP_ABORT_ACT;
            message.user_data0 = object;

            abort = set->message->sendWait(&message);
        }
    }

    return abort;
}

void TpApp::setDisableEventType(int32_t type)
{
    TpAppData *set = (TpAppData *)this->data_;

    if (set)
    {
        set->eventType = type;
    }
}

ITpAppData *TpApp::appObjectSet()
{
    return (TpAppData *)this->data_;
}

int32_t TpApp::disableEventType()
{
    TpAppData *set = (TpAppData *)this->data_;
    int32_t type = 0;

    if (set)
    {
        type = set->eventType;
    }

    return type;
}

void TpApp::postEvent(std::function<void()> task)
{
    TpAppData *set = (TpAppData *)this->data_;

    if (!set->running)
        return;

    {
        std::lock_guard<std::mutex> lock(set->queueSlotMutex_);
        set->slotTasks_.push(task);
    }
}

void TpApp::postUpdateEvent(TpWidget *updateObj, const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h, bool onlyBlit)
{
    if (!updateObj)
        return;

    TpAppData *set = (TpAppData *)this->data_;

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
