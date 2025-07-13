#ifndef __TP_MEDIA_TILE_BUTTON_H
#define __TP_MEDIA_TILE_BUTTON_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include <tpString.h>

TP_DEF_VOID_TYPE_VAR(ItpMediaTileButtonData);

/// @brief 多媒体磁铁开关按钮
class tpMediaTileButton : public tpChildWidget
{
public:
    tpMediaTileButton(tpChildWidget *parent = nullptr);

    virtual ~tpMediaTileButton();

    /// @brief 设置按钮图标
    /// @param iconPath 按钮图标绝对路径
    void setIcon(const tpString &iconPath);

    /// @brief 设置文本
    /// @param text 文本字符串
    void setText(const tpString &text);

public
signals:
    /// @brief 按钮点击信号槽，鼠标释放时触发
    /// @param bool 按钮选中状态
    declare_signal(onClicked, bool);

protected:
    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

    /// @brief
    /// @param event
    virtual void onThemeChangeEvent(tpThemeChangeEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpMediaTileButton); }

private:
    ItpMediaTileButtonData *data_;
};

#endif
