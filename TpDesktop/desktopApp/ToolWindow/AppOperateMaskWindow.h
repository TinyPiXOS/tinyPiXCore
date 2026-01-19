#ifndef __APP_OPERATE_MASK_WINDOW_H
#define __APP_OPERATE_MASK_WINDOW_H

/*
    APP按钮长按遮罩窗口
*/

#include "TpDialog.h"
#include "TpEvent.h"
#include "TpCore.h"

class AppOperateMaskWindow
    : public TpDialog
{
public:
    AppOperateMaskWindow();
    virtual ~AppOperateMaskWindow();

    /// @brief 设置APP的区域，坐标为相对于屏幕左上角的XY坐标
    /// @param rect 坐标区域
    /// @param roundCorners 圆角值
    void setAppRect(const TpRect &rect, const uint32_t& roundCorners);

protected:
	virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;

private:
    TpRect appRect_;
    uint32_t roundCorners_;
};

#endif