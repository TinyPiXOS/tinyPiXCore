
#ifndef __TP_TOAST_H
#define __TP_TOAST_H

#include "TpDialog.h"
#include "TpString.h"
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ITpToastData);
/// @brief 消息提示组件；一般无交互操作；自动消失
class TpToast : public TpDialog
{
public:
    enum IconType
    {
        None,
        Info,
        Success,
        Error,
        Warning
    };

public:
    TpToast();
    virtual ~TpToast();

    /// @brief TpToast无模态效果；内部还是show
    virtual void exec() final override;

    /// @brief 设置持续时间
    /// @param timeMs 单位毫秒；默认为2000Ms
    void setDuaration(int32_t timeMs = 2000);

    /// @brief 设置文本
    /// @param text 文本字符串
    void setText(const TpString &text);

    /// @brief 设置图标类型，内部根据类型自动设置图标
    /// @param type 图标类型
    void setIcon(IconType type);

    /// @brief 设置自定义图标
    /// @param iconPath 图标绝对路径
    void setIcon(const TpString &iconPath);

    /// @brief 设置自定义窗口；设置后文本和icon设置无效；置空给入nullptr；内部不会释放原始窗口指针
    /// @param widget 自定义窗口
    void setWidget(TpWidget *widget);

    /// @brief 获取当前设置的自定义窗口；未设置则为空
    /// @return 自定义窗口
    TpWidget* widget();

    /// @brief 设置窗口显隐
    /// @param visible true显示，false隐藏
    virtual void setVisible(bool visible = true) override;
    
public
signals:

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpToast); }

private:
    ITpToastData *data_;
};

#endif
