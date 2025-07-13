#ifndef __TP_CHECK_BOX_H
#define __TP_CHECK_BOX_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpFont.h"

TP_DEF_VOID_TYPE_VAR(ItpCheckBoxData);

/// @brief 复选按钮
class tpCheckBox : public tpChildWidget
{
public:
	tpCheckBox(tpChildWidget *parent = nullptr);
	virtual ~tpCheckBox();

	/// @brief 设置文本字符串
	/// @param text
	virtual void setText(const tpString &text);

public:
	virtual tpFont *font();

public
signals:
	declare_signal(onClicked, bool);

protected:
	virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
	virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
	virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

	virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

protected:
	virtual tpString pluginType() override { return TO_STRING(tpCheckBox); }

private:
	ItpCheckBoxData *data_;
};

#endif
