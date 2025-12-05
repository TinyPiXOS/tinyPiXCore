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

TpMainWindow::TpMainWindow(const char *type)
    : TpScreen(type)
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

    // 调整窗口大小
    refreshMainWindow(appData, this, widgetData);

    setBackGroundColor(_RGBA(255, 255, 255, 255));

    tinyPiX_wf_set_visible(widgetData->agent, true);
    widgetData->visible = true;
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
