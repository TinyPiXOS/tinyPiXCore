#ifndef __TP_VRECT_BUTTON_H
#define __TP_VRECT_BUTTON_H

#include "tpChildWidget.h"
#include "tpColors.h"
#include "tpSignalSlot.h"
#include <tpString.h>

// 需要在四个地方根据样式刷新UI，构造、设置父窗口。resize、themechange

TP_DEF_VOID_TYPE_VAR(ItpButtonData);

class tpFont;
/// @brief 按钮类
class tpButton : public tpChildWidget
{
public:
	enum ButtonTextStyle
	{
		IconOnly,
		TextOnly,
		TextBesideIcon, // 水平，先图标后文本
	};

public:
	tpButton(tpChildWidget *parent = nullptr);

	tpButton(const tpString &iconPath, const tpString &text, tpChildWidget *parent = nullptr);

	tpButton(const tpString &text, tpChildWidget *parent = nullptr);

	virtual ~tpButton();

public:
	/// @brief 设置按钮文本
	/// @param text 文本内容
	virtual void setText(const tpString &text);

	/// @brief 获取按钮文本字体
	/// @return 字体指针
	virtual tpFont *font();

public:
	/// @brief 设置按钮的图标
	/// @param iconPath 图标文件的绝对路径
	void setIcon(const tpString &iconPath);

	/// @brief 设置图标大小，只有ButtonTextStyle==IconOnly模式下有效；默认是充满整个按钮；图标会居中显示
	/// @param size 图标尺寸
	void setIconSize(const ItpSize &size);

	/// @brief 设置图标大小，只有ButtonTextStyle==IconOnly模式下有效；默认是充满整个按钮；图标会居中显示
	/// @param width 图标宽度
	/// @param height 图标高度
	void setIconSize(const uint32_t &width, const uint32_t &height);

	/// @brief 设置按钮样式
	/// @param buttonStyle 按钮样式枚举值
	void setButtonStyle(tpButton::ButtonTextStyle buttonStyle = tpButton::TextOnly);

	/// @brief 设置边框圆角
	/// @param roundPercent 取值范围0-1，0.5为正圆，越小越接近矩形
	virtual void setRoundCorners(const uint32_t &roundPercent) override;

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
	virtual tpString pluginType() override { return TO_STRING(tpButton); }

private:
	void Init();

private:
	ItpButtonData *data_;
};

#endif
