#ifndef __TP_VSLIDER_H
#define __TP_VSLIDER_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpColors.h"

TP_DEF_VOID_TYPE_VAR(ItpSliderData);
/// @brief 可拖动滑块
class tpSlider : public tpChildWidget
{
public:
	enum SliderDirect
	{
		Horizon,
		Vertical
	};

public:
	tpSlider(tpChildWidget *parent = nullptr, const SliderDirect &direct = Horizon);
	virtual ~tpSlider();

public:
	/// @brief 设置取值范围
	/// @param min 最小值
	/// @param max 最大值
	virtual void setRange(const int32_t &min = 0, const int32_t &max = 100);
	/// @brief 设置滑块条方向
	/// @param direct 方向枚举
	virtual void setDirection(const SliderDirect &direct = Horizon);

	/// @brief 获取当前值
	/// @param position
	virtual int32_t value();
	/// @brief 设置当前值
	/// @param value 当前值
	virtual void setValue(const int32_t &value);

public
signals:
	/// @brief 值变化信号
	/// @param int 当前值
	declare_signal(valueChanged, int32_t);

	/// @brief 范围变化信号
	/// @param int 当前最小值
	/// @param int 当前最大值
	declare_signal(rangeChanged, int32_t, int32_t);

protected:
	virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
	virtual bool onMouseMoveEvent(tpMouseEvent *event) override;
	virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;
	virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

protected:
	virtual tpString pluginType() override { return TO_STRING(tpSlider); }

private:
	ItpSliderData *data_;
};

#endif
