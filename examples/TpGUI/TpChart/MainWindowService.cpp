#include "MainWindowService.h"
#include "TpChart.h"
#include "TpTimer.h"
#include <TpEvent.h>
#include <cmath>
#include <TpFile.h>

#ifndef _RGB
#define _RGB(r, g, b) ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#endif

namespace {

/// @brief 启用图表交互
/// @param chart 图表指针
/// @param pieMode 是否为饼图
static void enableChartInteraction(TpChart* chart, bool pieMode)
{
    if (!chart) {
        return;
    }

    chart->setTooltipVisible(true);
    chart->setSelectionEnabled(true);
    chart->setLegendClickable(true);
    chart->setCrosshairVisible(!pieMode);
    chart->setPanEnabled(!pieMode);
    chart->setWheelZoomEnabled(!pieMode);
}

}

MainWindowService::MainWindowService()
    : TpMainWindow(), m_chartLine_1(NULL), m_chartBar_1(NULL), m_lineSeries1(NULL), m_lineSeries2(NULL), m_barSeries_1(NULL), m_currentX(-40.0), m_timer(NULL), m_lineSeries3(NULL), m_barSeries_2(NULL)
{
    initCharts();
}

MainWindowService::~MainWindowService() {
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = NULL;
    }
    if (m_barTimer) {
        m_barTimer->stop();
        delete m_barTimer;
        m_barTimer = NULL;
    }
    // 删除图表（图表不负责删除系列，系列需手动释放）
    if (m_chartLine_1) {
        delete m_chartLine_1;
        m_chartLine_1 = NULL;
    }
    if (m_chartBar_1) {
        delete m_chartBar_1;
        m_chartBar_1 = NULL;
    }
    if (m_chartLine_2) {
        delete m_chartLine_2;
        m_chartLine_2 = NULL;
    }
    if (m_chartBar_2) {
        delete m_chartBar_2;
        m_chartBar_2 = NULL;
    }
    // 删除系列
    if (m_lineSeries1) delete m_lineSeries1;
    if (m_lineSeries2) delete m_lineSeries2;
    if (m_barSeries_1) delete m_barSeries_1;
    if (m_lineSeries3) delete m_lineSeries3;
    if (m_barSeries_2) delete m_barSeries_2;
}

void MainWindowService::initCharts() {
    // 折线图1 - 动态（上方左侧）
    m_chartLine_1 = new TpChart();
    m_chartLine_1->setParent(this);
    m_chartLine_1->setRect(10, 10, 550, 350);
    m_chartLine_1->setAxisLabels("Time (seconds)", "Usage (%)");
    m_chartLine_1->setTitle("System Monitor - Line Chart (Dynamic)");
    m_chartLine_1->setBackgroundColor(_RGB(255, 255, 255));

    // 折线图2 - 静态（上方右侧）
    m_chartLine_2 = new TpChart();
    m_chartLine_2->setParent(this);
    m_chartLine_2->setRect(570, 10, 470, 350);
    m_chartLine_2->setAxisLabels("X", "Amplitude");
    m_chartLine_2->setTitle("Static Line Chart (Sine / Cosine)");
    m_chartLine_2->setBackgroundColor(_RGB(255, 255, 255));

    // 柱状图1 - 动态（下方左侧）
    m_chartBar_1 = new TpChart();
    m_chartBar_1->setParent(this);
    m_chartBar_1->setRect(10, 400, 550, 300);
    m_chartBar_1->setAxisLabels("Category", "Value");
    m_chartBar_1->setTitle("System Monitor - Bar Chart (Dynamic)");
    m_chartBar_1->setBackgroundColor(_RGB(255, 255, 255));

    // 柱状图2 - 静态（下方右侧）
    m_chartBar_2 = new TpChart();
    m_chartBar_2->setParent(this);
    m_chartBar_2->setRect(570, 400, 470, 300);
    m_chartBar_2->setAxisLabels("Category", "Value");
    m_chartBar_2->setTitle("Static Bar Chart");
    m_chartBar_2->setBackgroundColor(_RGB(255, 255, 255));

    // 加载 CSS 样式（所有图表共享同一份样式表）
    TpString cssPath = "./chart_style.css";
    TpFile cssFile(cssPath);
    TpString cssContent;
    if (cssFile.exists()) {
        cssFile.open(TpFile::ReadOnly);
        if (cssFile.isOpen()) {
            cssContent = cssFile.readAll();
        }
    }

    // ==================== 折线图1（动态）配置 ====================
    m_chartLine_1->setStyleSheet(cssContent);
    m_chartLine_1->axisX()->setAutoRange(true);
    m_chartLine_1->axisX()->setAutoRangeNice(false);
    m_chartLine_1->axisX()->setTickCount(9);
    m_chartLine_1->axisX()->setRollingMode(true);
    m_chartLine_1->axisX()->setXPadding(0.0, 0.05);
    m_chartLine_1->axisY()->setAutoRange(true);
    m_chartLine_1->axisY()->setSymmetric(true);
    m_chartLine_1->axisY()->setTickCount(7);
    m_chartLine_1->axisY()->setYPadding(0.3, 0.5);
    enableChartInteraction(m_chartLine_1, false);

    m_lineSeries1 = new TpLineSeries();
    m_lineSeries1->setName("CPU Usage");
    m_lineSeries1->setColor(_RGB(0, 200, 0));
    m_lineSeries1->setLineWidth(3);
    m_lineSeries1->setSmooth(true);
    m_lineSeries1->setMaxPointCount(200);

    m_lineSeries2 = new TpLineSeries();
    m_lineSeries2->setName("Memory Usage");
    m_lineSeries2->setColor(_RGB(255, 128, 0));
    m_lineSeries2->setLineWidth(2);
    m_lineSeries2->setSmooth(true);
    m_lineSeries2->setMaxPointCount(200);

    // 为动态折线图添加初始数据
    for (int i = 0; i < 200; ++i) {
        float dynamicAmp1 = 100 + 50 * std::sin(m_currentX * 0.1);
        float yValue1 = dynamicAmp1 * std::sin(m_currentX * 0.8);
        float dynamicAmp2 = 80 + 30 * std::cos(m_currentX * 0.15);
        float yValue2 = dynamicAmp2 * std::cos(m_currentX * 1.5);
        m_lineSeries1->addPoint(m_currentX, yValue1);
        m_lineSeries2->addPoint(m_currentX, yValue2);
        m_currentX += 0.1;
    }
    m_chartLine_1->addSeries(m_lineSeries1);
    m_chartLine_1->addSeries(m_lineSeries2);
    m_chartLine_1->show();

    // ==================== 折线图2（静态）配置 ====================
    m_chartLine_2->setStyleSheet(cssContent);
    m_chartLine_2->axisX()->setAutoRange(false);
    m_chartLine_2->axisX()->setRange(0, 20);
    m_chartLine_2->axisX()->setTickCount(5);
    m_chartLine_2->axisX()->setRollingMode(false);
    m_chartLine_2->axisY()->setAutoRange(false);
    m_chartLine_2->axisY()->setRange(-1.5, 1.5);   // 范围足够
    m_chartLine_2->axisY()->setTickCount(7);
    m_chartLine_2->axisY()->setSymmetric(true);
    m_chartLine_2->setGridXVisible(true);
    m_chartLine_2->setGridYVisible(true);
    enableChartInteraction(m_chartLine_2, false);

    // 只有一个折线系列，名称为 "Static Line"，关闭平滑（折线效果）
    m_lineSeries3 = new TpLineSeries();
    m_lineSeries3->setName("Static Line");
    m_lineSeries3->setSmooth(false);   // 关键：不平滑
    m_lineSeries3->setLineWidth(2);
    m_lineSeries3->setMaxPointCount(0);

    // 手动添加少量离散点，形成明显折线（不是曲线）
    m_lineSeries3->addPoint(0, 0);
    m_lineSeries3->addPoint(4, 1.2);
    m_lineSeries3->addPoint(8, -0.8);
    m_lineSeries3->addPoint(12, 0.5);
    m_lineSeries3->addPoint(16, -1.0);
    m_lineSeries3->addPoint(20, 0.2);

    m_chartLine_2->addSeries(m_lineSeries3);
    m_chartLine_2->show();

    // ==================== 柱状图1（动态）配置 ====================
    m_chartBar_1->setStyleSheet(cssContent);
    m_chartBar_1->axisX()->setMode(TpAxis::AxisMode::Category);
    m_chartBar_1->axisX()->setCategoryCount(10);
    m_chartBar_1->axisX()->setAutoRange(false);
    m_chartBar_1->axisY()->setAutoRange(true);
    m_chartBar_1->axisY()->setYPadding(0.1, 0);
    m_chartBar_1->setGridXVisible(false);
    enableChartInteraction(m_chartBar_1, false);

    m_barSeries_1 = new TpBarSeries();
    m_barSeries_1->setName("System Value");
    m_barSeries_1->setColor(_RGB(0, 120, 212));
    m_barSeries_1->setGradientColor(_RGB(0, 80, 180));
    m_barSeries_1->setLabelsVisible(true);
    for (int i = 0; i < 10; ++i) {
        double value = 50 + 30 * std::sin(i * 0.5);
        m_barSeries_1->addPoint(i, value);
    }
    m_chartBar_1->addSeries(m_barSeries_1);
    m_chartBar_1->show();

    // ==================== 柱状图2（静态）配置 ====================
    m_chartBar_2->setStyleSheet(cssContent);
    m_chartBar_2->axisX()->setMode(TpAxis::AxisMode::Category);
    m_chartBar_2->axisX()->setCategoryCount(8);
    m_chartBar_2->axisX()->setAutoRange(false);
    m_chartBar_2->axisY()->setAutoRange(false);
    m_chartBar_2->axisY()->setRange(0, 100);
    m_chartBar_2->axisY()->setTickCount(6);
    m_chartBar_2->setGridXVisible(false);
    m_chartBar_2->setGridYVisible(true);
    enableChartInteraction(m_chartBar_2, false);

    m_barSeries_2 = new TpBarSeries();
    m_barSeries_2->setName("Static Bar");
    m_barSeries_2->setLabelsVisible(true);
    // 静态数据：8个柱子的值
    double staticBarData[] = {45, 78, 32, 88, 56, 72, 91, 40};
    for (int i = 0; i < 8; ++i) {
        m_barSeries_2->addPoint(i, staticBarData[i]);
    }
    m_chartBar_2->addSeries(m_barSeries_2);
    m_chartBar_2->show();

    // ==================== 定时器（仅用于动态图表更新） ====================
    m_barTimer = new TpTimer(200);
    connect(m_barTimer, timeout, this, &MainWindowService::onBarTimeout);
    m_barTimer->start();

    m_timer = new TpTimer(50);
    connect(m_timer, timeout, this, &MainWindowService::onTimeout);
    m_timer->start();
}

void MainWindowService::onTimeout() {
    if (m_lineSeries1 && m_lineSeries2 && m_chartLine_1) {
        m_currentX += 0.1;
        float dynamicAmp1 = 100 + 50 * std::sin(m_currentX * 0.1);
        float yValue1 = dynamicAmp1 * std::sin(m_currentX * 0.8);
        float dynamicAmp2 = 80 + 30 * std::cos(m_currentX * 0.15);
        float yValue2 = dynamicAmp2 * std::cos(m_currentX * 1.5);
        m_lineSeries1->addPoint(m_currentX, yValue1);
        m_lineSeries2->addPoint(m_currentX, yValue2);
        m_chartLine_1->update();
    }
}

void MainWindowService::onBarTimeout() {
    if (m_barSeries_1 && m_chartBar_1) {
        m_barSeries_1->clear();
        for (int i = 0; i < 10; ++i) {
            double value = 50 + 30 * std::sin(i * 0.5 + m_currentX * 0.2);
            m_barSeries_1->addPoint(i, value);
        }
        m_chartBar_1->update();
    }
}

bool MainWindowService::onPaintEvent(TpPaintEvent *event) {
    return TpMainWindow::onPaintEvent(event);
}

bool MainWindowService::onActiveEvent(TpActiveEvent *event) {
    return TpMainWindow::onActiveEvent(event);
}

bool MainWindowService::appChange(int32_t, int32_t, int32_t, int32_t, int32_t, uint8_t, int32_t) {
    return true;
}
