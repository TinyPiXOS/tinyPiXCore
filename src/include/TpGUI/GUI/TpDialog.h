#ifndef __TP_DIALOG_H
#define __TP_DIALOG_H

#include "TpWidget.h"

TP_DEF_VOID_TYPE_VAR(ITpDialogData);
class TpDialog
    : public TpWidget
{
public:
    TpDialog(const char *type = "tinyPiX_USE_Float");

    virtual ~TpDialog();

    /// @brief 模态显示
    /// @return 返回点击ID索引
    virtual void exec();

    /// @brief 关闭窗口
    virtual void close() override;

    /// @brief 设置窗口显隐
    /// @param visible true显示，false隐藏
    virtual void setVisible(bool visible = true) override;

public:
    virtual void setBeMoved(bool moved = false);
    virtual bool moved();

    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;

    virtual bool onPaintEvent(TpPaintEvent *event) override;

public:
    virtual Tp::TpObjectType objectType() final;

    /// @brief 组件类名，子类实现，返回子类类名字符串，用于匹配CSS中对应样式
    /// @return 类名字符串
    virtual TpString pluginType() override { return TO_STRING(TpDialog); }

private:
    ITpDialogData *data_;
};

#endif
