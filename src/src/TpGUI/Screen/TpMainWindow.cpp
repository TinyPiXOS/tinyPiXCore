#include "TpMainWindow.h"
#include "TpApp.h"
#include "TpDef.h"

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

enum
{
    ITP_FULL_STYLE,
    ITP_POP_STYLE,
};

TpMainWindow::TpMainWindow(const char *type)
    : TpScreen(type)
{
    TpMainWindowData *mainWindowData = new TpMainWindowData();
    data_ = mainWindowData;

    if (this->objectType() != Tp::TP_TOP_OBJECT)
    {
        TpApp::Inst()->sendDelete(this);
    }

    TpObjectData *set = static_cast<TpObjectData *>(this->objectSets());
    if (!set)
        return;

    set->top = this->topObject();

    uint32_t rW = 0, rH = 0;
    tinyPiX_wf_get_display_size(set->agent, &rW, &rH);

    tinyPiX_wf_set_rect(set->agent, 0, 30, rW, rH - 30);

    set->offsetX = 0;
    set->offsetY = 30;

    TpWidget::setRect(0, 30, rW, rH - 30);

    // set->absoluteRect.setRect(0, 0, rW, rH);
    // set->logicalRect.setRect(0, 0, rW, rH);

    // mainWindowData->alpha = 0xff;
    // mainWindowData->color = TpColors::Black;
    // mainWindowData->attr = ITP_POP_STYLE;

    // tinyPiX_wf_send_app_state(set->agent, TP_INVALIDATE_VALUE, visible(), objectActive(),
    //                           mainWindowData->color, mainWindowData->alpha, mainWindowData->attr);
}

TpMainWindow::~TpMainWindow()
{
    TpMainWindowData *mainWindowData = static_cast<TpMainWindowData *>(data_);
    if (mainWindowData)
    {
        delete mainWindowData;
        mainWindowData = nullptr;
        data_ = nullptr;
    }
}

Tp::ItpObjectType TpMainWindow::objectType()
{
    return Tp::TP_TOP_OBJECT;
}

bool TpMainWindow::onActiveEvent(TpActiveEvent *event)
{
    return true;
    // TpFixScreenData *screenData = static_cast<TpFixScreenData *>(data_);
    // if (!screenData)
    //     return false;

    // return this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);
}
