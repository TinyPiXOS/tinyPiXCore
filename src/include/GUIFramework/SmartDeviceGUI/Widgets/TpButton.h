#ifndef __TP_SMART_GUI_BUTTON_H
#define __TP_SMART_GUI_BUTTON_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "SingleGUI/Widgets/TpButton.h"
#include "SmartDeviceGUI/FrameworkGlobal.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

/// @brief SmartDeviceGUI按钮
class TpButton : public ::TpButton
{
public:
	TpButton(TpChildWidget *parent = nullptr);

	virtual ~TpButton();

public:
	virtual bool onPaintEvent(TpObjectPaintEvent *event) override;

	virtual TpString pluginType() override { return TO_STRING(TpButton); }

};

SMART_DEVICE_GUI_NAMESPACE_END

#endif
