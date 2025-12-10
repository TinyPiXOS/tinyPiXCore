#ifndef __TP_VSLIDER_H
#define __TP_VSLIDER_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpColors.h"

TP_DEF_VOID_TYPE_VAR(ITpSliderData);
/// @brief 可拖动滑块
class TpSlider : public TpWidget
{
public:
	enum SliderDirect
	{
		Horizon,
		Vertical
	};

public:
	TpSlider(TpWidget *parent = nullptr, const SliderDirect &direct = Horizon);
	virtual ~TpSlider();

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
	virtual bool onMousePressEvent(TpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
	virtual bool onMouseMoveEvent(TpMouseEvent *event) override;
	virtual bool onLeaveEvent(TpLeaveEvent *event) override;
	virtual bool onPaintEvent(TpPaintEvent *event) override;

protected:
	virtual TpString pluginType() override { return TO_STRING(TpSlider); }

private:
	ITpSliderData *data_;
};

#endif
