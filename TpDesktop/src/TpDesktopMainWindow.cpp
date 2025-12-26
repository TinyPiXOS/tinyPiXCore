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
#include "TpWidget_p.h"
#include "TpGateway.h"
#include "TpDeskStatusInfo.h"

#include <InteractData/TpDesktopData.h>

// 桌面工具栏变化，主窗口要刷新尺寸
static void refreshMainWindow(const TpDeskStatusBarInfo& statusInfo, TpMainWindow *mainWindow, TpWidgetData *mainWindowObjData)
{
    // 偏移的XY坐标；和相对于物理屏幕需要裁剪的的宽高值
    int32_t mainWindowX = 0;
    int32_t mainWindowY = 0;
    int32_t offsetW = 0;
    int32_t offsetH = 0;

    if (!TpApp::Inst()->isDesktop() && statusInfo.statusBarVislble)
    {
        int32_t statusBarLocation = statusInfo.statusBarLocation;
        if (statusBarLocation == 0)
        {
            mainWindowY = statusInfo.statusBarHeight;
            offsetH = mainWindowY;
        }
        else if (statusBarLocation == 1)
        {
            offsetW = statusInfo.statusBarWidth;
        }
        else if (statusBarLocation == 2)
        {
            offsetH = statusInfo.statusBarHeight;
        }
        else if (statusBarLocation == 3)
        {
            mainWindowX = statusInfo.statusBarWidth;
            offsetW = mainWindowX;
        }
        else
        {
            mainWindowY = statusInfo.statusBarHeight;
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
        if (recvInfo == TpDeskStatusInfo::Instance()->statusInfo())
            return;

        TpDeskStatusInfo::Instance()->setStatusInfo(recvInfo);

        // 更新主屏
        TpWidgetData *mainWindowData = static_cast<TpWidgetData *>(this->objectSets());
        refreshMainWindow(recvInfo, this, mainWindowData);
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

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    // 调整窗口大小
    refreshMainWindow(TpDeskStatusInfo::Instance()->statusInfo(), this, widgetData);
}

TpDesktopMainWindow::~TpDesktopMainWindow()
{
}
