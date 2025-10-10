#ifndef __NAVIGATION_BAR_H
#define __NAVIGATION_BAR_H

/*
    导航条
*/

#include "TpDialog.h"
#include "TpLabel.h"
#include "TpEvent.h"
#include "TpTime.h"

class navigationBar
    : public TpDialog
{
public:
    navigationBar();
    virtual ~navigationBar();

    /// @brief 设置导航条颜色
    /// @param appColor 应用底部颜色，根据应用颜色，选择设置导航条是黑色/白色
    void setColor(const int32_t &appColor);

protected:
    virtual bool onResizeEvent(TpResizeEvent *event) override;
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;
    virtual bool onLeaveEvent(TpLeaveEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

private:
    TpLabel *lineLabel_;

    // 鼠标按下的相对坐标
    TpPoint mousePressPoint_;

    TpTime mousePressTime_;

    // 上一次动画执行时间，避免重复执行动画
    TpTime lastAnimationTime_;
};

#endif
