#ifndef __TP_CHECK_BOX_H
#define __TP_CHECK_BOX_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpFont.h"

TP_DEF_VOID_TYPE_VAR(ITpCheckBoxData);

/// @brief 复选按钮
class TpCheckBox : public TpWidget
{
public:
	TpCheckBox(TpWidget *parent = nullptr);
	virtual ~TpCheckBox();

	/// @brief 设置文本字符串
	/// @param text
	virtual void setText(const TpString &text);

public:
	virtual TpFont *font();

public
signals:
	declare_signal(onClicked, bool);

protected:
	virtual bool onMousePressEvent(TpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
	virtual bool onResizeEvent(TpResizeEvent *event) override;
	virtual bool onPaintEvent(TpPaintEvent *event) override;

	virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

protected:
	virtual TpString pluginType() override { return TO_STRING(TpCheckBox); }

private:
	ITpCheckBoxData *data_;
};

#endif
