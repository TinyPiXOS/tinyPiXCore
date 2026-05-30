#include "MainWindowService.h"
#include "TpChart.h"
#include "TpSeries.h"
#include <TpEvent.h>
#include <TpFile.h>
#include <cmath>

#ifndef _RGB
#define _RGB(r, g, b) ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#endif

MainWindowService::MainWindowService()
    : TpMainWindow(), m_scatterChart(NULL), m_pieChart(NULL), m_scatterSeriesA(NULL), m_scatterSeriesB(NULL), m_pieSeries(NULL)
{
    initCharts();
}

MainWindowService::~MainWindowService() {
    if (m_scatterChart) {
        delete m_scatterChart;
        m_scatterChart = NULL;
    }
    if (m_pieChart) {
        delete m_pieChart;
        m_pieChart = NULL;
    }
}

void MainWindowService::initCharts() {
    m_scatterChart = new TpChart();
    m_scatterChart->setParent(this);
    m_scatterChart->setRect(10, 10, 500, 700);
    m_scatterChart->setAxisLabels("X", "Y");
    m_scatterChart->setTitle("Scatter Chart");
    m_scatterChart->setBackgroundColor(_RGB(255, 255, 255));
    m_scatterChart->axisX()->setAutoRange(true);
    m_scatterChart->axisX()->setTickCount(7);
    m_scatterChart->axisX()->setRollingMode(false);
    m_scatterChart->axisY()->setAutoRange(true);
    m_scatterChart->axisY()->setSymmetric(true);
    m_scatterChart->axisY()->setTickCount(7);
    m_scatterChart->setGridXVisible(true);
    m_scatterChart->setGridYVisible(true);

    m_scatterSeriesA = new TpScatterSeries();
    m_scatterSeriesA->setName("Cluster A");
    m_scatterSeriesA->setColor(_RGB(0, 184, 148));
    m_scatterSeriesA->setBorderColor(_RGB(0, 120, 96));
    m_scatterSeriesA->setPointSize(6);
    m_scatterSeriesA->setLabelsVisible(true);
    m_scatterSeriesA->setLabelColor(_RGB(60, 60, 60));
    m_scatterSeriesA->setLabelSize(9);
    m_scatterSeriesA->addPoint(-6.0, 2.0);
    m_scatterSeriesA->addPoint(-4.8, 2.8);
    m_scatterSeriesA->addPoint(-3.6, 1.5);
    m_scatterSeriesA->addPoint(-2.4, 3.4);
    m_scatterSeriesA->addPoint(-1.2, 2.6);
    m_scatterSeriesA->addPoint(0.0, 4.0);

    m_scatterSeriesB = new TpScatterSeries();
    m_scatterSeriesB->setName("Cluster B");
    m_scatterSeriesB->setColor(_RGB(255, 140, 0));
    m_scatterSeriesB->setBorderColor(_RGB(200, 100, 0));
    m_scatterSeriesB->setPointSize(5);
    m_scatterSeriesB->setLabelsVisible(false);
    m_scatterSeriesB->addPoint(1.0, -1.0);
    m_scatterSeriesB->addPoint(2.0, -2.2);
    m_scatterSeriesB->addPoint(3.0, -1.4);
    m_scatterSeriesB->addPoint(4.0, -3.0);
    m_scatterSeriesB->addPoint(5.0, -1.8);
    m_scatterSeriesB->addPoint(6.0, -2.6);

    m_scatterChart->addSeries(m_scatterSeriesA);
    m_scatterChart->addSeries(m_scatterSeriesB);
    m_scatterChart->show();

    m_pieChart = new TpChart();
    m_pieChart->setParent(this);
    m_pieChart->setRect(520, 10, 500, 700);
    m_pieChart->setTitle("Donut Pie Chart");
    m_pieChart->setBackgroundColor(_RGB(255, 255, 255));
    m_pieChart->setGridXVisible(false);
    m_pieChart->setGridYVisible(false);

    m_pieSeries = new TpPieSeries();
    m_pieSeries->setName("Market Share");
    m_pieSeries->setColor(_RGB(120, 120, 120));
    m_pieSeries->setDonutVisible(true);
    m_pieSeries->setDonutRatio(0.58);
    m_pieSeries->setLabelsVisible(true);
    m_pieSeries->setPercentVisible(true);
    m_pieSeries->setLabelColor(_RGB(35, 35, 35));
    m_pieSeries->setLabelSize(10);
    m_pieSeries->setStartAngle(270);
    m_pieSeries->setExplodedIndex(1);
    m_pieSeries->setExplodeDistance(16);
    m_pieSeries->addSlice("Desktop", 42.0, _RGB(0, 120, 212));
    m_pieSeries->addSlice("Mobile", 31.0, _RGB(255, 140, 0));
    m_pieSeries->addSlice("Tablet", 12.0, _RGB(0, 184, 148));
    m_pieSeries->addSlice("TV", 8.0, _RGB(193, 80, 246));
    m_pieSeries->addSlice("Other", 7.0, _RGB(230, 126, 34));

    m_pieChart->addSeries(m_pieSeries);
    m_pieChart->show();

    TpString cssPath = "./chart_style.css";
    TpFile cssFile(cssPath);
    TpString cssContent;
    if (cssFile.exists()) {
        cssFile.open(TpFile::ReadOnly);
        if (cssFile.isOpen()) {
            cssContent = cssFile.readAll();
        }
    }

    m_scatterChart->setStyleSheet(cssContent);
    m_pieChart->setStyleSheet(cssContent);
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
