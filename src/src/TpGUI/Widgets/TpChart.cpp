/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：刘杨
 * 邮箱 (Email)：2825143438@qq.com
 * 版权所有 (Copyright)：© 2026 刘杨. All rights reserved.
 * 描述 (Description)：图表控件，支持折线图、柱状图、饼图等多种图表类型，提供数据系列管理、坐标轴配置及交互功能
 */

#include "TpChart.h"
#include "TpChart_p.h"
#include "TpBarGeometryHelper_p.h"
#include "TpRenderUtils.h"
#include "TpSeries.h"
#include "TpApp.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpWidget.h"
#include <cfloat>
#include <cmath>
#include <cstdint>

namespace {


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

static void updateAxisRange(TpChartData* chartData);
static TpRect calculateLayout(TpChartData* chartData, const TpRect& totalRect);
static void drawBackground(TpChart* chart, TpChartData* chartData, TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect);
static void drawGrid(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect);
static void drawTitle(TpChartData* chartData, TpPainter* painter, const TpRect& totalRect);
static bool isPieChartMode(TpChartData* chartData);
static void drawPieChart(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect);
static void prepareLayoutState(TpChartData* chartData, const TpRect& totalRect, TpRect& chartRect, bool& showTitleAndLegend, bool& showAxisLabels,
                               bool& showAxisTicks, bool& pieChartMode, double& scale);
static void buildLegendData(TpChartData* chartData, const TpRect& totalRect, const TpRect& chartRect, TpVector<const char*>& names,
                            TpVector<int32_t>& colors, TpVector<int32_t>& endColors, TpVector<int32_t>& types);
static void drawLegend(TpChartData* chartData, TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect);
static void clearHoverState(TpChartData* chartData);
static bool hitTestAt(TpChartData* chartData, const TpRect& chartRect, const TpPoint& pos, bool pieMode, int32_t& seriesIndex, int32_t& pointIndex,
                      int32_t& sliceIndex, TpString& text, TpPoint& hitPos);
static void zoomAxisAt(TpChartData* chartData, const TpPoint& pos, const TpRect& chartRect, double factor);
static void panAxisTo(TpChartData* chartData, const TpPoint& pos, const TpRect& chartRect);
static bool toggleLegendAt(TpChartData* chartData, const TpPoint& pos, const TpRect& totalRect, const TpRect& chartRect);
static void drawTooltip(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect);
static void drawCrosshair(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect);
static void drawHoverHighlight(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect);
static void drawSelection(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect);
static tpShared<TpCssData> currentStatusCss(TpChart* chart);
// 构造与析构
TpChart::TpChart()
{
    TpChartData *chartData = new TpChartData();
    data_ = chartData;
    chartData->backgroundColor = _RGB(255, 255, 255);
    chartData->marginTop = 60;
    chartData->marginBottom = 60;
    chartData->marginLeft = 60;
    chartData->marginRight = 20;
    chartData->gridXVisible = true;
    chartData->gridYVisible = true;
    chartData->gridColor = _RGB(230, 230, 230);

    this->setBackGroundColor(0xFFFFFFFF, true);

    chartData->axisX = new TpAxis();
    chartData->axisX->setMode(TpAxis::AxisMode::Value);

    chartData->axisY = new TpAxis();
    chartData->axisY->setMode(TpAxis::AxisMode::Value);
}

TpChart::~TpChart() {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    if (chartData) {
        chartData->destroying = true;
        removeAllSeries();
        if (chartData->axisX) delete chartData->axisX;
        if (chartData->axisY) delete chartData->axisY;
        delete chartData;
        data_ = nullptr;
    }
}

/// @brief 设置图表标题
/// @param title 标题文本
void TpChart::setTitle(const char* title) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->title = title;
    this->update();
}

/// @brief 获取 X 轴对象
TpAxis* TpChart::axisX() {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    return chartData->axisX;
}

/// @brief 获取 Y 轴对象
TpAxis* TpChart::axisY() {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    return chartData->axisY;
}

/// @brief 设置图表背景色
void TpChart::setBackgroundColor(int32_t color) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->backgroundColor = color;
    this->update();
}

/// @brief 设置坐标轴标签
void TpChart::setAxisLabels(const char* xLabel, const char* yLabel) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->labelX = xLabel;
    chartData->labelY = yLabel;
    this->update();
}

/// @brief 设置 X 轴网格线是否显示
void TpChart::setGridXVisible(bool visible) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->gridXVisible = visible;
    this->update();
}

/// @brief 设置 Y 轴网格线是否显示
void TpChart::setGridYVisible(bool visible) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->gridYVisible = visible;
    this->update();
}

/// @brief 设置网格线颜色
void TpChart::setGridColor(int32_t color) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->gridColor = color;
    this->update();
}

/// @brief 设置是否显示提示信息
void TpChart::setTooltipVisible(bool visible) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->tooltipVisible = visible;
    if (!visible) {
        chartData->hasHover = false;
        chartData->tooltipText.clear();
    }
    this->update();
}

/// @brief 设置是否显示十字线
void TpChart::setCrosshairVisible(bool visible) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->crosshairVisible = visible;
    this->update();
}

/// @brief 设置是否允许点选系列或数据点
void TpChart::setSelectionEnabled(bool enabled) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->selectionEnabled = enabled;
    if (!enabled) {
        chartData->selectedSeriesIndex = -1;
        chartData->selectedPointIndex = -1;
        chartData->selectedSliceIndex = -1;
    }
    this->update();
}

/// @brief 设置是否允许拖拽平移图表
void TpChart::setPanEnabled(bool enabled) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->panEnabled = enabled;
}

/// @brief 设置是否允许滚轮缩放图表
void TpChart::setWheelZoomEnabled(bool enabled) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->wheelZoomEnabled = enabled;
}

/// @brief 设置图例是否可点击
void TpChart::setLegendClickable(bool enabled) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->legendClickable = enabled;
    this->update();
}

/// @brief 恢复默认视图范围
void TpChart::resetView() {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    chartData->axisX->setAutoRange(true);
    chartData->axisY->setAutoRange(true);
    clearHoverState(chartData);
    chartData->hasPointerPos = false;
    chartData->mousePressed = false;
    chartData->isDragging = false;
    chartData->selectedSeriesIndex = -1;
    chartData->selectedPointIndex = -1;
    chartData->selectedSliceIndex = -1;
    this->update();
}

/// @brief 获取当前选中的系列索引
int32_t TpChart::selectedSeriesIndex() const {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    return chartData ? chartData->selectedSeriesIndex : -1;
}

/// @brief 获取当前选中的数据点索引（仅折线图和柱状图适用）
int32_t TpChart::selectedPointIndex() const {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    return chartData ? chartData->selectedPointIndex : -1;
}

/// @brief 获取当前选中的扇区索引（仅饼图适用）
int32_t TpChart::selectedSliceIndex() const {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    return chartData ? chartData->selectedSliceIndex : -1;
}

/// @brief 添加一个数据系列，图表会接管该系列的生命周期
void TpChart::addSeries(TpSeries* series) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    if (series) {
        chartData->seriesList.push_back(series);
        this->update();
    }
}

/// @brief 移除所有数据系列
void TpChart::removeAllSeries() {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
        delete chartData->seriesList[i];
    }
    chartData->seriesList.clear();
    chartData->legendItems.clear();
    clearHoverState(chartData);
    chartData->hasPointerPos = false;
    chartData->mousePressed = false;
    chartData->isDragging = false;
    chartData->selectedSeriesIndex = -1;
    chartData->selectedPointIndex = -1;
    chartData->selectedSliceIndex = -1;
    if (!chartData->destroying) {
        this->update();
    }
}


/// @brief 获取当前图表状态对应的 CSS 数据
static tpShared<TpCssData> currentStatusCss(TpChart* chart)
{
    if (!chart) {
        return nullptr;
    }

    TpCssParser::MouseStatus status = TpCssParser::Enabled;
    if (!chart->enabled()) {
        status = TpCssParser::Disabled;
    } else if (chart->checkable() && chart->checked()) {
        status = TpCssParser::Checked;
    }

    return chart->readCss(chart->pluginType(), status);
}

/// @brief 根据数据系列自动计算坐标轴范围，适用于数值轴
static void updateAxisRange(TpChartData* chartData) {
    if (!chartData || !chartData->axisX || !chartData->axisY) {
        return;
    }

    bool autoRangeX = chartData->axisX->isAutoRange();
    bool autoRangeY = chartData->axisY->isAutoRange();
    bool hasVisibleSeries = false;
    bool allPieSeries = true;

    if (!autoRangeX && !autoRangeY) {
        return;
    }

    if (chartData->seriesList.empty()) {
        if (autoRangeX) chartData->axisX->setRange(0, 10);
        if (autoRangeY) chartData->axisY->setRange(0, 10);
        return;
    }

    double minX = DBL_MAX, maxX = -DBL_MAX;
    double minY = DBL_MAX, maxY = -DBL_MAX;
    bool hasData = false;

    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
        TpSeries* s = chartData->seriesList[i];
        if (!s || !s->isVisible()) continue;

        hasVisibleSeries = true;
        if (s->type() != TpSeries::TypePie) {
            allPieSeries = false;
        }

        if (s->type() == TpSeries::TypePie) {
            continue;
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
        if (autoRangeX) chartData->axisX->setRange(0, 10);
        if (autoRangeY) chartData->axisY->setRange(0, 10);
        return;
    }

    if (autoRangeX) {
        double spanX = maxX - minX;
        if (chartData->axisX->isRollingMode()) {
            double dataSpan = maxX - minX;
            if (std::isfinite(dataSpan) && dataSpan > 0.0) {
                spanX = dataSpan;
            } else {
                double rollingSpan = chartData->axisX->max() - chartData->axisX->min();
                if (std::isfinite(rollingSpan) && rollingSpan > 0.0) {
                    spanX = rollingSpan;
                }
            }
            if (!std::isfinite(spanX) || spanX <= 0.0) {
                spanX = 1.0;
            }
            chartData->axisX->setRange(maxX - spanX, maxX);
        } else {
            if (!std::isfinite(spanX) || spanX <= 0.0) spanX = 1.0;
            double leftPad = chartData->axisX->xLeftPaddingRatio() * spanX;
            double rightPad = chartData->axisX->xRightPaddingRatio() * spanX;
            chartData->axisX->setRange(minX - leftPad, maxX + rightPad);
        }
    }

    if (autoRangeY) {
        double spanY = maxY - minY;
        if (spanY <= 0) spanY = 1.0;

        double topPad = chartData->axisY->yTopPaddingRatio() * spanY;
        double bottomPad = chartData->axisY->yBottomPaddingRatio() * spanY;

        double newMin = minY - bottomPad;
        double newMax = maxY + topPad;

        // 如果数据全为正数且 minY>0，则强制 newMin=0-bottomPad，使得柱状图底部有足够空间显示
        if (minY > 0) {
            newMin = 0 - bottomPad;
            // 如果数据全为负数且 maxY<0，则强制 newMax=0+topPad，使得柱状图顶部有足够空间显示
        } else if (maxY < 0) {
            newMax = 0 + topPad;
            // 如果数据跨越正负且包含0，则强制 newMin<=0<=newMax，使得柱状图能够正确显示
        }
        // 如果数据跨越正负但不包含0，则保持原计算的 newMin 和 newMax，以确保数据点都在坐标轴范围内

        chartData->axisY->setRange(newMin, newMax);
    }
}

// 绘制事件处理函数
bool TpChart::onPaintEvent(TpPaintEvent* event) {
    TpChartData *chartData = static_cast<TpChartData *>(data_);
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

    prepareLayoutState(chartData, totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    if (!pieChartMode) {
        updateAxisRange(chartData);

        int32_t tickDensityX = showAxisTicks ? (int32_t)(80 * scale) : 120;
        int32_t tickDensityY = showAxisTicks ? (int32_t)(50 * scale) : 80;
        int32_t targetTicksX = chartRect.width() / (tickDensityX > 0 ? tickDensityX : 1);
        int32_t targetTicksY = chartRect.height() / (tickDensityY > 0 ? tickDensityY : 1);

        chartData->axisX->updateNiceTicks(targetTicksX > 2 ? targetTicksX : 2);
        chartData->axisY->updateNiceTicks(targetTicksY > 2 ? targetTicksY : 2);
    }

    drawBackground(this, chartData, painter, totalRect, chartRect);
    if (!pieChartMode) {
        drawGrid(chartData, painter, chartRect);
    }

    TpCssParser::MouseStatus seriesStatus = this->enabled() ? TpCssParser::Enabled : TpCssParser::Disabled;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* s = chartData->seriesList[i];
            if (s && s->isVisible()) {
                if (s->type() == TpSeries::TypeLine) {
                    s->applyCssData("TpLineSeries", seriesStatus);
                } else if (s->type() == TpSeries::TypeBar) {
                    s->applyCssData("TpBarSeries", seriesStatus);
                } else if (s->type() == TpSeries::TypeScatter) {
                    s->applyCssData("TpScatterSeries", seriesStatus);
                } else if (s->type() == TpSeries::TypePie) {
                    s->applyCssData("TpPieSeries", seriesStatus);
                }
            }
        }

    if (pieChartMode) {
        drawPieChart(chartData, painter, chartRect);
    } else {
        int32_t barSeriesCount = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* s = chartData->seriesList[i];
            if (s && s->isVisible() && s->type() == TpSeries::TypeBar) {
                barSeriesCount++;
            }
        }

        int32_t barIndex = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* s = chartData->seriesList[i];
            if (s && s->isVisible()) {
                if (s->type() == TpSeries::TypePie) {
                    continue;
                }
                if (s->type() == TpSeries::TypeBar) {
                    static_cast<TpBarSeries*>(s)->setLayoutInfo(barIndex++, barSeriesCount);
                }
                s->draw(painter, *chartData->axisX, *chartData->axisY, chartRect);
            }
        }

        if (showAxisTicks) {
            uint32_t black = _RGB(0, 0, 0);
            TpRenderUtils::drawAxisX(painter, chartRect, *chartData->axisX, *chartData->axisY, black, false, 0);
            TpRenderUtils::drawAxisY(painter, chartRect, *chartData->axisY, *chartData->axisX, black, false, 0);
        }
    }

    if (showAxisLabels && !pieChartMode) {
        int32_t labelFontSize = (int32_t)(20 * scale);
        if (labelFontSize < 9) labelFontSize = 9;

        TpFont labelFont;
        labelFont.setFontSize(labelFontSize);
        labelFont.setFontColor(_RGB(30, 30, 30));

        if (chartData->labelX.length() > 0) {
            labelFont.setText(chartData->labelX);
            int32_t textW = labelFont.pixelWidth() > 0 ? labelFont.pixelWidth() : chartData->labelX.length() * (int32_t)(8 * scale);
            int32_t lx = chartRect.x() + (chartRect.width() / 2) - (textW / 2);
            int32_t ly = chartRect.bottom() + (int32_t)(40 * scale);
            painter->drawText(labelFont, lx, ly);
        }

        if (chartData->labelY.length() > 0) {
            TpFont yLabelFont;
            yLabelFont.setFontSize(labelFontSize);
            yLabelFont.setFontColor(_RGB(30, 30, 30));
            yLabelFont.setVerticalText(true);
            yLabelFont.setText(chartData->labelY);

            int32_t textHeight = (int32_t)(chartData->labelY.length() * labelFontSize);
            int32_t lx = totalRect.x() + (int32_t)(5 * scale);
            int32_t ly = chartRect.y() + (chartRect.height() / 2) - (textHeight / 2);

            painter->drawText(yLabelFont, lx, ly);
        }
    }

    if (showTitleAndLegend) {
        drawTitle(chartData, painter, totalRect);
        drawLegend(chartData, painter, totalRect, chartRect);
    }

    if (chartData->hasHover) {
        drawHoverHighlight(chartData, painter, chartRect);
    }

    if (chartData->selectionEnabled) {
        drawSelection(chartData, painter, chartRect);
    }

    if (chartData->crosshairVisible && chartData->hasHover && !pieChartMode) {
        drawCrosshair(chartData, painter, chartRect);
    }

    if (chartData->tooltipVisible && chartData->hasHover) {
        drawTooltip(chartData, painter, chartRect);
    }

    return true;
}

// 辅助函数实现
/// @brief 根据当前数据和总绘制区域计算实际的图表绘制区域，考虑标题、坐标轴标签和刻度等占用的空间
static TpRect calculateLayout(TpChartData* chartData, const TpRect& totalRect) {
    int32_t top = chartData->marginTop;
    if (chartData->title.empty()) top -= 20;

    int32_t chartW = totalRect.width() - chartData->marginLeft - chartData->marginRight;
    int32_t chartH = totalRect.height() - top - chartData->marginBottom;

    if (chartW < 10) chartW = 10;
    if (chartH < 10) chartH = 10;

    return TpRect(
        totalRect.x() + chartData->marginLeft,
        totalRect.y() + top,
        chartW,
        chartH
    );
}

static void drawBackground(TpChart* chart, TpChartData* chartData, TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect) {
    tpShared<TpCssData> curCssData = currentStatusCss(chart);
    int32_t bgColor = chartData ? chartData->backgroundColor : _RGB(255, 255, 255);
    bool useGradient = false;

    if (curCssData) {
        bgColor = curCssData->backgroundColor();
        useGradient = curCssData->backgroundColorIsGradient();
    }

    TpRenderUtils::fillGradientRect(painter, totalRect, _RGB(248, 248, 248), _RGB(248, 248, 248));

    if (useGradient && curCssData) {
        TpBrush brush(curCssData->backgroundColorGradiant());
        painter->setBrush(brush);
        painter->drawRect(chartRect.x(), chartRect.y(), chartRect.width(), chartRect.height(), 0);
    } else {
            painter->setPen(bgColor);
        painter->setBrush(TpBrush(bgColor));
        painter->drawRect(chartRect.x(), chartRect.y(), chartRect.width(), chartRect.height(), 0);
        painter->setBrush(TpBrush(Tp::NoBrush));
    }
}

/// @brief 绘制网格线，基于坐标轴的刻度位置进行绘制，支持 X 轴和 Y 轴独立控制显示与否
static void drawGrid(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect) {
    if (!painter || !chartData || !chartData->axisX || !chartData->axisY) {
        return;
    }

    TpPen gridPen(chartData->gridColor, 1);

    if (chartData->gridXVisible) {
        const TpVector<double>& xTicks = chartData->axisX->getTickValues();
        painter->setPen(gridPen);
        for (int32_t i = 0; i < xTicks.size(); ++i) {
            int32_t x = chartData->axisX->mapToPixel(xTicks[i], chartRect.width(), chartRect.x(), false);
            if (x >= chartRect.x() && x <= chartRect.right()) {
                painter->drawLine(x, chartRect.y(), x, chartRect.bottom());
            }
        }
    }

    if (chartData->gridYVisible) {
        const TpVector<double>& yTicks = chartData->axisY->getTickValues();
        painter->setPen(gridPen);
        for (int32_t i = 0; i < yTicks.size(); ++i) {
            int32_t y = chartData->axisY->mapToPixel(yTicks[i], chartRect.height(), chartRect.y(), true);
            if (y >= chartRect.y() && y <= chartRect.bottom()) {
                painter->drawLine(chartRect.x(), y, chartRect.right(), y);
            }
        }
    }

    // 4. 如果坐标轴范围跨越0点，则绘制0轴线以增强视觉效果
    double yMin = chartData->axisY->min();
    double yMax = chartData->axisY->max();
    if (yMin < 0 && yMax > 0) {
        int32_t yZero = chartData->axisY->mapToPixel(0.0, chartRect.height(), chartRect.y(), true);
        if (yZero >= chartRect.y() && yZero <= chartRect.bottom()) {
            TpPen zeroPen(_RGB(150, 150, 150), 1);
            painter->setPen(zeroPen);
            painter->drawLine(chartRect.x(), yZero, chartRect.right(), yZero);
        }
    }
}

/// @brief 绘制图表标题，自动根据总绘制区域大小调整字体大小和位置，确保标题清晰可见且不与其他元素重叠
static void drawTitle(TpChartData* chartData, TpPainter* painter, const TpRect& totalRect) {
    if (chartData->title.empty()) return;

    double scaleX = totalRect.width() / 800.0;
    double scaleY = totalRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    int32_t titleFontSize = (int32_t)(18 * scale);
    if (titleFontSize < 10) titleFontSize = 10;

    TpFont font;
    font.setFontSize(titleFontSize);
    font.setFontColor(_RGB(0, 0, 0));
    font.setBold(true);
    font.setText(chartData->title);

    int32_t textW = font.pixelWidth();
    if (textW <= 0) textW = chartData->title.length() * (int32_t)(12 * scale);

    int32_t x = totalRect.x() + (totalRect.width() - textW) / 2;
    int32_t y = totalRect.y() + (int32_t)(15 * scale);

    painter->drawText(font, x, y);
}

static bool isPieChartMode(TpChartData* chartData) {
    bool hasVisibleSeries = false;

    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
        TpSeries* s = chartData->seriesList[i];
        if (!s || !s->isVisible()) continue;
        hasVisibleSeries = true;
        if (s->type() != TpSeries::TypePie) {
            return false;
        }
    }

    return hasVisibleSeries;
}

static void drawPieChart(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect) {
    if (!painter || !chartData) return;

    int32_t pieCount = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
        TpSeries* s = chartData->seriesList[i];
        if (s && s->isVisible() && s->type() == TpSeries::TypePie) {
            pieCount++;
        }
    }

    if (pieCount == 0) return;

    int32_t slotTop = chartRect.y();
    int32_t slotHeight = chartRect.height() / pieCount;
    if (slotHeight <= 0) slotHeight = chartRect.height();

    int32_t pieIndex = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
        TpSeries* s = chartData->seriesList[i];
        if (!s || !s->isVisible() || s->type() != TpSeries::TypePie) continue;

        int32_t currentTop = chartRect.y() + pieIndex * slotHeight;
        int32_t currentHeight = (pieIndex == pieCount - 1) ? (chartRect.bottom() - currentTop + 1) : slotHeight;
        if (currentHeight < 20) currentHeight = 20;

        TpRect pieRect(chartRect.x(), currentTop, chartRect.width(), currentHeight);
        static_cast<TpPieSeries*>(s)->draw(painter, *chartData->axisX, *chartData->axisY, pieRect);
        pieIndex++;
    }
}

/// @brief 根据总绘制区域大小和当前数据状态计算图表布局相关的状态变量，包括实际的图表绘制区域、是否显示标题和图例、是否显示坐标轴标签和刻度、是否进入饼图模式以及整体缩放比例等，以便后续绘制函数能够根据这些状态进行适配性的绘制
static void prepareLayoutState(TpChartData* chartData, const TpRect& totalRect, TpRect& chartRect, bool& showTitleAndLegend,
                               bool& showAxisLabels, bool& showAxisTicks, bool& pieChartMode, double& scale)
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
    pieChartMode = isPieChartMode(chartData);

    if (pieChartMode) {
        chartData->marginTop = chartData->title.empty() || !showTitleAndLegend ? (int32_t)(35 * scale) : (int32_t)(70 * scale);
        chartData->marginBottom = (int32_t)(25 * scale);
        chartData->marginLeft = (int32_t)(20 * scale);
        chartData->marginRight = (int32_t)(20 * scale);
    } else if (!showAxisTicks) {
        chartData->marginTop = 5;
        chartData->marginBottom = 5;
        chartData->marginLeft = 5;
        chartData->marginRight = 5;
    } else if (!showAxisLabels) {
        chartData->marginTop = (int32_t)(20 * scale);
        chartData->marginBottom = (int32_t)(25 * scale);
        chartData->marginLeft = (int32_t)(35 * scale);
        chartData->marginRight = (int32_t)(15 * scale);
    } else {
        chartData->marginTop = (chartData->title.empty() || !showTitleAndLegend) ? (int32_t)(40 * scale) : (int32_t)(80 * scale);
        chartData->marginBottom = chartData->labelX.empty() ? (int32_t)(40 * scale) : (int32_t)(60 * scale);
        chartData->marginLeft = chartData->labelY.empty() ? (int32_t)(50 * scale) : (int32_t)(80 * scale);
        chartData->marginRight = (int32_t)(30 * scale);
    }

    chartRect = calculateLayout(chartData, totalRect);
}

/// @brief 根据当前数据系列构造图例项的数据，包括图例文本、颜色、类型和位置等信息，以便后续绘制图例时能够正确显示每个系列对应的图例项，并且在饼图模式下能够显示每个扇区对应的图例项
static void buildLegendData(TpChartData* chartData, const TpRect& totalRect, const TpRect& chartRect, TpVector<const char*>& names,
                            TpVector<int32_t>& colors, TpVector<int32_t>& endColors, TpVector<int32_t>& types)
{
    chartData->legendItems.clear();
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

    bool pieMode = isPieChartMode(chartData);
    TpVector<int32_t> itemWidths;
    TpVector<int32_t> itemHeights;

    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
        TpSeries* s = chartData->seriesList[i];
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
                chartData->legendItems.push_back(item);

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
        chartData->legendItems.push_back(item);

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
    for (int32_t i = 0; i < chartData->legendItems.size(); ++i) {
        int32_t itemWidth = (i < itemWidths.size()) ? itemWidths[i] : totalItemBaseW;
        int32_t itemHeight = (i < itemHeights.size()) ? itemHeights[i] : legendSize;
        chartData->legendItems[i].rect = TpRect(currentX, startY, itemWidth, itemHeight + 4);
        currentX += itemWidth + itemSpacing;
    }

    for (int32_t i = 0; i < chartData->legendItems.size(); ++i) {
        names.push_back(chartData->legendItems[i].text.c_str());
        colors.push_back(chartData->legendItems[i].colorStart);
        endColors.push_back(chartData->legendItems[i].colorEnd);
        types.push_back(chartData->legendItems[i].type);
    }
}

/// @brief 绘制图例，基于之前构建的图例数据进行绘制，支持不同类型的图例项（线条、柱状、散点和饼图），并且根据当前系列的可见状态调整图例项的颜色显示，同时自动适配不同总绘制区域大小下的图例布局和字体大小，确保图例清晰可见且不与其他元素重叠
static void drawLegend(TpChartData* chartData, TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect)
{
    if (!painter || !chartData) return;

    TpVector<const char*> names;
    TpVector<int32_t> colors;
    TpVector<int32_t> endColors;
    TpVector<int32_t> types;
    buildLegendData(chartData, totalRect, chartRect, names, colors, endColors, types);

    if (chartData->legendItems.size() == 0) {
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

    for (int32_t i = 0; i < chartData->legendItems.size(); ++i) {
        const LegendItem& item = chartData->legendItems[i];
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

/// @brief 清除当前的悬浮状态，包括悬浮的系列索引、数据点索引、扇区索引和提示文本等，以便在鼠标移动到其他位置或者没有命中任何数据点时能够正确更新悬浮状态并隐藏相关的高亮和提示信息
static void clearHoverState(TpChartData* chartData)
{
    chartData->hasHover = false;
    chartData->hoverSeriesIndex = -1;
    chartData->hoverPointIndex = -1;
    chartData->hoverSliceIndex = -1;
    chartData->tooltipText.clear();
}

/// @brief 根据鼠标点击位置进行命中测试，判断是否命中某个数据点或者饼图扇区，并且返回对应的系列索引、数据点索引、扇区索引和提示文本等信息，以便在鼠标悬浮或者点击时能够正确显示相关的高亮和提示信息
static bool hitTestAt(TpChartData* chartData, const TpRect& chartRect, const TpPoint& pos, bool pieMode, int32_t& seriesIndex,
                      int32_t& pointIndex, int32_t& sliceIndex, TpString& text, TpPoint& hitPos)
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
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* s = chartData->seriesList[i];
            if (s && s->isVisible() && s->type() == TpSeries::TypePie) {
                pieCount++;
            }
        }

        int32_t pieIndex = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* s = chartData->seriesList[i];
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
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
        TpSeries* s = chartData->seriesList[i];
        if (s && s->isVisible() && s->type() == TpSeries::TypeBar) {
            barSeriesCount++;
        }
    }

    int32_t barLayoutIndex = 0;
    double bestDistance = DBL_MAX;
    bool hit = false;

    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
        TpSeries* s = chartData->seriesList[i];
        if (!s || !s->isVisible()) {
            continue;
        }

        if (s->type() == TpSeries::TypeBar) {
            TpBarSeries* barSeries = static_cast<TpBarSeries*>(s);
            int32_t layoutIndex = barLayoutIndex++;
            const TpVector<TpDataPoint>& data = barSeries->data();
            for (int32_t k = 0; k < data.size(); ++k) {
                TpDataPoint pt = data[k];
                TpRect barRect = tpBuildBarRect(*chartData->axisX, *chartData->axisY, chartRect, pt.x, pt.y, layoutIndex, barSeriesCount);

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
            int32_t px = chartData->axisX->mapToPixel(pt.x, chartRect.width(), chartRect.x(), false);
            int32_t py = chartData->axisY->mapToPixel(pt.y, chartRect.height(), chartRect.y(), true);
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

/// @brief 根据鼠标点击位置进行缩放，计算新的坐标轴范围以实现以鼠标位置为中心的缩放效果，并且确保新的坐标轴范围不会过小或者过大，以保持图表的可读性和适当的缩放级别，同时在缩放过程中自动禁用坐标轴的自动范围功能，以便用户能够手动调整坐标轴范围
static void zoomAxisAt(TpChartData* chartData, const TpPoint& pos, const TpRect& chartRect, double factor)
{
    if (!chartData || factor <= 0.0) return;

    if (chartData->axisX->isAutoRange()) {
        chartData->axisX->setAutoRange(false);
    }
    if (chartData->axisY->isAutoRange()) {
        chartData->axisY->setAutoRange(false);
    }

    double xAnchor = chartData->axisX->mapToValue(pos.x(), chartRect.width(), chartRect.x(), false);
    double yAnchor = chartData->axisY->mapToValue(pos.y(), chartRect.height(), chartRect.y(), true);

    double xMin = chartData->axisX->min();
    double xMax = chartData->axisX->max();
    double yMin = chartData->axisY->min();
    double yMax = chartData->axisY->max();

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

    chartData->axisX->setRange(newXMin, newXMax);
    chartData->axisY->setRange(newYMin, newYMax);
}

/// @brief 根据鼠标拖动位置进行平移，计算新的坐标轴范围以实现图表的平移效果，并且确保新的坐标轴范围不会过小或者过大，以保持图表的可读性和适当的缩放级别，同时在平移过程中自动禁用坐标轴的自动范围功能，以便用户能够手动调整坐标轴范围
static void panAxisTo(TpChartData* chartData, const TpPoint& pos, const TpRect& chartRect)
{
    if (!chartData) return;

    double width = chartRect.width() > 0 ? chartRect.width() : 1;
    double height = chartRect.height() > 0 ? chartRect.height() : 1;

    double startXSpan = chartData->dragStartXMax - chartData->dragStartXMin;
    double startYSpan = chartData->dragStartYMax - chartData->dragStartYMin;
    double valuePerPixelX = startXSpan / width;
    double valuePerPixelY = startYSpan / height;

    double deltaX = (pos.x() - chartData->dragStartPos.x()) * valuePerPixelX;
    double deltaY = (pos.y() - chartData->dragStartPos.y()) * valuePerPixelY;

    chartData->axisX->setAutoRange(false);
    chartData->axisY->setAutoRange(false);
    chartData->axisX->setRange(chartData->dragStartXMin - deltaX, chartData->dragStartXMax - deltaX);
    chartData->axisY->setRange(chartData->dragStartYMin - deltaY, chartData->dragStartYMax - deltaY);
}

/// @brief 根据鼠标点击位置进行图例项的切换，判断是否点击在某个图例项上，并且根据对应的系列索引和扇区索引来切换该系列或者扇区的可见状态，同时在切换后清除当前的悬浮状态以更新相关的高亮和提示信息，并且确保在切换过程中能够正确处理饼图模式下的扇区切换以及其他类型系列的整体切换
static bool toggleLegendAt(TpChartData* chartData, const TpPoint& pos, const TpRect& totalRect, const TpRect& chartRect)
{
    if (!chartData || !chartData->legendClickable) return false;

    TpVector<const char*> names;
    TpVector<int32_t> colors;
    TpVector<int32_t> endColors;
    TpVector<int32_t> types;
    buildLegendData(chartData, totalRect, chartRect, names, colors, endColors, types);

    for (int32_t i = 0; i < chartData->legendItems.size(); ++i) {
        const LegendItem& item = chartData->legendItems[i];
        if (!pointInRect(item.rect, pos)) {
            continue;
        }

        TpSeries* s = (item.seriesIndex >= 0 && item.seriesIndex < chartData->seriesList.size()) ? chartData->seriesList[item.seriesIndex] : nullptr;
        if (!s) return false;

        if (item.sliceIndex >= 0 && s->type() == TpSeries::TypePie) {
            TpPieSeries* pie = static_cast<TpPieSeries*>(s);
            bool visible = pie->isSliceVisible(item.sliceIndex);
            pie->setSliceVisible(item.sliceIndex, !visible);
            if (chartData->selectedSeriesIndex == item.seriesIndex && chartData->selectedSliceIndex == item.sliceIndex && visible) {
                chartData->selectedSeriesIndex = -1;
                chartData->selectedPointIndex = -1;
                chartData->selectedSliceIndex = -1;
            }
        } else {
            bool visible = s->isVisible();
            s->setVisible(!visible);
            if (chartData->selectedSeriesIndex == item.seriesIndex) {
                chartData->selectedSeriesIndex = -1;
                chartData->selectedPointIndex = -1;
                chartData->selectedSliceIndex = -1;
            }
        }

        clearHoverState(chartData);
        return true;
    }

    return false;
}

/// @brief 绘制提示框，根据当前的悬浮状态和提示文本来绘制一个包含提示信息的矩形框，并且根据当前总绘制区域的大小自动调整提示框的字体大小和布局，以确保提示信息清晰可见且不与其他元素重叠，同时在绘制过程中考虑到提示框的位置和边界，以避免提示框被裁剪或者超出可见范围
static void drawTooltip(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect)
{
    if (!painter || !chartData || !chartData->tooltipVisible || !chartData->hasHover || chartData->tooltipText.empty()) {
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
    font.setText(chartData->tooltipText);

    int32_t textW = font.pixelWidth();
    int32_t textH = font.pixelHeight();
    if (textW <= 0) textW = chartData->tooltipText.length() * (int32_t)(fontSize / 2 + 1);
    if (textH <= 0) textH = fontSize;

    int32_t boxW = textW + (int32_t)(12 * scale);
    int32_t boxH = textH + (int32_t)(8 * scale);
    int32_t x = chartData->hoverPos.x() + (int32_t)(12 * scale);
    int32_t y = chartData->hoverPos.y() + (int32_t)(12 * scale);

    if (x + boxW > chartRect.right()) {
        x = chartData->hoverPos.x() - boxW - (int32_t)(12 * scale);
    }
    if (y + boxH > chartRect.bottom()) {
        y = chartData->hoverPos.y() - boxH - (int32_t)(12 * scale);
    }
    if (x < chartRect.x()) x = chartRect.x();
    if (y < chartRect.y()) y = chartRect.y();

    int32_t fillColor = dimColor(_RGB(20, 20, 20), 220);
    painter->setPen(TpPen(fillColor, 1));
    painter->setBrush(TpBrush(fillColor));
    painter->drawRect(x, y, boxW, boxH, 4);
    painter->drawText(font, x + (int32_t)(6 * scale), y + (int32_t)(4 * scale));
}

/// @brief 绘制十字准线，根据当前的悬浮位置来绘制水平和垂直的辅助线，以帮助用户更准确地对齐和读取数据点的位置，同时在绘制过程中考虑到十字准线的颜色和透明度，以确保十字准线清晰可见但不会过于突兀或者干扰其他元素的显示
static void drawCrosshair(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect)
{
    if (!painter || !chartData || !chartData->crosshairVisible || !chartData->hasHover || !pointInRect(chartRect, chartData->hoverPos)) {
        return;
    }

    TpPen pen(dimColor(_RGB(100, 100, 100), 160), 1);
    painter->setPen(pen);
    painter->drawLine(chartData->hoverPos.x(), chartRect.y(), chartData->hoverPos.x(), chartRect.bottom());
    painter->drawLine(chartRect.x(), chartData->hoverPos.y(), chartRect.right(), chartData->hoverPos.y());
}

/// @brief 绘制悬浮高亮，根据当前的悬浮系列索引和数据点索引来绘制一个高亮的效果，以突出显示当前悬浮的数据点或者饼图扇区，同时在绘制过程中考虑到高亮的颜色和透明度，以确保高亮效果清晰可见但不会过于突兀或者干扰其他元素的显示，并且根据当前总绘制区域的大小自动调整高亮的尺寸和布局，以适应不同大小的图表
static void drawHoverHighlight(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect)
{
    if (!painter || !chartData || !chartData->hasHover || chartData->hoverSeriesIndex < 0 || chartData->hoverSeriesIndex >= chartData->seriesList.size()) {
        return;
    }

    TpSeries* s = chartData->seriesList[chartData->hoverSeriesIndex];
    if (!s || !s->isVisible()) {
        return;
    }

    double scaleX = chartRect.width() / 800.0;
    double scaleY = chartRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    if (s->type() == TpSeries::TypePie && chartData->hoverSliceIndex >= 0) {
        TpPieSeries* pieSeries = static_cast<TpPieSeries*>(s);
        int32_t pieCount = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* cur = chartData->seriesList[i];
            if (cur && cur->isVisible() && cur->type() == TpSeries::TypePie) {
                pieCount++;
            }
        }

        int32_t pieIndex = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* cur = chartData->seriesList[i];
            if (!cur || !cur->isVisible() || cur->type() != TpSeries::TypePie) {
                continue;
            }
            if (i != chartData->hoverSeriesIndex) {
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

                if (k == chartData->hoverSliceIndex) {
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

    if (s->type() == TpSeries::TypeBar && chartData->hoverPointIndex >= 0) {
        TpBarSeries* barSeries = static_cast<TpBarSeries*>(s);
        const TpVector<TpDataPoint>& data = barSeries->data();
        if (chartData->hoverPointIndex >= data.size()) return;

        int32_t barSeriesCount = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* cur = chartData->seriesList[i];
            if (cur && cur->isVisible() && cur->type() == TpSeries::TypeBar) {
                barSeriesCount++;
            }
        }

        int32_t barIndex = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* cur = chartData->seriesList[i];
            if (!cur || !cur->isVisible()) continue;
            if (cur->type() == TpSeries::TypeBar) {
                if (i == chartData->hoverSeriesIndex) break;
                barIndex++;
            }
        }

        TpDataPoint pt = data[chartData->hoverPointIndex];
        TpRect barRect = tpBuildBarRect(*chartData->axisX, *chartData->axisY, chartRect, pt.x, pt.y, barIndex, barSeriesCount);
        TpRect outerRect(barRect.x() - 2, barRect.y() - 2, barRect.width() + 4, barRect.height() + 4);
        int32_t color = s->color() == 0 ? _RGB(80, 80, 80) : s->color();
        painter->setPen(TpPen(dimColor(color, 255), 2));
        painter->setBrush(TpBrush(dimColor(color, 70)));
        painter->drawRect(outerRect, 0);
        return;
    }

    if (chartData->hoverPointIndex < 0) {
        return;
    }

    const TpVector<TpDataPoint>& data = s->data();
    if (chartData->hoverPointIndex >= data.size()) return;

    const TpDataPoint& pt = data[chartData->hoverPointIndex];
    int32_t px = chartData->axisX->mapToPixel(pt.x, chartRect.width(), chartRect.x(), false);
    int32_t py = chartData->axisY->mapToPixel(pt.y, chartRect.height(), chartRect.y(), true);
    int32_t radius = s->type() == TpSeries::TypeScatter ? 7 : 6;
    radius = (int32_t)(radius * scale);
    if (radius < 5) radius = 5;

    int32_t color = s->color() == 0 ? _RGB(80, 80, 80) : s->color();
    TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 5, dimColor(color, 150), dimColor(color, 35));
    TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 1, dimColor(color, 255), _RGB(255, 255, 255));
}

/// @brief 绘制选中高亮，根据当前的选中系列索引和数据点索引来绘制一个更明显的高亮效果，以突出显示当前选中的数据点或者饼图扇区，同时在绘制过程中考虑到高亮的颜色和透明度，以确保选中高亮效果清晰可见但不会过于突兀或者干扰其他元素的显示，并且根据当前总绘制区域的大小自动调整高亮的尺寸和布局，以适应不同大小的图表
static void drawSelection(TpChartData* chartData, TpPainter* painter, const TpRect& chartRect)
{
    if (!painter || !chartData || chartData->selectedSeriesIndex < 0 || chartData->selectedSeriesIndex >= chartData->seriesList.size()) {
        return;
    }

    TpSeries* s = chartData->seriesList[chartData->selectedSeriesIndex];
    if (!s || !s->isVisible()) {
        return;
    }

    double scaleX = chartRect.width() / 800.0;
    double scaleY = chartRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    if (s->type() == TpSeries::TypePie && chartData->selectedSliceIndex >= 0) {
        TpPieSeries* pieSeries = static_cast<TpPieSeries*>(s);
        int32_t pieCount = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* cur = chartData->seriesList[i];
            if (cur && cur->isVisible() && cur->type() == TpSeries::TypePie) {
                pieCount++;
            }
        }

        int32_t pieIndex = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* cur = chartData->seriesList[i];
            if (!cur || !cur->isVisible() || cur->type() != TpSeries::TypePie) {
                continue;
            }
            if (i != chartData->selectedSeriesIndex) {
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

                if (k == chartData->selectedSliceIndex) {
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

    if (s->type() == TpSeries::TypeBar && chartData->selectedPointIndex >= 0) {
        TpBarSeries* barSeries = static_cast<TpBarSeries*>(s);
        const TpVector<TpDataPoint>& data = barSeries->data();
        if (chartData->selectedPointIndex >= data.size()) return;

        int32_t barSeriesCount = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* cur = chartData->seriesList[i];
            if (cur && cur->isVisible() && cur->type() == TpSeries::TypeBar) {
                barSeriesCount++;
            }
        }

        int32_t barIndex = 0;
    for (int32_t i = 0; i < chartData->seriesList.size(); ++i) {
            TpSeries* cur = chartData->seriesList[i];
            if (!cur || !cur->isVisible()) continue;
            if (cur->type() == TpSeries::TypeBar) {
                if (i == chartData->selectedSeriesIndex) break;
                barIndex++;
            }
        }

        TpDataPoint pt = data[chartData->selectedPointIndex];
        TpRect barRect = tpBuildBarRect(*chartData->axisX, *chartData->axisY, chartRect, pt.x, pt.y, barIndex, barSeriesCount);
        TpRect outerRect(barRect.x() - 2, barRect.y() - 2, barRect.width() + 4, barRect.height() + 4);
        int32_t color = s->color() == 0 ? _RGB(80, 80, 80) : s->color();
        TpPen pen(dimColor(color, 255), 3);
        painter->setPen(pen);
        painter->setBrush(TpBrush(dimColor(color, 80)));
        painter->drawRect(outerRect, 0);
        return;
    }

    const TpVector<TpDataPoint>& data = s->data();
    if (chartData->selectedPointIndex >= 0 && chartData->selectedPointIndex < data.size()) {
        const TpDataPoint& pt = data[chartData->selectedPointIndex];
        int32_t px = chartData->axisX->mapToPixel(pt.x, chartRect.width(), chartRect.x(), false);
        int32_t py = chartData->axisY->mapToPixel(pt.y, chartRect.height(), chartRect.y(), true);
        int32_t radius = s->type() == TpSeries::TypeScatter ? 7 : 6;
        radius = (int32_t)(radius * scale);
        if (radius < 5) radius = 5;

        int32_t color = s->color() == 0 ? _RGB(80, 80, 80) : s->color();
        TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 6, dimColor(color, 180), dimColor(color, 35));
        TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 2, dimColor(color, 255), _RGB(255, 255, 255));
    }
}

/// @brief 处理鼠标移动事件，根据当前鼠标位置来更新悬浮状态和提示信息，并且在鼠标拖动时进行图表的平移操作，同时在鼠标移动过程中自动切换图例项的可见状态以实现交互式的图例功能，并且确保在鼠标移动过程中能够正确处理不同类型系列的悬浮和提示信息的显示，以提供更丰富和直观的用户体验
bool TpChart::onMouseMoveEvent(TpMouseEvent* event)
{
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    if (!event) return false;

    TpPoint pos = event->pos();
    chartData->pointerPos = pos;
    chartData->hasPointerPos = true;
    TpRect totalRect(0, 0, this->width(), this->height());
    TpRect chartRect;
    bool showTitleAndLegend = false;
    bool showAxisLabels = false;
    bool showAxisTicks = false;
    bool pieChartMode = false;
    double scale = 1.0;
    prepareLayoutState(chartData, totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    if (chartData->mousePressed && chartData->panEnabled && !pieChartMode) {
        int32_t dx = std::abs(pos.x() - chartData->dragStartPos.x());
        int32_t dy = std::abs(pos.y() - chartData->dragStartPos.y());
        if (!chartData->isDragging && (dx > 3 || dy > 3)) {
            chartData->isDragging = true;
            chartData->axisX->setAutoRange(false);
            chartData->axisY->setAutoRange(false);
            chartData->dragStartXMin = chartData->axisX->min();
            chartData->dragStartXMax = chartData->axisX->max();
            chartData->dragStartYMin = chartData->axisY->min();
            chartData->dragStartYMax = chartData->axisY->max();
        }

        if (chartData->isDragging) {
            panAxisTo(chartData, pos, chartRect);
            clearHoverState(chartData);
            this->update();
            return true;
        }
    }

    if (!pointInRect(chartRect, pos)) {
        clearHoverState(chartData);
        this->update();
        return true;
    }

    int32_t seriesIndex = -1;
    int32_t pointIndex = -1;
    int32_t sliceIndex = -1;
    TpString text;
    TpPoint hitPos;

    if (hitTestAt(chartData, chartRect, pos, pieChartMode, seriesIndex, pointIndex, sliceIndex, text, hitPos)) {
        chartData->hasHover = true;
        chartData->hoverPos = hitPos;
        chartData->hoverSeriesIndex = seriesIndex;
        chartData->hoverPointIndex = pointIndex;
        chartData->hoverSliceIndex = sliceIndex;
        chartData->tooltipText = text;
    } else if (!pieChartMode && (chartData->tooltipVisible || chartData->crosshairVisible)) {
        chartData->hasHover = true;
        chartData->hoverPos = pos;
        chartData->hoverSeriesIndex = -1;
        chartData->hoverPointIndex = -1;
        chartData->hoverSliceIndex = -1;
        if (chartData->tooltipVisible) {
            chartData->tooltipText = "X:";
            chartData->tooltipText += TpString::number(chartData->axisX->mapToValue(pos.x(), chartRect.width(), chartRect.x(), false), 2);
            chartData->tooltipText += " Y:";
            chartData->tooltipText += TpString::number(chartData->axisY->mapToValue(pos.y(), chartRect.height(), chartRect.y(), true), 2);
        } else {
            chartData->tooltipText.clear();
        }
    } else {
        clearHoverState(chartData);
    }

    this->update();
    return true;
}

/// @brief 处理鼠标按下事件，根据当前鼠标位置来更新按下状态和拖动状态，并且在鼠标按下时进行图表的平移准备工作，同时确保在鼠标按下过程中能够正确处理不同类型系列的悬浮和提示信息的显示，以提供更丰富和直观的用户体验
bool TpChart::onMousePressEvent(TpMouseEvent* event)
{
    TpChartData *chartData = static_cast<TpChartData *>(data_);
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
    prepareLayoutState(chartData, totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    if (chartData->legendClickable && showTitleAndLegend && toggleLegendAt(chartData, pos, totalRect, chartRect)) {
        TpWidget::onMousePressEvent(event);
        return true;
    }

    if (!pointInRect(chartRect, pos)) {
        return TpWidget::onMousePressEvent(event);
    }

    chartData->mousePressed = true;
    chartData->isDragging = false;
    chartData->pressPos = pos;
    chartData->dragStartPos = pos;
    chartData->dragStartXMin = chartData->axisX->min();
    chartData->dragStartXMax = chartData->axisX->max();
    chartData->dragStartYMin = chartData->axisY->min();
    chartData->dragStartYMax = chartData->axisY->max();

    TpWidget::onMousePressEvent(event);
    return true;
}

/// @brief 处理鼠标释放事件，根据当前鼠标位置和拖动状态来更新按下状态和选中状态，并且在鼠标释放时进行图表的平移结束工作，同时确保在鼠标释放过程中能够正确处理不同类型系列的悬浮和提示信息的显示，以提供更丰富和直观的用户体验
bool TpChart::onMouseRleaseEvent(TpMouseEvent* event)
{
    TpChartData *chartData = static_cast<TpChartData *>(data_);
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
    prepareLayoutState(chartData, totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    bool wasDragging = chartData->isDragging;
    chartData->mousePressed = false;
    chartData->isDragging = false;

    if (!wasDragging && chartData->selectionEnabled && pointInRect(chartRect, pos)) {
        int32_t seriesIndex = -1;
        int32_t pointIndex = -1;
        int32_t sliceIndex = -1;
        TpString text;
        TpPoint hitPos;
        if (hitTestAt(chartData, chartRect, pos, pieChartMode, seriesIndex, pointIndex, sliceIndex, text, hitPos)) {
            chartData->selectedSeriesIndex = seriesIndex;
            chartData->selectedPointIndex = pointIndex;
            chartData->selectedSliceIndex = sliceIndex;
        } else {
            chartData->selectedSeriesIndex = -1;
            chartData->selectedPointIndex = -1;
            chartData->selectedSliceIndex = -1;
        }
    }

    TpWidget::onMouseRleaseEvent(event);
    this->update();
    return true;
}

/// @brief 处理鼠标滚轮事件，根据当前鼠标位置和滚轮方向来更新图表的缩放状态，并且在鼠标滚轮时进行图表的缩放操作，同时确保在鼠标滚轮过程中能够正确处理不同类型系列的悬浮和提示信息的显示，以提供更丰富和直观的用户体验
bool TpChart::onWheelEvent(TpWheelEvent* event)
{
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    if (!event || !chartData->wheelZoomEnabled) {
        return false;
    }

    TpRect totalRect(0, 0, this->width(), this->height());
    TpRect chartRect;
    bool showTitleAndLegend = false;
    bool showAxisLabels = false;
    bool showAxisTicks = false;
    bool pieChartMode = false;
    double scale = 1.0;
    prepareLayoutState(chartData, totalRect, chartRect, showTitleAndLegend, showAxisLabels, showAxisTicks, pieChartMode, scale);

    TpPoint mousePos = chartData->hasHover ? chartData->hoverPos : (chartData->hasPointerPos ? chartData->pointerPos : TpPoint(chartRect.x() + chartRect.width() / 2, chartRect.y() + chartRect.height() / 2));
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
    zoomAxisAt(chartData, mousePos, chartRect, factor);
    this->update();
    return true;
}

/// @brief 处理鼠标离开事件，当鼠标离开图表区域时，清除悬浮状态和提示信息，并且重置相关的状态变量以确保图表能够正确地响应后续的鼠标事件，同时在鼠标离开过程中确保图表的显示状态能够正确更新，以提供更丰富和直观的用户体验
bool TpChart::onLeaveEvent(TpLeaveEvent* event)
{
    TpChartData *chartData = static_cast<TpChartData *>(data_);
    clearHoverState(chartData);
    chartData->hasPointerPos = false;
    chartData->mousePressed = false;
    chartData->isDragging = false;
    TpWidget::onLeaveEvent(event);
    this->update();
    return true;
}
