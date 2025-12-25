#include "TpDesktopMainWindow.h"
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
#include "TpGateway.h"
#include <TpInteractDataDef/TpDesktopData.h>

struct TpDesktopMainWindowData
{
    // 桌面信息；无桌面则数据无用
    TpDeskStatusBarInfo deskStatusBarInfo_;
};

// 桌面工具栏变化，主窗口要刷新尺寸
static void refreshMainWindow(TpDesktopMainWindowData* deskWindowData, TpMainWindow *mainWindow, TpWidgetData *mainWindowObjData)
{
    // 偏移的XY坐标；和相对于物理屏幕需要裁剪的的宽高值
    int32_t mainWindowX = 0;
    int32_t mainWindowY = 0;
    int32_t offsetW = 0;
    int32_t offsetH = 0;

    if (!TpApp::Inst()->isDesktop() && deskWindowData->deskStatusBarInfo_.statusBarVislble)
    {
        int32_t statusBarLocation = deskWindowData->deskStatusBarInfo_.statusBarLocation;
        if (statusBarLocation == 0)
        {
            mainWindowY = deskWindowData->deskStatusBarInfo_.statusBarHeight;
            offsetH = mainWindowY;
        }
        else if (statusBarLocation == 1)
        {
            offsetW = deskWindowData->deskStatusBarInfo_.statusBarWidth;
        }
        else if (statusBarLocation == 2)
        {
            offsetH = deskWindowData->deskStatusBarInfo_.statusBarHeight;
        }
        else if (statusBarLocation == 3)
        {
            mainWindowX = deskWindowData->deskStatusBarInfo_.statusBarWidth;
            offsetW = mainWindowX;
        }
        else
        {
            mainWindowY = deskWindowData->deskStatusBarInfo_.statusBarHeight;
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

    TpResizeEventData input;
    input.object = mainWindow;
    input.nw = rW - offsetW;
    input.nh = rH - offsetH;
    input.question = TpResizeEvent::TP_NORMAL_CHANGE;
    TpResizeEvent event;
    bool ret = event.construct(&input);

    if (ret)
    {
        refreshCacheImage(mainWindowObjData);
        IssueObjEvent(mainWindow, event, onResizeEvent, true);
    }
}

TpDesktopMainWindow::TpDesktopMainWindow()
    : TpMainWindow()
{
    TpDesktopMainWindowData* deskWindowData = new TpDesktopMainWindowData();
    data_ = deskWindowData;

#if 1 // 处理桌面 topbar信息

    // 初始化网关
    bool gatewayInitRes = initializeGateway();

    // 接收桌面工具栏信息
    auto RecvDeskBarFunc = [=](const char *topic, const void *data, uint32_t dataLen)
    {
        TpDeskStatusBarInfo recvInfo;
        recvInfo.StructDeserialize(data, dataLen);

        std::cout << "桌面信息：" << recvInfo.statusBarLocation << " , " << recvInfo.statusBarWidth
                  << " , " << recvInfo.statusBarHeight << " , " << recvInfo.statusBarVislble << std::endl;

        // 主屏幕根据Bar数据是否变化决定是否刷新主屏
        if (recvInfo == deskWindowData->deskStatusBarInfo_)
            return;

        deskWindowData->deskStatusBarInfo_ = recvInfo;

        // 更新主屏
        // TpWidgetData *mainWindowData = static_cast<TpWidgetData *>(set->mainWindow->objectSets());
        // refreshMainWindow(set, set->mainWindow, mainWindowData);
    };

    // 订阅桌面数据
    subscribeGatewayData(TpDeskStatusBarInfoKey, RecvDeskBarFunc);

    // 尝试读取桌面信息；如果没有桌面则读取失败
    if (!TpApp::Inst()->isDesktop())
    {
        // 通知桌面应用启动
        bool pubRunData = true;
        // std::cout << "发布应用上线!" <<std::endl;
        publishGatewayData(TpDeskAppStartKey, &pubRunData, sizeof(bool));
    }

#endif

    // TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    // 调整窗口大小
    // refreshMainWindow(appData, this, widgetData);
}

TpDesktopMainWindow::~TpDesktopMainWindow()
{
}

void TpDesktopMainWindow::setBackGroundColor(const TpColors &color, bool enable)
{
    // TpDesktopMainWindow 不能透明,且必须有背景色
    TpColors newColor = color;
    newColor.setAlpha(255);
    TpScreen::setBackGroundColor(newColor, true);
}

void TpDesktopMainWindow::setBackGroundColor(int32_t color, bool enable)
{
    TpScreen::setBackGroundColor(_RGBA(_R(color), _G(color), _B(color), 255), true);
}

void TpDesktopMainWindow::setBackGroundColor(const TpBrush &bgBrush, bool enable)
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

void TpDesktopMainWindow::setEnableBackGroundColor(bool enable)
{
    TpScreen::setEnableBackGroundColor(true);
}

void TpDesktopMainWindow::setBorderColor(const TpColors &color, bool enable)
{
    // TpDesktopMainWindow没有边框颜色
    TpScreen::setBorderColor(color, false);
}

void TpDesktopMainWindow::setBorderColor(int32_t color, bool enable)
{
    TpScreen::setBorderColor(color, false);
}

void TpDesktopMainWindow::setBorderColor(const TpBrush &borderBrush, bool enable)
{
    TpScreen::setBorderColor(borderBrush, false);
}

void TpDesktopMainWindow::setEnabledBorderColor(bool enable)
{
    TpScreen::setEnabledBorderColor(false);
}

bool TpDesktopMainWindow::onResizeEvent(TpResizeEvent *event)
{
    return TpScreen::onResizeEvent(event);
}
