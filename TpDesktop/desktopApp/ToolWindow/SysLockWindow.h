#ifndef __SYS_LOCK_WINDOW_H
#define __SYS_LOCK_WINDOW_H

#include "TpDialog.h"
#include "TpButton.h"
#include "TpEvent.h"
#include "TpLabel.h"
#include "TpTimer.h"
#include "TpLineEdit.h"

class SysLockWindow
    : public TpDialog
{
public:
    SysLockWindow();
    virtual ~SysLockWindow();

    virtual void setVisible(bool visible = true) override;

public:
    // 系统显示大小变换，锁屏窗口大小也要变化
    void resizeLockWindow(const TpRect &mainWindowRect);

protected:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;

    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;

    virtual bool onLeaveEvent(TpLeaveEvent *event) override;

private:
    void initUi();
    
    // 刷新锁屏界面显示，true为壁纸界面，false为输入密码界面
    void refreshLockWindow(const bool& windowStatus);

    // 系统登录
    void slotLogin();

    // 更新时间
    void slotUpdateSystemTime();
    
private:
    bool mouseLeftPress_;
    TpPoint pressPoint_;

    // 更新时间定时器
	TpTimer *updateTimetimer_;

    // 锁屏壁纸界面提示信息；时间、解锁提示、解锁方向提示
    TpLabel *systemDateTimeLabel_;
    TpLabel *unlockPromptLabel_;
    TpLabel *unlockDirectLabel_;

    // 输密码解锁界面
    TpLabel *systemNameLabel_;
    TpLabel *welcomeLabel_;
    TpLabel *userIconLabel_;

    TpLineEdit* pwdEdit_;
    TpButton* loginBtn_;
};

#endif