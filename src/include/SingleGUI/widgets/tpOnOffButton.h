#ifndef __TP_VONOFF_BUTTON_H
#define __TP_VONOFF_BUTTON_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpColors.h"
#include <string>

TP_DEF_VOID_TYPE_VAR(ItpOnOffButtonData);

/// @brief 开关状态按钮
class tpOnOffButton : public tpChildWidget
{
public:
	enum Direction
	{
		TP_HORIZONTAL,
		TP_VERTICAL,
	};

public:
	tpOnOffButton(tpChildWidget *parent = nullptr, const Direction &horizontal = TP_HORIZONTAL);
	virtual ~tpOnOffButton();

public:
	/// @brief 设置开关状态
	/// @param onOff 开关状态
	virtual void setOnOff(bool onOff = true);
	/// @brief 获取当前开关状态
	/// @return 开关状态
	virtual bool onOff();

public:
	/// @brief 设置开启状态文本
	/// @param text 文本字符串
	// virtual void setOnText(const tpString &text);
	/// @brief 设置关闭状态文本
	/// @param text
	// virtual void setOffText(const tpString &text);

	/// @brief 设置开启状态滑块颜色
	/// @param color 颜色
	virtual void setOnColor(const uint32_t &color);
	/// @brief 设置关闭状态滑块颜色
	/// @param color 颜色
	virtual void setOffColor(const uint32_t &color);

	/// @brief 设置开启状态背景颜色
	/// @param color 颜色
	virtual void setOnBackColor(const uint32_t &color);
	/// @brief 设置关闭状态背景颜色
	/// @param color 颜色
	virtual void setOffBackColor(const uint32_t &color);

public:
	declare_signal(onClicked, bool);

public:
	virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
	virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
	virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

private:
	ItpOnOffButtonData *data_;
};

#endif
