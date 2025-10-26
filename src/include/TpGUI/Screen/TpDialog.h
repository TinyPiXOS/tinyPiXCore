#ifndef __TP_DIALOG_H
#define __TP_DIALOG_H

#include "TpScreen.h"

TP_DEF_VOID_TYPE_VAR(ItpDialogData);
class TpDialog
    : public TpScreen
{
public:
    TpDialog(const char *type = "tinyPiX_USE_Float");

    virtual ~TpDialog();

    /// @brief 模态显示
    /// @return 返回点击ID索引
    virtual uint32_t exec();

    /// @brief 关闭窗口
    virtual void close() override;

public:
    virtual Tp::ItpObjectType objectType() final;

    /// @brief 组件类名，子类实现，返回子类类名字符串，用于匹配CSS中对应样式
    /// @return 类名字符串
    virtual TpString pluginType() override { return TO_STRING(TpDialog); }

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;

private:
    ItpDialogData *data_;
};

#endif
