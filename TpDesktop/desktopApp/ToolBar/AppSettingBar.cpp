#include "AppSettingBar.h"
#include "StatusBar.h"
#include "DeskTopGlobal.hpp"
#include <iostream>
#include "TpString.h"
#include "TpVariant.h"
#include "TpBluetoothLocal.h"
#include "TpMessageBox.h"
#include "TpSound.h"

#ifndef SETTING_BAR_COLOR
#define SETTING_BAR_COLOR _RGBA(93, 97, 208, 204)
#endif

AppSettingBar::AppSettingBar()
    : TpDialog("tinyPiX_SYS_Float_0531acbf04"), mouseLeftPress_(false), wifiNetwork_(nullptr)
{
    this->setEnabledBorderColor(false);
    this->setBackGroundColor(SETTING_BAR_COLOR);

    dateTimeLabel_ = new TpLabel(this);
    dateTimeLabel_->setProperty("type", "controlPanelDateTimeLabel");
    dateTimeLabel_->setText("2025年12月12日 周六");
    dateTimeLabel_->font()->setFontColor(_RGB(255, 255, 255));
    dateTimeLabel_->font()->setFontSize(globalMainScreen_->width() * 0.016); // TpDisplay::sp2Px(16)
    dateTimeLabel_->setSize(dateTimeLabel_->font()->pixelWidth(), dateTimeLabel_->font()->pixelHeight());

    powerOffBtn_ = new TpIconTopButton(applicationDirPath() + "/../res/controlPanel/控制面板-电源.png", "", this);
    powerOffBtn_->setIconSize(globalMainScreen_->width() * 0.032, globalMainScreen_->width() * 0.032);
    powerOffBtn_->setTextVisible(false);
    powerOffBtn_->setEnableBackGroundColor(false);
    powerOffBtn_->setEnabledBorderColor(false);
    connect(powerOffBtn_, onClicked, this, &AppSettingBar::slotPowerOff);

    // 声音进度条
    voiceProgessBar_ = new TpIconSlider(this);
    voiceProgessBar_->setIcon(applicationDirPath() + "/../res/controlPanel/控制面板-音量.png");
    voiceProgessBar_->setRange(0, 100);
    voiceProgessBar_->setValue(50);
    voiceProgessBar_->setFixedSize(globalMainScreen_->width() * 0.2824, globalMainScreen_->height() * 0.0888);
    connect(voiceProgessBar_, onValueChanged, this, &AppSettingBar::slotChangeVoice);

    // // 亮度进度条
    lightProgessBar_ = new TpIconSlider(this);
    lightProgessBar_->setIcon(applicationDirPath() + "/../res/controlPanel/控制面板-亮度.png");
    lightProgessBar_->setRange(0, 100);
    lightProgessBar_->setValue(100);
    lightProgessBar_->setEnabled(false);
    lightProgessBar_->setFixedSize(globalMainScreen_->width() * 0.2824, globalMainScreen_->height() * 0.0888);
    connect(lightProgessBar_, onValueChanged, this, &AppSettingBar::slotChangelight);

    wifiBtn_ = new TpPanelSwitchButton(this);
    wifiBtn_->setCheckable(true);
    wifiBtn_->setText("WIFI");
    wifiBtn_->setIcon(applicationDirPath() + "/../res/controlPanel/控制面板-WIFI.png");
    wifiBtn_->font()->setFontColor(_RGB(255, 255, 255));
    wifiBtn_->setFixedSize(globalMainScreen_->width() * 0.14166, globalMainScreen_->width() * 0.14166);
    connect(wifiBtn_, onClicked, this, &AppSettingBar::slotSwitchWifi);

    bluetoothBtn_ = new TpPanelSwitchButton(this);
    bluetoothBtn_->setCheckable(true);
    bluetoothBtn_->setText("蓝牙");
    bluetoothBtn_->font()->setFontColor(_RGB(255, 255, 255));
    bluetoothBtn_->setIcon(applicationDirPath() + "/../res/controlPanel/控制面板-蓝牙.png");
    bluetoothBtn_->setFixedSize(globalMainScreen_->width() * 0.14166, globalMainScreen_->width() * 0.14166);
    connect(bluetoothBtn_, onClicked, this, &AppSettingBar::slotSwitchBluetooth);

    sysLockBtn_ = new TpPanelSwitchButton(this);
    sysLockBtn_->setCheckable(true);
    sysLockBtn_->setText("锁定");
    sysLockBtn_->font()->setFontColor(_RGB(255, 255, 255));
    sysLockBtn_->setFixedSize(globalMainScreen_->width() * 0.14166, globalMainScreen_->width() * 0.14166);
    sysLockBtn_->setIcon(applicationDirPath() + "/../res/controlPanel/控制面板-锁定.png");

    // 初始化无线网卡信息
    TpList<tpShared<TpNetworkInterface>> netIfList = TpNetworkInterface::allDevice();
    for (auto &netIf : netIfList)
    {
        if (netIf->isWireless())
        {
            wifiNetwork_ = netIf;
            break;
        }
    }
    // 如果没有无线网卡，提示不可用
    if (!wifiNetwork_)
    {
        wifiBtn_->setEnabled(false);
    }

    // 获取蓝牙设备
    TpList<tpShared<TpBluetoothHostInfo>> bluetoothDeviceList = TpBluetoothLocal::allDevice();
    bluetoothBtn_->setEnabled(bluetoothDeviceList.size() > 0);

    powerManageWindow_ = new PowerManage();

    setRect(0, 0, globalMainScreen_->width(), globalMainScreen_->height());

    // 默认完全透明
    setWindowOpacity(0);
    resizeOperatorBtn();
}

AppSettingBar::~AppSettingBar()
{
}

void AppSettingBar::setVisible(bool visible)
{
    TpDialog::setVisible(visible);

    if (visible)
    {
        // 刷新音量
        // TpList<TpString> soundList = TpSound::getDevices();
        // if (soundList.size() > 0)
        // {
        //     voiceProgessBar_->setEnabled(true);

        //     TpSound sound(soundList.front());
        //     voiceProgessBar_->setValue(sound.getSystemVolume());
        // }
        // else
        // {
        //     voiceProgessBar_->setValue(0);
        //     voiceProgessBar_->setEnabled(false);
        // }
    }
}

void AppSettingBar::updateTime(const int32_t &year, const int32_t &month, const int32_t &day, const TpString &weekDay)
{
    dateTimeLabel_->setText(TpString::number(year) + "年" + TpString::number(month) + "月" + TpString::number(day) + "日" + weekDay);
}

void AppSettingBar::setBluetoothStatus(const bool &status)
{
    bluetoothBtn_->setChecked(status);
}

bool AppSettingBar::onMousePressEvent(TpMouseEvent *event)
{
    mouseLeftPress_ = event->state();
    pressPoint_ = event->globalPos();

    return true;
}

bool AppSettingBar::onMouseRleaseEvent(TpMouseEvent *event)
{
    mouseLeftPress_ = event->state();

    return true;
}

bool AppSettingBar::onMouseMoveEvent(TpMouseEvent *event)
{
    // TpDialog::onMouseMoveEvent(event);

    if (mouseLeftPress_)
    {
        TpPoint curMousePos = event->globalPos();

        if ((curMousePos.y() - pressPoint_.y()) <= -5)
        {
            // 触发一次上拉事件后，不再重复触发
            mouseLeftPress_ = false;

            setWindowOpacity(0);
            this->setVisible(false);

            // std::cout << "AppSettingBar visible false " << std::endl;
        }
    }

    return true;
}

bool AppSettingBar::onResizeEvent(TpResizeEvent *event)
{
    TpDialog::onResizeEvent(event);
    resizeOperatorBtn();

    return true;
}

void AppSettingBar::slotSwitchBluetooth(bool checked)
{
#if 0
    bool blueIsOpen = false;
    TpList<TpBluetoothLocal> blueToothDeviceList = TpBluetoothLocal::getAllDevice();
    if (blueToothDeviceList.size() == 0)
    {
        bluetoothBtn_->setChecked(false);

        TpMessageBox msg("tinyPiX_SYS_Float_0531acbf04");
        msg.setMessageType(TpMessageBox::Information);
        msg.setText("未检测到蓝牙设备!");

        TpVector<TpString> btnList;
        btnList.emplace_back("确认");

        msg.setButtonList(btnList);
        msg.exec();

        return;
    }

    for (auto &blueDevice : blueToothDeviceList)
    {
        blueDevice.powerOn();
    }
#endif
}

void AppSettingBar::slotSwitchWifi(bool checked)
{
    if (!wifiNetwork_)
        return;

    checked ? wifiNetwork_->openDevice() : wifiNetwork_->closeDevice();
}

void AppSettingBar::slotChangeVoice(int32_t value)
{
    // TODO ; ARM暂时屏蔽音量修改
    return;

    // 刷新音量
    TpList<TpString> soundList = TpSound::getDevices();
    if (soundList.size() > 0)
    {
        TpSound sound(soundList.front());
        sound.setSystemVolume(value);
    }
}

void AppSettingBar::slotChangelight(int32_t value)
{
}

void AppSettingBar::resizeOperatorBtn()
{
    TpRect settingBarRect = this->rect();
    uint32_t panelHMargin = (settingBarRect.width() - BOTTOM_BAR_WIDTH) / 2.0;

    uint32_t firstRowY = MAIN_PANEL_TOP_DISTANCE + TOP_BAR_HEIGHT;
    dateTimeLabel_->move(panelHMargin, firstRowY);

    powerOffBtn_->move(settingBarRect.width() - panelHMargin - powerOffBtn_->width(), firstRowY);

    uint32_t secondRowY = firstRowY + dateTimeLabel_->height() + globalMainScreen_->height() * 0.0305;
    lightProgessBar_->move(panelHMargin, secondRowY);
    voiceProgessBar_->move(panelHMargin, secondRowY + lightProgessBar_->height() + globalMainScreen_->height() * 0.03472);

    // std::cout << "lightProgessBar_->width " << lightProgessBar_->width() << " " << lightProgessBar_->height();
    // 按钮宽度161

    sysLockBtn_->move(rect().width() - panelHMargin - sysLockBtn_->width(), secondRowY);

    bluetoothBtn_->move(sysLockBtn_->pos().x() - bluetoothBtn_->width() - globalMainScreen_->height() * 0.03472, secondRowY);

    wifiBtn_->move(bluetoothBtn_->pos().x() - wifiBtn_->width() - globalMainScreen_->height() * 0.03472, secondRowY);
}

void AppSettingBar::slotPowerOff(bool checked)
{
    powerManageWindow_->setVisible(true);
    powerManageWindow_->update();
}
