#ifndef __APP_TASK_MANAGE_WINDOW_H
#define __APP_TASK_MANAGE_WINDOW_H

/*
    应用任务管理器
*/

#include "TpDialog.h"
#include "TpButton.h"
#include "TpEvent.h"
#include "TpScrollPanel.h"
#include "TpVector.h"
#include "AppPreviewWidget.h"
#include "TpHash.h"

class AppTaskManageWindow
    : public TpDialog
{
public:
    AppTaskManageWindow();
    virtual ~AppTaskManageWindow();

    virtual void setVisible(bool visible = true) override;

protected:
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;

    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;

    virtual bool onResizeEvent(TpResizeEvent *event) override;

    virtual bool onLeaveEvent(TpLeaveEvent *event) override;

private:
    // 清理所有应用
    void slotClearAllApp(bool);

    // 单个应用点击关闭事件
    void slotKillApp(const TpString& uuid);

    // 打开指定应用
    void slotOpenApp(const TpString& uuid);

private:
    TpScrollPanel *taskScrollPanel_;

    TpPoint mousePressPoint_;
    
    uint32_t taskHeight_;
    uint32_t taskWidth_;

    TpButton *clearAllBtn_;

    // <uuid, 应用预览窗>
    TpHash<TpString, AppPreviewWidget *> allTaskWidgetMap_;
};

#endif