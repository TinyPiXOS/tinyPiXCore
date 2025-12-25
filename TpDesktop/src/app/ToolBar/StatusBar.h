#ifndef __TOP_BAR_H
#define __TOP_BAR_H

#include "TpDialog.h"
#include "TpLabel.h"
#include "TpTimer.h"
#include "TpEvent.h"
#include "TpBattery.h"
#include "TpGateway.h"
#include "DeskTopGlobal.hpp"

class StatusBar
    : public TpDialog,
      public ITpGatewayHander
{
public:
    StatusBar();
    virtual ~StatusBar();

    /// @brief 设置状态栏背景颜色；会根据状态栏颜色调整内容颜色
    /// @param appColor 工具栏颜色
    void setColor(const int32_t &color);

    virtual void setVisible(bool visible = true) override;

public:
    virtual void recvData(const char *topic, const void *data, const uint32_t &size) override;

protected:
    virtual bool onResizeEvent(TpResizeEvent *event) override;
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;

    virtual bool onLeaveEvent(TpLeaveEvent *event) override;

    virtual bool onPaintEvent(TpPaintEvent *event) override;

private:
    void initUI();

    void caculateTopAppPos();

    void slotUpdateSystemTime();

    // 将周几的数字转为汉字显示
    TpString transWeekData(const int32_t &dayOfWeek);

    // 通知应用topbar数据发生变化
    void refreshDeskBarInfo();

    void changeStyle(StatusBarStyle style);

private:
    TpPoint pressPoint_;

    TpLabel *sysDateLabel_;
    TpLabel *sysTimeLabel_;

    // 蓝牙和网络连接状态
    TpLabel *wifiLabel_;
    TpLabel *blueToothLabel_;

    // 电量显示窗
    TpBattery *elecBattery_;

    TpTimer *updateTimetimer_;
};

#endif
