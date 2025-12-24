#ifndef __TP_SLIDER_PRPGRESS_BAR_H
#define __TP_SLIDER_PRPGRESS_BAR_H

#include <TpWidget.h>
#include <TpSignalSlot.h>

TP_DEF_VOID_TYPE_VAR(ITpSliderProgressBarData);

/// @brief 滑块进度条组件；可拖动滑块改变进度
class TpSliderProgressBar : public TpWidget
{
public:
    TpSliderProgressBar(TpWidget *parent);
    ~TpSliderProgressBar();

public:
    /// @brief 设置取值范围
    /// @param min 最小值
    /// @param max 最大值
    virtual void setRange(const int32_t &min = 0, const int32_t &max = 100);

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
    virtual bool onPaintEvent(TpPaintEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpSliderProgressBar); }

private:
    ITpSliderProgressBarData *data_;
};

#endif