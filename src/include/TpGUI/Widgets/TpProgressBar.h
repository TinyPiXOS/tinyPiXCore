#ifndef __TP_PROGRESS_BAR_H
#define __TP_PROGRESS_BAR_H

#include "TpWidget.h"
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ITpProgressBarData);
/// @brief 进度条组件；用于显示进度，不可操作
class TpProgressBar : public TpWidget
{
public:
    /// @brief 进度条方向
    enum Direct
    {
        Horizon,
        Vertical
    };

public:
    TpProgressBar(TpWidget *parent = nullptr, const Direct &direct = Horizon);
    virtual ~TpProgressBar();

    /// @brief 设置取值范围
    /// @param min 最小值
    /// @param max 最大值
    virtual void setRange(const int32_t &min, const int32_t &max);
    /// @brief 设置进度条方向
    /// @param direct 方向枚举
    virtual void setDirection(const Direct &direct = Horizon);
    /// @brief 获取当前进度条方向
    /// @return 进度条方向枚举
    virtual Direct direction() const;

    /// @brief 获取当前值
    /// @param position
    virtual int32_t value() const;
    /// @brief 设置当前值
    /// @param value 当前值
    virtual void setValue(const int32_t &value);

    /// @brief 设置百分比文本显隐状态
    /// @param visible true显示；false不显示
    void setTextVisible(bool visible);
    /// @brief 获取当前是否显示百分比文本
    /// @return true显示，false不显示
    bool textVisible() const;

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpProgressBar); }

private:
    ITpProgressBarData *data_;
};

#endif
