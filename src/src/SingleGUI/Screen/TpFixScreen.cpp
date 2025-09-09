/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
#include "TpFixScreen.h"
#include "TpApp.h"
#include "TpDef.h"
#include "TpColors.h"
#include <tinyPiXWF.h>
#include <mutex>
#include "Utils/TpCssParser.h"
#include "SingleGUI/TpDefaultCss.h"
#include "TpString.h"
#include "TpVariant.h"
#include "TpDefaultCss.h"

struct TpFixScreenData
{
    uint8_t alpha;
    uint32_t color;
    int32_t attr;

    // std::shared_ptr<TpCssParser> TpCssParser_;

    TpFixScreenData()
        : alpha(0), color(0), attr(0)
    {
        // TpCssParser_ = std::make_shared<TpCssParser>(defaultCssStr());
    }
};

TpFixScreen::TpFixScreen(const char *type)
    : TpScreen(type)
{
    TpFixScreenData *screenData = new TpFixScreenData();
    data_ = screenData;

    // TpApp::Inst()->sendRegister(this);

    if (this->objectType() != TP_TOP_OBJECT)
    {
        TpApp::Inst()->sendDelete(this);
    }

    ItpObjectSet *set = (ItpObjectSet *)this->objectSets();
    if (set)
    {
        uint32_t rW = 0, rH = 0;
        tinyPiX_wf_get_display_size(set->agent, &rW, &rH);

        set->absoluteRect.setRect(0, 0, rW, rH);

        set->logicalRect.setRect(0, 0, rW, rH);

        screenData->alpha = 0xff;
        screenData->color = TpColors::Black;
        screenData->attr = TpFixScreen::ITP_POP_STYLE;

        this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);
    }

    set->top = this->topObject();
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

ItpObjectType TpFixScreen::objectType()
{
    return TP_TOP_OBJECT;
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

    ItpObjectSet *set = (ItpObjectSet *)this->objectSets();

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

    return this->setVScreenAttribute(screenData->alpha, screenData->color, screenData->attr);
}
