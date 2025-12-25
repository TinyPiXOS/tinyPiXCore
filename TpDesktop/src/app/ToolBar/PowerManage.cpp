#include "PowerManage.h"
#include "TpVariant.h"
#include "DeskTopGlobal.hpp"

#define POWER_MANAGE_BG_COLOR _RGBA(0, 0, 0, 153)

PowerManage::PowerManage()
    : TpDialog("tinyPiX_SYS_Float_0531acbf04")
{
    setBackGroundColor(POWER_MANAGE_BG_COLOR);

    powerRestartBtn_ = new TpButton("重新启动", this);
    powerRestartBtn_->setProperty("type", "ControlPanelPowerButton");
    powerRestartBtn_->setFixedSize(globalMainScreen_->width() * 0.2824, globalMainScreen_->height() * 0.0888);

    powerOffBtn_ = new TpButton("关机", this);
    powerOffBtn_->setProperty("type", "ControlPanelPowerButton");
    powerOffBtn_->setFixedSize(globalMainScreen_->width() * 0.2824, globalMainScreen_->height() * 0.0888);

    closeBtn_ = new TpButton(this);
    closeBtn_->setButtonStyle(TpButton::IconOnly);
    closeBtn_->setIcon(applicationDirPath() + "/../res/controlPanel/控制面板-关闭.svg");
    closeBtn_->setEnableBackGroundColor(false);
    closeBtn_->setSize(globalMainScreen_->width() * 0.055555, globalMainScreen_->width() * 0.055555);
    closeBtn_->setProperty("type", "ControlPanelPowerCloseButton");

    connect(closeBtn_, onClicked, [=](bool)
            { close(); });

    setRect(0, 0, globalMainScreen_->width(), globalMainScreen_->height());

    uint32_t btnX = (rect().width() - powerOffBtn_->width()) / 2.0;

    powerRestartBtn_->move(btnX, globalMainScreen_->height() * 0.33333);
    powerOffBtn_->move(btnX, powerRestartBtn_->pos().y() + powerRestartBtn_->height() + globalMainScreen_->height() * 0.051388);

    uint32_t closeBtnX = (rect().width() - closeBtn_->width()) / 2.0;

    closeBtn_->move(closeBtnX, powerOffBtn_->pos().y() + powerOffBtn_->height() + globalMainScreen_->height() * 0.298611);

    setVisible(false);
}

PowerManage::~PowerManage()
{
}

bool PowerManage::onMousePressEvent(TpMouseEvent *event)
{
    setVisible(false);

    return true;
}

bool PowerManage::onMouseRleaseEvent(TpMouseEvent *event)
{
    return true;
}

bool PowerManage::onResizeEvent(TpResizeEvent *event)
{
    TpDialog::onResizeEvent(event);
    return true;
}
