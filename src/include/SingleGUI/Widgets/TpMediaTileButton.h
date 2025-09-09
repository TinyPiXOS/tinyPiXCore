#ifndef __TP_MEDIA_TILE_BUTTON_H
#define __TP_MEDIA_TILE_BUTTON_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include <TpString.h>

TP_DEF_VOID_TYPE_VAR(ItpMediaTileButtonData);

/// @brief 多媒体磁铁开关按钮
class TpMediaTileButton : public TpChildWidget
{
public:
    TpMediaTileButton(TpChildWidget *parent = nullptr);

    virtual ~TpMediaTileButton();

    /// @brief 设置按钮图标
    /// @param iconPath 按钮图标绝对路径
    void setIcon(const TpString &iconPath);

    /// @brief 设置文本
    /// @param text 文本字符串
    void setText(const TpString &text);

public
signals:
    /// @brief 按钮点击信号槽，鼠标释放时触发
    /// @param bool 按钮选中状态
    declare_signal(onClicked, bool);

protected:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

    /// @brief
    /// @param event
    virtual void onThemeChangeEvent(TpThemeChangeEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpMediaTileButton); }

private:
    ItpMediaTileButtonData *data_;
};

#endif
