#include "TpDesktopMainWindow.h"
#include "TpApp.h"
#include "TpColors.h"
#include <tinyPiXWF.h>
#include <mutex>
#include "TpCssParser.h"
#include "TpString.h"
#include "TpVariant.h"
#include "TpGateway.h"
#include "TpDeskStatusInfo.h"

#include <InteractData/TpDesktopData.h>

static TpRect caculateMainScreenRect(const TpDeskStatusBarInfo &statusInfo, TpMainWindow *mainWindow)
{
    // 偏移的XY坐标；和相对于物理屏幕需要裁剪的的宽高值
    TpRect resRect;

    if (!TpApp::Inst()->isDesktop() && statusInfo.statusBarVislble)
    {
        TpSize displayScreenSize = mainWindow->screenSize();

        int32_t statusBarLocation = statusInfo.statusBarLocation;
        if (statusBarLocation == 0)
        {
            resRect.setY(statusInfo.statusBarHeight);
            resRect.setWidth(displayScreenSize.width());
            resRect.setHeight(displayScreenSize.height() - statusInfo.statusBarHeight);
        }
        else if (statusBarLocation == 1)
        {
            resRect.setWidth(displayScreenSize.width() - statusInfo.statusBarWidth);
            resRect.setHeight(displayScreenSize.height());
        }
        else if (statusBarLocation == 2)
        {
            resRect.setWidth(displayScreenSize.width());
            resRect.setHeight(displayScreenSize.height() - statusInfo.statusBarHeight);
        }
        else if (statusBarLocation == 3)
        {
            resRect.setX(statusInfo.statusBarWidth);
            resRect.setWidth(displayScreenSize.width() - statusInfo.statusBarWidth);
            resRect.setHeight(displayScreenSize.height());
        }
        else
        {
            resRect.setY(statusInfo.statusBarHeight);
            resRect.setWidth(displayScreenSize.width());
            resRect.setHeight(displayScreenSize.height() - statusInfo.statusBarHeight);
        }
    }

    return resRect;
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

        TpApp::Inst()->setClipRect(caculateMainScreenRect(recvInfo, this));
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
}

TpDesktopMainWindow::~TpDesktopMainWindow()
{
}
