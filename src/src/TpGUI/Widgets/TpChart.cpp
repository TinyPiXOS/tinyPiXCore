/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：刘杨
 * 邮箱 (Email)：2825143438@qq.com
 * 版权所有 (Copyright)：© 2026 刘杨. All rights reserved.
 * 描述 (Description)：图表类实现 (TpChart) - 响应式缩放与极限尺寸修复版
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
};


// 构造与析构
TpChart::TpChart()
{
    m_impl = new Impl();
    m_impl->backgroundColor = _RGB(255, 255, 255);
    m_impl->marginTop = 60;    // 留给标题
    m_impl->marginBottom = 60; // 留给X轴刻度
    m_impl->marginLeft = 60;   // 留给Y轴刻度
    m_impl->marginRight = 20;  // 防止溢出

    // 显式设置背景颜色
    this->setBackGroundColor(0xFFFFFFFF, true);

    // 初始化CSS数据
    enabledCssData = nullptr;
    disabledCssData = nullptr;
    hoverCssData = nullptr;
    pressCssData = nullptr;
    checkedCssData = nullptr;

    // 刷新基础CSS样式
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
/// 设置图表标题
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

/// 设置图表背景颜色
void TpChart::setBackgroundColor(int32_t color) {
    m_impl->backgroundColor = color;
    this->update();
}

/// 设置坐标轴标签
void TpChart::setAxisLabels(const char* xLabel, const char* yLabel) {
    m_impl->labelX = xLabel;
    m_impl->labelY = yLabel;
    this->update();
}

/// 添加数据系列
void TpChart::addSeries(TpSeries* series) {
    if (series) {
        m_impl->seriesList.push_back(series);
    }
}

/// 移除所有数据系列
void TpChart::removeAllSeries() {
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        delete m_impl->seriesList[i];
    }
    m_impl->seriesList.clear();
}

/// 设置样式表（CSS）
void TpChart::setStyleSheet(const TpString& styleSheet) {
    TpApp::Inst()->cssParser()->parseCss(styleSheet);
    refreshBaseCss();
}

/// 获取当前样式表（CSS）
TpString TpChart::styleSheet() {
    return TpApp::Inst()->cssParser()->cssStr();
}

/// 获取当前状态的CSS数据
tpShared<TpCssData> TpChart::currentStatusCss() {
    // TpChart 主要使用启用状态的CSS
    // 如果需要悬停/按下效果，可以在外部通过 setChecked() 等方式控制
    tpShared<TpCssData> curCssData = enabledCssData;
    if (!enabled()) {
        curCssData = disabledCssData;
    } else if (checkable() && checked()) {
        curCssData = checkedCssData;
    }
    return curCssData;
}

/// 刷新基础CSS样式
void TpChart::refreshBaseCss() {
    enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    disabledCssData = readCss(pluginType(), TpCssParser::Disabled);
    hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    checkedCssData = readCss(pluginType(), TpCssParser::Checked);
}

// 内部逻辑：自动计算范围
/// 根据所有 Series 数据自动调整坐标轴范围
void TpChart::updateAxisRange() {
    if (m_impl->seriesList.empty()) {
        // 如果没有数据，且是自动范围，才设置默认值
        if (m_impl->axisX->isAutoRange()) m_impl->axisX->setRange(0, 10);
        if (m_impl->axisY->isAutoRange()) m_impl->axisY->setRange(0, 10);
        return;
    }

    double minX = DBL_MAX, maxX = -DBL_MAX;
    double minY = DBL_MAX, maxY = -DBL_MAX;
    bool hasData = false;
    
    // 标记是否有柱状图
    bool hasBarSeries = false; 

    // 遍历所有 Series 找出最大最小值
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (!s || !s->isVisible()) continue;

        if (s->type() == TpSeries::TypeBar) {
            hasBarSeries = true;
        }

        const TpVector<TpDataPoint>& data = s->data();
        for (int32_t k = 0; k < data.size(); ++k) {
            double x = data[k].x;
            double y = data[k].y;
            if (x < minX) minX = x;
            if (x > maxX) maxX = x;
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
            hasData = true;
        }
    }

    if (!hasData) {
        if (m_impl->axisX->isAutoRange()) m_impl->axisX->setRange(0, 10);
        if (m_impl->axisY->isAutoRange()) m_impl->axisY->setRange(0, 10);
        return;
    }

    // 判断 X 轴是否开启自动范围
    if (m_impl->axisX->isAutoRange()) {
        if (hasBarSeries) {
            double spanX = maxX - minX;
            if (spanX == 0) spanX = 1.0; // 容错：防止只有一个数据点时跨度为0
            
            // 留白大小：取总跨度的 8%
            double paddingX = spanX * 0.08;
            if (paddingX < 1.0) paddingX = 1.0; 

            // 强行把 X 轴两边撑开
            m_impl->axisX->setRange(minX - paddingX, maxX + paddingX);
        } else {
            // 如果只有折线图，保持原来的贴边逻辑
            m_impl->axisX->setRange(minX, maxX);
        }
    }

    // 判断 Y 轴是否开启自动范围
    if (m_impl->axisY->isAutoRange()) {
        double rangeY = maxY - minY;
        if (rangeY == 0) rangeY = 1.0;
        
        double setMinY = (minY > 0) ? 0 : minY; 
        
        m_impl->axisY->setRange(setMinY, maxY + rangeY * 0.1);
    }
}

// 核心绘制逻辑
/// 重写绘制事件
bool TpChart::onPaintEvent(TpPaintEvent* event) {
    if (!event) return false;
    TpPainter* painter = event->painter();
    if (!painter) return false;

    // 先调用父类绘制事件（处理背景绘制）
    TpWidget::onPaintEvent(event);

    // 获取当前状态的CSS数据
    tpShared<TpCssData> curCssData = currentStatusCss();

    // 获取整体画布区域尺寸
    TpRect totalRect(0, 0, this->width(), this->height());
    int32_t w = totalRect.width();
    int32_t h = totalRect.height();

    // 以 800x600 为标准设计基准分辨率
    double scaleX = w / 800.0;
    double scaleY = h / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY; 
    
    // 限制缩放极值：最小缩至 0.5 倍，最大放至 2.5 倍
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    bool showTitleAndLegend = (w >= 300 && h >= 250); // 正常模式：空间充足，全量显示
    bool showAxisLabels     = (w >= 250 && h >= 200); // 紧凑模式：隐藏标题图例，显示XY标签
    bool showAxisTicks      = (w >= 120 && h >= 80);  // 极小模式：隐藏标签，仅保留刻度与折线

    if (!showAxisTicks) {
        // 极限微小模式 (如 100x50)：纯数据展示 (Sparkline)，移除所有文本预留空间
        m_impl->marginTop = 5;
        m_impl->marginBottom = 5; 
        m_impl->marginLeft = 5;   
        m_impl->marginRight = 5;
    } else if (!showAxisLabels) {
        // 紧凑模式：仅保留坐标轴刻度数字的空间
        m_impl->marginTop = (int32_t)(20 * scale);
        m_impl->marginBottom = (int32_t)(25 * scale); 
        m_impl->marginLeft = (int32_t)(35 * scale);   
        m_impl->marginRight = (int32_t)(15 * scale);
    } else {
        // 正常模式：结合内容是否存在与缩放因子，优雅分配外边距
        m_impl->marginTop = (m_impl->title.empty() || !showTitleAndLegend) ? (int32_t)(40 * scale) : (int32_t)(80 * scale);
        m_impl->marginBottom = m_impl->labelX.empty() ? (int32_t)(40 * scale) : (int32_t)(60 * scale);
        m_impl->marginLeft = m_impl->labelY.empty() ? (int32_t)(50 * scale) : (int32_t)(80 * scale);
        m_impl->marginRight = (int32_t)(30 * scale);
    }

    TpRect chartRect = calculateLayout(totalRect);

    // 更新范围与刻度计算 
    updateAxisRange();
    
    // 根据缩放后的尺寸动态计算应该展示几个刻度
    int32_t tickDensityX = showAxisTicks ? (int32_t)(80 * scale) : 120;
    int32_t tickDensityY = showAxisTicks ? (int32_t)(50 * scale) : 80;
    int32_t targetTicksX = chartRect.width() / (tickDensityX > 0 ? tickDensityX : 1);
    int32_t targetTicksY = chartRect.height() / (tickDensityY > 0 ? tickDensityY : 1);

    m_impl->axisX->updateNiceTicks(targetTicksX > 2 ? targetTicksX : 2);
    m_impl->axisY->updateNiceTicks(targetTicksY > 2 ? targetTicksY : 2);

    // 绘制背景与网格
    drawBackground(painter, totalRect, chartRect);
    drawGrid(painter, chartRect);

    // 获取当前状态的CSS数据（用于系列绘制）
    tpShared<TpCssData> seriesCssData = currentStatusCss();

    // 绘制数据 Series (柱状图/折线图)
    int32_t barSeriesCount = 0;
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        if (m_impl->seriesList[i]->type() == TpSeries::TypeBar) barSeriesCount++;
    }

    int32_t barIndex = 0;
    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (s && s->isVisible()) {
            if (s->type() == TpSeries::TypeBar) {
                static_cast<TpBarSeries*>(s)->setLayoutInfo(barIndex++, barSeriesCount);
            }

            // 根据系列类型应用CSS样式
            if (s->type() == TpSeries::TypeLine) {
                // 折线图系列应用 CSS
                s->applyCssData("TpLineSeries", TpCssParser::Enabled);
            } else if (s->type() == TpSeries::TypeBar) {
                // 柱状图系列应用 CSS
                s->applyCssData("TpBarSeries", TpCssParser::Enabled);
            }

            s->draw(painter, *m_impl->axisX, *m_impl->axisY, chartRect);
        }
    }

    // 绘制坐标轴线条与刻度数字
    if (showAxisTicks) {
        uint32_t black = _RGB(0, 0, 0);
        TpRenderUtils::drawAxisX(painter, chartRect, *m_impl->axisX, *m_impl->axisY, black, false, 0);
        TpRenderUtils::drawAxisY(painter, chartRect, *m_impl->axisY, *m_impl->axisX, black, false, 0);
    }

    // 绘制坐标系描述标签
    if (showAxisLabels) {
        // 计算动态字体大小 (基准 20px)
        int32_t labelFontSize = (int32_t)(20 * scale);
        if (labelFontSize < 9) labelFontSize = 9; // 保底字号防止看不清

        TpFont labelFont;
        labelFont.setFontSize(labelFontSize);
        labelFont.setFontColor(_RGB(30, 30, 30));

        // X 轴标签 
        if (m_impl->labelX.length() > 0) {
            labelFont.setText(m_impl->labelX);
            int32_t textW = labelFont.pixelWidth() > 0 ? labelFont.pixelWidth() : m_impl->labelX.length() * (int32_t)(8 * scale);
            int32_t lx = chartRect.x() + (chartRect.width() / 2) - (textW / 2);
            int32_t ly = chartRect.bottom() + (int32_t)(40 * scale); 
            painter->drawText(labelFont, lx, ly);
        }

        // Y 轴标签 (支持垂直文本旋转)
        if (m_impl->labelY.length() > 0) {
            TpFont yLabelFont;
            yLabelFont.setFontSize(labelFontSize);
            yLabelFont.setFontColor(_RGB(30, 30, 30));
            yLabelFont.setVerticalText(true);
            yLabelFont.setText(m_impl->labelY);

            // 垂直文本的长度 = 字符数 × 字体大小 (从上到下的跨度)
            int32_t textLength = m_impl->labelY.length() * labelFontSize;

            // 水平方向：在 marginLeft 空间内，整体向左靠
            int32_t textH = yLabelFont.pixelWidth() > 0 ? yLabelFont.pixelWidth() : labelFontSize;
            int32_t lx = totalRect.x() + (int32_t)(5 * scale);

            // 垂直文本的总高度 = 字符数 × 字体大小
            int32_t textHeight = (int32_t)(m_impl->labelY.length() * labelFontSize);
            // 垂直方向：标签中点对齐坐标轴中点
            // 由于旋转后基线变化，需要手动调整偏移量
            int32_t ly = chartRect.y() + (chartRect.height() / 2) - (textHeight / 2);

            painter->drawText(yLabelFont, lx, ly);
        }
    }

    // 绘制标题与图例
    if (showTitleAndLegend) {
        drawTitle(painter, totalRect);
        drawLegend(painter, totalRect, chartRect);
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

/// 绘制背景
void TpChart::drawBackground(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect) {
    // 使用CSS背景色（如果设置了）
    tpShared<TpCssData> curCssData = currentStatusCss();
    int32_t bgColor = curCssData->backgroundColor();

    TpRenderUtils::fillGradientRect(painter, totalRect, _RGB(240, 240, 240), _RGB(255, 255, 255));

    // 使用CSS背景色或默认背景色
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
    // 简单根据刻度数画网格
    int32_t rows = m_impl->axisY->getTickValues().size() - 1;
    int32_t cols = m_impl->axisX->getTickValues().size() - 1;
    
    if (rows < 1) rows = 5;
    if (cols < 1) cols = 6;

    TpRenderUtils::drawGrid(painter, chartRect, rows, cols, _RGB(220, 220, 220));

    // Y=0 基准线
    if (m_impl->axisY->containsZero()) {
        int32_t yZero = m_impl->axisY->ZeroPixel(chartRect.height(), chartRect.y(), true);
        if (yZero >= chartRect.y() && yZero <= chartRect.bottom()) {
            TpPen zeroPen(_RGB(0, 0, 0), 1);
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

    // 动态计算标题字体大小 (基准 18px)
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

/// 绘制图例
void TpChart::drawLegend(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect) {
    TpVector<const char*> names;
    TpVector<int32_t> colors;
    TpVector<int32_t> endColors;
    TpVector<int32_t> types;

    for (int32_t i = 0; i < m_impl->seriesList.size(); ++i) {
        TpSeries* s = m_impl->seriesList[i];
        if (s->name().empty()) continue;

        names.push_back(s->name().c_str());
        colors.push_back(s->color());
        
        if (s->type() == TpSeries::TypeBar) {
            // 安全强转
            endColors.push_back(static_cast<TpBarSeries*>(s)->colorEnd());
            types.push_back(TpRenderUtils::TypeBar);
        } else {
            endColors.push_back(s->color());
            types.push_back(TpRenderUtils::TypeLine);
        }
    }

    if (names.size() > 0) {
        // 注意：TpRenderUtils 里面的绘制图例如果文字也显得太大，
        // 你可能需要进去修改 TpRenderUtils::drawLegendOutside 的实现逻辑，也给它加上缩放计算。
        TpRenderUtils::drawLegendOutside(painter, totalRect, chartRect, names, colors, endColors, types);
    }
}