#ifndef __TP_SMART_GUI_CIRCULAR_PROGRESS_BAR_H
#define __TP_SMART_GUI_CIRCULAR_PROGRESS_BAR_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "SmartDeviceGUI/FrameworkGlobal.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ITpCircularProgressBarData);
/// @brief 环形进度条
class TpCircularProgressBar : public TpChildWidget
{
public:
	TpCircularProgressBar(TpChildWidget *parent = nullptr);

	virtual ~TpCircularProgressBar();

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

    /// @brief 设置圆环线宽
    /// @param width 宽度
    void setLineWidth(const uint32_t& width);

    /// @brief 获取线宽
    /// @return 线宽
    uint32_t lineWidth();

public:
	virtual bool onPaintEvent(TpPaintEvent *event) override;

	virtual TpString pluginType() override { return TO_STRING(smartDeviceGUI__TpCircularProgressBar); }

private:
	ITpCircularProgressBarData *data_;
};

SMART_DEVICE_GUI_NAMESPACE_END

#endif
