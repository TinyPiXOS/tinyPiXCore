#ifndef __TP_BATTERY_H
#define __TP_BATTERY_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpString.h"
#include "smartDeviceGUI/FrameworkGlobal.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ITpSemiCircleProgressBarData);
/// @brief 环形进度条
class TpSemiCircleProgressBar : public tpChildWidget
{
public:
	TpSemiCircleProgressBar(tpChildWidget *parent = nullptr);

	virtual ~TpSemiCircleProgressBar();

    /// @brief 设置取值范围；最小值需小于最大值
    /// @param min 最小值
    /// @param max 最大值
    void setRange(const int32_t& min, const int32_t& max);

	/// @brief 设置当前值；内部自动计算百分比
	/// @param value 当前值
	void setValue(const int32_t &value);

	/// @brief 获取当前值
	/// @return 当前值
	int32_t value();

public:
	virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

	virtual tpString pluginType() override { return TO_STRING(TpSemiCircleProgressBar); }

private:
	ITpSemiCircleProgressBarData *data_;
};

SMART_DEVICE_GUI_NAMESPACE_END

#endif
