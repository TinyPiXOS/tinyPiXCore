#include "MainWindowService.h"
#include "TpChart.h"
#include "TpTimer.h"
#include <TpEvent.h>
#include <cmath>

#ifndef _RGB
#define _RGB(r, g, b) ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#endif

namespace {
/// @brief 鍚敤鍥捐〃浜や簰
/// @param chart 鍥捐〃鎸囬拡
/// @param pieMode 鏄惁涓洪ゼ鍥?
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
    // Charts own the series after addSeries(); the pointers stay here for timer updates.
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
    // No manual series deletion here; chart teardown owns that lifecycle.
}
void MainWindowService::initCharts() {
    // 鎶樼嚎鍥? - 鍔ㄦ€侊紙涓婃柟宸︿晶锛?
    m_chartLine_1 = new TpChart();
    m_chartLine_1->setParent(this);
    m_chartLine_1->setRect(10, 10, 550, 350);
    m_chartLine_1->setAxisLabels("Time (seconds)", "Usage (%)");
    m_chartLine_1->setTitle("System Monitor - Line Chart (Dynamic)");
    m_chartLine_1->setBackgroundColor(_RGB(255, 255, 255));
    // 鎶樼嚎鍥? - 闈欐€侊紙涓婃柟鍙充晶锛?
    m_chartLine_2 = new TpChart();
    m_chartLine_2->setParent(this);
    m_chartLine_2->setRect(570, 10, 470, 350);
    m_chartLine_2->setAxisLabels("X", "Amplitude");
    m_chartLine_2->setTitle("Static Line Chart (Sine / Cosine)");
    m_chartLine_2->setBackgroundColor(_RGB(255, 255, 255));
    // 鏌辩姸鍥? - 鍔ㄦ€侊紙涓嬫柟宸︿晶锛?
    m_chartBar_1 = new TpChart();
    m_chartBar_1->setParent(this);
    m_chartBar_1->setRect(10, 400, 550, 300);
    m_chartBar_1->setAxisLabels("Category", "Value");
    m_chartBar_1->setTitle("System Monitor - Bar Chart (Dynamic)");
    m_chartBar_1->setBackgroundColor(_RGB(255, 255, 255));
    // 鏌辩姸鍥? - 闈欐€侊紙涓嬫柟鍙充晶锛?
    m_chartBar_2 = new TpChart();
    m_chartBar_2->setParent(this);
    m_chartBar_2->setRect(570, 400, 470, 300);
    m_chartBar_2->setAxisLabels("Category", "Value");
    m_chartBar_2->setTitle("Static Bar Chart");
    m_chartBar_2->setBackgroundColor(_RGB(255, 255, 255));
    // ==================== 鎶樼嚎鍥?锛堝姩鎬侊級閰嶇疆 ====================
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
    // 涓哄姩鎬佹姌绾垮浘娣诲姞鍒濆鏁版嵁
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
    // ==================== 鎶樼嚎鍥?锛堥潤鎬侊級閰嶇疆 ====================
    m_chartLine_2->axisX()->setAutoRange(false);
    m_chartLine_2->axisX()->setRange(0, 20);
    m_chartLine_2->axisX()->setTickCount(5);
    m_chartLine_2->axisX()->setRollingMode(false);
    m_chartLine_2->axisY()->setAutoRange(false);
    m_chartLine_2->axisY()->setRange(-1.5, 1.5);   // 鑼冨洿瓒冲
    m_chartLine_2->axisY()->setTickCount(7);
    m_chartLine_2->axisY()->setSymmetric(true);
    m_chartLine_2->setGridXVisible(true);
    m_chartLine_2->setGridYVisible(true);
    enableChartInteraction(m_chartLine_2, false);
    // 鍙湁涓€涓姌绾跨郴鍒楋紝鍚嶇О涓?"Static Line"锛屽叧闂钩婊戯紙鎶樼嚎鏁堟灉锛?
    m_lineSeries3 = new TpLineSeries();
    m_lineSeries3->setName("Static Line");
    m_lineSeries3->setSmooth(false);   // 鍏抽敭锛氫笉骞虫粦
    m_lineSeries3->setLineWidth(2);
    m_lineSeries3->setMaxPointCount(0);
    // 鎵嬪姩娣诲姞灏戦噺绂绘暎鐐癸紝褰㈡垚鏄庢樉鎶樼嚎锛堜笉鏄洸绾匡級
    m_lineSeries3->addPoint(0, 0);
    m_lineSeries3->addPoint(4, 1.2);
    m_lineSeries3->addPoint(8, -0.8);
    m_lineSeries3->addPoint(12, 0.5);
    m_lineSeries3->addPoint(16, -1.0);
    m_lineSeries3->addPoint(20, 0.2);
    m_chartLine_2->addSeries(m_lineSeries3);
    m_chartLine_2->show();
    // ==================== 鏌辩姸鍥?锛堝姩鎬侊級閰嶇疆 ====================
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
    // ==================== 鏌辩姸鍥?锛堥潤鎬侊級閰嶇疆 ====================
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
    // 闈欐€佹暟鎹細8涓煴瀛愮殑鍊?
    double staticBarData[] = {45, 78, 32, 88, 56, 72, 91, 40};
    for (int i = 0; i < 8; ++i) {
        m_barSeries_2->addPoint(i, staticBarData[i]);
    }
    m_chartBar_2->addSeries(m_barSeries_2);
    m_chartBar_2->show();
    // ==================== 瀹氭椂鍣紙浠呯敤浜庡姩鎬佸浘琛ㄦ洿鏂帮級 ====================
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
