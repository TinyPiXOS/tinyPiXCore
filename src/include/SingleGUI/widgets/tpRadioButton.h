#ifndef __TP_VRADIO_BUTTON_H
#define __TP_VRADIO_BUTTON_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpRadioButtonData);

class tpColors;
class tpFont;
class tpString;
/// @brief 单选按钮
class tpRadioButton : public tpChildWidget
{
public:
	tpRadioButton(tpChildWidget *parent = nullptr);
	tpRadioButton(const tpString &text, tpChildWidget *parent = nullptr);

	virtual ~tpRadioButton();

public:
	/// @brief 根据字体宽度和高度绘制，当设置时对齐将无效
	/// @param enable 是否自动调整
	virtual void setAutoFit(bool enable = false);

	/// @brief 设置按钮与文本间距值
	/// @param space 间距
	virtual void setSpacing(uint32_t space = 1);

public:
	virtual void setBoxColor(uint32_t color);
	virtual void setBoxColor(tpColors &color);
	virtual void setCheckColor(uint32_t color);
	virtual void setCheckColor(tpColors &color);

public:
	virtual void setRect(const int32_t &x, const int32_t &y, const uint32_t &w, const uint32_t &h) override;

public:
	/// @brief 设置按钮文本
	/// @param text 
	virtual void setText(const tpString &text);

public:
	virtual tpFont *font();

public
signals:
	declare_signal(onClicked, bool);

public:
	virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
	virtual bool onPaintEvent(tpObjectPaintEvent *event);

private:
	ItpRadioButtonData *data_;
};

#endif
