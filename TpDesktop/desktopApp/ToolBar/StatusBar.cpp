#include "StatusBar.h"
#include "TpFont.h"
#include "TpString.h"
#include "DeskTopGlobal.hpp"
#include "SystemInfo/TpDisplay.h"
#include "TpTime.h"
#include "TpDate.h"
#include "TpEvent.h"
#include "TpImage.h"
#include "TpBluetoothLocal.h"
#include "TpNetworkInterface.h"
#include <cmath>
#include <InteractData/TpDesktopData.h>
#include "NavigationBar.h"
#include "TpGraphicsBlurEffect.h"

bool globalSystemLockStatus = false;

#ifndef TOP_BAR_COLOR
#define TOP_BAR_COLOR _RGBA(0, 0, 0, 0)
#endif

// 计算相对亮度（WCAG标准）
static double calculateLuminance(int r, int g, int b)
{
    double rsRGB = r / 255.0;
    double gsRGB = g / 255.0;
    double bsRGB = b / 255.0;

    double rLinear = (rsRGB <= 0.03928) ? rsRGB / 12.92 : pow((rsRGB + 0.055) / 1.055, 2.4);
    double gLinear = (gsRGB <= 0.03928) ? gsRGB / 12.92 : pow((gsRGB + 0.055) / 1.055, 2.4);
    double bLinear = (bsRGB <= 0.03928) ? bsRGB / 12.92 : pow((bsRGB + 0.055) / 1.055, 2.4);

    return 0.2126 * rLinear + 0.7152 * gLinear + 0.0722 * bLinear;
}

StatusBar::StatusBar()
    : TpDialog("tinyPiX_SYS_Float_0531acbf04")
{
    subscribeGatewayData(TpDeskAppStartKey, this);
    subscribeGatewayData(TpChangeDeskStatusBarVisibleKey, this);
    subscribeGatewayData(TpChangeDeskStatusBarStyleKey, this);

    setBackGroundColor(TOP_BAR_COLOR);

    initUI();
}

StatusBar::~StatusBar()
{
}

void StatusBar::setColor(const int32_t &color)
{
    setBackGroundColor(color);
    double luminance = calculateLuminance(_R(color), _G(color), _B(color));
    changeStyle((luminance > 0.5) ? Black : White);

    // 同步更新导航条样式
    globalNavigationBar_->setStyle((luminance > 0.5) ? Black : White);
}

void StatusBar::setVisible(bool visible)
{
    TpDialog::setVisible(visible);

    // 将工具栏数据写入共享内存
    refreshDeskBarInfo();
}

void StatusBar::recvData(const char *topic, const void *data, const uint32_t &size)
{
    // 收到应用上线数据，发布数据
    TpString topicStr(topic);
    if (topicStr.compare(TpDeskAppStartKey) == 0)
    {
        // std::cout << "发布状态栏信息" <<std::endl;
        refreshDeskBarInfo();
    }
    else if (topicStr.compare(TpChangeDeskStatusBarVisibleKey) == 0)
    {
        TpChangeDeskStatusBarVisible recvData;
        recvData.StructDeserialize(data, size);
        setVisible(recvData.visible);
    }
    else if (topicStr.compare(TpChangeDeskStatusBarStyleKey) == 0)
    {
        // std::cout << "********************收到状态栏样式变化: " << topic << std::endl;

        TpChangeDeskStatusBarStyle recvData;
        recvData.StructDeserialize(data, size);

        // 调整状态栏组件显示颜色
        setColor(recvData.bgRgba);
    }
    else
    {
    }
}

bool StatusBar::onResizeEvent(TpResizeEvent *event)
{
    TpDialog::onResizeEvent(event);

    caculateTopAppPos();

    // 将工具栏数据通知应用
    if (visible())
        refreshDeskBarInfo();

    return true;
}

bool StatusBar::onMousePressEvent(TpMouseEvent *event)
{
    TpDialog::onMousePressEvent(event);

    pressPoint_ = event->globalPos();

    // 获取桌面抓图作为背景
    if (globalTopSettingBar_ && !tpFuzzyCompare(globalTopSettingBar_->windowOpacity(), 1.0f))
    {
        // IPiWFSurface *deskSurface = tinyPiX_sys_get_process_surface(globalAgent, getpid());
        // TpImage newBgImage;
        // newBgImage.load(deskSurface);

        // globalTopSettingBar_->setBackGroundImage(newBgImage);

        // TpGraphicsBlurEffect btnBlurEffect;
        // btnBlurEffect.setBlurRadius(100);
        // globalTopSettingBar_->setGraphicsEffect(btnBlurEffect);
        // globalTopSettingBar_->setEnableGraphicsEffect(true);
    }

    return true;
}

bool StatusBar::onMouseRleaseEvent(TpMouseEvent *event)
{
    if (globalTopSettingBar_)
    {
        if (globalTopSettingBar_->windowOpacity() < 0.5)
        {
            globalTopSettingBar_->setVisible(false);
        }
        else
        {
            globalTopSettingBar_->setWindowOpacity(1);
            globalTopSettingBar_->setVisible(true);
        }
    }
    return true;
}

bool StatusBar::onMouseMoveEvent(TpMouseEvent *event)
{
    // static int index = 0;
    // std::cout << "onMouseMoveEvent  " << index++ << std::endl;

    if (globalTopSettingBar_ /*&& !tpFuzzyCompare(globalTopSettingBar_->windowOpacity(), 1.0f)*/ && event->state())
    {
        TpPoint curMousePos = event->globalPos();

        int32_t moveY = curMousePos.y() - pressPoint_.y();
        if (moveY > 0)
        {
            globalTopSettingBar_->setWindowOpacity(moveY * 0.1);
            globalTopSettingBar_->setVisible(true);
        }
    }

    return true;
}

bool StatusBar::onLeaveEvent(TpLeaveEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_OBJECT_LEAVE_TYPE)
    {
        if (event->leave())
        {
            update();
        }
    }

    return true;
}

bool StatusBar::onPaintEvent(TpPaintEvent *event)
{
    TpDialog::onPaintEvent(event);
    return true;
}

void StatusBar::initUI()
{
    sysTimeLabel_ = new TpLabel(this);
    sysTimeLabel_->font()->setFontForeColor(_RGB(255, 255, 255));
    sysTimeLabel_->font()->setFontSize(TpDisplay::sp2Px(12));
    sysTimeLabel_->font()->setBold(true);
    sysTimeLabel_->setText("122:22");
    sysTimeLabel_->setWidth(sysTimeLabel_->font()->pixelWidth());
    sysTimeLabel_->setHeight(TpDisplay::dp2Px(18));
    // sysTimeLabel_->setBackGroundColor(_RGB(255, 0, 0));

    sysDateLabel_ = new TpLabel(this);
    sysDateLabel_->font()->setFontForeColor(_RGB(255, 255, 255));
    sysDateLabel_->font()->setFontSize(TpDisplay::sp2Px(10));
    sysDateLabel_->font()->setBold(true);
    sysDateLabel_->setText("12月22日 周二");
    sysDateLabel_->setWidth(sysDateLabel_->font()->pixelWidth());
    sysDateLabel_->setHeight(TpDisplay::dp2Px(18));
    // sysDateLabel_->setBackGroundColor(_RGB(255, 0,0));

    wifiLabel_ = new TpLabel(this);
    wifiLabel_->setSize(TpDisplay::dp2Px(18), TpDisplay::dp2Px(18));
    wifiLabel_->setBackGroundImage(TpImage(applicationDirPath() + "/../res/TopBar/WIFI.png"));
    wifiLabel_->setVisible(false);

    blueToothLabel_ = new TpLabel(this);
    blueToothLabel_->setSize(TpDisplay::dp2Px(18), TpDisplay::dp2Px(17));
    blueToothLabel_->setBackGroundImage(TpImage(applicationDirPath() + "/../res/TopBar/蓝牙.png"));
    blueToothLabel_->setVisible(false);

    elecBattery_ = new TpBattery(this);
    elecBattery_->setWidth(TpDisplay::dp2Px(30));
    elecBattery_->setHeight(TpDisplay::dp2Px(18));
    elecBattery_->setValue(80);

    updateTimetimer_ = new TpTimer(50000);
    connect(updateTimetimer_, timeout, this, &StatusBar::slotUpdateSystemTime);
    updateTimetimer_->start();

    // 系统启动就要刷新一下时间
    slotUpdateSystemTime();
}

void StatusBar::caculateTopAppPos()
{
    TpRect topBarRect = rect();

    // std::cout << " topBarRect.h : " << topBarRect.h << " statusBtnInterval : " << statusBtnInterval << " statusBtnWh : " << std::endl;
    // std::cout << " statusBtnWh :" << statusBtnWh << std::endl;

    // 添加系统时间和系统日期
    sysTimeLabel_->move(TpDisplay::dp2Px(16), (topBarRect.height() - sysTimeLabel_->height()) / 2.0);

    uint32_t sysDateXPos = sysTimeLabel_->rect().x() + sysTimeLabel_->width() + TpDisplay::dp2Px(10);
    sysDateLabel_->move(sysDateXPos, (topBarRect.height() - sysDateLabel_->height()) / 2.0);

    elecBattery_->move(width() - elecBattery_->width() - TpDisplay::dp2Px(16), (topBarRect.height() - elecBattery_->height()) / 2.0);

    wifiLabel_->move(elecBattery_->pos().x() - wifiLabel_->width() - TpDisplay::dp2Px(5), (topBarRect.height() - wifiLabel_->height()) / 2.0);

    blueToothLabel_->move(wifiLabel_->pos().x() - blueToothLabel_->width() - TpDisplay::dp2Px(5), (topBarRect.height() - blueToothLabel_->height()) / 2.0);
}

void StatusBar::slotUpdateSystemTime()
{
    // 更新时间
    TpTime currentTime = TpTime::currentTime();
    TpString curTimeStr = currentTime.toString("HH:mm");
    sysTimeLabel_->setText(curTimeStr);

    // 更新日期
    TpDate currentDate = TpDate::currentDate();
    TpString curDateStr = TpString::number(currentDate.month()) + "月" + TpString::number(currentDate.day()) + "日 " + transWeekData(currentDate.dayOfWeek());
    sysDateLabel_->setText(curDateStr);

#if 0
    // 获取蓝牙设备状态;取第一个蓝牙设备的状态
    bool blueIsOpen = false;
    TpList<TpBluetoothLocal> blueToothDeviceList = TpBluetoothLocal::getAllDevice();
    if (blueToothDeviceList.size() > 0)
    {
        TpBluetoothLocal& firstBlueDevice = blueToothDeviceList.front();
        blueIsOpen = firstBlueDevice.isPowerOn();
        blueToothLabel_->setVisible(blueIsOpen);
    }

    // 设置网络状态;任意一网卡有网就显示连接状态
    bool isOnline = false;
    TpList<TpNetworkInterface> networkList = TpNetworkInterface::getAllDevice();
    for (auto &internetDevice : networkList)
    {
        // 网卡关闭不处理
        // if (internetDevice.de)

        isOnline = internetDevice.isOnlineInternet();
        if (isOnline)
            break;
    }
    wifiLabel_->setVisible(isOnline);
#endif

    if (globalTopSettingBar_)
    {
        // globalTopSettingBar_->setBluetoothStatus(blueIsOpen);
        globalTopSettingBar_->updateTime(currentDate.year(), currentDate.month(), currentDate.day(), transWeekData(currentDate.dayOfWeek()));
    }
}

TpString StatusBar::transWeekData(const int32_t &dayOfWeek)
{
    switch (dayOfWeek)
    {
    case 1:
        return "周一";
    case 2:
        return "周二";
    case 3:
        return "周三";
    case 4:
        return "周四";
    case 5:
        return "周五";
    case 6:
        return "周六";
    case 7:
        return "周日";
    default:
        return "周一";
    }
}

void StatusBar::refreshDeskBarInfo()
{
    // 更新工具栏尺寸
    TpDeskStatusBarInfo config;
    config.statusBarLocation = 0;
    config.statusBarWidth = width();
    config.statusBarHeight = height();
    config.statusBarVislble = visible();

    TpStructPackager packa;
    config.StructSerialize(packa);

    publishGatewayData(config.dataHead_.type_.c_str(), packa.data(), packa.size());
}

void StatusBar::changeStyle(StatusBarStyle style)
{
    // wifiLabel_->setBackGroundImage(TpImage(applicationDirPath() + "/../res/TopBar/WIFI.png"));
    // blueToothLabel_->setBackGroundImage(TpImage(applicationDirPath() + "/../res/TopBar/蓝牙.png"));

    if (style == StatusBarStyle::White)
    {
        sysTimeLabel_->font()->setFontForeColor(_RGB(255, 255, 255));
        sysDateLabel_->font()->setFontForeColor(_RGB(255, 255, 255));
        elecBattery_->setStyle(TpBattery::White);
    }
    else
    {
        sysTimeLabel_->font()->setFontForeColor(_RGB(0, 0, 0));
        sysDateLabel_->font()->setFontForeColor(_RGB(0, 0, 0));
        elecBattery_->setStyle(TpBattery::Black);
    }
}
