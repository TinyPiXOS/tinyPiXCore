#ifndef __TP_ICON_TOP_BUTTON_H
#define __TP_ICON_TOP_BUTTON_H

#include "TpWidget.h"
#include "TpColors.h"
#include "TpSignalSlot.h"
#include <TpString.h>

TP_DEF_VOID_TYPE_VAR(ITpIconTopButtonData);

class TpFont;
/// @brief 图标按钮类，图标在上。文本在下;只需要设置iconSize即可，内部会自动根据icon大小调整整个按钮的大小
class TpIconTopButton : public TpWidget
{
public:
    TpIconTopButton(TpWidget *parent);

    TpIconTopButton(const TpString &iconPath, const TpString &text, TpWidget *parent);

    virtual ~TpIconTopButton();

public:
    /// @brief 设置按钮文本
    /// @param text 文本字符串
    virtual void setText(const TpString &text);

    /// @brief 设置图标
    /// @param iconPath 图标绝对路径
    void setIcon(const TpString &iconPath);

    virtual TpFont *font();

    /// @brief 设置图标大小
    /// @param width 图标宽度
    /// @param height图标高度
    void setIconSize(const uint32_t &width, const uint32_t &height);
    /// @brief 设置图标大小
    /// @param size 图标尺寸
    void setIconSize(const TpSize &size);
    /// @brief 获取图标大小
    /// @return 图标尺寸
    TpSize iconSize();

    /// @brief 设置图标文本显隐，设置后控制标签显示/隐藏,需要在设置IconSize接口调用后使用
    /// @param visible true显示，false隐藏
    void setTextVisible(const bool &visible);

public:
    /// @brief 设置边框圆角
    /// @param roundPercent px
    virtual void setRoundCorners(const uint32_t &round) override;

    virtual void setParent(TpObject *parent) override;

protected:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onMouseLongPressEvent(TpMouseEvent *event) override;

    virtual bool onLeaveEvent(TpLeaveEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

    /// @brief
    /// @param event
    virtual void onThemeChangeEvent(TpThemeChangeEvent *event) override;

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
    virtual TpString pluginType() override { return TO_STRING(TpIconTopButton); }

private:
    void Init();

private:
    ITpIconTopButtonData *data_;
};

#endif
