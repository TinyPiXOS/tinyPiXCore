#ifndef __TP_BATTERY_H
#define __TP_BATTERY_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpString.h"

TP_DEF_VOID_TYPE_VAR(ItpBatteryData);
/// @brief 电池UI组件
class tpBattery : public tpChildWidget
{
public:
	tpBattery(tpChildWidget *parent);

	virtual ~tpBattery();

	/// @brief 设置电量，最大100，最小0
	/// @param value 电量值
	void setValue(const int32_t &value);

	/// @brief 获取当前电量
	/// @return 电量值[0,100]
	int32_t value();

	/// @brief 设置告警值
	/// @param value 告警值
	void setAlamValue(const int32_t& value);
	/// @brief 获取当前告警值
	/// @return 
	int32_t alamValue();

	/// @brief 设置告警颜色
	/// @param color 颜色_RGB
	void setAlamColor(const int32_t& color);
	/// @brief 获取告警颜色值
	/// @return 颜色RGB
	int32_t alamColor();

public:
	virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

	virtual tpString pluginType() override { return TO_STRING(tpBattery); }

private:
	ItpBatteryData *data_;
};

#endif
