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

TpMainWindow::TpMainWindow()
    : TpScreen("tinyPiX_USE_Float")
{
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

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    widgetData->top = this->topObject();

    uint32_t rW = 0, rH = 0;
    tinyPiX_wf_get_display_size(widgetData->agent, &rW, &rH);
    tinyPiX_wf_set_rect(widgetData->agent, 0, 0, rW, rH);

    widgetData->offsetX = 0;
    widgetData->offsetY = 0;

    widgetData->absoluteRect.setX(0);
    widgetData->absoluteRect.setY(0);

    refreshCacheImage(widgetData);

    setBackGroundColor(_RGBA(255, 255, 255, 255));

    tinyPiX_wf_set_visible(widgetData->agent, true);
    widgetData->visible = true;

    // tinyPiX_wf_set_visible(widgetData->agent, false);
    // widgetData->visible = false;
}

TpMainWindow::~TpMainWindow()
{
}

Tp::TpObjectType TpMainWindow::objectType()
{
    return Tp::TP_MAIN_WINDOW_OBJECT;
}

void TpMainWindow::setBackGroundColor(const TpColors &color, bool enable)
{
    // TpMainWindow 不能透明,且必须有背景色
    TpColors newColor = color;
    newColor.setAlpha(255);
    TpScreen::setBackGroundColor(newColor, true);
}

void TpMainWindow::setBackGroundColor(int32_t color, bool enable)
{
    TpScreen::setBackGroundColor(_RGBA(_R(color), _G(color), _B(color), 255), true);
}

void TpMainWindow::setBackGroundColor(const TpBrush &bgBrush, bool enable)
{
    TpBrush newBrush = bgBrush;
    TpColors setColorObj = newBrush.color();
    setColorObj.setAlpha(255);
    newBrush.setColor(setColorObj);

    TpGradient *brushGradiwnt = newBrush.gradient();
    if (brushGradiwnt)
    {
        TpList<std::pair<float, int32_t>> colorAtList = brushGradiwnt->getColors();
        for (auto &colorAt : colorAtList)
        {
            colorAt.second = _RGBA(_R(colorAt.second), _G(colorAt.second), _B(colorAt.second), 255);
            brushGradiwnt->setColorAt(colorAt.first, colorAt.second);
        }
    }

    TpScreen::setBackGroundColor(newBrush, true);
}

void TpMainWindow::setEnableBackGroundColor(bool enable)
{
    TpScreen::setEnableBackGroundColor(true);
}

void TpMainWindow::setBorderColor(const TpColors &color, bool enable)
{
    // TpMainWindow没有边框颜色
    TpScreen::setBorderColor(color, false);
}

void TpMainWindow::setBorderColor(int32_t color, bool enable)
{
    TpScreen::setBorderColor(color, false);
}

void TpMainWindow::setBorderColor(const TpBrush &borderBrush, bool enable)
{
    TpScreen::setBorderColor(borderBrush, false);
}

void TpMainWindow::setEnabledBorderColor(bool enable)
{
    TpScreen::setEnabledBorderColor(false);
}

bool TpMainWindow::onResizeEvent(TpResizeEvent *event)
{
    return TpScreen::onResizeEvent(event);
}
