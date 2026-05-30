/*
 * 版权声明 (Copyright Declaration)
 * 作者: 刘杨
 * 邮箱: 825143438@qq.com
 * 版权所有: 2026 刘杨. All rights reserved.
 * 描述: 图表类实现 (TpChart) - 响应式缩放与极限尺寸修复版
 */

#include "TpChart.h"
#include "TpRenderUtils.h"
#include "TpSeries.h"
#include "TpApp.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpWidget.h"
#include <cfloat>
#include <cmath>
#include <cstdint> // 引入定宽整数类型

// 宏定义防止颜色报错
#ifndef _RGB
#define _RGB(r, g, b) ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#endif

// 私有实现结构体定义
namespace {

/// @brief 图例项
struct LegendItem
{
    TpString text;
    int32_t colorStart;
    int32_t colorEnd;
    int32_t type;
    bool visible;
    int32_t seriesIndex;
    int32_t pointIndex;
    int32_t sliceIndex;
    TpRect rect;

    LegendItem()
        : colorStart(0)
        , colorEnd(0)
        , type(0)
        , visible(true)
        , seriesIndex(-1)
        , pointIndex(-1)
        , sliceIndex(-1)
    {
    }
};

static int32_t dimColor(int32_t color, int32_t alpha)
{
    if (alpha < 0) alpha = 0;
    if (alpha > 255) alpha = 255;
    return (color & 0x00FFFFFF) | (alpha << 24);
}

static double normalizeAngle(double angle)
{
    while (angle < 0.0) angle += 360.0;
    while (angle >= 360.0) angle -= 360.0;
    return angle;
}

static double distanceSq(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    double dx = static_cast<double>(x0 - x1);
    double dy = static_cast<double>(y0 - y1);
    return dx * dx + dy * dy;
}

static bool pointInRect(const TpRect& rect, const TpPoint& pos)
{
    return pos.x() >= rect.x() && pos.x() <= rect.right() && pos.y() >= rect.y() && pos.y() <= rect.bottom();
}

static TpRect pieSeriesRect(const TpRect& chartRect, int32_t pieCount, int32_t pieIndex)
{
    if (pieCount <= 0) {
        return chartRect;
    }

    int32_t slotHeight = chartRect.height() / pieCount;
    if (slotHeight <= 0) {
        slotHeight = chartRect.height();
    }

    int32_t currentTop = chartRect.y() + pieIndex * slotHeight;
    int32_t currentHeight = (pieIndex == pieCount - 1) ? (chartRect.bottom() - currentTop + 1) : slotHeight;
    if (currentHeight < 20) currentHeight = 20;

    return TpRect(chartRect.x(), currentTop, chartRect.width(), currentHeight);
}

}

struct TpChart::Impl {
    TpString title;                  // 图表标题
    int32_t backgroundColor;         // 背景颜色

    TpAxis* axisX;                   // X 轴
    TpAxis* axisY;                   // Y 轴

    TpString labelX;                 // X 轴标签
    TpString labelY;                 // Y 轴标签

    TpVector<TpSeries*> seriesList;  // 数据系列列表

    int32_t marginTop;               // 上边距
    int32_t marginBottom;            // 下边距
    int32_t marginLeft;              // 左边距
    int32_t marginRight;             // 右边距

    bool gridXVisible;               // X 轴网格线可见性
    bool gridYVisible;               // Y 轴网格线可见性
    int32_t gridColor;               // 网格线基础颜色
    bool cssAppliedToSeries = false; // CSS 样式是否已应用到 series
    bool tooltipVisible = false;
    bool crosshairVisible = false;
    bool selectionEnabled = false;
    bool panEnabled = false;
    bool wheelZoomEnabled = false;
    bool legendClickable = false;

    bool mousePressed = false;
    bool isDragging = false;
    TpPoint pressPos;
    TpPoint dragStartPos;
    double dragStartXMin = 0.0;
    double dragStartXMax = 0.0;
    double dragStartYMin = 0.0;
    double dragStartYMax = 0.0;

    bool hasHover = false;
    TpPoint hoverPos;
    int32_t hoverSeriesIndex = -1;
    int32_t hoverPointIndex = -1;
    int32_t hoverSliceIndex = -1;
    TpString tooltipText;

    bool hasPointerPos = false;
    TpPoint pointerPos;

    int32_t selectedSeriesIndex = -1;
    int32_t selectedPointIndex = -1;
    int32_t selectedSliceIndex = -1;

    TpVector<LegendItem> legendItems;
};


// 构造与析构
TpChart::TpChart()
{
    m_impl = new Impl();
    m_impl->backgroundColor = _RGB(255, 255, 255);
    m_impl->marginTop = 60;    // 留给标题
    m_impl->marginBottom = 60; // 留给 X 轴刻度
    m_impl->marginLeft = 60;   // 留给 Y 轴刻度
    m_impl->marginRight = 20;  // 防止内容溢出
    m_impl->gridXVisible = true;
    m_impl->gridYVisible = true;
    m_impl->gridColor = _RGB(230, 230, 230); // 默认浅灰色网格线

    // 显式设置背景颜色
    this->setBackGroundColor(0xFFFFFFFF, true);

    // 初始化 CSS 数据
    enabledCssData = nullptr;
    disabledCssData = nullptr;
    hoverCssData = nullptr;
    pressCssData = nullptr;
    checkedCssData = nullptr;

    // 刷新基础 CSS 样式
    refreshBaseCss();

    // 自动创建默认坐标轴
    m_impl->axisX = new TpAxis();
    m_impl->axisX->setMode(TpAxis::AxisMode::Value);

    m_impl->axisY = new TpAxis();
    m_impl->axisY->setMode(TpAxis::AxisMode::Value);
}

TpChart::~TpChart() {
    if (m_impl) {
        if (m_impl->axisX) delete m_impl->axisX;
        if (m_impl->axisY) delete m_impl->axisY;
        removeAllSeries();
        delete m_impl;
        m_impl = nullptr;
    }
}


// 公共接口
/// @brief 设置图表标题
void TpChart::setTitle(const char* title) {
    m_impl->title = title;
    this->update(); 
}

/// 获取 X 轴引用
TpAxis* TpChart::axisX() {
    return m_impl->axisX;
}

/// 获取 Y 轴引用
TpAxis* TpChart::axisY() {
    return m_impl->axisY;
}

/// @brief 设置图表背景颜色
void TpChart::setBackgroundColor(int32_t color) {
    m_impl->backgroundColor = color;
    this->update();
}

/// @brief 设置坐标轴标签
void TpChart::setAxisLabels(const char* xLabel, const char* yLabel) {
    m_impl->labelX = xLabel;
    m_impl->labelY = yLabel;
    this->update();
}

/// @brief 设置 X 轴网格线可见性
void TpChart::setGridXVisible(bool visible) {
    m_impl->gridXVisible = visible;
    this->update();
}

/// @brief 设置 Y 轴网格线可见性
void TpChart::setGridYVisible(bool visible) {
    m_impl->gridYVisible = visible;
    this->update();
}

/// 设置网格线颜色
void TpChart::setGridColor(int32_t color) {
    m_impl->gridColor = color;
    this->update();
}

/// @brief 设置是否显示提示框
void TpChart::setTooltipVisible(bool visible) {
    m_impl->tooltipVisible = visible;
    if (!visible) {
        m_impl->hasHover = false;
        m_impl->tooltipText.clear();
    }
    this->update();
}

/// @brief 设置是否显示十字线
void TpChart::setCrosshairVisible(bool visible) {
    m_impl->crosshairVisible = visible;
    this->update();
}

/// @brief 设置是否启用选中
void TpChart::setSelectionEnabled(bool enabled) {
    m_impl->selectionEnabled = enabled;
    if (!enabled) {
        m_impl->selectedSeriesIndex = -1;
        m_impl->selectedPointIndex = -1;
        m_impl->selectedSliceIndex = -1;
    }
    this->update();
}

/// @brief 设置是否允许拖拽平移
void TpChart::setPanEnabled(bool enabled) {
    m_impl->panEnabled = enabled;
}

/// @brief 设置是否允许滚轮缩放
void TpChart::setWheelZoomEnabled(bool enabled) {
    m_impl->wheelZoomEnabled = enabled;
}

/// @brief 设置图例是否可点击
void TpChart::setLegendClickable(bool enabled) {
    m_impl->legendClickable = enabled;
    this->update();
}

/// 恢复默认视图
void TpChart::resetView() {
    m_impl->axisX->setAutoRange(true);
    m_impl->axisY->setAutoRange(true);
    clearHoverState();
    m_impl->hasPointerPos = false;
    m_impl->mousePressed = false;
    m_impl->isDragging = false;
    m_impl->selectedSeriesIndex = -1;
    m_impl->selectedPointIndex = -1;
    m_impl->selectedSliceIndex = -1;
    this->update();
}

/// 获取当前选中系列索引
int32_t TpChart::selectedSeriesIndex() const {
    return m_impl ? m_impl->selectedSeriesIndex : -1;
}

/// 获取当前选中的数据点索引
int32_t TpChart::selectedPointIndex() const {
    return m_impl ? m_impl->selectedPointIndex : -1;
}

/// 获取当前选中切片索引
int32_t TpChart::selectedSliceIndex() const {
    return m_impl ? m_impl->selectedSliceIndex : -1;
}

/// 添加数据系列
void TpChart::addSeries(TpSeries* series) {
    if (series) {
        m_impl->seriesList.push_back(series);
        m_impl->cssAppliedToSeries = false;
        this->update();
    }
}

/// 移除所有数据系列
void TpChart::removeAllSeries() {
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        delete m_impl->seriesList[i];
    }
    m_impl->seriesList.clear();
    m_impl->legendItems.clear();
    clearHoverState();
    m_impl->hasPointerPos = false;
    m_impl->mousePressed = false;
    m_impl->isDragging = false;
    m_impl->selectedSeriesIndex = -1;
    m_impl->selectedPointIndex = -1;
    m_impl->selectedSliceIndex = -1;
    this->update();
}

/// @brief 设置样式表（CSS）
void TpChart::setStyleSheet(const TpString& styleSheet) {
    TpApp::Inst()->cssParser()->parseCss(styleSheet);
    refreshBaseCss();
    m_impl->cssAppliedToSeries = false;
}

/// 获取当前样式表（CSS）
TpString TpChart::styleSheet() {
    return TpApp::Inst()->cssParser()->cssStr();
}

/// 获取当前状态的 CSS 数据
tpShared<TpCssData> TpChart::currentStatusCss() {
    // TpChart 主要使用启用状态的 CSS
    // 如果需要悬停/按下效果，可以在外部通过 setChecked() 等方式控制
    tpShared<TpCssData> curCssData = enabledCssData;
    if (!enabled()) {
        curCssData = disabledCssData;
    } else if (checkable() && checked()) {
        curCssData = checkedCssData;
    }
    return curCssData;
}

/// 刷新基础 CSS 样式
void TpChart::refreshBaseCss() {
    enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    disabledCssData = readCss(pluginType(), TpCssParser::Disabled);
    hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    checkedCssData = readCss(pluginType(), TpCssParser::Checked);
}

// 内部逻辑：自动计算范围
/// @brief 根据所有 Series 数据自动调整坐标轴范围
void TpChart::updateAxisRange() {
    bool autoRangeX = m_impl->axisX->isAutoRange();
    bool autoRangeY = m_impl->axisY->isAutoRange();
    bool hasVisibleSeries = false;
    bool allPieSeries = true;

    if (!autoRangeX && !autoRangeY) {
        return;
    }

    if (m_impl->seriesList.empty()) {
        // 如果没有数据，并且是自动范围，则设置默认值
        if (autoRangeX) m_impl->axisX->setRange(0, 10);
        if (autoRangeY) m_impl->axisY->setRange(0, 10);
        return;
    }

    double minX = DBL_MAX, maxX = -DBL_MAX;
    double minY = DBL_MAX, maxY = -DBL_MAX;
    bool hasData = false;
    
    // 标记是否有柱状图
    bool hasBarSeries = false; 

    // 遍历所有 Series，找出最大最小值
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (!s || !s->isVisible()) continue;

        hasVisibleSeries = true;
        if (s->type() != TpSeries::TypePie) {
            allPieSeries = false;
        }

        if (s->type() == TpSeries::TypePie) {
            continue;
        }

        if (autoRangeX && s->type() == TpSeries::TypeBar) {
            hasBarSeries = true;
        }

        const TpVector<TpDataPoint>& data = s->data();
        for (int32_t k = 0; k < data.size(); ++k) {
            if (autoRangeX) {
                double x = data[k].x;
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
            }
            if (autoRangeY) {
                double y = data[k].y;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
            hasData = true;
        }
    }

    if (!hasVisibleSeries || allPieSeries) {
        return;
    }

    if (!hasData) {
        if (autoRangeX) m_impl->axisX->setRange(0, 10);
        if (autoRangeY) m_impl->axisY->setRange(0, 10);
        return;
    }

    // 判断 X 轴是否开启自动范围
    if (autoRangeX) {
        if (hasBarSeries) {
           double spanX = maxX - minX;
            if (spanX <= 0) spanX = 1.0;
            double leftPad = m_impl->axisX->xLeftPaddingRatio() * spanX;
            double rightPad = m_impl->axisX->xRightPaddingRatio() * spanX;
            m_impl->axisX->setRange(minX - leftPad, maxX + rightPad);
        } else {
            // 折线图：使用左右留白比例（默认均匀），则贴边
            double spanX = maxX - minX;
            if (spanX <= 0) spanX = 1.0;
            double leftPad = m_impl->axisX->xLeftPaddingRatio() * spanX;
            double rightPad = m_impl->axisX->xRightPaddingRatio() * spanX;
            m_impl->axisX->setRange(minX - leftPad, maxX + rightPad);
        }
    }

    // 判断 Y 轴是否开启自动范围
    if (autoRangeY) {
        double spanY = maxY - minY;
        if (spanY <= 0) spanY = 1.0;
        
        double topPad = m_impl->axisY->yTopPaddingRatio() * spanY;
        double bottomPad = m_impl->axisY->yBottomPaddingRatio() * spanY;
        
        double newMin = minY - bottomPad;
        double newMax = maxY + topPad;
        
        // 强制包含零点，但保留留白扩展
        if (minY > 0) {
            // 数据全为正：最小值设为 0，并向下扩展底部留白
            newMin = 0 - bottomPad;
            // 最大值仍为 maxY + topPad（但 maxY>0，因此顶部留白自然生效）
        } else if (maxY < 0) {
            // 数据全为负：最大值设为 0，并向上扩展顶部留白
            newMax = 0 + topPad;
            // 最小值仍为 minY - bottomPad（自然生效）
        }
        // 跨零点的情况：留白已经在 newMin/newMax 中体现，无需额外处理
        
        m_impl->axisY->setRange(newMin, newMax);
    }
}

// 核心绘制逻辑
/// @brief 主绘制事件
bool TpChart::onPaintEvent(TpPaintEvent* event) {
    if (!event) {
        return false;
    }

    TpPainter* painter = event->painter();
    if (!painter) {
        return false;
    }

    TpWidget::onPaintEvent(event);

    TpRect totalRect(0, 0, this->width(), this->height());
    TpRect chartRect;
    bool showTitleAndLegend = false;
    bool showAxisLabels = false;
    bool showAxisTicks = false;
    bool pieChartMode = false;
    double scale = 1.0;

    prepareLayoutState(totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    if (!pieChartMode) {
        updateAxisRange();

        int32_t tickDensityX = showAxisTicks ? (int32_t)(80 * scale) : 120;
        int32_t tickDensityY = showAxisTicks ? (int32_t)(50 * scale) : 80;
        int32_t targetTicksX = chartRect.width() / (tickDensityX > 0 ? tickDensityX : 1);
        int32_t targetTicksY = chartRect.height() / (tickDensityY > 0 ? tickDensityY : 1);

        m_impl->axisX->updateNiceTicks(targetTicksX > 2 ? targetTicksX : 2);
        m_impl->axisY->updateNiceTicks(targetTicksY > 2 ? targetTicksY : 2);
    }

    drawBackground(painter, totalRect, chartRect);
    if (!pieChartMode) {
        drawGrid(painter, chartRect);
    }

    if (!m_impl->cssAppliedToSeries) {
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* s = m_impl->seriesList[i];
            if (s && s->isVisible()) {
                if (s->type() == TpSeries::TypeLine) {
                    s->applyCssData("TpLineSeries", TpCssParser::Enabled);
                } else if (s->type() == TpSeries::TypeBar) {
                    s->applyCssData("TpBarSeries", TpCssParser::Enabled);
                } else if (s->type() == TpSeries::TypeScatter) {
                    s->applyCssData("TpScatterSeries", TpCssParser::Enabled);
                } else if (s->type() == TpSeries::TypePie) {
                    s->applyCssData("TpPieSeries", TpCssParser::Enabled);
                }
            }
        }
        m_impl->cssAppliedToSeries = true;
    }

    if (pieChartMode) {
        drawPieChart(painter, chartRect);
    } else {
        int32_t barSeriesCount = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* s = m_impl->seriesList[i];
            if (s && s->isVisible() && s->type() == TpSeries::TypeBar) {
                barSeriesCount++;
            }
        }

        int32_t barIndex = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* s = m_impl->seriesList[i];
            if (s && s->isVisible()) {
                if (s->type() == TpSeries::TypePie) {
                    continue;
                }
                if (s->type() == TpSeries::TypeBar) {
                    static_cast<TpBarSeries*>(s)->setLayoutInfo(barIndex++, barSeriesCount);
                }
                s->draw(painter, *m_impl->axisX, *m_impl->axisY, chartRect);
            }
        }

        if (showAxisTicks) {
            uint32_t black = _RGB(0, 0, 0);
            TpRenderUtils::drawAxisX(painter, chartRect, *m_impl->axisX, *m_impl->axisY, black, false, 0);
            TpRenderUtils::drawAxisY(painter, chartRect, *m_impl->axisY, *m_impl->axisX, black, false, 0);
        }
    }

    if (showAxisLabels && !pieChartMode) {
        int32_t labelFontSize = (int32_t)(20 * scale);
        if (labelFontSize < 9) labelFontSize = 9;

        TpFont labelFont;
        labelFont.setFontSize(labelFontSize);
        labelFont.setFontColor(_RGB(30, 30, 30));

        if (m_impl->labelX.length() > 0) {
            labelFont.setText(m_impl->labelX);
            int32_t textW = labelFont.pixelWidth() > 0 ? labelFont.pixelWidth() : m_impl->labelX.length() * (int32_t)(8 * scale);
            int32_t lx = chartRect.x() + (chartRect.width() / 2) - (textW / 2);
            int32_t ly = chartRect.bottom() + (int32_t)(40 * scale);
            painter->drawText(labelFont, lx, ly);
        }

        if (m_impl->labelY.length() > 0) {
            TpFont yLabelFont;
            yLabelFont.setFontSize(labelFontSize);
            yLabelFont.setFontColor(_RGB(30, 30, 30));
            yLabelFont.setVerticalText(true);
            yLabelFont.setText(m_impl->labelY);

            int32_t textHeight = (int32_t)(m_impl->labelY.length() * labelFontSize);
            int32_t lx = totalRect.x() + (int32_t)(5 * scale);
            int32_t ly = chartRect.y() + (chartRect.height() / 2) - (textHeight / 2);

            painter->drawText(yLabelFont, lx, ly);
        }
    }

    if (showTitleAndLegend) {
        drawTitle(painter, totalRect);
        drawLegend(painter, totalRect, chartRect);
    }

    if (m_impl->hasHover) {
        drawHoverHighlight(painter, chartRect);
    }

    if (m_impl->selectionEnabled) {
        drawSelection(painter, chartRect);
    }

    if (m_impl->crosshairVisible && m_impl->hasHover && !pieChartMode) {
        drawCrosshair(painter, chartRect);
    }

    if (m_impl->tooltipVisible && m_impl->hasHover) {
        drawTooltip(painter, chartRect);
    }

    return true;
}

// 内部私有实现
/// 计算布局矩形
TpRect TpChart::calculateLayout(const TpRect& totalRect) {
    int32_t top = m_impl->marginTop;
    if (m_impl->title.empty()) top -= 20;

    int32_t chartW = totalRect.width() - m_impl->marginLeft - m_impl->marginRight;
    int32_t chartH = totalRect.height() - top - m_impl->marginBottom;

    if (chartW < 10) chartW = 10;
    if (chartH < 10) chartH = 10;

    return TpRect(
        totalRect.x() + m_impl->marginLeft,
        totalRect.y() + top,
        chartW,
        chartH
    );
}

/// 绘制鑳屾櫙
void TpChart::drawBackground(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect) {
    // 使用 CSS 背景颜色（如果设置了）
    tpShared<TpCssData> curCssData = currentStatusCss();
    int32_t bgColor = curCssData->backgroundColor();

    TpRenderUtils::fillGradientRect(painter, totalRect, _RGB(248, 248, 248), _RGB(248, 248, 248));

    // 使用 CSS 背景颜色或默认背景颜色
    if (curCssData->backgroundColorIsGradient()) {
        TpBrush brush(curCssData->backgroundColorGradiant());
        painter->setBrush(brush);
        painter->drawRect(chartRect.x(), chartRect.y(), chartRect.width(), chartRect.height(), 0);
    } else {
        // 使用 drawRect 填充矩形（设置颜色和实心画刷）
        painter->setPen(bgColor);
        painter->setBrush(TpBrush(bgColor));
        painter->drawRect(chartRect.x(), chartRect.y(), chartRect.width(), chartRect.height(), 0);
        painter->setBrush(TpBrush(Tp::NoBrush));
    }
}

/// 绘制网格
void TpChart::drawGrid(TpPainter* painter, const TpRect& chartRect) {
    // 1. 基础安全检查
    if (!painter || !m_impl || !m_impl->axisX || !m_impl->axisY) {
        return; 
    }

    TpPen gridPen(m_impl->gridColor, 1);

    // 2. 绘制垂直网格线
    if (m_impl->gridXVisible) {
        const TpVector<double>& xTicks = m_impl->axisX->getTickValues();
        painter->setPen(gridPen);
        for (int32_t i = 0; i < xTicks.size(); ++i) {
            int32_t x = m_impl->axisX->mapToPixel(xTicks[i], chartRect.width(), chartRect.x(), false);
            if (x >= chartRect.x() && x <= chartRect.right()) {
                painter->drawLine(x, chartRect.y(), x, chartRect.bottom());
            }
        }
    }

    // 3. 绘制水平网格线
    if (m_impl->gridYVisible) {
        const TpVector<double>& yTicks = m_impl->axisY->getTickValues();
        painter->setPen(gridPen);
        for (int32_t i = 0; i < yTicks.size(); ++i) {
            int32_t y = m_impl->axisY->mapToPixel(yTicks[i], chartRect.height(), chartRect.y(), true);
            if (y >= chartRect.y() && y <= chartRect.bottom()) {
                painter->drawLine(chartRect.x(), y, chartRect.right(), y);
            }
        }
    }

    // 4. 零刻度线（X 轴主线）强化绘制
    // 增加逻辑保护，防止 min/max 异常导致的问题
    double yMin = m_impl->axisY->min();
    double yMax = m_impl->axisY->max();
    if (yMin < 0 && yMax > 0) {
        int32_t yZero = m_impl->axisY->mapToPixel(0.0, chartRect.height(), chartRect.y(), true);
        if (yZero >= chartRect.y() && yZero <= chartRect.bottom()) {
            TpPen zeroPen(_RGB(150, 150, 150), 1);
            painter->setPen(zeroPen);
            painter->drawLine(chartRect.x(), yZero, chartRect.right(), yZero);
        }
    }
}

/// 绘制标题
void TpChart::drawTitle(TpPainter* painter, const TpRect& totalRect) {
    if (m_impl->title.empty()) return;
    
    // 在这里重新计算一次 scale，保持接口不被污染
    double scaleX = totalRect.width() / 800.0;
    double scaleY = totalRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY; 
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    // 动态计算标题字体大小（基准 18px）
    int32_t titleFontSize = (int32_t)(18 * scale);
    if (titleFontSize < 10) titleFontSize = 10;

    TpFont font;
    font.setFontSize(titleFontSize);
    font.setFontColor(_RGB(0, 0, 0));
    font.setBold(true);
    font.setText(m_impl->title);
    
    int32_t textW = font.pixelWidth();
    if (textW <= 0) textW = m_impl->title.length() * (int32_t)(12 * scale); 

    int32_t x = totalRect.x() + (totalRect.width() - textW) / 2;
    int32_t y = totalRect.y() + (int32_t)(15 * scale); 
    
    painter->drawText(font, x, y);
}

bool TpChart::isPieChartMode() const {
    bool hasVisibleSeries = false;

    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (!s || !s->isVisible()) continue;
        hasVisibleSeries = true;
        if (s->type() != TpSeries::TypePie) {
            return false;
        }
    }

    return hasVisibleSeries;
}

void TpChart::drawPieChart(TpPainter* painter, const TpRect& chartRect) {
    if (!painter || !m_impl) return;

    int32_t pieCount = 0;
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (s && s->isVisible() && s->type() == TpSeries::TypePie) {
            pieCount++;
        }
    }

    if (pieCount == 0) return;

    int32_t slotTop = chartRect.y();
    int32_t slotHeight = chartRect.height() / pieCount;
    if (slotHeight <= 0) slotHeight = chartRect.height();

    int32_t pieIndex = 0;
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (!s || !s->isVisible() || s->type() != TpSeries::TypePie) continue;

        int32_t currentTop = chartRect.y() + pieIndex * slotHeight;
        int32_t currentHeight = (pieIndex == pieCount - 1) ? (chartRect.bottom() - currentTop + 1) : slotHeight;
        if (currentHeight < 20) currentHeight = 20;

        TpRect pieRect(chartRect.x(), currentTop, chartRect.width(), currentHeight);
        static_cast<TpPieSeries*>(s)->draw(painter, *m_impl->axisX, *m_impl->axisY, pieRect);
        pieIndex++;
    }
}

/// @brief 计算当前布局状态
void TpChart::prepareLayoutState(const TpRect& totalRect, TpRect& chartRect, bool& showTitleAndLegend, bool& showAxisLabels,
                                 bool& showAxisTicks, bool& pieChartMode, double& scale)
{
    int32_t w = totalRect.width();
    int32_t h = totalRect.height();

    double scaleX = w / 800.0;
    double scaleY = h / 600.0;
    scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    showTitleAndLegend = (w >= 300 && h >= 250);
    showAxisLabels = (w >= 250 && h >= 200);
    showAxisTicks = (w >= 120 && h >= 80);
    pieChartMode = isPieChartMode();

    if (pieChartMode) {
        m_impl->marginTop = m_impl->title.empty() || !showTitleAndLegend ? (int32_t)(35 * scale) : (int32_t)(70 * scale);
        m_impl->marginBottom = (int32_t)(25 * scale);
        m_impl->marginLeft = (int32_t)(20 * scale);
        m_impl->marginRight = (int32_t)(20 * scale);
    } else if (!showAxisTicks) {
        m_impl->marginTop = 5;
        m_impl->marginBottom = 5;
        m_impl->marginLeft = 5;
        m_impl->marginRight = 5;
    } else if (!showAxisLabels) {
        m_impl->marginTop = (int32_t)(20 * scale);
        m_impl->marginBottom = (int32_t)(25 * scale);
        m_impl->marginLeft = (int32_t)(35 * scale);
        m_impl->marginRight = (int32_t)(15 * scale);
    } else {
        m_impl->marginTop = (m_impl->title.empty() || !showTitleAndLegend) ? (int32_t)(40 * scale) : (int32_t)(80 * scale);
        m_impl->marginBottom = m_impl->labelX.empty() ? (int32_t)(40 * scale) : (int32_t)(60 * scale);
        m_impl->marginLeft = m_impl->labelY.empty() ? (int32_t)(50 * scale) : (int32_t)(80 * scale);
        m_impl->marginRight = (int32_t)(30 * scale);
    }

    chartRect = calculateLayout(totalRect);
}

/// @brief 构建图例数据
void TpChart::buildLegendData(const TpRect& totalRect, const TpRect& chartRect, TpVector<const char*>& names,
                              TpVector<int32_t>& colors, TpVector<int32_t>& endColors, TpVector<int32_t>& types)
{
    m_impl->legendItems.clear();
    names.clear();
    colors.clear();
    endColors.clear();
    types.clear();

    double scaleX = totalRect.width() / 800.0;
    double scaleY = totalRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    int32_t legendSize = (int32_t)(12 * scale);
    if (legendSize < 9) legendSize = 9;
    int32_t startY = totalRect.y() + (int32_t)(45 * scale);
    int32_t itemSpacing = (int32_t)(20 * scale);
    int32_t iconW = (int32_t)(16 * scale);
    int32_t iconH = (int32_t)(12 * scale);
    int32_t textOffset = iconW + (int32_t)(4 * scale);
    int32_t totalItemBaseW = textOffset + (int32_t)(4 * scale);

    TpFont font;
    font.setFontSize(legendSize);
    font.setFontColor(_RGB(0, 0, 0));

    bool pieMode = isPieChartMode();
    TpVector<int32_t> itemWidths;
    TpVector<int32_t> itemHeights;

    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (!s) continue;

        if (s->type() == TpSeries::TypePie && pieMode && s->isVisible()) {
            TpPieSeries* pieSeries = static_cast<TpPieSeries*>(s);
            for (int32_t k = 0; k < pieSeries->sliceCount(); ++k) {
                const TpString& sliceName = pieSeries->sliceName(k);
                if (sliceName.empty()) continue;

                LegendItem item;
                item.text = sliceName;
                item.colorStart = pieSeries->sliceColor(k);
                item.colorEnd = item.colorStart;
                item.type = TpRenderUtils::TypePie;
                item.visible = pieSeries->isSliceVisible(k);
                item.seriesIndex = i;
                item.sliceIndex = k;
                m_impl->legendItems.push_back(item);

                font.setText(item.text);
                int32_t textW = font.pixelWidth();
                int32_t textH = font.pixelHeight();
                if (textW < 0) textW = 0;
                if (textH < 0) textH = legendSize;
                itemWidths.push_back(totalItemBaseW + textW);
                itemHeights.push_back(textH > iconH ? textH : iconH);
            }
            continue;
        }

        if (s->name().empty()) continue;

        LegendItem item;
        item.text = s->name();
        item.colorStart = s->color();
        item.colorEnd = s->color();
        if (s->type() == TpSeries::TypeBar) {
            item.colorEnd = static_cast<TpBarSeries*>(s)->colorEnd();
            item.type = TpRenderUtils::TypeBar;
        } else if (s->type() == TpSeries::TypeScatter) {
            item.type = TpRenderUtils::TypeScatter;
        } else if (s->type() == TpSeries::TypePie) {
            item.type = TpRenderUtils::TypePie;
        } else {
            item.type = TpRenderUtils::TypeLine;
        }
        item.visible = s->isVisible();
        item.seriesIndex = i;
        item.sliceIndex = -1;
        m_impl->legendItems.push_back(item);

        font.setText(item.text);
        int32_t textW = font.pixelWidth();
        int32_t textH = font.pixelHeight();
        if (textW < 0) textW = 0;
        if (textH < 0) textH = legendSize;
        itemWidths.push_back(totalItemBaseW + textW);
        itemHeights.push_back(textH > iconH ? textH : iconH);
    }

    int32_t totalContentWidth = 0;
    for (int32_t i = 0; i < itemWidths.size(); ++i) {
        totalContentWidth += itemWidths[i];
        if (i < itemWidths.size() - 1) {
            totalContentWidth += itemSpacing;
        }
    }

    int32_t currentX = totalRect.x() + (totalRect.width() - totalContentWidth) / 2;
    for (int32_t i = 0; i < m_impl->legendItems.size(); ++i) {
        int32_t itemWidth = (i < itemWidths.size()) ? itemWidths[i] : totalItemBaseW;
        int32_t itemHeight = (i < itemHeights.size()) ? itemHeights[i] : legendSize;
        m_impl->legendItems[i].rect = TpRect(currentX, startY, itemWidth, itemHeight + 4);
        currentX += itemWidth + itemSpacing;
    }

    for (int32_t i = 0; i < m_impl->legendItems.size(); ++i) {
        names.push_back(m_impl->legendItems[i].text.c_str());
        colors.push_back(m_impl->legendItems[i].colorStart);
        endColors.push_back(m_impl->legendItems[i].colorEnd);
        types.push_back(m_impl->legendItems[i].type);
    }
}

/// @brief 绘制图例
void TpChart::drawLegend(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect)
{
    if (!painter || !m_impl) return;

    TpVector<const char*> names;
    TpVector<int32_t> colors;
    TpVector<int32_t> endColors;
    TpVector<int32_t> types;
    buildLegendData(totalRect, chartRect, names, colors, endColors, types);

    if (m_impl->legendItems.size() == 0) {
        return;
    }

    double scaleX = totalRect.width() / 800.0;
    double scaleY = totalRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    int32_t legendSize = (int32_t)(12 * scale);
    if (legendSize < 9) legendSize = 9;
    int32_t iconW = (int32_t)(16 * scale);
    int32_t iconH = (int32_t)(12 * scale);
    int32_t textOffset = iconW + (int32_t)(4 * scale);

    TpFont font;
    font.setFontSize(legendSize);

    for (int32_t i = 0; i < m_impl->legendItems.size(); ++i) {
        const LegendItem& item = m_impl->legendItems[i];
        int32_t currentX = item.rect.x();
        int32_t y = item.rect.y();
        int32_t textColor = item.visible ? _RGB(0, 0, 0) : dimColor(_RGB(0, 0, 0), 110);

        font.setText(item.text);
        font.setFontColor(textColor);
        int32_t textW = font.pixelWidth();
        int32_t textH = font.pixelHeight();
        if (textH <= 0) textH = legendSize;

        int32_t textCenterY = y + textH / 2;
        int32_t iconTopY = y + (textH - iconH) / 2 + 2;

        int32_t colorStart = item.visible ? item.colorStart : dimColor(item.colorStart, 110);
        int32_t colorEnd = item.visible ? item.colorEnd : dimColor(item.colorEnd, 110);

        if (item.type == TpRenderUtils::TypeBar) {
            TpRect iconRect(currentX, iconTopY, iconW, iconH);
            TpRenderUtils::fillGradientRect(painter, iconRect, colorStart, colorEnd);
        } else if (item.type == TpRenderUtils::TypeScatter) {
            int32_t pointR = (int32_t)(3 * scale);
            if (pointR < 1) pointR = 1;
            TpPoint center(currentX + (iconW / 2), textCenterY);
            TpRenderUtils::drawAnchorPoint(painter, center, pointR, colorStart, colorStart);
        } else if (item.type == TpRenderUtils::TypePie) {
            TpPoint center(currentX + (iconW / 2), textCenterY);
            int32_t pieR = iconH / 2;
            if (pieR < 2) pieR = 2;
            painter->setPen(TpPen(colorStart, 1));
            painter->setBrush(TpBrush(colorStart));
            painter->drawPie(center, pieR, 270, 330);
        } else {
            int32_t lineY = textCenterY;
            TpPen linePen(colorStart, 2);
            painter->setPen(linePen);
            painter->drawLine(currentX, lineY, currentX + iconW, lineY);

            int32_t pointR = (int32_t)(3 * scale);
            if (pointR < 1) pointR = 1;
            TpRect pointRect(currentX + (iconW / 2) - pointR, lineY - pointR, pointR * 2, pointR * 2);
            TpRenderUtils::fillGradientRect(painter, pointRect, colorStart, colorStart);
        }

        painter->drawText(font, currentX + textOffset, y);
    }
}

/// @brief 清理悬停状态
void TpChart::clearHoverState()
{
    m_impl->hasHover = false;
    m_impl->hoverSeriesIndex = -1;
    m_impl->hoverPointIndex = -1;
    m_impl->hoverSliceIndex = -1;
    m_impl->tooltipText.clear();
}

/// @brief 命中测试
bool TpChart::hitTestAt(const TpRect& chartRect, const TpPoint& pos, bool pieMode, int32_t& seriesIndex, int32_t& pointIndex,
                        int32_t& sliceIndex, TpString& text, TpPoint& hitPos)
{
    seriesIndex = -1;
    pointIndex = -1;
    sliceIndex = -1;
    text.clear();
    hitPos = pos;

    if (!pointInRect(chartRect, pos)) {
        return false;
    }

    double scaleX = chartRect.width() / 800.0;
    double scaleY = chartRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    if (pieMode) {
        int32_t pieCount = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* s = m_impl->seriesList[i];
            if (s && s->isVisible() && s->type() == TpSeries::TypePie) {
                pieCount++;
            }
        }

        int32_t pieIndex = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* s = m_impl->seriesList[i];
            if (!s || !s->isVisible() || s->type() != TpSeries::TypePie) {
                continue;
            }

            TpPieSeries* pieSeries = static_cast<TpPieSeries*>(s);
            TpRect pieRect = pieSeriesRect(chartRect, pieCount, pieIndex);
            int32_t rectX = pieRect.x();
            int32_t rectY = pieRect.y();
            int32_t rectW = pieRect.width();
            int32_t rectH = pieRect.height();

            int32_t padding = 12;
            if (rectW < 160 || rectH < 160) padding = 6;
            int32_t minSide = rectW < rectH ? rectW : rectH;
            int32_t diameter = minSide - padding * 2;
            if (diameter < 20) diameter = minSide;
            int32_t radius = diameter / 2;
            if (radius < 1) {
                pieIndex++;
                continue;
            }

            int32_t centerX = rectX + rectW / 2;
            int32_t centerY = rectY + rectH / 2;
            double totalValue = 0.0;
            for (int32_t k = 0; k < pieSeries->sliceCount(); ++k) {
                if (pieSeries->isSliceVisible(k) && pieSeries->sliceValue(k) > 0.0) {
                    totalValue += pieSeries->sliceValue(k);
                }
            }

            if (totalValue <= 0.0) {
                pieIndex++;
                continue;
            }

            double currentAngle = pieSeries->startAngle();
            for (int32_t k = 0; k < pieSeries->sliceCount(); ++k) {
                if (!pieSeries->isSliceVisible(k) || pieSeries->sliceValue(k) <= 0.0) {
                    continue;
                }

                double sweep = (pieSeries->sliceValue(k) * 360.0) / totalValue;
                if (sweep <= 0.0) {
                    continue;
                }

                double midAngle = currentAngle + (sweep * 0.5);
                double midRad = midAngle * 3.14159265358979323846 / 180.0;
                int32_t drawCenterX = centerX;
                int32_t drawCenterY = centerY;
                if (pieSeries->explodedIndex() == k && pieSeries->explodeDistance() > 0) {
                    drawCenterX += static_cast<int32_t>(std::cos(midRad) * pieSeries->explodeDistance() + 0.5);
                    drawCenterY += static_cast<int32_t>(std::sin(midRad) * pieSeries->explodeDistance() + 0.5);
                }

                double dx = static_cast<double>(pos.x() - drawCenterX);
                double dy = static_cast<double>(pos.y() - drawCenterY);
                double distSq = dx * dx + dy * dy;
                if (distSq > static_cast<double>(radius * radius)) {
                    currentAngle += sweep;
                    continue;
                }

                if (pieSeries->donutVisible()) {
                    int32_t innerRadius = static_cast<int32_t>(radius * pieSeries->donutRatio());
                    if (innerRadius > 0 && distSq < static_cast<double>(innerRadius * innerRadius)) {
                        currentAngle += sweep;
                        continue;
                    }
                }

                double angle = normalizeAngle(std::atan2(dy, dx) * 180.0 / 3.14159265358979323846);
                double startAngle = normalizeAngle(currentAngle);
                double endAngle = normalizeAngle(currentAngle + sweep);
                bool inSweep = false;
                if (sweep >= 360.0) {
                    inSweep = true;
                } else if (startAngle <= endAngle) {
                    inSweep = (angle >= startAngle && angle <= endAngle);
                } else {
                    inSweep = (angle >= startAngle || angle <= endAngle);
                }

                if (inSweep) {
                    seriesIndex = i;
                    sliceIndex = k;
                    text = pieSeries->sliceName(k);
                    if (!text.empty()) {
                        text += ": ";
                    }
                    text += TpString::number(pieSeries->sliceValue(k), 2);
                    text += " (";
                    double percent = (pieSeries->sliceValue(k) * 100.0) / totalValue;
                    text += TpString::number(percent, 1);
                    text += "% )";
                    hitPos = TpPoint(drawCenterX, drawCenterY);
                    return true;
                }

                currentAngle += sweep;
            }

            pieIndex++;
        }

        return false;
    }

    int32_t barSeriesCount = 0;
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (s && s->isVisible() && s->type() == TpSeries::TypeBar) {
            barSeriesCount++;
        }
    }

    int32_t barLayoutIndex = 0;
    double bestDistance = DBL_MAX;
    bool hit = false;

    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (!s || !s->isVisible()) {
            continue;
        }

        if (s->type() == TpSeries::TypeBar) {
            TpBarSeries* barSeries = static_cast<TpBarSeries*>(s);
            int32_t layoutIndex = barLayoutIndex++;
            const TpVector<TpDataPoint>& data = barSeries->data();
            int32_t rectW = chartRect.width();
            int32_t rectH = chartRect.height();
            int32_t rectX = chartRect.x();
            int32_t rectY = chartRect.y();
            int32_t yZero = m_impl->axisY->ZeroPixel(rectH, rectY, true);

            int32_t x0 = m_impl->axisX->mapToPixel(m_impl->axisX->min(), rectW, rectX, false);
            int32_t x1 = m_impl->axisX->mapToPixel(m_impl->axisX->min() + 1.0, rectW, rectX, false);
            int32_t unitPixelWidth = std::abs(x1 - x0);
            if (unitPixelWidth <= 0) unitPixelWidth = 50;
            int32_t groupWidth = static_cast<int32_t>(unitPixelWidth * 0.6);
            int32_t barWidth = groupWidth / (barSeriesCount > 0 ? barSeriesCount : 1);
            if (barWidth < 1) barWidth = 1;

            for (int32_t k = 0; k < data.size(); ++k) {
                TpDataPoint pt = data[k];
                int32_t xCenter = m_impl->axisX->mapToPixel(pt.x, rectW, rectX, false);
                int32_t barLeft = xCenter - (groupWidth / 2) + layoutIndex * barWidth;
                int32_t yVal = m_impl->axisY->mapToPixel(pt.y, rectH, rectY, true);
                int32_t top = (pt.y >= 0) ? yVal : yZero;
                int32_t height = std::abs(yVal - yZero);
                if (height == 0) height = 1;
                TpRect barRect(barLeft, top, barWidth, height);

                if (pointInRect(barRect, pos)) {
                    seriesIndex = i;
                    pointIndex = k;
                    hitPos = TpPoint(barRect.x() + barRect.width() / 2, barRect.y() + barRect.height() / 2);
                    text = s->name();
                    if (!text.empty()) {
                        text += ": ";
                    }
                    text += TpString::number(pt.y, 2);
                    return true;
                }
            }
            continue;
        }

        const TpVector<TpDataPoint>& data = s->data();
        int32_t pointRadius = (s->type() == TpSeries::TypeScatter) ? 8 : 6;
        pointRadius = (int32_t)(pointRadius * scale);
        if (pointRadius < 4) pointRadius = 4;

        for (int32_t k = 0; k < data.size(); ++k) {
            const TpDataPoint& pt = data[k];
            int32_t px = m_impl->axisX->mapToPixel(pt.x, chartRect.width(), chartRect.x(), false);
            int32_t py = m_impl->axisY->mapToPixel(pt.y, chartRect.height(), chartRect.y(), true);
            double d2 = distanceSq(pos.x(), pos.y(), px, py);
            if (d2 <= static_cast<double>(pointRadius * pointRadius) && d2 < bestDistance) {
                bestDistance = d2;
                seriesIndex = i;
                pointIndex = k;
                sliceIndex = -1;
                hitPos = TpPoint(px, py);
                text = s->name();
                if (!text.empty()) {
                    text += ": ";
                }
                text += "(";
                text += TpString::number(pt.x, 2);
                text += ", ";
                text += TpString::number(pt.y, 2);
                text += ")";
                hit = true;
            }
        }
    }

    return hit;
}
/// @brief 按当前位置缩放坐标轴
void TpChart::zoomAxisAt(const TpPoint& pos, const TpRect& chartRect, double factor)
{
    if (!m_impl || factor <= 0.0) return;

    if (m_impl->axisX->isAutoRange()) {
        m_impl->axisX->setAutoRange(false);
    }
    if (m_impl->axisY->isAutoRange()) {
        m_impl->axisY->setAutoRange(false);
    }

    double xAnchor = m_impl->axisX->mapToValue(pos.x(), chartRect.width(), chartRect.x(), false);
    double yAnchor = m_impl->axisY->mapToValue(pos.y(), chartRect.height(), chartRect.y(), true);

    double xMin = m_impl->axisX->min();
    double xMax = m_impl->axisX->max();
    double yMin = m_impl->axisY->min();
    double yMax = m_impl->axisY->max();

    double xSpan = xMax - xMin;
    double ySpan = yMax - yMin;
    if (xSpan == 0.0) xSpan = 1.0;
    if (ySpan == 0.0) ySpan = 1.0;

    double newXMin = xAnchor - (xAnchor - xMin) * factor;
    double newXMax = xAnchor + (xMax - xAnchor) * factor;
    double newYMin = yAnchor - (yAnchor - yMin) * factor;
    double newYMax = yAnchor + (yMax - yAnchor) * factor;

    double minXSpan = std::abs(xSpan) / (chartRect.width() > 0 ? chartRect.width() : 1);
    double minYSpan = std::abs(ySpan) / (chartRect.height() > 0 ? chartRect.height() : 1);
    if (minXSpan < 1e-6) minXSpan = 1e-6;
    if (minYSpan < 1e-6) minYSpan = 1e-6;

    if (newXMax - newXMin < minXSpan) {
        double midX = (newXMin + newXMax) * 0.5;
        newXMin = midX - minXSpan * 0.5;
        newXMax = midX + minXSpan * 0.5;
    }
    if (newYMax - newYMin < minYSpan) {
        double midY = (newYMin + newYMax) * 0.5;
        newYMin = midY - minYSpan * 0.5;
        newYMax = midY + minYSpan * 0.5;
    }

    if (newXMin == newXMax) {
        newXMin -= 1.0;
        newXMax += 1.0;
    }
    if (newYMin == newYMax) {
        newYMin -= 1.0;
        newYMax += 1.0;
    }

    m_impl->axisX->setRange(newXMin, newXMax);
    m_impl->axisY->setRange(newYMin, newYMax);
}

/// @brief 按当前位置平移坐标轴
void TpChart::panAxisTo(const TpPoint& pos, const TpRect& chartRect)
{
    if (!m_impl) return;

    double width = chartRect.width() > 0 ? chartRect.width() : 1;
    double height = chartRect.height() > 0 ? chartRect.height() : 1;

    double startXSpan = m_impl->dragStartXMax - m_impl->dragStartXMin;
    double startYSpan = m_impl->dragStartYMax - m_impl->dragStartYMin;
    double valuePerPixelX = startXSpan / width;
    double valuePerPixelY = startYSpan / height;

    double deltaX = (pos.x() - m_impl->dragStartPos.x()) * valuePerPixelX;
    double deltaY = (pos.y() - m_impl->dragStartPos.y()) * valuePerPixelY;

    m_impl->axisX->setAutoRange(false);
    m_impl->axisY->setAutoRange(false);
    m_impl->axisX->setRange(m_impl->dragStartXMin - deltaX, m_impl->dragStartXMax - deltaX);
    m_impl->axisY->setRange(m_impl->dragStartYMin - deltaY, m_impl->dragStartYMax - deltaY);
}

/// @brief 处理图例点击
bool TpChart::toggleLegendAt(const TpPoint& pos, const TpRect& totalRect, const TpRect& chartRect)
{
    if (!m_impl || !m_impl->legendClickable) return false;

    TpVector<const char*> names;
    TpVector<int32_t> colors;
    TpVector<int32_t> endColors;
    TpVector<int32_t> types;
    buildLegendData(totalRect, chartRect, names, colors, endColors, types);

    for (int32_t i = 0; i < m_impl->legendItems.size(); ++i) {
        const LegendItem& item = m_impl->legendItems[i];
        if (!pointInRect(item.rect, pos)) {
            continue;
        }

        TpSeries* s = (item.seriesIndex >= 0 && item.seriesIndex < m_impl->seriesList.size()) ? m_impl->seriesList[item.seriesIndex] : nullptr;
        if (!s) return false;

        if (item.sliceIndex >= 0 && s->type() == TpSeries::TypePie) {
            TpPieSeries* pie = static_cast<TpPieSeries*>(s);
            bool visible = pie->isSliceVisible(item.sliceIndex);
            pie->setSliceVisible(item.sliceIndex, !visible);
            if (m_impl->selectedSeriesIndex == item.seriesIndex && m_impl->selectedSliceIndex == item.sliceIndex && visible) {
                m_impl->selectedSeriesIndex = -1;
                m_impl->selectedPointIndex = -1;
                m_impl->selectedSliceIndex = -1;
            }
        } else {
            bool visible = s->isVisible();
            s->setVisible(!visible);
            if (m_impl->selectedSeriesIndex == item.seriesIndex) {
                m_impl->selectedSeriesIndex = -1;
                m_impl->selectedPointIndex = -1;
                m_impl->selectedSliceIndex = -1;
            }
        }

        m_impl->cssAppliedToSeries = false;
        clearHoverState();
        this->update();
        return true;
    }

    return false;
}

/// @brief 绘制提示框
void TpChart::drawTooltip(TpPainter* painter, const TpRect& chartRect)
{
    if (!painter || !m_impl || !m_impl->tooltipVisible || !m_impl->hasHover || m_impl->tooltipText.empty()) {
        return;
    }

    double scaleX = chartRect.width() / 800.0;
    double scaleY = chartRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    int32_t fontSize = (int32_t)(12 * scale);
    if (fontSize < 9) fontSize = 9;

    TpFont font;
    font.setFontSize(fontSize);
    font.setFontColor(_RGB(255, 255, 255));
    font.setText(m_impl->tooltipText);

    int32_t textW = font.pixelWidth();
    int32_t textH = font.pixelHeight();
    if (textW <= 0) textW = m_impl->tooltipText.length() * (int32_t)(fontSize / 2 + 1);
    if (textH <= 0) textH = fontSize;

    int32_t boxW = textW + (int32_t)(12 * scale);
    int32_t boxH = textH + (int32_t)(8 * scale);
    int32_t x = m_impl->hoverPos.x() + (int32_t)(12 * scale);
    int32_t y = m_impl->hoverPos.y() + (int32_t)(12 * scale);

    if (x + boxW > chartRect.right()) {
        x = m_impl->hoverPos.x() - boxW - (int32_t)(12 * scale);
    }
    if (y + boxH > chartRect.bottom()) {
        y = m_impl->hoverPos.y() - boxH - (int32_t)(12 * scale);
    }
    if (x < chartRect.x()) x = chartRect.x();
    if (y < chartRect.y()) y = chartRect.y();

    int32_t fillColor = dimColor(_RGB(20, 20, 20), 220);
    painter->setPen(TpPen(fillColor, 1));
    painter->setBrush(TpBrush(fillColor));
    painter->drawRect(x, y, boxW, boxH, 4);
    painter->drawText(font, x + (int32_t)(6 * scale), y + (int32_t)(4 * scale));
}

/// @brief 绘制十字线
void TpChart::drawCrosshair(TpPainter* painter, const TpRect& chartRect)
{
    if (!painter || !m_impl || !m_impl->crosshairVisible || !m_impl->hasHover || !pointInRect(chartRect, m_impl->hoverPos)) {
        return;
    }

    TpPen pen(dimColor(_RGB(100, 100, 100), 160), 1);
    painter->setPen(pen);
    painter->drawLine(m_impl->hoverPos.x(), chartRect.y(), m_impl->hoverPos.x(), chartRect.bottom());
    painter->drawLine(chartRect.x(), m_impl->hoverPos.y(), chartRect.right(), m_impl->hoverPos.y());
}

/// @brief 绘制悬停高亮
void TpChart::drawHoverHighlight(TpPainter* painter, const TpRect& chartRect)
{
    if (!painter || !m_impl || !m_impl->hasHover || m_impl->hoverSeriesIndex < 0 || m_impl->hoverSeriesIndex >= m_impl->seriesList.size()) {
        return;
    }

    TpSeries* s = m_impl->seriesList[m_impl->hoverSeriesIndex];
    if (!s || !s->isVisible()) {
        return;
    }

    double scaleX = chartRect.width() / 800.0;
    double scaleY = chartRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    if (s->type() == TpSeries::TypePie && m_impl->hoverSliceIndex >= 0) {
        TpPieSeries* pieSeries = static_cast<TpPieSeries*>(s);
        int32_t pieCount = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* cur = m_impl->seriesList[i];
            if (cur && cur->isVisible() && cur->type() == TpSeries::TypePie) {
                pieCount++;
            }
        }

        int32_t pieIndex = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* cur = m_impl->seriesList[i];
            if (!cur || !cur->isVisible() || cur->type() != TpSeries::TypePie) {
                continue;
            }
            if (i != m_impl->hoverSeriesIndex) {
                pieIndex++;
                continue;
            }

            TpRect pieRect = pieSeriesRect(chartRect, pieCount, pieIndex);
            int32_t rectX = pieRect.x();
            int32_t rectY = pieRect.y();
            int32_t rectW = pieRect.width();
            int32_t rectH = pieRect.height();
            int32_t padding = 12;
            if (rectW < 160 || rectH < 160) padding = 6;
            int32_t minSide = rectW < rectH ? rectW : rectH;
            int32_t diameter = minSide - padding * 2;
            if (diameter < 20) diameter = minSide;
            int32_t radius = diameter / 2;
            if (radius < 1) return;

            int32_t centerX = rectX + rectW / 2;
            int32_t centerY = rectY + rectH / 2;
            double totalValue = 0.0;
            for (int32_t k = 0; k < pieSeries->sliceCount(); ++k) {
                if (pieSeries->isSliceVisible(k) && pieSeries->sliceValue(k) > 0.0) {
                    totalValue += pieSeries->sliceValue(k);
                }
            }
            if (totalValue <= 0.0) return;

            double currentAngle = pieSeries->startAngle();
            for (int32_t k = 0; k < pieSeries->sliceCount(); ++k) {
                if (!pieSeries->isSliceVisible(k) || pieSeries->sliceValue(k) <= 0.0) continue;

                double sweep = (pieSeries->sliceValue(k) * 360.0) / totalValue;
                if (sweep <= 0.0) continue;

                if (k == m_impl->hoverSliceIndex) {
                    double midAngle = currentAngle + (sweep * 0.5);
                    double midRad = midAngle * 3.14159265358979323846 / 180.0;
                    int32_t drawCenterX = centerX;
                    int32_t drawCenterY = centerY;
                    if (pieSeries->explodedIndex() == k && pieSeries->explodeDistance() > 0) {
                        drawCenterX += static_cast<int32_t>(std::cos(midRad) * pieSeries->explodeDistance() + 0.5);
                        drawCenterY += static_cast<int32_t>(std::sin(midRad) * pieSeries->explodeDistance() + 0.5);
                    }

                    int32_t outerRadius = radius + (int32_t)(6 * scale);
                    TpHollowMask mask;
                    if (pieSeries->donutVisible()) {
                        int32_t innerRadius = static_cast<int32_t>(radius * pieSeries->donutRatio());
                        if (innerRadius > 0) {
                            mask.addCircleHollow(drawCenterX, drawCenterY, innerRadius);
                        }
                    }

                    int32_t color = pieSeries->sliceColor(k);
                    if (color == 0) color = s->color();
                    if (color == 0) color = _RGB(80, 80, 80);

                    painter->setPen(TpPen(dimColor(color, 255), 2));
                    painter->setBrush(TpBrush(dimColor(color, 85)));
                    painter->drawPie(TpPoint(drawCenterX, drawCenterY), outerRadius, (int32_t)currentAngle, (int32_t)(currentAngle + sweep), mask);
                    return;
                }

                currentAngle += sweep;
            }

            break;
        }
        return;
    }

    if (s->type() == TpSeries::TypeBar && m_impl->hoverPointIndex >= 0) {
        TpBarSeries* barSeries = static_cast<TpBarSeries*>(s);
        const TpVector<TpDataPoint>& data = barSeries->data();
        if (m_impl->hoverPointIndex >= data.size()) return;

        int32_t barSeriesCount = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* cur = m_impl->seriesList[i];
            if (cur && cur->isVisible() && cur->type() == TpSeries::TypeBar) {
                barSeriesCount++;
            }
        }

        int32_t barIndex = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* cur = m_impl->seriesList[i];
            if (!cur || !cur->isVisible()) continue;
            if (cur->type() == TpSeries::TypeBar) {
                if (i == m_impl->hoverSeriesIndex) break;
                barIndex++;
            }
        }

        TpDataPoint pt = data[m_impl->hoverPointIndex];
        int32_t rectW = chartRect.width();
        int32_t rectH = chartRect.height();
        int32_t rectX = chartRect.x();
        int32_t rectY = chartRect.y();
        int32_t yZero = m_impl->axisY->ZeroPixel(rectH, rectY, true);
        int32_t x0 = m_impl->axisX->mapToPixel(m_impl->axisX->min(), rectW, rectX, false);
        int32_t x1 = m_impl->axisX->mapToPixel(m_impl->axisX->min() + 1.0, rectW, rectX, false);
        int32_t unitPixelWidth = std::abs(x1 - x0);
        if (unitPixelWidth <= 0) unitPixelWidth = 50;
        int32_t groupWidth = static_cast<int32_t>(unitPixelWidth * 0.62);
        int32_t barWidth = groupWidth / (barSeriesCount > 0 ? barSeriesCount : 1);
        if (barWidth < 1) barWidth = 1;

        int32_t xCenter = m_impl->axisX->mapToPixel(pt.x, rectW, rectX, false);
        int32_t barLeft = xCenter - (groupWidth / 2) + barIndex * barWidth;
        int32_t yVal = m_impl->axisY->mapToPixel(pt.y, rectH, rectY, true);
        int32_t top = (pt.y >= 0) ? yVal : yZero;
        int32_t height = std::abs(yVal - yZero);
        if (height == 0) height = 1;

        TpRect outerRect(barLeft - 2, top - 2, barWidth + 4, height + 4);
        int32_t color = s->color() == 0 ? _RGB(80, 80, 80) : s->color();
        painter->setPen(TpPen(dimColor(color, 255), 2));
        painter->setBrush(TpBrush(dimColor(color, 70)));
        painter->drawRect(outerRect, 0);
        return;
    }

    if (m_impl->hoverPointIndex < 0) {
        return;
    }

    const TpVector<TpDataPoint>& data = s->data();
    if (m_impl->hoverPointIndex >= data.size()) return;

    const TpDataPoint& pt = data[m_impl->hoverPointIndex];
    int32_t px = m_impl->axisX->mapToPixel(pt.x, chartRect.width(), chartRect.x(), false);
    int32_t py = m_impl->axisY->mapToPixel(pt.y, chartRect.height(), chartRect.y(), true);
    int32_t radius = s->type() == TpSeries::TypeScatter ? 7 : 6;
    radius = (int32_t)(radius * scale);
    if (radius < 5) radius = 5;

    int32_t color = s->color() == 0 ? _RGB(80, 80, 80) : s->color();
    TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 5, dimColor(color, 150), dimColor(color, 35));
    TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 1, dimColor(color, 255), _RGB(255, 255, 255));
}

/// @brief 绘制选中效果
void TpChart::drawSelection(TpPainter* painter, const TpRect& chartRect)
{
    if (!painter || !m_impl || m_impl->selectedSeriesIndex < 0 || m_impl->selectedSeriesIndex >= m_impl->seriesList.size()) {
        return;
    }

    TpSeries* s = m_impl->seriesList[m_impl->selectedSeriesIndex];
    if (!s || !s->isVisible()) {
        return;
    }

    double scaleX = chartRect.width() / 800.0;
    double scaleY = chartRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    if (s->type() == TpSeries::TypePie && m_impl->selectedSliceIndex >= 0) {
        TpPieSeries* pieSeries = static_cast<TpPieSeries*>(s);
        int32_t pieCount = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* cur = m_impl->seriesList[i];
            if (cur && cur->isVisible() && cur->type() == TpSeries::TypePie) {
                pieCount++;
            }
        }

        int32_t pieIndex = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* cur = m_impl->seriesList[i];
            if (!cur || !cur->isVisible() || cur->type() != TpSeries::TypePie) {
                continue;
            }
            if (i != m_impl->selectedSeriesIndex) {
                pieIndex++;
                continue;
            }

            TpRect pieRect = pieSeriesRect(chartRect, pieCount, pieIndex);
            int32_t rectX = pieRect.x();
            int32_t rectY = pieRect.y();
            int32_t rectW = pieRect.width();
            int32_t rectH = pieRect.height();
            int32_t padding = 12;
            if (rectW < 160 || rectH < 160) padding = 6;
            int32_t minSide = rectW < rectH ? rectW : rectH;
            int32_t diameter = minSide - padding * 2;
            if (diameter < 20) diameter = minSide;
            int32_t radius = diameter / 2;
            if (radius < 1) return;

            int32_t centerX = rectX + rectW / 2;
            int32_t centerY = rectY + rectH / 2;
            double totalValue = 0.0;
            for (int32_t k = 0; k < pieSeries->sliceCount(); ++k) {
                if (pieSeries->isSliceVisible(k) && pieSeries->sliceValue(k) > 0.0) {
                    totalValue += pieSeries->sliceValue(k);
                }
            }
            if (totalValue <= 0.0) return;

            double currentAngle = pieSeries->startAngle();
            for (int32_t k = 0; k < pieSeries->sliceCount(); ++k) {
                if (!pieSeries->isSliceVisible(k) || pieSeries->sliceValue(k) <= 0.0) continue;

                double sweep = (pieSeries->sliceValue(k) * 360.0) / totalValue;
                if (sweep <= 0.0) continue;

                if (k == m_impl->selectedSliceIndex) {
                    double midAngle = currentAngle + (sweep * 0.5);
                    double midRad = midAngle * 3.14159265358979323846 / 180.0;
                    int32_t drawCenterX = centerX;
                    int32_t drawCenterY = centerY;
                    if (pieSeries->explodedIndex() == k && pieSeries->explodeDistance() > 0) {
                        drawCenterX += static_cast<int32_t>(std::cos(midRad) * pieSeries->explodeDistance() + 0.5);
                        drawCenterY += static_cast<int32_t>(std::sin(midRad) * pieSeries->explodeDistance() + 0.5);
                    }

                    int32_t outerRadius = radius + (int32_t)(7 * scale);
                    TpHollowMask mask;
                    if (pieSeries->donutVisible()) {
                        int32_t innerRadius = static_cast<int32_t>(radius * pieSeries->donutRatio());
                        if (innerRadius > 0) {
                            mask.addCircleHollow(drawCenterX, drawCenterY, innerRadius);
                        }
                    }

                    int32_t color = pieSeries->sliceColor(k);
                    if (color == 0) color = s->color();
                    if (color == 0) color = _RGB(80, 80, 80);

                    painter->setPen(TpPen(dimColor(color, 255), 3));
                    painter->setBrush(TpBrush(dimColor(color, 95)));
                    painter->drawPie(TpPoint(drawCenterX, drawCenterY), outerRadius, (int32_t)currentAngle, (int32_t)(currentAngle + sweep), mask);
                    return;
                }

                currentAngle += sweep;
            }

            break;
        }
    }

    if (s->type() == TpSeries::TypeBar && m_impl->selectedPointIndex >= 0) {
        TpBarSeries* barSeries = static_cast<TpBarSeries*>(s);
        const TpVector<TpDataPoint>& data = barSeries->data();
        if (m_impl->selectedPointIndex >= data.size()) return;

        int32_t barSeriesCount = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* cur = m_impl->seriesList[i];
            if (cur && cur->isVisible() && cur->type() == TpSeries::TypeBar) {
                barSeriesCount++;
            }
        }

        int32_t barIndex = 0;
        for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
            TpSeries* cur = m_impl->seriesList[i];
            if (!cur || !cur->isVisible()) continue;
            if (cur->type() == TpSeries::TypeBar) {
                if (i == m_impl->selectedSeriesIndex) break;
                barIndex++;
            }
        }

        TpDataPoint pt = data[m_impl->selectedPointIndex];
        int32_t rectW = chartRect.width();
        int32_t rectH = chartRect.height();
        int32_t rectX = chartRect.x();
        int32_t rectY = chartRect.y();
        int32_t yZero = m_impl->axisY->ZeroPixel(rectH, rectY, true);
        int32_t x0 = m_impl->axisX->mapToPixel(m_impl->axisX->min(), rectW, rectX, false);
        int32_t x1 = m_impl->axisX->mapToPixel(m_impl->axisX->min() + 1.0, rectW, rectX, false);
        int32_t unitPixelWidth = std::abs(x1 - x0);
        if (unitPixelWidth <= 0) unitPixelWidth = 50;
        int32_t groupWidth = static_cast<int32_t>(unitPixelWidth * 0.64);
        int32_t barWidth = groupWidth / (barSeriesCount > 0 ? barSeriesCount : 1);
        if (barWidth < 1) barWidth = 1;

        int32_t xCenter = m_impl->axisX->mapToPixel(pt.x, rectW, rectX, false);
        int32_t barLeft = xCenter - (groupWidth / 2) + barIndex * barWidth;
        int32_t yVal = m_impl->axisY->mapToPixel(pt.y, rectH, rectY, true);
        int32_t top = (pt.y >= 0) ? yVal : yZero;
        int32_t height = std::abs(yVal - yZero);
        if (height == 0) height = 1;

        TpRect barRect(barLeft - 2, top - 2, barWidth + 4, height + 4);
        int32_t color = s->color() == 0 ? _RGB(80, 80, 80) : s->color();
        TpPen pen(dimColor(color, 255), 3);
        painter->setPen(pen);
        painter->setBrush(TpBrush(dimColor(color, 80)));
        painter->drawRect(barRect, 0);
        return;
    }

    const TpVector<TpDataPoint>& data = s->data();
    if (m_impl->selectedPointIndex >= 0 && m_impl->selectedPointIndex < data.size()) {
        const TpDataPoint& pt = data[m_impl->selectedPointIndex];
        int32_t px = m_impl->axisX->mapToPixel(pt.x, chartRect.width(), chartRect.x(), false);
        int32_t py = m_impl->axisY->mapToPixel(pt.y, chartRect.height(), chartRect.y(), true);
        int32_t radius = s->type() == TpSeries::TypeScatter ? 7 : 6;
        radius = (int32_t)(radius * scale);
        if (radius < 5) radius = 5;

        int32_t color = s->color() == 0 ? _RGB(80, 80, 80) : s->color();
        TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 6, dimColor(color, 180), dimColor(color, 35));
        TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 2, dimColor(color, 255), _RGB(255, 255, 255));
    }
}

/// @brief 鼠标绉诲姩浜嬩欢
bool TpChart::onMouseMoveEvent(TpMouseEvent* event)
{
    if (!event) return false;

    TpPoint pos = event->pos();
    m_impl->pointerPos = pos;
    m_impl->hasPointerPos = true;
    TpRect totalRect(0, 0, this->width(), this->height());
    TpRect chartRect;
    bool showTitleAndLegend = false;
    bool showAxisLabels = false;
    bool showAxisTicks = false;
    bool pieChartMode = false;
    double scale = 1.0;
    prepareLayoutState(totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    if (m_impl->mousePressed && m_impl->panEnabled && !pieChartMode) {
        int32_t dx = std::abs(pos.x() - m_impl->dragStartPos.x());
        int32_t dy = std::abs(pos.y() - m_impl->dragStartPos.y());
        if (!m_impl->isDragging && (dx > 3 || dy > 3)) {
            m_impl->isDragging = true;
            m_impl->axisX->setAutoRange(false);
            m_impl->axisY->setAutoRange(false);
            m_impl->dragStartXMin = m_impl->axisX->min();
            m_impl->dragStartXMax = m_impl->axisX->max();
            m_impl->dragStartYMin = m_impl->axisY->min();
            m_impl->dragStartYMax = m_impl->axisY->max();
        }

        if (m_impl->isDragging) {
            panAxisTo(pos, chartRect);
            clearHoverState();
            this->update();
            return true;
        }
    }

    if (!pointInRect(chartRect, pos)) {
        clearHoverState();
        this->update();
        return true;
    }

    int32_t seriesIndex = -1;
    int32_t pointIndex = -1;
    int32_t sliceIndex = -1;
    TpString text;
    TpPoint hitPos;

    if (hitTestAt(chartRect, pos, pieChartMode, seriesIndex, pointIndex, sliceIndex, text, hitPos)) {
        m_impl->hasHover = true;
        m_impl->hoverPos = hitPos;
        m_impl->hoverSeriesIndex = seriesIndex;
        m_impl->hoverPointIndex = pointIndex;
        m_impl->hoverSliceIndex = sliceIndex;
        m_impl->tooltipText = text;
    } else if (!pieChartMode && (m_impl->tooltipVisible || m_impl->crosshairVisible)) {
        m_impl->hasHover = true;
        m_impl->hoverPos = pos;
        m_impl->hoverSeriesIndex = -1;
        m_impl->hoverPointIndex = -1;
        m_impl->hoverSliceIndex = -1;
        if (m_impl->tooltipVisible) {
            m_impl->tooltipText = "X:";
            m_impl->tooltipText += TpString::number(m_impl->axisX->mapToValue(pos.x(), chartRect.width(), chartRect.x(), false), 2);
            m_impl->tooltipText += " Y:";
            m_impl->tooltipText += TpString::number(m_impl->axisY->mapToValue(pos.y(), chartRect.height(), chartRect.y(), true), 2);
        } else {
            m_impl->tooltipText.clear();
        }
    } else {
        clearHoverState();
    }

    this->update();
    return true;
}

/// @brief 鼠标按下浜嬩欢
bool TpChart::onMousePressEvent(TpMouseEvent* event)
{
    if (!event) return false;
    if (event->button() != BUTTON_LEFT) {
        return TpWidget::onMousePressEvent(event);
    }

    TpPoint pos = event->pos();
    TpRect totalRect(0, 0, this->width(), this->height());
    TpRect chartRect;
    bool showTitleAndLegend = false;
    bool showAxisLabels = false;
    bool showAxisTicks = false;
    bool pieChartMode = false;
    double scale = 1.0;
    prepareLayoutState(totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    if (m_impl->legendClickable && showTitleAndLegend && toggleLegendAt(pos, totalRect, chartRect)) {
        TpWidget::onMousePressEvent(event);
        return true;
    }

    if (!pointInRect(chartRect, pos)) {
        return TpWidget::onMousePressEvent(event);
    }

    m_impl->mousePressed = true;
    m_impl->isDragging = false;
    m_impl->pressPos = pos;
    m_impl->dragStartPos = pos;
    m_impl->dragStartXMin = m_impl->axisX->min();
    m_impl->dragStartXMax = m_impl->axisX->max();
    m_impl->dragStartYMin = m_impl->axisY->min();
    m_impl->dragStartYMax = m_impl->axisY->max();

    TpWidget::onMousePressEvent(event);
    return true;
}

/// @brief 鼠标閲婃斁浜嬩欢
bool TpChart::onMouseRleaseEvent(TpMouseEvent* event)
{
    if (!event) return false;
    if (event->button() != BUTTON_LEFT) {
        return TpWidget::onMouseRleaseEvent(event);
    }

    TpPoint pos = event->pos();
    TpRect totalRect(0, 0, this->width(), this->height());
    TpRect chartRect;
    bool showTitleAndLegend = false;
    bool showAxisLabels = false;
    bool showAxisTicks = false;
    bool pieChartMode = false;
    double scale = 1.0;
    prepareLayoutState(totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    bool wasDragging = m_impl->isDragging;
    m_impl->mousePressed = false;
    m_impl->isDragging = false;

    if (!wasDragging && m_impl->selectionEnabled && pointInRect(chartRect, pos)) {
        int32_t seriesIndex = -1;
        int32_t pointIndex = -1;
        int32_t sliceIndex = -1;
        TpString text;
        TpPoint hitPos;
        if (hitTestAt(chartRect, pos, pieChartMode, seriesIndex, pointIndex, sliceIndex, text, hitPos)) {
            m_impl->selectedSeriesIndex = seriesIndex;
            m_impl->selectedPointIndex = pointIndex;
            m_impl->selectedSliceIndex = sliceIndex;
        } else {
            m_impl->selectedSeriesIndex = -1;
            m_impl->selectedPointIndex = -1;
            m_impl->selectedSliceIndex = -1;
        }
    }

    TpWidget::onMouseRleaseEvent(event);
    this->update();
    return true;
}

/// @brief 滚轮浜嬩欢
bool TpChart::onWheelEvent(TpWheelEvent* event)
{
    if (!event || !m_impl->wheelZoomEnabled) {
        return false;
    }

    TpRect totalRect(0, 0, this->width(), this->height());
    TpRect chartRect;
    bool showTitleAndLegend = false;
    bool showAxisLabels = false;
    bool showAxisTicks = false;
    bool pieChartMode = false;
    double scale = 1.0;
    prepareLayoutState(totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    TpPoint mousePos = m_impl->hasHover ? m_impl->hoverPos : (m_impl->hasPointerPos ? m_impl->pointerPos : TpPoint(chartRect.x() + chartRect.width() / 2, chartRect.y() + chartRect.height() / 2));
    if (!pointInRect(chartRect, mousePos)) {
        mousePos = TpPoint(chartRect.x() + chartRect.width() / 2, chartRect.y() + chartRect.height() / 2);
    }

    if (pieChartMode || !pointInRect(chartRect, mousePos)) {
        return false;
    }

    double step = static_cast<double>(event->angleDelta()) / 120.0;
    if (step > 4.0) step = 4.0;
    if (step < -4.0) step = -4.0;
    double factor = std::pow(0.94, step);
    zoomAxisAt(mousePos, chartRect, factor);
    this->update();
    return true;
}

/// @brief 离开浜嬩欢
bool TpChart::onLeaveEvent(TpLeaveEvent* event)
{
    clearHoverState();
    m_impl->hasPointerPos = false;
    m_impl->mousePressed = false;
    m_impl->isDragging = false;
    TpWidget::onLeaveEvent(event);
    this->update();
    return true;
}
