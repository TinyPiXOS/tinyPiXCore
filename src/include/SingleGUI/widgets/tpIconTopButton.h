#ifndef __TP_ICON_TOP_BUTTON_H
#define __TP_ICON_TOP_BUTTON_H

#include "tpChildWidget.h"
#include "tpColors.h"
#include "tpSignalSlot.h"
#include <tpString.h>

TP_DEF_VOID_TYPE_VAR(ItpIconTopButtonData);

class tpFont;
/// @brief 图标按钮类，图标在上。文本在下;只需要设置iconSize即可，内部会自动根据icon大小调整整个按钮的大小
class tpIconTopButton : public tpChildWidget
{
public:
    tpIconTopButton(tpChildWidget *parent);

    tpIconTopButton(const tpString &iconPath, const tpString &text, tpChildWidget *parent);

    virtual ~tpIconTopButton();

public:
    /// @brief 设置按钮文本
    /// @param text 文本字符串
    virtual void setText(const tpString &text);

    /// @brief 设置图标
    /// @param iconPath 图标绝对路径
    void setIcon(const tpString &iconPath);

    virtual tpFont *font();

    /// @brief 设置图标大小
    /// @param width 图标宽度
    /// @param height图标高度
    void setIconSize(const uint32_t &width, const uint32_t &height);
    /// @brief 设置图标大小
    /// @param size 图标尺寸
    void setIconSize(const ItpSize &size);
    /// @brief 获取图标大小
    /// @return 图标尺寸
    ItpSize iconSize();

    /// @brief 设置图标文本显隐，设置后控制标签显示/隐藏,需要在设置IconSize接口调用后使用
    /// @param visible true显示，false隐藏
    void setTextVisible(const bool &visible);

public:
    /// @brief 设置边框圆角
    /// @param roundPercent px
    virtual void setRoundCorners(const uint32_t &round) override;

    virtual void setParent(tpObject *parent) override;

protected:
    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onMouseLongPressEvent(tpMouseEvent *event) override;

    virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

    /// @brief
    /// @param event
    virtual void onThemeChangeEvent(tpThemeChangeEvent *event) override;

public
signals:
    /// @brief 鼠标按下信号
    /// @param bool 选中状态
    declare_signal(onPressed);
    /// @brief 点击信号；鼠标释放信号
    /// @param bool 选中状态
    declare_signal(onClicked, bool);
    /// @brief 长按信号
    declare_signal(onLongPress);

protected:
    virtual tpString pluginType() override { return TO_STRING(tpIconTopButton); }

private:
    void Init();

private:
    ItpIconTopButtonData *data_;
};

#endif
