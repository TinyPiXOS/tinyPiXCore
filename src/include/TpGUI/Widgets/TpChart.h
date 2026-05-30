/*
 * 版权声明 (Copyright Declaration)
 * 作者: 刘杨
 * 邮箱: 825143438@qq.com
 * 版权所有: 2026 刘杨. All rights reserved.
 * 描述: TpChart 图表控件类 API 定义
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
    /// @param color 颜色值（ARGB 格式）
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
    virtual bool onPaintEvent(TpPaintEvent* event) override;

    /// @brief 鼠标移动事件
    /// @param event 鼠标事件指针
    virtual bool onMouseMoveEvent(TpMouseEvent* event) override;

    /// @brief 鼠标按下事件
    /// @param event 鼠标事件指针
    virtual bool onMousePressEvent(TpMouseEvent* event) override;

    /// @brief 鼠标释放事件
    /// @param event 鼠标事件指针
    virtual bool onMouseRleaseEvent(TpMouseEvent* event) override;

    /// @brief 滚轮事件
    /// @param event 滚轮浜嬩欢鎸囬拡
    virtual bool onWheelEvent(TpWheelEvent* event) override;

    /// @brief 离开事件
    /// @param event 离开浜嬩欢鎸囬拡
    virtual bool onLeaveEvent(TpLeaveEvent* event) override;

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
    /// @param color 颜色值（ARGB 格式）
    void setGridColor(int32_t color);

    /// @brief 设置是否显示提示框
    /// @param visible 是否可见
    void setTooltipVisible(bool visible);

    /// @brief 设置是否显示十字线
    /// @param visible 是否可见
    void setCrosshairVisible(bool visible);

    /// @brief 设置是否启用选中
    /// @param enabled 是否启用
    void setSelectionEnabled(bool enabled);

    /// @brief 设置是否允许拖拽平移
    /// @param enabled 是否启用
    void setPanEnabled(bool enabled);

    /// @brief 设置是否允许滚轮缩放
    /// @param enabled 是否启用
    void setWheelZoomEnabled(bool enabled);

    /// @brief 设置图例是否可点击
    /// @param enabled 是否启用
    void setLegendClickable(bool enabled);

    /// @brief 恢复默认视图
    void resetView();

    /// @brief 获取当前选中系列索引
    /// @return 系列索引
    int32_t selectedSeriesIndex() const;

    /// @brief 获取当前选中数据点索引
    /// @return 数据点索引
    int32_t selectedPointIndex() const;

    /// @brief 获取当前选中切片索引
    /// @return 切片索引
    int32_t selectedSliceIndex() const;

private:
    /// @brief 计算布局矩形
    /// @param totalRect 鎬讳綋鐭╁舰
    /// @return 绘图区域矩形
    TpRect calculateLayout(const TpRect& totalRect);

    /// @brief 更新坐标轴范围
    void updateAxisRange();

    /// @brief 绘制背景
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param totalRect 鎬讳綋鐭╁舰
    /// @param chartRect 缁樺浘鍖虹煩褰?
    void drawBackground(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect);

    /// @brief 绘制网格
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param chartRect 缁樺浘鍖虹煩褰?
    void drawGrid(TpPainter* painter, const TpRect& chartRect);

    /// @brief 绘制坐标轴
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param chartRect 缁樺浘鍖虹煩褰?
    void drawAxes(TpPainter* painter, const TpRect& chartRect);

    /// @brief 绘制标题
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param totalRect 鎬讳綋鐭╁舰
    void drawTitle(TpPainter* painter, const TpRect& totalRect);

    /// @brief 绘制图例
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param totalRect 鎬讳綋鐭╁舰
    void drawLegend(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect);

    /// @brief 判断是否进入饼图模式
    /// @return 是否为纯饼图模式
    bool isPieChartMode() const;

    /// @brief 绘制饼图模式
    /// @param painter 画笔指针
    /// @param chartRect 绘图区域矩形
    void drawPieChart(TpPainter* painter, const TpRect& chartRect);

    /// @brief 绘制提示框
    /// @param painter 画笔指针
    /// @param chartRect 绘图区域矩形
    void drawTooltip(TpPainter* painter, const TpRect& chartRect);

    /// @brief 绘制十字线
    /// @param painter 画笔指针
    /// @param chartRect 绘图区域矩形
    void drawCrosshair(TpPainter* painter, const TpRect& chartRect);

    /// @brief 绘制悬停高亮
    /// @param painter 画笔指针
    /// @param chartRect 绘图区域矩形
    void drawHoverHighlight(TpPainter* painter, const TpRect& chartRect);

    /// @brief 绘制选中效果
    /// @param painter 画笔指针
    /// @param chartRect 绘图区域矩形
    void drawSelection(TpPainter* painter, const TpRect& chartRect);

    /// @brief 构建图例数据
    /// @param totalRect 总区域矩形
    /// @param chartRect 绘图区域矩形
    /// @param names 图例名称
    /// @param colors 图例颜色
    /// @param endColors 图例结束颜色
    /// @param types 图例类型
    void buildLegendData(const TpRect& totalRect, const TpRect& chartRect, TpVector<const char*>& names,
                         TpVector<int32_t>& colors, TpVector<int32_t>& endColors, TpVector<int32_t>& types);

    /// @brief 计算当前布局状态
    /// @param totalRect 总区域矩形
    /// @param chartRect 绘图区域矩形
    /// @param showTitleAndLegend 是否显示标题和图例
    /// @param showAxisLabels 是否显示坐标标签
    /// @param showAxisTicks 是否显示坐标刻度
    /// @param pieChartMode 是否为饼图模式
    /// @param scale 缩放系数
    void prepareLayoutState(const TpRect& totalRect, TpRect& chartRect, bool& showTitleAndLegend, bool& showAxisLabels,
                            bool& showAxisTicks, bool& pieChartMode, double& scale);

    /// @brief 命中测试
    /// @param chartRect 绘图区域矩形
    /// @param pos 鼠标位置
    /// @param pieMode 是否为饼图模式
    /// @param seriesIndex 命中系列索引
    /// @param pointIndex 命中数据点索引
    /// @param sliceIndex 命中切片索引
    /// @param text 提示文本
    /// @param hitPos 命中位置
    /// @return 是否命中
    bool hitTestAt(const TpRect& chartRect, const TpPoint& pos, bool pieMode, int32_t& seriesIndex, int32_t& pointIndex,
                   int32_t& sliceIndex, TpString& text, TpPoint& hitPos);

    void clearHoverState();

    /// @brief 按当前位置缩放坐标轴
    /// @param pos 鼠标位置
    /// @param chartRect 绘图区域矩形
    /// @param factor 缩放因子
    void zoomAxisAt(const TpPoint& pos, const TpRect& chartRect, double factor);

    /// @brief 按当前位置平移坐标轴
    /// @param pos 鼠标位置
    /// @param chartRect 绘图区域矩形
    void panAxisTo(const TpPoint& pos, const TpRect& chartRect);

    /// @brief 处理图例点击
    /// @param pos 鼠标位置
    /// @param totalRect 总区域矩形
    /// @param chartRect 绘图区域矩形
    bool toggleLegendAt(const TpPoint& pos, const TpRect& totalRect, const TpRect& chartRect);

    /// @brief 获取当前状态的 CSS 数据
    tpShared<TpCssData> currentStatusCss();

    /// @brief 刷新基础 CSS 样式
    void refreshBaseCss();

private:
    /// @brief 私有实现结构
    struct Impl;
    Impl* m_impl;

    /// @brief CSS 数据 - 启用状态
    tpShared<TpCssData> enabledCssData;

    /// @brief CSS 数据 - 禁用状态
    tpShared<TpCssData> disabledCssData;

    /// @brief CSS 数据 - 悬停状态
    tpShared<TpCssData> hoverCssData;

    /// @brief CSS 数据 - 按下状态
    tpShared<TpCssData> pressCssData;

    /// @brief CSS 数据 - 选中状态
    tpShared<TpCssData> checkedCssData;
};

#endif // TPCHART_H
