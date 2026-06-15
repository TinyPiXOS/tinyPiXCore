/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：刘杨
 * 邮箱 (Email)：2825143438@qq.com
 * 版权所有 (Copyright)：© 2026 刘杨. All rights reserved.
 * 描述 (Description)：图表控件，支持折线图、柱状图、饼图等多种图表类型，提供数据系列管理、坐标轴配置及交互功能
 */

#ifndef TPCHART_H
#define TPCHART_H

#include <TpWidget.h>
#include <TpVector.h>
#include <TpString.h>
#include "TpAxis.h"
#include "TpSeries.h"

class TpChart : public TpWidget
{
public:
    /// @brief 创建图表控件
    TpChart();
    /// @brief 销毁图表控件
    virtual ~TpChart();

    TpChart(const TpChart&) = delete;
    TpChart& operator=(const TpChart&) = delete;

public:
    struct Impl;

public:
    /// @brief 设置图表标题
    /// @param title 标题文本
    void setTitle(const char* title);

    /// @brief 设置图表背景色
    /// @param color RGBA 颜色值
    void setBackgroundColor(int32_t color);

    /// @brief 获取 X 轴对象
    TpAxis* axisX();

    /// @brief 获取 Y 轴对象
    TpAxis* axisY();

    /// @brief 添加一个数据系列，图表会接管该系列的生命周期
    /// @param series 数据系列指针
    void addSeries(TpSeries* series);

    /// @brief 移除所有数据系列
    void removeAllSeries();

    virtual TpString pluginType() override { return "TpChart"; }

    /// @brief 设置坐标轴标签
    /// @param xLabel X 轴标签
    /// @param yLabel Y 轴标签
    void setAxisLabels(const char* xLabel, const char* yLabel);

    /// @brief 设置 X 轴网格线是否显示
    void setGridXVisible(bool visible);

    /// @brief 设置 Y 轴网格线是否显示
    void setGridYVisible(bool visible);

    /// @brief 设置网格线颜色
    void setGridColor(int32_t color);

    /// @brief 设置是否显示提示信息
    void setTooltipVisible(bool visible);

    /// @brief 设置是否显示十字线
    void setCrosshairVisible(bool visible);

    /// @brief 设置是否允许点选系列或数据点
    void setSelectionEnabled(bool enabled);

    /// @brief 设置是否允许拖拽平移图表
    void setPanEnabled(bool enabled);

    /// @brief 设置是否允许滚轮缩放图表
    void setWheelZoomEnabled(bool enabled);

    /// @brief 设置图例是否可点击
    void setLegendClickable(bool enabled);

    /// @brief 恢复默认视图范围
    void resetView();

    /// @brief 获取当前选中的系列索引
    int32_t selectedSeriesIndex() const;

    /// @brief 获取当前选中的数据点索引
    int32_t selectedPointIndex() const;

    /// @brief 获取当前选中的扇区索引
    int32_t selectedSliceIndex() const;

protected:
    /// @brief 设置样式表，内部会同步刷新图表自身的 CSS 状态
    virtual void setStyleSheet(const TpString& styleSheet) override;
    virtual bool onPaintEvent(TpPaintEvent* event) override;
    virtual bool onMouseMoveEvent(TpMouseEvent* event) override;
    virtual bool onMousePressEvent(TpMouseEvent* event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent* event) override;
    virtual bool onWheelEvent(TpWheelEvent* event) override;
    virtual bool onLeaveEvent(TpLeaveEvent* event) override;

private:
    Impl* data_;
};

#endif // TPCHART_H
