#include "TpApp.h"
#include "TpObject.h"
#include "TpClipboard.h"
#include "TpMessage.h"
#include "TpAutoObject.h"
#include "TpScreen.h"
#include "TpConfig.h"
#include "TpThread.h"
#include "TpTimer.h"
#include "TpMD5.h"
#include "TpDefaultCss.h"
#include "TpEvent.h"
#include "TpDef.h"
#include "TpChildWidget.h"
#include "TpSurface.h"
#include "TpVirtualKeyboard.h"
#include "TpMap.h"
#include "TpRect.h"
#include "TpObjectFunction.hpp"
#include "TpFixScreen.h"

#include <tinyPiXApi.h>
#include <mutex>
#include <unistd.h>
#include <getopt.h>
#include <functional>
#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <thread>
#include <queue>
#include "thorVG/thorvg.h"

#define PROCESS_MAX_NAME_LENGTH 1024

class appExe;

struct ItpProcessInfo
{
    int32_t id;
    pid_t pid;
    char process[PROCESS_MAX_NAME_LENGTH];
};

struct UpdateCommand
{
    TpChildWidget *updateObj = nullptr;
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;
    bool onlyBlit = false;

    UpdateCommand()
    {
    }
};
// typedef std::shared_ptr<UpdateCommand> UpdateCommandSPtr;

struct TpAppData
{
    // 主线程ID
    std::thread::id mainThreadId;

    TpList<TpObject *> objectList;
    std::map<TpObject *, bool> vReserveMap;
    // 所有floatscreen列表，用于更新主题样式
    TpList<TpChildWidget *> floatScreenList;

    std::mutex gMutex;

    TpScreen *vScreen;

    TpClipboard *clipboard;

    TpMessage *message;

    appExe *thread;

    int32_t eventType;
    ItpProcessInfo pInfo;
    // ItpConfigSet appConfigurationSet;

    bool running;
    bool waitRun;

    Tp::SystemTheme systemTheme = Tp::Default;
    tpShared<TpCssParser> cssParser_ = tpMakeShared<TpCssParser>();

    // 全局唯一单例虚拟键盘
    TpVirtualKeyboard *virtualKeyboard = nullptr;
    TpChildWidget *curInputObj = nullptr;

    std::mutex queueSlotMutex_;
    std::queue<std::function<void()>> slotTasks_;

    std::mutex queueUpdateMutex_;
    std::queue<UpdateCommand> updateTasks_;
};

class appExe : public TpThread
{
public:
    appExe() : TpThread() {};

    appExe(TpApp *app) : TpThread()
    {
        theApp = app;
    };

    virtual ~appExe() {

    };

    virtual void run()
    {
        TpAppData *set = (TpAppData *)theApp->appObjectSet();
        ItpUserEvent message;
        bool ret = false;
        if (!set)
            return;

        while (true)
        {
            if (set->vScreen == nullptr)
            {
                break;
            }

            ret = set->message->recvWait(&message);
            if (!ret)
                continue;

            switch (message.type)
            {
            case TpApp::TP_REGISTER_ACT:
            {
                // add to objectList
                set->gMutex.lock();
                TpChildWidget *childWidgetObj = (TpChildWidget *)message.user_data0;
                if (childWidgetObj)
                {
                    switch (childWidgetObj->objectType())
                    {
                    case Tp::TP_FLOAT_OBJECT:
                    {
                        set->vReserveMap[childWidgetObj] = childWidgetObj->visible();
                    }
                    break;
                    }
                    set->objectList.push_back(childWidgetObj);
                    set->gMutex.unlock();
                }
            }
            break;
            case TpApp::TP_DELETE_ACT:
            {
                TpObject *object = (TpObject *)message.user_data0;

                if (object == nullptr)
                {
                    continue;
                }

                set->gMutex.lock();

                if (object)
                {
                    std::map<TpObject *, bool>::iterator mapiter = set->vReserveMap.find(object);

                    if (mapiter != set->vReserveMap.end())
                    {
                        set->vReserveMap.erase(mapiter);
                    }

                    auto objFindIter = std::find(set->objectList.begin(), set->objectList.end(), object);
                    if (objFindIter != set->objectList.end())
                    {
                        set->objectList.remove(*objFindIter);
                    }

                    auto floatFindIter = std::find(set->floatScreenList.begin(), set->floatScreenList.end(), object);
                    if (floatFindIter != set->floatScreenList.end())
                    {
                        set->floatScreenList.remove(*floatFindIter);
                    }

                    // ItpObjectSet *vScreenObjDaata = (ItpObjectSet *)set->vScreen->objectSets();
                    // vScreenObjDaata->tmp.deleteObject(object);

                    if (object == set->vScreen)
                    {
                        goto finished;
                    }
                }
            deleted:
                set->gMutex.unlock();

                // std::cout << "指针释放 " << std::endl;
                delete object;
                object = nullptr;
                // set->vScreen->update();
            }
            break;
            case TpApp::TP_ABORT_ACT:
            {
            finished:
                goto appover;
            }
            break;
            case TpApp::TP_RETURN_ACT:
            {
                TpObject *vScreen = set->vScreen;

                if (vScreen == message.user_data0)
                {
                    TpScreen *screenObj = static_cast<TpScreen *>(vScreen);

                    // exclude desktop
                    if (screenObj->objectLayer() != Tp::TP_WM_DESK)
                    {
                        if (vScreen)
                        {
                            screenObj->setVisible(false);
                        }

                        set->gMutex.lock();

                        std::map<TpObject *, bool>::iterator iter = set->vReserveMap.begin();
                        for (; iter != set->vReserveMap.end(); iter++)
                        {
                            TpChildWidget *tmp = static_cast<TpChildWidget *>(iter->first);
                            iter->second = tmp->visible();
                            tmp->setVisible(false);
                        }

                        set->gMutex.unlock();
                    }
                }
            }
            break;
            case TpApp::TP_ACTIVE_ACT:
            {
                bool actived = message.user_code;
                TpObject *object = (TpObject *)message.user_data0;

                TpObject *vScreen = set->vScreen;

                if (((TpScreen *)vScreen)->objectLayer() != Tp::TP_WM_DESK)
                {
                    set->gMutex.lock();

                    if (actived)
                    {
                        std::map<TpObject *, bool>::iterator mapiter = set->vReserveMap.begin();
                        for (; mapiter != set->vReserveMap.end(); mapiter++)
                        {
                            TpChildWidget *tmp = static_cast<TpChildWidget *>(mapiter->first);

                            if (tmp != set->vScreen)
                            {
                                tmp->setVisible(mapiter->second);
                            }
                        }
                    }
                    else
                    {
                        std::map<TpObject *, bool>::iterator mapiter = set->vReserveMap.begin();
                        for (; mapiter != set->vReserveMap.end(); mapiter++)
                        {
                            TpChildWidget *tmp = static_cast<TpChildWidget *>(mapiter->first);

                            if (tmp != set->vScreen)
                            {
                                mapiter->second = tmp->visible();
                                tmp->setVisible(false);
                            }
                        }
                    }

                    set->gMutex.unlock();
                }
            }
            break;
            }
        }
    appover:
        set->running = false;

        if (set->waitRun == false)
        {
            exit(0);
        }
    };

private:
    TpApp *theApp;
};

static TpApp *appInst = nullptr;

// 刷新指令下发
static void DownUpdateCommand(std::queue<UpdateCommand> &updateCommandQueue)
{
    if (updateCommandQueue.size() == 0)
        return;

#if 1
    TpMap<IPiWFApiAgent *, TpRect> pixwmMergeUpdateRect;
    TpMap<TpChildWidget *, ItpObjectPaintInput> mergeUpdateWidget;

    while (!updateCommandQueue.empty())
    {
        UpdateCommand task = updateCommandQueue.front();
        updateCommandQueue.pop();

        if (!task.updateObj->visible())
            continue;

        ItpObjectSet *updateObjSet = static_cast<ItpObjectSet *>(task.updateObj->objectSets());
        ItpObjectSet *topScreenSet = static_cast<ItpObjectSet *>(updateObjSet->top->objectSets());

        if (pixwmMergeUpdateRect.contains(topScreenSet->agent))
        {
            TpRect taskRect(task.x, task.y, task.w, task.h);

            TpRect &hasRect = pixwmMergeUpdateRect[topScreenSet->agent];
            hasRect.unions(taskRect);
        }
        else
        {
            pixwmMergeUpdateRect[topScreenSet->agent] = TpRect(task.x, task.y, task.w, task.h);
        }

        if (!mergeUpdateWidget.contains(task.updateObj))
        {
            ItpObjectPaintInput paintInput;

            IPiWFSurface *surface_t = tinyPiX_wf_get_surface(topScreenSet->agent);
            if (surface_t == nullptr)
                continue;

            tpShared<TpSurface> surface = tpMakeShared<TpSurface>(surface_t, task.updateObj->rect());

            paintInput.object = task.updateObj;
            paintInput.surface = surface;
            paintInput.updateRect.setX(task.x);
            paintInput.updateRect.setY(task.y);
            paintInput.updateRect.setWidth(task.w);
            paintInput.updateRect.setHeight(task.h);

            mergeUpdateWidget[task.updateObj] = paintInput;
        }
    }

    for (const auto &updateWidgetIter : mergeUpdateWidget)
    {
        ItpObjectSet *updateObjSet = static_cast<ItpObjectSet *>(updateWidgetIter.first->objectSets());
        ItpObjectSet *topScreenSet = static_cast<ItpObjectSet *>(updateObjSet->top->objectSets());

        ItpObjectPaintInput paintInput = updateWidgetIter.second;

        tinyPiX_wf_lock_mutex(topScreenSet->agent);

        // std::cout << "局部刷新：刷新区域： " << updateWidgetIter.first << " : " << paintInput.updateRect.x << " , " << paintInput.updateRect.y << " , "
        //           << paintInput.updateRect.w << " , " << paintInput.updateRect.h << std::endl;

        // int32_t surfaceWidth = paintInput.surface->width();
        // int32_t surfaceHeight = paintInput.surface->height();
        // std::cout << "Surface尺寸： " << surfaceWidth << "  " << surfaceHeight << std::endl;

        TpPaintEvent event;
        event.construct(&paintInput);

        // 刷新前清除scene
        TpPainter *childPainter = event.painter();

        auto canvasPair = updateWidgetIter.first->canvasPtr();
        tvg::SwCanvas *childCanvas = (tvg::SwCanvas *)canvasPair.first;
        tvg::Scene *childScene = (tvg::Scene *)canvasPair.second;

        childPainter->addScene(childCanvas, childScene);

        bool ret = updateWidgetIter.first->onPaintEvent(&event);

        // 清除所有现有效果
        childScene->push(tvg::SceneEffect::ClearAll);
        if (updateWidgetIter.first->enableBlur())
        {
            TpGraphicsBlurEffect blurEffectObj = updateWidgetIter.first->graphicsEffect();
            childScene->push(tvg::SceneEffect::GaussianBlur, blurEffectObj.blurRadius(), (int32_t)blurEffectObj.direction(), (int32_t)blurEffectObj.border(), blurEffectObj.quality());
        }

        // 控件不可用，绘制遮罩层
        paintEnabledBox(updateWidgetIter.first, event.painter());

        // 绘制完成刷新绘制
        childPainter->sync();

        if (ret)
        {
            childPaint(updateObjSet, &event);
        }

        tinyPiX_wf_unlock_mutex(topScreenSet->agent);
    }

    for (const auto &updateInfo : pixwmMergeUpdateRect)
    {
        const TpRect &updateRect = updateInfo.second;
        // std::cout << "111PiXWM 更新区域 " << updateRect.x() << " " << updateRect.y() << " " << updateRect.width() << " " << updateRect.height() << std::endl;
        tinyPiX_wf_update(updateInfo.first, updateRect.x(), updateRect.y(), updateRect.width(), updateRect.height(), true, false);
        // tinyPiX_wf_update(updateInfo.first, 0, 0, 1080, 720, true, false);
    }

#else
    // 遍历刷新指令，合并相邻和相同区域
    while (!updateCommandQueue.empty())
    {
        UpdateCommand task = updateCommandQueue.front();
        updateCommandQueue.pop();

        TpScreen *topScreen = dynamic_cast<TpScreen *>(task.updateObj);

        ItpObjectSet *set = static_cast<ItpObjectSet *>(topScreen->objectSets());

        if (!set)
            continue;

        // printf("id=%d, visible=%d, actived=%d\n", this->objectSysID(), set->visible, this->actived());
        if (!set->visible || !topScreen->actived())
            continue;

        TpRect updateRect(task.x, task.y, task.w, task.h);

        tinyPiX_wf_lock_mutex(set->agent);

        IPiDSSurface *surface_t = tinyPiX_wf_get_surface(set->agent);

        if (surface_t == nullptr)
            continue;

        tpShared<TpSurface> surface = tpMakeShared<TpSurface>(surface_t);

        ItpObjectPaintInput input;
        TpPaintEvent event;
        input.object = topScreen;

        input.surface = surface;

        input.updateRect.x = updateRect.x();
        input.updateRect.y = updateRect.y();
        input.updateRect.w = updateRect.width();
        input.updateRect.h = updateRect.height();
        event.construct(&input);

        // 刷新前清除scene
        TpPainter *childPainter = event.painter();

        tvg::Scene *childScene = (tvg::Scene *)topScreen->testScenePtr();
        childScene->remove();
        childPainter->addScene(childScene);

        bool ret = topScreen->onPaintEvent(&event);

        // 绘制完成刷新绘制
        childPainter->sync();

        if (ret)
        {
            childPaint(set, &event);
        }

        tinyPiX_wf_unlock_mutex(set->agent);

        if (task.onlyBlit == false)
        {
            // tinyPiX_wf_update(set->agent, input.updateRect.x, input.updateRect.y, input.updateRect.w, input.updateRect.h, task.clip, task.sync);
            tinyPiX_wf_update(set->agent, input.updateRect.x, input.updateRect.y, input.updateRect.w, input.updateRect.h, true, false);
        }
    }
#endif
}

static inline bool holdAppSecondRun(const char *runPath, const char *uuid)
{
    int32_t fd;
    int32_t lock_result;
    struct flock lock;
    char pFileName[PATH_MAX] = {0};
    sprintf(pFileName, "%s/.%s", runPath, uuid);

    fd = open(pFileName, O_RDWR | O_CREAT, 0644);

    if (fd < 0)
    {
        return true;
    }

    lock_result = lockf(fd, F_TEST, 0);

    if (lock_result < 0)
    {
        return true;
    }

    lock_result = lockf(fd, F_LOCK, 0);

    if (lock_result < 0)
    {
        return true;
    }

    return false;
}

static inline bool decideRunOnce(const char *appName)
{
    char tempPath[PATH_MAX] = {0};
    char *currentPath = get_current_dir_name();

    if (currentPath == nullptr)
    {
        return false;
    }

    sprintf(tempPath, "%s/%s", currentPath, appName);
    const char *md5 = TpMD5::getnerateMD5(tempPath, strlen(tempPath));

    if (md5 == nullptr)
    {
        return false;
    }

    return holdAppSecondRun(currentPath, md5);
}

static inline bool checkDigitals(char *args)
{
    if (args == NULL)
    {
        return false;
    }

    int32_t length = strlen(args), i;
    char key;

    if (length == 0)
    {
        return false;
    }

    for (i = 0; i < length; ++i)
    {
        key = args[i];

        if (key < '0' ||
            key > '9')
        {
            return false;
        }
    }

    return true;
}

static inline bool parseArgs(TpAppData *set, int32_t argc, char *argv[])
{
    bool ret = false;
    char ch;

    while ((ch = getopt(argc, argv, "p:i:n")) != -1)
    {
        switch (ch)
        {
        case 'p':
        {
            ret = checkDigitals(optarg);

            if (ret)
            {
                set->pInfo.id = atoi(optarg);
            }
        }
        break;
        case 'i':
        {
            ret = checkDigitals(optarg);

            if (ret)
            {
                set->pInfo.pid = atoi(optarg);
            }
        }
        break;
        case 'n':
        {
            int32_t length = strlen(optarg);

            if (length > 0)
            {
                memcpy(set->pInfo.process, optarg, length);
            }
        }
        break;
        default:
            return false;
            break;
        }
    }

    return true;
}

static void sendThemeChangedEvent(TpAppData *setData, const Tp::SystemTheme &sysTheme)
{
    TpThemeChangeEvent *themeEvent = new TpThemeChangeEvent();

    TpString cssFilePath = parseThemeFile(sysTheme);
    setData->cssParser_->parseCss(cssFilePath);

    // 在 app的run函数中，调用主题改变事件函数，通知所有组件
    TpChildWidget *screenWidget = dynamic_cast<TpChildWidget *>(setData->vScreen);
    if (screenWidget)
    {
        // 初始化CSS样式表
        // screenWidget->setStyleSheet(cssFilePath);
        screenWidget->onThemeChangeEvent(themeEvent);
        screenWidget->update();
    }

    for (const auto &floatScrenPtr : setData->floatScreenList)
    {
        // floatScrenPtr->setStyleSheet(cssFilePath);
        floatScrenPtr->onThemeChangeEvent(themeEvent);
        floatScrenPtr->update();
    }

    delete themeEvent;
    themeEvent = nullptr;
}

static void initVirtualKeyboard(TpAppData *set)
{
    if (set->virtualKeyboard)
        return;

    set->virtualKeyboard = new TpVirtualKeyboard();

    // 初始化虚拟键盘相关
    connect(set->virtualKeyboard, inputPinyin, [=](const TpString &pinyin)
            {
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(Tp::Pinyin, pinyin);
				} });
    connect(set->virtualKeyboard, finishChinese, [=](const TpString &chinese)
            {
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(Tp::Chinese, chinese);
				} });
    connect(set->virtualKeyboard, deleteSymbol, [=]()
            {
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(Tp::Delete, "");
				} });
    connect(set->virtualKeyboard, inputCharacter, [=](const TpString &character)
            {
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(Tp::Symbol, character);
				} });
}

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

    if (object->objectType() != Tp::TP_TOP_OBJECT)
    {
        std::cout << "bind screen type error !" << std::endl;
        return false;
    }

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

void TpApp::wakeUpVirtualKeyboard(TpChildWidget *object)
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
        TpChildWidget *floatScreenWidget = dynamic_cast<TpChildWidget *>(object);
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

void TpApp::postUpdateEvent(TpChildWidget *updateObj, const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h, bool onlyBlit)
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
