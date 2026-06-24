#include "MainWindowServiceAdvanced.h"
#include "TpChart.h"
#include "TpSeries.h"
#include <TpEvent.h>
#include <cmath>

#ifndef _RGB
#define _RGB(r, g, b) ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#endif

namespace {

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

MainWindowServiceAdvanced::MainWindowServiceAdvanced()
    : TpMainWindow(), scatterChart_(NULL), pieChart_(NULL), scatterSeriesA_(NULL), scatterSeriesB_(NULL), pieSeries_(NULL)
{
    initCharts();
}

MainWindowServiceAdvanced::~MainWindowServiceAdvanced() {
    if (scatterChart_) {
        delete scatterChart_;
        scatterChart_ = NULL;
    }
    if (pieChart_) {
        delete pieChart_;
        pieChart_ = NULL;
    }
}

void MainWindowServiceAdvanced::initCharts() {
    scatterChart_ = new TpChart();
    scatterChart_->setParent(this);
    scatterChart_->setRect(10, 10, 500, 700);
    scatterChart_->setAxisLabels("X", "Y");
    scatterChart_->setTitle("Scatter Chart");
    scatterChart_->setBackgroundColor(_RGB(255, 255, 255));
    scatterChart_->axisX()->setAutoRange(true);
    scatterChart_->axisX()->setTickCount(7);
    scatterChart_->axisX()->setRollingMode(false);
    scatterChart_->axisY()->setAutoRange(true);
    scatterChart_->axisY()->setSymmetric(true);
    scatterChart_->axisY()->setTickCount(7);
    scatterChart_->setGridXVisible(true);
    scatterChart_->setGridYVisible(true);
    enableChartInteraction(scatterChart_, false);

    scatterSeriesA_ = new TpScatterSeries();
    scatterSeriesA_->setName("Cluster A");
    scatterSeriesA_->setColor(_RGB(0, 184, 148));
    scatterSeriesA_->setBorderColor(_RGB(0, 120, 96));
    scatterSeriesA_->setPointSize(6);
    scatterSeriesA_->setLabelsVisible(true);
    scatterSeriesA_->setLabelColor(_RGB(60, 60, 60));
    scatterSeriesA_->setLabelSize(9);
    scatterSeriesA_->addPoint(-6.0, 2.0);
    scatterSeriesA_->addPoint(-4.8, 2.8);
    scatterSeriesA_->addPoint(-3.6, 1.5);
    scatterSeriesA_->addPoint(-2.4, 3.4);
    scatterSeriesA_->addPoint(-1.2, 2.6);
    scatterSeriesA_->addPoint(0.0, 4.0);

    scatterSeriesB_ = new TpScatterSeries();
    scatterSeriesB_->setName("Cluster B");
    scatterSeriesB_->setColor(_RGB(255, 140, 0));
    scatterSeriesB_->setBorderColor(_RGB(200, 100, 0));
    scatterSeriesB_->setPointSize(5);
    scatterSeriesB_->setLabelsVisible(false);
    scatterSeriesB_->addPoint(1.0, -1.0);
    scatterSeriesB_->addPoint(2.0, -2.2);
    scatterSeriesB_->addPoint(3.0, -1.4);
    scatterSeriesB_->addPoint(4.0, -3.0);
    scatterSeriesB_->addPoint(5.0, -1.8);
    scatterSeriesB_->addPoint(6.0, -2.6);

    scatterChart_->addSeries(scatterSeriesA_);
    scatterChart_->addSeries(scatterSeriesB_);
    scatterChart_->show();

    pieChart_ = new TpChart();
    pieChart_->setParent(this);
    pieChart_->setRect(520, 10, 500, 700);
    pieChart_->setTitle("Donut Pie Chart");
    pieChart_->setBackgroundColor(_RGB(255, 255, 255));
    pieChart_->setGridXVisible(false);
    pieChart_->setGridYVisible(false);
    enableChartInteraction(pieChart_, true);

    pieSeries_ = new TpPieSeries();
    pieSeries_->setName("Market Share");
    pieSeries_->setColor(_RGB(120, 120, 120));
    pieSeries_->setDonutVisible(true);
    pieSeries_->setDonutRatio(0.58);
    pieSeries_->setLabelsVisible(true);
    pieSeries_->setPercentVisible(true);
    pieSeries_->setLabelColor(_RGB(35, 35, 35));
    pieSeries_->setLabelSize(10);
    pieSeries_->setStartAngle(270);
    pieSeries_->setExplodedIndex(1);
    pieSeries_->setExplodeDistance(16);
    pieSeries_->addSlice("Desktop", 42.0, _RGB(0, 120, 212));
    pieSeries_->addSlice("Mobile", 31.0, _RGB(255, 140, 0));
    pieSeries_->addSlice("Tablet", 12.0, _RGB(0, 184, 148));
    pieSeries_->addSlice("TV", 8.0, _RGB(193, 80, 246));
    pieSeries_->addSlice("Other", 7.0, _RGB(230, 126, 34));

    pieChart_->addSeries(pieSeries_);
    pieChart_->show();
}

bool MainWindowServiceAdvanced::onPaintEvent(TpPaintEvent *event) {
    return TpMainWindow::onPaintEvent(event);
}

bool MainWindowServiceAdvanced::onActiveEvent(TpActiveEvent *event) {
    return TpMainWindow::onActiveEvent(event);
}

bool MainWindowServiceAdvanced::appChange(int32_t, int32_t, int32_t, int32_t, int32_t, uint8_t, int32_t) {
    return true;
}
