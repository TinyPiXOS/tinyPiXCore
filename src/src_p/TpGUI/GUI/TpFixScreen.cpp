#include "TpFixScreen.h"
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
#include "TpApp.h"

struct TpFixScreenData
{
    uint8_t alpha;
    uint32_t color;
    int32_t attr;

    TpFixScreenData()
        : alpha(0), color(0), attr(0)
    {
    }
};

TpFixScreen::TpFixScreen(const char *type)
    : TpScreen(type)
{
    TpFixScreenData *screenData = new TpFixScreenData();
    data_ = screenData;

    if (this->objectType() != Tp::TP_FIXSCREEN_OBJECT)
    {
        TpApp::Inst()->sendDelete(this);
    }

    setVisible(true);

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    uint32_t rW = 0, rH = 0;
    tinyPiX_wf_get_display_size(widgetData->agent, &rW, &rH);

    widgetData->absoluteRect.setRect(0, 0, rW, rH);
    widgetData->logicalRect.setRect(0, 0, rW, rH);

    screenData->alpha = 0xff;
    screenData->color = TpColors::Black;
    screenData->attr = TpFixScreen::ITP_POP_STYLE;

    this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);

    widgetData->top = this->topObject();
}

TpFixScreen::~TpFixScreen()
{
    TpFixScreenData *screenData = static_cast<TpFixScreenData *>(data_);
    if (screenData)
    {
        delete screenData;
        screenData = nullptr;
        data_ = nullptr;
    }
}

Tp::TpObjectType TpFixScreen::objectType()
{
    return Tp::TP_FIXSCREEN_OBJECT;
}

int32_t TpFixScreen::setVScreenAttribute(uint8_t alpha, uint32_t color, int32_t screenAttr)
{
    TpFixScreenData *screenData = static_cast<TpFixScreenData *>(data_);
    if (!screenData)
        return false;

    switch (screenAttr)
    {
    case TpFixScreen::ITP_FULL_STYLE:
    case TpFixScreen::ITP_POP_STYLE:
    {
    }
    break;
    default:
        return false;
    }

    TpObjectData *set = (TpObjectData *)this->objectSets();

    if (set)
    {
        screenData->alpha = alpha;
        screenData->color = color;
        screenData->attr = screenAttr;

        return tinyPiX_wf_send_app_state(set->agent, TP_INVALIDATE_VALUE, this->visible(), this->objectActive(), color, alpha, screenAttr);
    }

    return false;
}

bool TpFixScreen::onActiveEvent(TpActiveEvent *event)
{
    TpFixScreenData *screenData = static_cast<TpFixScreenData *>(data_);
    if (!screenData)
        return false;

    TpAppData *appData = (TpAppData *)TpApp::Inst()->appObjectSet();
    if (appData->mainWindow)
    {
        // pid_t pid = getpid();
        // std::cout << "当前进程 PID: " << pid << "  窗体显隐状态： " << visible() << std::endl;

        TpScreen* mainWindowScrenn = dynamic_cast<TpScreen*>(appData->mainWindow);

        TpWidgetData *mainWindowObjData = (TpWidgetData *)mainWindowScrenn->objectSets();
        tinyPiX_wf_set_visible(mainWindowObjData->agent, visible());
        mainWindowScrenn->setVisible(visible());
        // mainWindowObjData->visible = visible();
    }

    return this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);
}
