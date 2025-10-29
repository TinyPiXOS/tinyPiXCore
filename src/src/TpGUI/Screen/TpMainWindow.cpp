#include "TpMainWindow.h"
#include "TpApp.h"
#include "TpDef.h"
#include "TpColors.h"
#include <tinyPiXWF.h>
#include <mutex>
#include "TpCssParser.h"
#include "TpDefaultCss.h"
#include "TpString.h"
#include "TpVariant.h"
#include "TpDefaultCss.h"
#include "TpApp_p.h"

struct TpMainWindowData
{
    TpMainWindowData()
    {
    }
};

TpMainWindow::TpMainWindow(const char *type)
    : TpScreen(type)
{
    TpMainWindowData *screenData = new TpMainWindowData();
    data_ = screenData;

    if (this->objectType() != Tp::TP_MAIN_WINDOW_OBJECT)
    {
        TpApp::Inst()->sendDelete(this);
        return;
    }

    // 判断是否已经有mainwindow了
    TpAppData *appData = (TpAppData *)TpApp::Inst()->appObjectSet();
    if (appData->mainWindow)
    {
        TpApp::Inst()->sendDelete(this);
        return;
    }
    appData->mainWindow = this;

    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    set->top = this->topObject();

    // 调整窗口大小
    refreshMainWindow(appData, set);
}

TpMainWindow::~TpMainWindow()
{
    TpMainWindowData *screenData = static_cast<TpMainWindowData *>(data_);
    if (screenData)
    {
        delete screenData;
        screenData = nullptr;
        data_ = nullptr;
    }
}

Tp::TpObjectType TpMainWindow::objectType()
{
    return Tp::TP_MAIN_WINDOW_OBJECT;
}
