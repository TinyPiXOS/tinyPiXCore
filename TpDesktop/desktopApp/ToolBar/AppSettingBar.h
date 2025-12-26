#ifndef __APP_SETTING_BAR_H
#define __APP_SETTING_BAR_H

#include "TpDialog.h"
#include "TpButton.h"
#include "TpEvent.h"
#include "TpIconTopButton.h"
#include "TpVector.h"
#include "TpIconSlider.h"
#include "TpLabel.h"
#include "TpPanelSwitchButton.h"
#include "PowerManage.h"
#include "Network/TpNetworkInterface.h"
#include "Bluetooth/TpBluetoothLocal.h"

class AppSettingBar
    : public TpDialog
{
public:
    AppSettingBar();
    virtual ~AppSettingBar();

    virtual void setVisible(bool visible) override;

    // 更新系统时间
    void updateTime(const int32_t &year, const int32_t &month, const int32_t &day, const TpString &weekDay);

    // 设置蓝牙开关状态
    void setBluetoothStatus(const bool &status);

    // void set
protected:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;

    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;

    // virtual bool onLeaveEvent(TpLeaveEvent *event) override;

    virtual bool onResizeEvent(TpResizeEvent *event) override;

private:
    // 点击切换蓝牙状态按钮
    void slotSwitchBluetooth(bool checked);

    // 切换WIFI状态
    void slotSwitchWifi(bool checked);

    // 切换音量
    void slotChangeVoice(int32_t value);

    // 切换亮度
    void slotChangelight(int32_t value);

private:
    // 外部窗口大小变换，调整功能区按钮显示
    void resizeOperatorBtn();

    void slotPowerOff(bool checked);

private:
    bool mouseLeftPress_;
    TpPoint pressPoint_;

    // 日期lable
    TpLabel *dateTimeLabel_;

    TpIconTopButton *powerOffBtn_;

    // // 声音进度条
    TpIconSlider *voiceProgessBar_;

    // // 亮度进度条
    TpIconSlider *lightProgessBar_;

    TpPanelSwitchButton *wifiBtn_;
    TpPanelSwitchButton *bluetoothBtn_;
    // 锁定旋转
    TpPanelSwitchButton *sysLockBtn_;

    // 电源管理窗口
    PowerManage *powerManageWindow_;
    // TpVector<TpButton *> operatorBtnList_;

    // 无线网卡信息
    tpShared<TpNetworkInterface> wifiNetwork_;
};

#endif