/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：刘杨
 * 邮箱 (Email)：2825143438@qq.com
 * 版权所有 (Copyright)：© 2026 刘杨。All rights reserved.
 * 描述 (Description)：TpChart 图表控件类 API 定义
 */

#ifndef TPCHART_H
#define TPCHART_H

#include <TpWidget.h>
#include <TpVector.h>
#include <TpString.h>
#include "TpAxis.h"
#include "TpSeries.h"

/// @brief 图表控件类
class TpChart : public TpWidget
{
public:
    /// @brief 构造函数
    TpChart();

    /// @brief 析构函数
    virtual ~TpChart();

public:
    /// @brief 设置图表标题
    /// @param title 标题字符串
    void setTitle(const char* title);

    /// @brief 设置图表背景颜色
    /// @param color 颜色值 (ARGB 格式)
    void setBackgroundColor(int32_t color);

    /// @brief 获取 X 轴指针
    /// @return X 轴指针
    TpAxis* axisX();

    /// @brief 获取 Y 轴指针
    /// @return Y 轴指针
    TpAxis* axisY();

    /// @brief 添加数据系列
    /// @param series 数据系列指针
    void addSeries(TpSeries* series);

    /// @brief 移除所有数据系列
    void removeAllSeries();

    /// @brief 绘制事件
    /// @param event 绘制事件指针
    /// @return 返回 true 继续执行子控件绘制
    virtual bool onPaintEvent(TpPaintEvent* event) override;

    /// @brief 获取插件类型
    /// @return 插件类型字符串
    virtual TpString pluginType() override { return "TpChart"; }

    /// @brief 设置坐标轴标签
    /// @param xLabel X 轴标签字符串
    /// @param yLabel Y 轴标签字符串
    void setAxisLabels(const char* xLabel, const char* yLabel);

    /// @brief 设置样式表（CSS）
    /// @param styleSheet 样式表字符串
    void setStyleSheet(const TpString& styleSheet);

    /// @brief 获取当前样式表（CSS）
    /// @return 样式表字符串
    TpString styleSheet();

    /// @brief 设置 X 轴网格线是否可见（垂直网格线）
    /// @param visible 是否可见
    void setGridXVisible(bool visible);

    /// @brief 设置 Y 轴网格线是否可见（水平网格线）
    /// @param visible 是否可见
    void setGridYVisible(bool visible);

    /// @brief 设置网格线颜色
    /// @param color 颜色值 (ARGB 格式)
    void setGridColor(int32_t color);

private:
    /// @brief 计算布局矩形
    /// @param totalRect 总体矩形
    /// @return 绘图区矩形
    TpRect calculateLayout(const TpRect& totalRect);

    /// @brief 更新坐标轴范围
    void updateAxisRange();

    /// @brief 绘制背景
    /// @param painter 画笔指针
    /// @param totalRect 总体矩形
    /// @param chartRect 绘图区矩形
    void drawBackground(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect);

    /// @brief 绘制网格
    /// @param painter 画笔指针
    /// @param chartRect 绘图区矩形
    void drawGrid(TpPainter* painter, const TpRect& chartRect);

    /// @brief 绘制坐标轴
    /// @param painter 画笔指针
    /// @param chartRect 绘图区矩形
    void drawAxes(TpPainter* painter, const TpRect& chartRect);

    /// @brief 绘制标题
    /// @param painter 画笔指针
    /// @param totalRect 总体矩形
    void drawTitle(TpPainter* painter, const TpRect& totalRect);

    /// @brief 绘制图例
    /// @param painter 画笔指针
    /// @param totalRect 总体矩形
    /// @param chartRect 绘图区矩形
    void drawLegend(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect);

    /// @brief 判断是否进入饼图模式
    /// @return 是否为纯饼图模式
    bool isPieChartMode() const;

    /// @brief 绘制饼图模式
    /// @param painter 绘制器指针
    /// @param chartRect 绘图区域矩形
    void drawPieChart(TpPainter* painter, const TpRect& chartRect);

    /// @brief 获取当前状态的CSS数据
    tpShared<TpCssData> currentStatusCss();

    /// @brief 刷新基础CSS样式
    void refreshBaseCss();

private:
    /// @brief 私有实现结构体
    struct Impl;
    Impl* m_impl;

    /// @brief CSS数据 - 启用状态
    tpShared<TpCssData> enabledCssData;

    /// @brief CSS数据 - 禁用状态
    tpShared<TpCssData> disabledCssData;

    /// @brief CSS数据 - 悬停状态
    tpShared<TpCssData> hoverCssData;

    /// @brief CSS数据 - 按下状态
    tpShared<TpCssData> pressCssData;

    /// @brief CSS数据 - 选中状态
    tpShared<TpCssData> checkedCssData;
};

#endif // TPCHART_H
