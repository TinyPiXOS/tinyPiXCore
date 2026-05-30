/*
 * 鐗堟潈澹版槑 (Copyright Declaration)
 * 浣滆€?(Author)锛氬垬鏉?
 * 閭 (Email)锛?825143438@qq.com
 * 鐗堟潈鎵€鏈?(Copyright)锛毬?2026 鍒樻潹. All rights reserved.
 * 鎻忚堪 (Description)锛氬浘琛ㄧ被瀹炵幇 (TpChart) - 鍝嶅簲寮忕缉鏀句笌鏋侀檺灏哄淇鐗?
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
#include <cstdint> // 寮曞叆瀹氬鏁存暟绫诲瀷

// 瀹忓畾涔夐槻姝㈤鑹叉姤閿?
#ifndef _RGB
#define _RGB(r, g, b) ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#endif

// 绉佹湁瀹炵幇缁撴瀯浣撳畾涔?
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
    TpString title;                  // 鍥捐〃鏍囬
    int32_t backgroundColor;         // 鑳屾櫙棰滆壊

    TpAxis* axisX;                   // X 杞?
    TpAxis* axisY;                   // Y 杞?

    TpString labelX;                 // X 杞存爣绛?
    TpString labelY;                 // Y 杞存爣绛?

    TpVector<TpSeries*> seriesList;  // 鏁版嵁绯诲垪鍒楄〃

    int32_t marginTop;               // 涓婅竟璺?
    int32_t marginBottom;            // 涓嬭竟璺?
    int32_t marginLeft;              // 宸﹁竟璺?
    int32_t marginRight;             // 鍙宠竟璺?

    bool gridXVisible;               // X杞寸綉鏍肩嚎鍙鎬?
    bool gridYVisible;               // Y杞寸綉鏍肩嚎鍙鎬?
    int32_t gridColor;               // 缃戞牸绾垮熀纭€棰滆壊
    bool cssAppliedToSeries = false; // CSS鏍峰紡鏄惁宸插簲鐢ㄥ埌series
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

    int32_t selectedSeriesIndex = -1;
    int32_t selectedPointIndex = -1;
    int32_t selectedSliceIndex = -1;

    TpVector<LegendItem> legendItems;
};


// 鏋勯€犱笌鏋愭瀯
TpChart::TpChart()
{
    m_impl = new Impl();
    m_impl->backgroundColor = _RGB(255, 255, 255);
    m_impl->marginTop = 60;    // 鐣欑粰鏍囬
    m_impl->marginBottom = 60; // 鐣欑粰X杞村埢搴?
    m_impl->marginLeft = 60;   // 鐣欑粰Y杞村埢搴?
    m_impl->marginRight = 20;  // 闃叉婧㈠嚭
    m_impl->gridXVisible = true;
    m_impl->gridYVisible = true;
    m_impl->gridColor = _RGB(230, 230, 230); // 榛樿娴呯伆鑹茬綉鏍?

    // 鏄惧紡璁剧疆鑳屾櫙棰滆壊
    this->setBackGroundColor(0xFFFFFFFF, true);

    // 鍒濆鍖朇SS鏁版嵁
    enabledCssData = nullptr;
    disabledCssData = nullptr;
    hoverCssData = nullptr;
    pressCssData = nullptr;
    checkedCssData = nullptr;

    // 鍒锋柊鍩虹CSS鏍峰紡
    refreshBaseCss();

    // 鑷姩鍒涘缓榛樿鍧愭爣杞?
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


// 鍏叡鎺ュ彛
/// 璁剧疆鍥捐〃鏍囬
void TpChart::setTitle(const char* title) {
    m_impl->title = title;
    this->update(); 
}

/// 鑾峰彇 X 杞村紩鐢?
TpAxis* TpChart::axisX() {
    return m_impl->axisX;
}

/// 鑾峰彇 Y 杞村紩鐢?
TpAxis* TpChart::axisY() {
    return m_impl->axisY;
}

/// 璁剧疆鍥捐〃鑳屾櫙棰滆壊
void TpChart::setBackgroundColor(int32_t color) {
    m_impl->backgroundColor = color;
    this->update();
}

/// 璁剧疆鍧愭爣杞存爣绛?
void TpChart::setAxisLabels(const char* xLabel, const char* yLabel) {
    m_impl->labelX = xLabel;
    m_impl->labelY = yLabel;
    this->update();
}

/// 璁剧疆 X 杞寸綉鏍肩嚎鍙鎬?
void TpChart::setGridXVisible(bool visible) {
    m_impl->gridXVisible = visible;
    this->update();
}

/// 璁剧疆 Y 杞寸綉鏍肩嚎鍙鎬?
void TpChart::setGridYVisible(bool visible) {
    m_impl->gridYVisible = visible;
    this->update();
}

/// 璁剧疆缃戞牸绾块鑹?
void TpChart::setGridColor(int32_t color) {
    m_impl->gridColor = color;
    this->update();
}

/// 璁剧疆鏄惁鏄剧ず鎻愮ず妗?
void TpChart::setTooltipVisible(bool visible) {
    m_impl->tooltipVisible = visible;
    if (!visible) {
        m_impl->hasHover = false;
        m_impl->tooltipText.clear();
    }
    this->update();
}

/// 璁剧疆鏄惁鏄剧ず鍗佸瓧绾?
void TpChart::setCrosshairVisible(bool visible) {
    m_impl->crosshairVisible = visible;
    this->update();
}

/// 璁剧疆鏄惁鍚敤閫変腑
void TpChart::setSelectionEnabled(bool enabled) {
    m_impl->selectionEnabled = enabled;
    if (!enabled) {
        m_impl->selectedSeriesIndex = -1;
        m_impl->selectedPointIndex = -1;
        m_impl->selectedSliceIndex = -1;
    }
    this->update();
}

/// 璁剧疆鏄惁鍏佽鎷栧姩骞崇Щ
void TpChart::setPanEnabled(bool enabled) {
    m_impl->panEnabled = enabled;
}

/// 璁剧疆鏄惁鍏佽婊氳疆缂╂斁
void TpChart::setWheelZoomEnabled(bool enabled) {
    m_impl->wheelZoomEnabled = enabled;
}

/// 璁剧疆鍥句緥鏄惁鍙偣鍑?
void TpChart::setLegendClickable(bool enabled) {
    m_impl->legendClickable = enabled;
    this->update();
}

/// 鎭㈠榛樿瑙嗗浘
void TpChart::resetView() {
    m_impl->axisX->setAutoRange(true);
    m_impl->axisY->setAutoRange(true);
    clearHoverState();
    m_impl->mousePressed = false;
    m_impl->isDragging = false;
    m_impl->selectedSeriesIndex = -1;
    m_impl->selectedPointIndex = -1;
    m_impl->selectedSliceIndex = -1;
    this->update();
}

/// 鑾峰彇褰撳墠閫変腑绯诲垪绱㈠紩
int32_t TpChart::selectedSeriesIndex() const {
    return m_impl ? m_impl->selectedSeriesIndex : -1;
}

/// 鑾峰彇褰撳墠閫変腑鏁版嵁鐐圭储寮?
int32_t TpChart::selectedPointIndex() const {
    return m_impl ? m_impl->selectedPointIndex : -1;
}

/// 鑾峰彇褰撳墠閫変腑鍒囩墖绱㈠紩
int32_t TpChart::selectedSliceIndex() const {
    return m_impl ? m_impl->selectedSliceIndex : -1;
}

/// 娣诲姞鏁版嵁绯诲垪
void TpChart::addSeries(TpSeries* series) {
    if (series) {
        m_impl->seriesList.push_back(series);
        m_impl->cssAppliedToSeries = false;
        this->update();
    }
}

/// 绉婚櫎鎵€鏈夋暟鎹郴鍒?
void TpChart::removeAllSeries() {
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        delete m_impl->seriesList[i];
    }
    m_impl->seriesList.clear();
    m_impl->legendItems.clear();
    clearHoverState();
    m_impl->mousePressed = false;
    m_impl->isDragging = false;
    m_impl->selectedSeriesIndex = -1;
    m_impl->selectedPointIndex = -1;
    m_impl->selectedSliceIndex = -1;
    this->update();
}

/// 璁剧疆鏍峰紡琛紙CSS锛?
void TpChart::setStyleSheet(const TpString& styleSheet) {
    TpApp::Inst()->cssParser()->parseCss(styleSheet);
    refreshBaseCss();
    m_impl->cssAppliedToSeries = false;
}

/// 鑾峰彇褰撳墠鏍峰紡琛紙CSS锛?
TpString TpChart::styleSheet() {
    return TpApp::Inst()->cssParser()->cssStr();
}

/// 鑾峰彇褰撳墠鐘舵€佺殑CSS鏁版嵁
tpShared<TpCssData> TpChart::currentStatusCss() {
    // TpChart 涓昏浣跨敤鍚敤鐘舵€佺殑CSS
    // 濡傛灉闇€瑕佹偓鍋?鎸変笅鏁堟灉锛屽彲浠ュ湪澶栭儴閫氳繃 setChecked() 绛夋柟寮忔帶鍒?
    tpShared<TpCssData> curCssData = enabledCssData;
    if (!enabled()) {
        curCssData = disabledCssData;
    } else if (checkable() && checked()) {
        curCssData = checkedCssData;
    }
    return curCssData;
}

/// 鍒锋柊鍩虹CSS鏍峰紡
void TpChart::refreshBaseCss() {
    enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    disabledCssData = readCss(pluginType(), TpCssParser::Disabled);
    hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    checkedCssData = readCss(pluginType(), TpCssParser::Checked);
}

// 鍐呴儴閫昏緫锛氳嚜鍔ㄨ绠楄寖鍥?
/// 鏍规嵁鎵€鏈?Series 鏁版嵁鑷姩璋冩暣鍧愭爣杞磋寖鍥?
void TpChart::updateAxisRange() {
    bool autoRangeX = m_impl->axisX->isAutoRange();
    bool autoRangeY = m_impl->axisY->isAutoRange();
    bool hasVisibleSeries = false;
    bool allPieSeries = true;

    if (!autoRangeX && !autoRangeY) {
        return;
    }

    if (m_impl->seriesList.empty()) {
        // 濡傛灉娌℃湁鏁版嵁锛屼笖鏄嚜鍔ㄨ寖鍥达紝鎵嶈缃粯璁ゅ€?
        if (autoRangeX) m_impl->axisX->setRange(0, 10);
        if (autoRangeY) m_impl->axisY->setRange(0, 10);
        return;
    }

    double minX = DBL_MAX, maxX = -DBL_MAX;
    double minY = DBL_MAX, maxY = -DBL_MAX;
    bool hasData = false;
    
    // 鏍囪鏄惁鏈夋煴鐘跺浘
    bool hasBarSeries = false; 

    // 閬嶅巻鎵€鏈?Series 鎵惧嚭鏈€澶ф渶灏忓€?
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

    // 鍒ゆ柇 X 杞存槸鍚﹀紑鍚嚜鍔ㄨ寖鍥?
    if (autoRangeX) {
        if (hasBarSeries) {
           double spanX = maxX - minX;
            if (spanX <= 0) spanX = 1.0;
            double leftPad = m_impl->axisX->xLeftPaddingRatio() * spanX;
            double rightPad = m_impl->axisX->xRightPaddingRatio() * spanX;
            m_impl->axisX->setRange(minX - leftPad, maxX + rightPad);
        } else {
            // 鎶樼嚎鍥撅細浣跨敤宸﹀彸鐣欑櫧姣斾緥锛堥粯璁ゅ潎涓?锛屽垯璐磋竟锛?
            double spanX = maxX - minX;
            if (spanX <= 0) spanX = 1.0;
            double leftPad = m_impl->axisX->xLeftPaddingRatio() * spanX;
            double rightPad = m_impl->axisX->xRightPaddingRatio() * spanX;
            m_impl->axisX->setRange(minX - leftPad, maxX + rightPad);
        }
    }

    // 鍒ゆ柇 Y 杞存槸鍚﹀紑鍚嚜鍔ㄨ寖鍥?
    if (autoRangeY) {
        double spanY = maxY - minY;
        if (spanY <= 0) spanY = 1.0;
        
        double topPad = m_impl->axisY->yTopPaddingRatio() * spanY;
        double bottomPad = m_impl->axisY->yBottomPaddingRatio() * spanY;
        
        double newMin = minY - bottomPad;
        double newMax = maxY + topPad;
        
        // 寮哄埗鍖呭惈闆剁偣锛屼絾淇濈暀鐣欑櫧鎵╁睍
        if (minY > 0) {
            // 鏁版嵁鍏ㄤ负姝ｏ細鏈€灏忓€艰涓?0锛屽苟鍚戜笅鎵╁睍搴曢儴鐣欑櫧
            newMin = 0 - bottomPad;
            // 鏈€澶у€间粛涓?maxY + topPad锛堜絾 maxY>0锛屾墍浠ラ《閮ㄧ暀鐧借嚜鐒剁敓鏁堬級
        } else if (maxY < 0) {
            // 鏁版嵁鍏ㄤ负璐燂細鏈€澶у€艰涓?0锛屽苟鍚戜笂鎵╁睍椤堕儴鐣欑櫧
            newMax = 0 + topPad;
            // 鏈€灏忓€间粛涓?minY - bottomPad锛堣嚜鐒剁敓鏁堬級
        }
        // 璺ㄩ浂鐐圭殑鎯呭喌锛氱暀鐧藉凡缁忓湪 newMin/newMax 涓綋鐜帮紝鏃犻渶棰濆澶勭悊
        
        m_impl->axisY->setRange(newMin, newMax);
    }
}

// 鏍稿績缁樺埗閫昏緫
/// 鏈€涓婚缁樺埗浜嬩欢
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

// 鍐呴儴绉佹湁瀹炵幇
/// 璁＄畻甯冨眬鐭╁舰
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

/// 缁樺埗鑳屾櫙
void TpChart::drawBackground(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect) {
    // 浣跨敤CSS鑳屾櫙鑹诧紙濡傛灉璁剧疆浜嗭級
    tpShared<TpCssData> curCssData = currentStatusCss();
    int32_t bgColor = curCssData->backgroundColor();

    TpRenderUtils::fillGradientRect(painter, totalRect, _RGB(248, 248, 248), _RGB(248, 248, 248));

    // 浣跨敤CSS鑳屾櫙鑹叉垨榛樿鑳屾櫙鑹?
    if (curCssData->backgroundColorIsGradient()) {
        TpBrush brush(curCssData->backgroundColorGradiant());
        painter->setBrush(brush);
        painter->drawRect(chartRect.x(), chartRect.y(), chartRect.width(), chartRect.height(), 0);
    } else {
        // 浣跨敤 drawRect 濉厖鐭╁舰锛堣缃鑹插拰瀹炲績鐢诲埛锛?
        painter->setPen(bgColor);
        painter->setBrush(TpBrush(bgColor));
        painter->drawRect(chartRect.x(), chartRect.y(), chartRect.width(), chartRect.height(), 0);
        painter->setBrush(TpBrush(Tp::NoBrush));
    }
}

/// 缁樺埗缃戞牸
void TpChart::drawGrid(TpPainter* painter, const TpRect& chartRect) {
    // 1. 鍩虹瀹夊叏妫€鏌?
    if (!painter || !m_impl || !m_impl->axisX || !m_impl->axisY) {
        return; 
    }

    TpPen gridPen(m_impl->gridColor, 1);

    // 2. 缁樺埗鍨傜洿缃戞牸绾?
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

    // 3. 缁樺埗姘村钩缃戞牸绾?
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

    // 4. 闆跺埢搴︾嚎锛圶杞翠富绾匡級寮哄寲缁樺埗
    // 澧炲姞閫昏緫淇濇姢锛岄槻姝?min/max 寮傚父瀵艰嚧鐨勯棶棰?
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

/// 缁樺埗鏍囬
void TpChart::drawTitle(TpPainter* painter, const TpRect& totalRect) {
    if (m_impl->title.empty()) return;
    
    // 鍦ㄨ繖閲岄噸鏂拌绠椾竴娆?scale锛屼繚鎸佹帴鍙ｄ笉琚薄鏌?
    double scaleX = totalRect.width() / 800.0;
    double scaleY = totalRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY; 
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    // 鍔ㄦ€佽绠楁爣棰樺瓧浣撳ぇ灏?(鍩哄噯 18px)
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

/// 缁樺埗鍥句緥
void TpChart::drawLegend(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect) {
    TpVector<const char*> names;
    TpVector<int32_t> colors;
    TpVector<int32_t> endColors;
    TpVector<int32_t> types;
    bool pieMode = isPieChartMode();

    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (!s || !s->isVisible()) continue;

        if (s->type() == TpSeries::TypePie) {
            if (!pieMode) continue;
            TpPieSeries* pieSeries = static_cast<TpPieSeries*>(s);
            for (int32_t k = 0; k < pieSeries->sliceCount(); ++k) {
                const TpString& sliceName = pieSeries->sliceName(k);
                if (sliceName.empty()) continue;
                names.push_back(sliceName.c_str());
                colors.push_back(pieSeries->sliceColor(k));
                endColors.push_back(pieSeries->sliceColor(k));
                types.push_back(TpRenderUtils::TypePie);
            }
            continue;
        }

        if (s->type() == TpSeries::TypePie) {
            continue;
        }

        if (s->name().empty()) continue;

        names.push_back(s->name().c_str());
        colors.push_back(s->color());
        
        if (s->type() == TpSeries::TypeBar) {
            // 瀹夊叏寮鸿浆
            endColors.push_back(static_cast<TpBarSeries*>(s)->colorEnd());
            types.push_back(TpRenderUtils::TypeBar);
        } else if (s->type() == TpSeries::TypeScatter) {
            endColors.push_back(s->color());
            types.push_back(TpRenderUtils::TypeScatter);
        } else {
            endColors.push_back(s->color());
            types.push_back(TpRenderUtils::TypeLine);
        }
    }

    if (names.size() > 0) {
        TpRenderUtils::drawLegendOutside(painter, totalRect, chartRect, names, colors, endColors, types);
    }
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

/// @brief 璁＄畻褰撳墠甯冨眬鐘舵€?
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

/// @brief 鏋勫缓鍥句緥鏁版嵁
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

/// @brief 缁樺埗鍥句緥
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

/// @brief 娓呯悊鎮仠鐘舵€?
void TpChart::clearHoverState()
{
    m_impl->hasHover = false;
    m_impl->hoverSeriesIndex = -1;
    m_impl->hoverPointIndex = -1;
    m_impl->hoverSliceIndex = -1;
    m_impl->tooltipText.clear();
}

/// @brief 鍛戒腑娴嬭瘯
/// @brief 鍛戒腑娴嬭瘯
bool TpChart::hitTestAt(const TpRect& chartRect, const TpPoint& pos, bool pieMode, int32_t& seriesIndex, int32_t& pointIndex,
                        int32_t& sliceIndex, TpString& text, TpPoint& hitPos)
{
    seriesIndex = -1;
    pointIndex = -1;
    sliceIndex = -1;
    text.clear();
    hitPos = pos;

    if (!chartRect.contains(pos)) {
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

                if (barRect.contains(pos)) {
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
/// @brief 鎸夊綋鍓嶄綅缃缉鏀惧潗鏍囪酱
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

    double newXMin = xAnchor - (xAnchor - xMin) * factor;
    double newXMax = xAnchor + (xMax - xAnchor) * factor;
    double newYMin = yAnchor - (yAnchor - yMin) * factor;
    double newYMax = yAnchor + (yMax - yAnchor) * factor;

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

/// @brief 鎸夊綋鍓嶄綅缃钩绉诲潗鏍囪酱
void TpChart::panAxisTo(const TpPoint& pos, const TpRect& chartRect)
{
    if (!m_impl) return;

    double startX = m_impl->axisX->mapToValue(m_impl->dragStartPos.x(), chartRect.width(), chartRect.x(), false);
    double currentX = m_impl->axisX->mapToValue(pos.x(), chartRect.width(), chartRect.x(), false);
    double deltaX = currentX - startX;

    double startY = m_impl->axisY->mapToValue(m_impl->dragStartPos.y(), chartRect.height(), chartRect.y(), true);
    double currentY = m_impl->axisY->mapToValue(pos.y(), chartRect.height(), chartRect.y(), true);
    double deltaY = currentY - startY;

    m_impl->axisX->setAutoRange(false);
    m_impl->axisY->setAutoRange(false);
    m_impl->axisX->setRange(m_impl->dragStartXMin - deltaX, m_impl->dragStartXMax - deltaX);
    m_impl->axisY->setRange(m_impl->dragStartYMin - deltaY, m_impl->dragStartYMax - deltaY);
}

/// @brief 澶勭悊鍥句緥鐐瑰嚮
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
        if (!item.rect.contains(pos)) {
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

/// @brief 缁樺埗鎻愮ず妗?
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

/// @brief 缁樺埗鍗佸瓧绾?
void TpChart::drawCrosshair(TpPainter* painter, const TpRect& chartRect)
{
    if (!painter || !m_impl || !m_impl->crosshairVisible || !m_impl->hasHover || !chartRect.contains(m_impl->hoverPos)) {
        return;
    }

    TpPen pen(dimColor(_RGB(100, 100, 100), 160), 1);
    painter->setPen(pen);
    painter->drawLine(m_impl->hoverPos.x(), chartRect.y(), m_impl->hoverPos.x(), chartRect.bottom());
    painter->drawLine(chartRect.x(), m_impl->hoverPos.y(), chartRect.right(), m_impl->hoverPos.y());
}

/// @brief 缁樺埗閫変腑鏁堟灉
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

                    int32_t outerRadius = radius + (int32_t)(4 * scale);
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
                    painter->setBrush(TpBrush(dimColor(color, 110)));
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
        int32_t groupWidth = static_cast<int32_t>(unitPixelWidth * 0.6);
        int32_t barWidth = groupWidth / (barSeriesCount > 0 ? barSeriesCount : 1);
        if (barWidth < 1) barWidth = 1;

        int32_t xCenter = m_impl->axisX->mapToPixel(pt.x, rectW, rectX, false);
        int32_t barLeft = xCenter - (groupWidth / 2) + barIndex * barWidth;
        int32_t yVal = m_impl->axisY->mapToPixel(pt.y, rectH, rectY, true);
        int32_t top = (pt.y >= 0) ? yVal : yZero;
        int32_t height = std::abs(yVal - yZero);
        if (height == 0) height = 1;

        TpRect barRect(barLeft, top, barWidth, height);
        TpPen pen(dimColor(s->color(), 255), 2);
        painter->setPen(pen);
        painter->setBrush(TpBrush(dimColor(s->color(), 60)));
        painter->drawRect(barRect, 0);
        return;
    }

    const TpVector<TpDataPoint>& data = s->data();
    if (m_impl->selectedPointIndex >= 0 && m_impl->selectedPointIndex < data.size()) {
        const TpDataPoint& pt = data[m_impl->selectedPointIndex];
        int32_t px = m_impl->axisX->mapToPixel(pt.x, chartRect.width(), chartRect.x(), false);
        int32_t py = m_impl->axisY->mapToPixel(pt.y, chartRect.height(), chartRect.y(), true);
        int32_t radius = s->type() == TpSeries::TypeScatter ? 6 : 5;
        radius = (int32_t)(radius * scale);
        if (radius < 4) radius = 4;

        if (s->type() == TpSeries::TypeScatter) {
            TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 2, dimColor(s->color(), 255), _RGB(255, 255, 255));
        } else {
            TpRenderUtils::drawAnchorPoint(painter, TpPoint(px, py), radius + 2, dimColor(s->color(), 255), _RGB(255, 255, 255));
        }
    }
}

/// @brief 榧犳爣绉诲姩浜嬩欢
bool TpChart::onMouseMoveEvent(TpMouseEvent* event)
{
    if (!event) return false;

    TpPoint pos = event->pos();
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

    if (!chartRect.contains(pos)) {
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

/// @brief 榧犳爣鎸変笅浜嬩欢
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

    if (!chartRect.contains(pos)) {
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

/// @brief 榧犳爣閲婃斁浜嬩欢
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

    if (!wasDragging && m_impl->selectionEnabled && chartRect.contains(pos)) {
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

/// @brief 婊氳疆浜嬩欢
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

    TpPoint mousePos = m_impl->hasHover ? m_impl->hoverPos : TpPoint(chartRect.x() + chartRect.width() / 2, chartRect.y() + chartRect.height() / 2);
    if (!chartRect.contains(mousePos)) {
        mousePos = TpPoint(chartRect.x() + chartRect.width() / 2, chartRect.y() + chartRect.height() / 2);
    }

    if (pieChartMode || !chartRect.contains(mousePos)) {
        return false;
    }

    double factor = event->angleDelta() > 0 ? 0.9 : 1.1;
    zoomAxisAt(mousePos, chartRect, factor);
    this->update();
    return true;
}

/// @brief 绂诲紑浜嬩欢
bool TpChart::onLeaveEvent(TpLeaveEvent* event)
{
    clearHoverState();
    m_impl->mousePressed = false;
    m_impl->isDragging = false;
    TpWidget::onLeaveEvent(event);
    this->update();
    return true;
}
