#ifndef __TP_VTEXT_LABEL_H
#define __TP_VTEXT_LABEL_H

#include "tpChildWidget.h"
#include <string>

TP_DEF_VOID_TYPE_VAR(ItpTextLabelData);

class tpFont;
class tpLabel : public tpChildWidget
{
public:
	tpLabel(tpChildWidget *parent = nullptr);
	tpLabel(const tpString &text, tpChildWidget *parent = nullptr);
	virtual ~tpLabel();

public:
	virtual void setAutoFit(bool enable = false); // according to font width and height to render, when be set, align will be invalidate

	/// @brief 设置label根据文本长度和大小，自动实现换行
	/// @param wrap 是否自动换行
	void setWordWrap(bool wrap);

public:
	virtual void setRect(tpRect &rect);
	virtual void setRect(tpRect *rect);
	virtual void setRect(ItpRect &rect);
	virtual void setRect(ItpRect *rect);
	virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h);

public:
	/// @brief 设置显示文本
	/// @param text 文本字符串
	virtual void setText(const tpString &text);

public:
	/// @brief 获取文本字体指针
	/// @return 字体指针
	virtual tpFont *font();

public:
	/// @brief 设置文本居中模式；当前仅支持水平靠左、靠右、居中
	/// @param align 模式枚举值
	virtual void setAlign(const tinyPiX::AlignmentFlag align = tinyPiX::AlignLeft);

public:
	virtual bool onPaintEvent(tpObjectPaintEvent *event);
	virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;

protected:
	/// @brief 组件类名，子类实现，返回子类类名字符串，用于匹配CSS中对应样式
	/// @return 类名字符串
	virtual tpString pluginType() override { return TO_STRING(tpLabel); }

private:
	ItpTextLabelData *textLabelSet;
};

#endif
