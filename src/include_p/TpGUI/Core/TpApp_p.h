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
#include "TpFixScreen.h"
#include "TpShareMemory.h"
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

/// @brief 读取桌面信息;慎重修改，需和桌面保持协议一致
struct DeskTopBarInfo
{
    /// @brief 顶部工具栏宽度值
    int32_t topBarWidth;
    /// @brief 顶部工具栏高度值
    int32_t topBarHeight;
    /// @brief 顶部工具栏是否显示；true显示，false隐藏
    bool topBarisVislble;

    DeskTopBarInfo() : topBarWidth(0), topBarHeight(0), topBarisVislble(false)
    {
    }

    virtual ~DeskTopBarInfo() {}
};

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
    TpWidget *curInputObj = nullptr;

    std::mutex queueSlotMutex_;
    std::queue<std::function<void()>> slotTasks_;

    std::mutex queueUpdateMutex_;
    std::queue<UpdateCommand> updateTasks_;

    // 桌面信息；无桌面则数据无用
    bool isDesk = false;
    DeskTopBarInfo desktopBarInfo_;
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

                    // TpObjectData *vScreenObjDaata = (TpObjectData *)set->vScreen->objectSets();
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
    // std::pair<TpWidget *, ItpObjectPaintInput> fixScreenPair;
    // fixScreenPair.first = nullptr;

    while (!updateCommandQueue.empty())
    {
        UpdateCommand task = updateCommandQueue.front();
        updateCommandQueue.pop();

        if (task.updateObj->objectType() == Tp::TP_FLOAT_OBJECT || task.updateObj->objectType() == Tp::TP_TOP_OBJECT)
        {
            if (!task.updateObj->objectActive())
                continue;
        }
        if (!task.updateObj->visible())
            continue;

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

        // 存在该窗口则更新合并区域
        if (mergeUpdateWidget.contains(task.updateObj))
        {
            TpRect taskRect(task.x, task.y, task.w, task.h);

            // if (task.updateObj->objectType() == Tp::TP_TOP_OBJECT)
            // {
            //     fixScreenPair.second.updateRect.unions(taskRect);
            // }
            // else
            {
                ItpObjectPaintInput &paintInfo = mergeUpdateWidget[task.updateObj];
                paintInfo.updateRect.unions(taskRect);
            }
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

            // if (task.updateObj->objectType() == Tp::TP_TOP_OBJECT)
            // {
            //     fixScreenPair.first = task.updateObj;
            //     fixScreenPair.second = paintInput;
            // }
            // else
            {

                mergeUpdateWidget[task.updateObj] = paintInput;
            }
        }
    }

    // // 先绘制fixscreen
    // if (fixScreenPair.first)
    // {
    //     TpObjectData *updateObjSet = static_cast<TpObjectData *>(fixScreenPair.first->objectSets());
    //     TpObjectData *topScreenSet = static_cast<TpObjectData *>(updateObjSet->top->objectSets());

    //     ItpObjectPaintInput paintInput = fixScreenPair.second;

    //     tinyPiX_wf_lock_mutex(topScreenSet->agent);

    //     drawWidget(paintInput, fixScreenPair.first);

    //     tinyPiX_wf_unlock_mutex(topScreenSet->agent);
    // }

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