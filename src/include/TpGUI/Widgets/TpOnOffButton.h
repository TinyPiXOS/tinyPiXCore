#ifndef __TP_VONOFF_BUTTON_H
#define __TP_VONOFF_BUTTON_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpColors.h"
#include <string>

TP_DEF_VOID_TYPE_VAR(ItpOnOffButtonData);

/// @brief 开关状态按钮
class TpOnOffButton : public TpWidget
{
public:
	enum Direction
	{
		TP_HORIZONTAL,
		TP_VERTICAL,
	};

public:
	TpOnOffButton(TpWidget *parent = nullptr, const Direction &horizontal = TP_HORIZONTAL);
	virtual ~TpOnOffButton();

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
	// virtual void setOnText(const TpString &text);
	/// @brief 设置关闭状态文本
	/// @param text
	// virtual void setOffText(const TpString &text);

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
	virtual bool onMousePressEvent(TpMouseEvent *event) override;
	virtual bool onPaintEvent(TpPaintEvent *event) override;
	virtual bool onResizeEvent(TpResizeEvent *event) override;
	virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

private:
	ItpOnOffButtonData *data_;
};

#endif
