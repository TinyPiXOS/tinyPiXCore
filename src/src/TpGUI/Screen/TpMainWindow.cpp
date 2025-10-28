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

    if (this->objectType() != Tp::TP_FLOAT_OBJECT)
    {
        TpApp::Inst()->sendDelete(this);
    }

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    set->top = this->topObject();

    // 调整窗口大小
    TpAppData *appData = (TpAppData *)TpApp::Inst()->appObjectSet();

    int32_t fixScreenY = 0;
    if (!appData->isDesk && appData->desktopBarInfo_.topBarisVislble)
    {
        fixScreenY = appData->desktopBarInfo_.topBarHeight;
    }

    uint32_t rW = 0, rH = 0;
    tinyPiX_wf_get_display_size(set->agent, &rW, &rH);

    tinyPiX_wf_set_rect(set->agent, 0, fixScreenY, rW, rH - fixScreenY);
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

Tp::ItpObjectType TpMainWindow::objectType()
{
    return Tp::TP_FLOAT_OBJECT;
}
