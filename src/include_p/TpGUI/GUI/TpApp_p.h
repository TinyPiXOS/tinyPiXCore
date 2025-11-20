#ifndef PROCESS_MAX_NAME_LENGTH
#define PROCESS_MAX_NAME_LENGTH 1024
#endif

#ifndef __TP_APP_PRIVATE_H
#define __TP_APP_PRIVATE_H

#include "TpClipboard.h"
#include "TpAutoObject.h"
#include "TpScreen.h"
#include "TpConfig.h"
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
#include "TpObject_p.h"
#include "TpMainWindow.h"
#include "TpShareMemory.h"
#include "TpFixScreen.h"
#include "TpGateway.h"
#include "TpMainWindow.h"
#include "thorVG/thorvg.h"
#include "TpCoreApp_p.h"
#include <TpThread.h>
#include <TpApp.h>
#include <TpDir.h>

#include <tinyPiXApi.h>
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

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

class AppExec;
struct TpAppData : TpCoreAppData
{
    std::map<TpObject *, bool> vReserveMap;
    // 所有floatscreen列表，用于更新主题样式
    TpList<TpWidget *> floatScreenList;

    // 物理屏幕窗口
    TpFixScreen *vScreen;
    // 应用主窗口，每个应用只有一个
    TpMainWindow *mainWindow = nullptr;

    TpClipboard *clipboard;

    AppExec *appExecThread;

    int32_t eventType;

    Tp::SystemTheme systemTheme = Tp::Default;
    tpShared<TpCssParser> cssParser_ = tpMakeShared<TpCssParser>();

    // 全局唯一单例虚拟键盘
    TpVirtualKeyboard *virtualKeyboard = nullptr;
    TpWidget *curInputObj = nullptr;

    std::mutex queueUpdateMutex_;
    std::queue<UpdateCommand> updateTasks_;

    // 桌面信息；无桌面则数据无用
    bool isDesk = false;
    DeskStatusBarInfo deskStatusBarInfo_;
};

// UI应用处理线程
class AppExec : public TpThread
{
public:
    AppExec() : TpThread() {};

    AppExec(TpAppData *appData)
        : TpThread(), appData_(appData) {};

    virtual ~AppExec()
    {
        appData_ = nullptr;
    };

    virtual void run()
    {
        ItpUserEvent message;
        bool ret = false;
        if (!appData_)
            return;

        while (true)
        {
            if (appData_->vScreen == nullptr)
                break;

            ret = appData_->message->recvWait(&message);
            if (!ret)
                continue;

            switch (message.type)
            {
            case TpApp::TP_REGISTER_ACT:
            {
                // add to objectList
                appData_->gMutex.lock();
                TpWidget *childWidgetObj = (TpWidget *)message.user_data0;
                if (childWidgetObj)
                {
                    switch (childWidgetObj->objectType())
                    {
                    case Tp::TP_FLOAT_OBJECT:
                    {
                        appData_->vReserveMap[childWidgetObj] = childWidgetObj->visible();
                    }
                    break;
                    }
                    appData_->objectList.emplace_back(childWidgetObj);
                    appData_->gMutex.unlock();
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

                appData_->gMutex.lock();

                if (object)
                {
                    std::map<TpObject *, bool>::iterator mapiter = appData_->vReserveMap.find(object);

                    if (mapiter != appData_->vReserveMap.end())
                    {
                        appData_->vReserveMap.erase(mapiter);
                    }

                    auto objFindIter = std::find(appData_->objectList.begin(), appData_->objectList.end(), object);
                    if (objFindIter != appData_->objectList.end())
                    {
                        appData_->objectList.remove(*objFindIter);
                    }

                    auto floatFindIter = std::find(appData_->floatScreenList.begin(), appData_->floatScreenList.end(), object);
                    if (floatFindIter != appData_->floatScreenList.end())
                    {
                        appData_->floatScreenList.remove(*floatFindIter);
                    }

                    if (object == appData_->vScreen)
                    {
                        goto finished;
                    }
                }
            deleted:
                appData_->gMutex.unlock();

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
                TpObject *vScreen = appData_->vScreen;

                if (vScreen == message.user_data0)
                {
                    TpScreen *screenObj = static_cast<TpScreen *>(vScreen);

                    // exclude desktop
                    // if (screenObj->objectLayer() != Tp::TP_WM_DESK)
                    if (screenObj->objectType() != Tp::TP_MAIN_WINDOW_OBJECT)
                    {
                        if (vScreen)
                        {
                            screenObj->setVisible(false);
                        }

                        appData_->gMutex.lock();

                        std::map<TpObject *, bool>::iterator iter = appData_->vReserveMap.begin();
                        for (; iter != appData_->vReserveMap.end(); iter++)
                        {
                            TpWidget *tmp = static_cast<TpWidget *>(iter->first);
                            iter->second = tmp->visible();
                            tmp->setVisible(false);
                        }

                        appData_->gMutex.unlock();
                    }
                }
            }
            break;
            case TpApp::TP_ACTIVE_ACT:
            {
                bool actived = message.user_code;
                TpObject *object = (TpObject *)message.user_data0;

                TpObject *vScreen = appData_->vScreen;

                if (((TpScreen *)vScreen)->objectLayer() != Tp::TP_WM_DESK)
                {
                    appData_->gMutex.lock();

                    if (actived)
                    {
                        std::map<TpObject *, bool>::iterator mapiter = appData_->vReserveMap.begin();
                        for (; mapiter != appData_->vReserveMap.end(); mapiter++)
                        {
                            TpWidget *tmp = static_cast<TpWidget *>(mapiter->first);

                            if (tmp != appData_->vScreen)
                            {
                                tmp->setVisible(mapiter->second);
                            }
                        }
                    }
                    else
                    {
                        std::map<TpObject *, bool>::iterator mapiter = appData_->vReserveMap.begin();
                        for (; mapiter != appData_->vReserveMap.end(); mapiter++)
                        {
                            TpWidget *tmp = static_cast<TpWidget *>(mapiter->first);

                            if (tmp != appData_->vScreen)
                            {
                                mapiter->second = tmp->visible();
                                tmp->setVisible(false);
                            }
                        }
                    }

                    appData_->gMutex.unlock();
                }
            }
            break;
            }
        }
    appover:
        appData_->running = false;

        if (appData_->waitRun == false)
        {
            exit(0);
        }
    };

private:
    TpAppData *appData_;
};

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

    // static int32_t testRefreshIndex = 0;

    for (const auto &updateInfo : pixwmMergeUpdateRect)
    {
        const TpRect &updateRect = updateInfo.second;

        // static int testRefreshIndex = 0;
        // std::cout << "TpWM刷新： " << testRefreshIndex++ << "  ;" << updateRect.x() << " : " << updateRect.y() << " , " << updateRect.width() << " , "
                //   << updateRect.height() << std::endl;

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

    appData->appExecThread->start();

    return ret;
}

// 桌面工具栏变化，主窗口要刷新尺寸
static void refreshMainWindow(TpAppData *appData, TpMainWindow *mainWindow, TpWidgetData *mainWindowObjData)
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