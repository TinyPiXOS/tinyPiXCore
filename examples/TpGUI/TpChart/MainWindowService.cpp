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
    : TpMainWindow(), chartLine1_(NULL), chartBar1_(NULL), lineSeries1_(NULL), lineSeries2_(NULL), barSeries1_(NULL), currentX_(-40.0), timer_(NULL), lineSeries3_(NULL), barSeries2_(NULL)
{
    initCharts();
}
MainWindowService::~MainWindowService() {
    if (timer_) {
        timer_->stop();
        delete timer_;
        timer_ = NULL;
    }
    if (barTimer_) {
        barTimer_->stop();
        delete barTimer_;
        barTimer_ = NULL;
    }
    // Charts own the series after addSeries(); the pointers stay here for timer updates.
    if (chartLine1_) {
        delete chartLine1_;
        chartLine1_ = NULL;
    }
    if (chartBar1_) {
        delete chartBar1_;
        chartBar1_ = NULL;
    }
    if (chartLine2_) {
        delete chartLine2_;
        chartLine2_ = NULL;
    }
    if (chartBar2_) {
        delete chartBar2_;
        chartBar2_ = NULL;
    }
    // No manual series deletion here; chart teardown owns that lifecycle.
}
void MainWindowService::initCharts() {
    // 鎶樼嚎鍥? - 鍔ㄦ€侊紙涓婃柟宸︿晶锛?
    chartLine1_ = new TpChart();
    chartLine1_->setParent(this);
    chartLine1_->setRect(10, 10, 550, 350);
    chartLine1_->setAxisLabels("Time (seconds)", "Usage (%)");
    chartLine1_->setTitle("System Monitor - Line Chart (Dynamic)");
    chartLine1_->setBackgroundColor(_RGB(255, 255, 255));
    // 鎶樼嚎鍥? - 闈欐€侊紙涓婃柟鍙充晶锛?
    chartLine2_ = new TpChart();
    chartLine2_->setParent(this);
    chartLine2_->setRect(570, 10, 470, 350);
    chartLine2_->setAxisLabels("X", "Amplitude");
    chartLine2_->setTitle("Static Line Chart (Sine / Cosine)");
    chartLine2_->setBackgroundColor(_RGB(255, 255, 255));
    // 鏌辩姸鍥? - 鍔ㄦ€侊紙涓嬫柟宸︿晶锛?
    chartBar1_ = new TpChart();
    chartBar1_->setParent(this);
    chartBar1_->setRect(10, 400, 550, 300);
    chartBar1_->setAxisLabels("Category", "Value");
    chartBar1_->setTitle("System Monitor - Bar Chart (Dynamic)");
    chartBar1_->setBackgroundColor(_RGB(255, 255, 255));
    // 鏌辩姸鍥? - 闈欐€侊紙涓嬫柟鍙充晶锛?
    chartBar2_ = new TpChart();
    chartBar2_->setParent(this);
    chartBar2_->setRect(570, 400, 470, 300);
    chartBar2_->setAxisLabels("Category", "Value");
    chartBar2_->setTitle("Static Bar Chart");
    chartBar2_->setBackgroundColor(_RGB(255, 255, 255));
    // ==================== 鎶樼嚎鍥?锛堝姩鎬侊級閰嶇疆 ====================
    chartLine1_->axisX()->setAutoRange(true);
    chartLine1_->axisX()->setAutoRangeNice(false);
    chartLine1_->axisX()->setTickCount(9);
    chartLine1_->axisX()->setRollingMode(true);
    chartLine1_->axisX()->setXPadding(0.0, 0.05);
    chartLine1_->axisY()->setAutoRange(true);
    chartLine1_->axisY()->setSymmetric(true);
    chartLine1_->axisY()->setTickCount(7);
    chartLine1_->axisY()->setYPadding(0.3, 0.5);
    enableChartInteraction(chartLine1_, false);
    lineSeries1_ = new TpLineSeries();
    lineSeries1_->setName("CPU Usage");
    lineSeries1_->setColor(_RGB(0, 200, 0));
    lineSeries1_->setLineWidth(3);
    lineSeries1_->setSmooth(true);
    lineSeries1_->setMaxPointCount(200);
    lineSeries2_ = new TpLineSeries();
    lineSeries2_->setName("Memory Usage");
    lineSeries2_->setColor(_RGB(255, 128, 0));
    lineSeries2_->setLineWidth(2);
    lineSeries2_->setSmooth(true);
    lineSeries2_->setMaxPointCount(200);
    // 涓哄姩鎬佹姌绾垮浘娣诲姞鍒濆鏁版嵁
    for (int i = 0; i < 200; ++i) {
        float dynamicAmp1 = 100 + 50 * std::sin(currentX_ * 0.1);
        float yValue1 = dynamicAmp1 * std::sin(currentX_ * 0.8);
        float dynamicAmp2 = 80 + 30 * std::cos(currentX_ * 0.15);
        float yValue2 = dynamicAmp2 * std::cos(currentX_ * 1.5);
        lineSeries1_->addPoint(currentX_, yValue1);
        lineSeries2_->addPoint(currentX_, yValue2);
        currentX_ += 0.1;
    }
    chartLine1_->addSeries(lineSeries1_);
    chartLine1_->addSeries(lineSeries2_);
    chartLine1_->show();
    // ==================== 鎶樼嚎鍥?锛堥潤鎬侊級閰嶇疆 ====================
    chartLine2_->axisX()->setAutoRange(false);
    chartLine2_->axisX()->setRange(0, 20);
    chartLine2_->axisX()->setTickCount(5);
    chartLine2_->axisX()->setRollingMode(false);
    chartLine2_->axisY()->setAutoRange(false);
    chartLine2_->axisY()->setRange(-1.5, 1.5);   // 鑼冨洿瓒冲
    chartLine2_->axisY()->setTickCount(7);
    chartLine2_->axisY()->setSymmetric(true);
    chartLine2_->setGridXVisible(true);
    chartLine2_->setGridYVisible(true);
    enableChartInteraction(chartLine2_, false);
    // 鍙湁涓€涓姌绾跨郴鍒楋紝鍚嶇О涓?"Static Line"锛屽叧闂钩婊戯紙鎶樼嚎鏁堟灉锛?
    lineSeries3_ = new TpLineSeries();
    lineSeries3_->setName("Static Line");
    lineSeries3_->setSmooth(false);   // 鍏抽敭锛氫笉骞虫粦
    lineSeries3_->setLineWidth(2);
    lineSeries3_->setMaxPointCount(0);
    // 鎵嬪姩娣诲姞灏戦噺绂绘暎鐐癸紝褰㈡垚鏄庢樉鎶樼嚎锛堜笉鏄洸绾匡級
    lineSeries3_->addPoint(0, 0);
    lineSeries3_->addPoint(4, 1.2);
    lineSeries3_->addPoint(8, -0.8);
    lineSeries3_->addPoint(12, 0.5);
    lineSeries3_->addPoint(16, -1.0);
    lineSeries3_->addPoint(20, 0.2);
    chartLine2_->addSeries(lineSeries3_);
    chartLine2_->show();
    // ==================== 鏌辩姸鍥?锛堝姩鎬侊級閰嶇疆 ====================
    chartBar1_->axisX()->setMode(TpAxis::AxisMode::Category);
    chartBar1_->axisX()->setCategoryCount(10);
    chartBar1_->axisX()->setAutoRange(false);
    chartBar1_->axisY()->setAutoRange(true);
    chartBar1_->axisY()->setYPadding(0.1, 0);
    chartBar1_->setGridXVisible(false);
    enableChartInteraction(chartBar1_, false);
    barSeries1_ = new TpBarSeries();
    barSeries1_->setName("System Value");
    barSeries1_->setColor(_RGB(0, 120, 212));
    barSeries1_->setGradientColor(_RGB(0, 80, 180));
    barSeries1_->setLabelsVisible(true);
    for (int i = 0; i < 10; ++i) {
        double value = 50 + 30 * std::sin(i * 0.5);
        barSeries1_->addPoint(i, value);
    }
    chartBar1_->addSeries(barSeries1_);
    chartBar1_->show();
    // ==================== 鏌辩姸鍥?锛堥潤鎬侊級閰嶇疆 ====================
    chartBar2_->axisX()->setMode(TpAxis::AxisMode::Category);
    chartBar2_->axisX()->setCategoryCount(8);
    chartBar2_->axisX()->setAutoRange(false);
    chartBar2_->axisY()->setAutoRange(false);
    chartBar2_->axisY()->setRange(0, 100);
    chartBar2_->axisY()->setTickCount(6);
    chartBar2_->setGridXVisible(false);
    chartBar2_->setGridYVisible(true);
    enableChartInteraction(chartBar2_, false);
    barSeries2_ = new TpBarSeries();
    barSeries2_->setName("Static Bar");
    barSeries2_->setLabelsVisible(true);
    // 闈欐€佹暟鎹細8涓煴瀛愮殑鍊?
    double staticBarData[] = {45, 78, 32, 88, 56, 72, 91, 40};
    for (int i = 0; i < 8; ++i) {
        barSeries2_->addPoint(i, staticBarData[i]);
    }
    chartBar2_->addSeries(barSeries2_);
    chartBar2_->show();
    // ==================== 瀹氭椂鍣紙浠呯敤浜庡姩鎬佸浘琛ㄦ洿鏂帮級 ====================
    barTimer_ = new TpTimer(200);
    connect(barTimer_, timeout, this, &MainWindowService::onBarTimeout);
    barTimer_->start();
    timer_ = new TpTimer(50);
    connect(timer_, timeout, this, &MainWindowService::onTimeout);
    timer_->start();
}
void MainWindowService::onTimeout() {
    if (lineSeries1_ && lineSeries2_ && chartLine1_) {
        currentX_ += 0.1;
        float dynamicAmp1 = 100 + 50 * std::sin(currentX_ * 0.1);
        float yValue1 = dynamicAmp1 * std::sin(currentX_ * 0.8);
        float dynamicAmp2 = 80 + 30 * std::cos(currentX_ * 0.15);
        float yValue2 = dynamicAmp2 * std::cos(currentX_ * 1.5);
        lineSeries1_->addPoint(currentX_, yValue1);
        lineSeries2_->addPoint(currentX_, yValue2);
        chartLine1_->update();
    }
}
void MainWindowService::onBarTimeout() {
    if (barSeries1_ && chartBar1_) {
        barSeries1_->clear();
        for (int i = 0; i < 10; ++i) {
            double value = 50 + 30 * std::sin(i * 0.5 + currentX_ * 0.2);
            barSeries1_->addPoint(i, value);
        }
        chartBar1_->update();
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
