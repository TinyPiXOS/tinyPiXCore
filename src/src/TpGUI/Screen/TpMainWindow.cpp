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
    uint8_t alpha;
    uint32_t color;
    int32_t attr;

    TpMainWindowData()
        : alpha(0), color(0), attr(0)
    {
    }
};

TpMainWindow::TpMainWindow(const char *type)
    : TpScreen(type)
{
    TpMainWindowData *screenData = new TpMainWindowData();
    data_ = screenData;

    if (this->objectType() != Tp::TP_TOP_OBJECT)
    {
        TpApp::Inst()->sendDelete(this);
    }

    TpObjectData *set = static_cast<TpObjectData *>(this->objectSets());
    if (set)
    {
        uint32_t rW = 0, rH = 0;
        tinyPiX_wf_get_display_size(set->agent, &rW, &rH);

        set->absoluteRect.setRect(0, 0, rW, rH);
        set->logicalRect.setRect(0, 0, rW, rH);

        screenData->alpha = 0xff;
        screenData->color = TpColors::Black;
        screenData->attr = TpMainWindow::ITP_POP_STYLE;

        this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);
    }

    set->top = this->topObject();
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
    return Tp::TP_TOP_OBJECT;
}

int32_t TpMainWindow::setVScreenAttribute(uint8_t alpha, uint32_t color, int32_t screenAttr)
{
    TpMainWindowData *screenData = static_cast<TpMainWindowData *>(data_);
    if (!screenData)
        return false;

    switch (screenAttr)
    {
    case TpMainWindow::ITP_FULL_STYLE:
    case TpMainWindow::ITP_POP_STYLE:
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

bool TpMainWindow::onActiveEvent(TpActiveEvent *event)
{
    TpMainWindowData *screenData = static_cast<TpMainWindowData *>(data_);
    if (!screenData)
        return false;

    return this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);
}
