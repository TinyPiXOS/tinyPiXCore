#ifndef __TP_APP_DEFINE_H
#define __TP_APP_DEFINE_H

#include "TpWidget.h"
#include "TpCoreApp_p.h"
#include "TpMap.h"
#include "TpFixScreen.h"
#include "TpMainWindow.h"
#include "TpClipboard.h"
#include "TpVirtualKeyboard.h"
#include <TpInteractDataDef/TpDesktopData.h>
#include <TpApp.h>
#include <TpThread.h>

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
    TpDeskStatusBarInfo deskStatusBarInfo_;
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
        ITpUserEvent message;
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

#endif