#ifndef __TP_PERCENT_PROGRESS_H
#define __TP_PERCENT_PROGRESS_H

#include "TpWidget.h"

TP_DEF_VOID_TYPE_VAR(ItpPercentProgressBarData);
/// @brief 百分比显示的进度条
class TpPercentProgressBar : public TpWidget
{
public:
    TpPercentProgressBar(TpWidget *parent = nullptr);
    virtual ~TpPercentProgressBar();

public:
    /// @brief 设置进度条总容量值
    /// @param totalValue 总容量值
    virtual void setTotalValue(const double &totalValue);

    /// @brief 添加一个item
    /// @param name item的名称
    /// @param value item的数值，会根据总容量值设置百分比
    /// @return 返回添加的item的索引，从0开始
    virtual uint32_t addItem(const TpString &name, const double &value);

    /// @brief 指定索引值移除一个item
    /// @param index 索引值
    virtual void removeItem(const uint32_t &index);

    /// @brief 指定索引设置item的名称
    /// @param index 索引值
    /// @param text item名称
    virtual void setItemText(const uint32_t &index, const TpString &text);

    /// @brief 指定索引值设置item的颜色
    /// @param index 索引值
    /// @param color 颜色
    virtual void setItemColor(const uint32_t &index, const int32_t &color);

    /// @brief 指定索引值设置item的数值
    /// @param index 索引值
    /// @param value 数值
    virtual void setItemValue(const uint32_t &index, const double &value);

public:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

    virtual TpString pluginType() override { return TO_STRING(TpPercentProgressBar); }

private:
    ItpPercentProgressBarData *data_;
};

#endif
