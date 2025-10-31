#ifndef PROCESS_MAX_NAME_LENGTH
#define PROCESS_MAX_NAME_LENGTH 1024
#endif

#ifndef __TP_APP_PRIVATE_H
#define __TP_APP_PRIVATE_H

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
#include "TpWidget.h"
#include "TpSurface.h"
#include "TpVirtualKeyboard.h"
#include "TpMap.h"
#include "TpRect.h"
#include "TpObjectFunction.hpp"
#include "TpMainWindow.h"
#include "TpShareMemory.h"
#include "TpFixScreen.h"
#include "TpGateway.h"
#include "TpMainWindow.h"
#include "thorVG/thorvg.h"

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

#if 1 // 慎重修改，需和桌面保持协议一致

/// @brief 应用上线标识;应用启动时发送该主题；桌面会通知应用状态栏信息
const static TpString DeskApplicationRunTopic = "DeskApplicationRunTopicConfig";
/// @brief 桌面发布状态栏信息主题
const static TpString DeskStatusBarInfoTopic = "DeskStatusBarConfig";
/// @brief 读取桌面状态栏信息;慎重修改，需和桌面保持协议一致
struct DeskStatusBarInfo
{
    /// @brief 状态栏位置；0=上，1=右，2=下，3=左，其它值=上
    int32_t statusBarLocation;
    /// @brief 状态栏宽度值
    int32_t statusBarWidth;
    /// @brief 顶状态栏高度值
    int32_t statusBarHeight;
    /// @brief 状态栏是否显示；true显示，false隐藏
    bool statusBarVislble;

    DeskStatusBarInfo() : statusBarLocation(0), statusBarWidth(0), statusBarHeight(0), statusBarVislble(false)
    {
    }

    virtual ~DeskStatusBarInfo() {}

    bool operator==(const DeskStatusBarInfo &others)
    {
        return (statusBarLocation == others.statusBarLocation) &&
               (statusBarWidth == others.statusBarWidth) &&
               (statusBarHeight == others.statusBarHeight) &&
               (statusBarVislble == others.statusBarVislble);
    }
};
#endif

struct ItpProcessInfo
{
    int32_t id;
    pid_t pid;
    char process[PROCESS_MAX_NAME_LENGTH];
};

struct UpdateCommand
{
    TpWidget *updateObj = nullptr;
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;
    bool onlyBlit = false;

    UpdateCommand()
    {
    }
};

class appExe;
struct TpAppData
{
    // 主线程ID
    std::thread::id mainThreadId;

    TpList<TpObject *> objectList;
    std::map<TpObject *, bool> vReserveMap;
    // 所有floatscreen列表，用于更新主题样式
    TpList<TpWidget *> floatScreenList;

    std::mutex gMutex;

    // 物理屏幕窗口
    TpFixScreen *vScreen;
    // 应用主窗口，每个应用只有一个
    TpMainWindow *mainWindow = nullptr;

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
    TpWidget *curInputObj = nullptr;

    std::mutex queueSlotMutex_;
    std::queue<std::function<void()>> slotTasks_;

    std::mutex queueUpdateMutex_;
    std::queue<UpdateCommand> updateTasks_;

    // 桌面信息；无桌面则数据无用
    bool isDesk = false;
    DeskStatusBarInfo deskStatusBarInfo_;
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
                TpWidget *childWidgetObj = (TpWidget *)message.user_data0;
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

                    if (object == set->vScreen)
                    {
                        goto finished;
                    }
                }
            deleted:
                set->gMutex.unlock();

                delete object;
                object = nullptr;
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
                            TpWidget *tmp = static_cast<TpWidget *>(iter->first);
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
                            TpWidget *tmp = static_cast<TpWidget *>(mapiter->first);

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
                            TpWidget *tmp = static_cast<TpWidget *>(mapiter->first);

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

    TpMap<IPiWFApiAgent *, TpRect> pixwmMergeUpdateRect;
    TpMap<TpWidget *, ItpObjectPaintInput> mergeUpdateWidget;

    while (!updateCommandQueue.empty())
    {
        UpdateCommand task = updateCommandQueue.front();
        updateCommandQueue.pop();

        if (task.updateObj->objectType() == Tp::TP_FLOAT_OBJECT ||
            task.updateObj->objectType() == Tp::TP_FIXSCREEN_OBJECT ||
            task.updateObj->objectType() == Tp::TP_MAIN_WINDOW_OBJECT)
        {
            if (!task.updateObj->objectActive())
                continue;
        }

        TpObjectData *updateObjSet = static_cast<TpObjectData *>(task.updateObj->objectSets());
        TpObjectData *topScreenSet = static_cast<TpObjectData *>(updateObjSet->top->objectSets());

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

        // 隐藏窗口不处理paint，但要通知TpWM
        if (!task.updateObj->visible())
            continue;

        // 存在该窗口则更新合并区域
        if (mergeUpdateWidget.contains(task.updateObj))
        {
            TpRect taskRect(task.x, task.y, task.w, task.h);

            ItpObjectPaintInput &paintInfo = mergeUpdateWidget[task.updateObj];
            paintInfo.updateRect.unions(taskRect);
        }
        else
        {
            // 不存在构建新数据
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
        TpObjectData *updateObjSet = static_cast<TpObjectData *>(updateWidgetIter.first->objectSets());
        TpObjectData *topScreenSet = static_cast<TpObjectData *>(updateObjSet->top->objectSets());

        ItpObjectPaintInput paintInput = updateWidgetIter.second;

        tinyPiX_wf_lock_mutex(topScreenSet->agent);

        // std::cout << "局部刷新：刷新区域： " << updateWidgetIter.first << " : " << paintInput.updateRect.x << " , " << paintInput.updateRect.y << " , "
        //           << paintInput.updateRect.w << " , " << paintInput.updateRect.h << std::endl;

        // int32_t surfaceWidth = paintInput.surface->width();
        // int32_t surfaceHeight = paintInput.surface->height();
        // std::cout << "Surface尺寸： " << surfaceWidth << "  " << surfaceHeight << std::endl;

        drawWidget(paintInput, updateWidgetIter.first);

        tinyPiX_wf_unlock_mutex(topScreenSet->agent);
    }

    for (const auto &updateInfo : pixwmMergeUpdateRect)
    {
        const TpRect &updateRect = updateInfo.second;
        tinyPiX_wf_update(updateInfo.first, updateRect.x(), updateRect.y(), updateRect.width(), updateRect.height(), true, false);
    }
}

static bool bindVScreen(TpAppData *appData, TpFixScreen *object)
{
    if (!appData)
        return false;

    if (!object)
        return false;

    if (object->objectType() != Tp::TP_FIXSCREEN_OBJECT)
    {
        std::cout << "bind screen type error !" << std::endl;
        return false;
    }

    if (appData->vScreen)
    {
        std::cout << "bind screen only once !" << std::endl;
        return false;
    }

    bool ret = (appData->vScreen != object);

    if (ret)
    {
        appData->gMutex.lock();
        appData->vScreen = object;
        appData->gMutex.unlock();
    }

    appData->thread->start();

    return ret;
}

// 桌面工具栏变化，主窗口要刷新尺寸
static void refreshMainWindow(TpAppData *appData, TpMainWindow *mainWindow, TpObjectData *mainWindowObjData)
{
    // 偏移的XY坐标；和相对于物理屏幕需要裁剪的的宽高值
    int32_t mainWindowX = 0;
    int32_t mainWindowY = 0;
    int32_t offsetW = 0;
    int32_t offsetH = 0;

    if (!appData->isDesk && appData->deskStatusBarInfo_.statusBarVislble)
    {
        int32_t statusBarLocation = appData->deskStatusBarInfo_.statusBarLocation;
        if (statusBarLocation == 0)
        {
            mainWindowY = appData->deskStatusBarInfo_.statusBarHeight;
            offsetH = mainWindowY;
        }
        else if (statusBarLocation == 1)
        {
            offsetW = appData->deskStatusBarInfo_.statusBarWidth;
        }
        else if (statusBarLocation == 2)
        {
            offsetH = appData->deskStatusBarInfo_.statusBarHeight;
        }
        else if (statusBarLocation == 3)
        {
            mainWindowX = appData->deskStatusBarInfo_.statusBarWidth;
            offsetW = mainWindowX;
        }
        else
        {
            mainWindowY = appData->deskStatusBarInfo_.statusBarHeight;
            offsetH = mainWindowY;
        }
    }

    // 调整窗口大小和坐标
    uint32_t rW = 0, rH = 0;
    tinyPiX_wf_get_display_size(mainWindowObjData->agent, &rW, &rH);
    tinyPiX_wf_set_rect(mainWindowObjData->agent, mainWindowX, mainWindowY, rW - offsetW, rH - offsetH);

    mainWindowObjData->offsetX = mainWindowX;
    mainWindowObjData->offsetY = mainWindowY;

    mainWindowObjData->absoluteRect.setX(mainWindowX);
    mainWindowObjData->absoluteRect.setY(mainWindowY);
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
    TpWidget *screenWidget = dynamic_cast<TpWidget *>(setData->vScreen);
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

#endif