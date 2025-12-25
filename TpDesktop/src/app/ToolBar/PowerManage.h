#ifndef __SYS_POWER_MANAGE_H
#define __SYS_POWER_MANAGE_H

#include "TpDialog.h"
#include "TpButton.h"
#include "TpEvent.h"

/// @brief 电源管理界面
class PowerManage
    : public TpDialog
{
public:
    PowerManage();
    virtual ~PowerManage();

protected:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;

    virtual bool onResizeEvent(TpResizeEvent *event) override;

private:
    // void slotPowerOff(bool checked);

private:
    TpButton *powerRestartBtn_;
    TpButton *powerOffBtn_;
    TpButton *closeBtn_;
};

#endif