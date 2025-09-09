#ifndef __TP_VRECT_BUTTON_H
#define __TP_VRECT_BUTTON_H

#include "TpChildWidget.h"
#include "TpColors.h"
#include "TpSignalSlot.h"
#include <TpString.h>

// 需要在四个地方根据样式刷新UI，构造、设置父窗口。resize、themechange

TP_DEF_VOID_TYPE_VAR(ItpButtonData);

class TpFont;
/// @brief 按钮类
class TpButton : public TpChildWidget
{
public:
	enum ButtonTextStyle
	{
		IconOnly,
		TextOnly,
		TextBesideIcon, // 水平，先图标后文本
	};

public:
	TpButton(TpChildWidget *parent = nullptr);

	TpButton(const TpString &iconPath, const TpString &text, TpChildWidget *parent = nullptr);

	TpButton(const TpString &text, TpChildWidget *parent = nullptr);

	virtual ~TpButton();

public:
	/// @brief 设置按钮文本
	/// @param text 文本内容
	virtual void setText(const TpString &text);

	/// @brief 获取按钮文本字体
	/// @return 字体指针
	virtual TpFont *font();

public:
	/// @brief 设置按钮的图标
	/// @param iconPath 图标文件的绝对路径
	void setIcon(const TpString &iconPath);

	/// @brief 设置图标大小，只有ButtonTextStyle==IconOnly模式下有效；默认是充满整个按钮；图标会居中显示
	/// @param size 图标尺寸
	void setIconSize(const TpSize &size);

	/// @brief 设置图标大小，只有ButtonTextStyle==IconOnly模式下有效；默认是充满整个按钮；图标会居中显示
	/// @param width 图标宽度
	/// @param height 图标高度
	void setIconSize(const uint32_t &width, const uint32_t &height);

	/// @brief 设置按钮样式
	/// @param buttonStyle 按钮样式枚举值
	void setButtonStyle(TpButton::ButtonTextStyle buttonStyle = TpButton::TextOnly);

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
	virtual TpString pluginType() override { return TO_STRING(TpButton); }

private:
	void Init();

private:
	ItpButtonData *data_;
};

#endif
