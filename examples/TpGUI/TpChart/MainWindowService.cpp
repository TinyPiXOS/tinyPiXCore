#include "MainWindowService.h"
#include "TpChart.h"
#include "TpTimer.h"
#include <TpEvent.h>
#include <cmath>
#include <TpFile.h>

#ifndef _RGB
#define _RGB(r, g, b) ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#endif

MainWindowService::MainWindowService()
    : TpMainWindow(), m_chartLine(NULL), m_chartBar(NULL), m_lineSeries1(NULL), m_lineSeries2(NULL), m_barSeries(NULL), m_currentX(-40.0), m_timer(NULL)
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
    if (m_chartLine) {
        delete m_chartLine;
        m_chartLine = NULL;
    }
    if (m_chartBar) {
        delete m_chartBar;
        m_chartBar = NULL;
    }
}

void MainWindowService::initCharts() {
    // 折线图 - 上方
    m_chartLine = new TpChart();
    m_chartLine->setParent(this);
    m_chartLine->setRect(100, 10, 700, 350);
    m_chartLine->setAxisLabels("Time (seconds)", "Usage (%)");
    m_chartLine->setTitle("System Monitor - Line Chart");
    m_chartLine->setBackgroundColor(_RGB(255, 255, 255));

    // 加载并应用 CSS 样式
    TpString cssPath = "./data/chart_style.css";
    TpFile cssFile(cssPath);
    if (cssFile.exists()) {
        cssFile.open(TpFile::ReadOnly);
        if (cssFile.isOpen()) {
            TpString cssContent = cssFile.readAll();
            m_chartLine->setStyleSheet(cssContent);
        }
    }

    m_chartLine->axisX()->setAutoRange(true);
    m_chartLine->axisY()->setAutoRange(true);

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

    // 柱状图 - 下方
    m_chartBar = new TpChart();
    m_chartBar->setParent(this);
    m_chartBar->setRect(100, 400, 700, 300);
    m_chartBar->setAxisLabels("Category", "Value");
    m_chartBar->setTitle("System Monitor - Bar Chart");
    m_chartBar->setBackgroundColor(_RGB(255, 255, 255));

    // 加载并应用 CSS 样式
    if (cssFile.exists()) {
        cssFile.open(TpFile::ReadOnly);
        if (cssFile.isOpen()) {
            TpString cssContent = cssFile.readAll();
            m_chartBar->setStyleSheet(cssContent);
        }
    }

    m_chartBar->axisX()->setAutoRange(true);
    m_chartBar->axisY()->setAutoRange(true);

    m_barSeries = new TpBarSeries();
    m_barSeries->setName("System Value");
    m_barSeries->setColor(_RGB(0, 120, 212));
    m_barSeries->setGradientColor(_RGB(0, 80, 180));
    m_barSeries->setLabelsVisible(true);

    for (int i = 0; i < 200; ++i) {
        float dynamicAmp1 = 100 + 50 * std::sin(m_currentX * 0.1);
        float yValue1 = dynamicAmp1 * std::sin(m_currentX * 0.8);

        float dynamicAmp2 = 80 + 30 * std::cos(m_currentX * 0.15);
        float yValue2 = dynamicAmp2 * std::cos(m_currentX * 1.5);

        m_lineSeries1->addPoint(m_currentX, yValue1);
        m_lineSeries2->addPoint(m_currentX, yValue2);

        m_currentX += 0.1;
    }

    // 添加折线图系列
    m_chartLine->addSeries(m_lineSeries1);
    m_chartLine->addSeries(m_lineSeries2);
    m_chartLine->show();

    // 添加柱状图系列 - 初始数据
    for (int i = 0; i < 10; ++i) {
        double value = 50 + 30 * std::sin(i * 0.5);
        m_barSeries->addPoint(i, value);
    }
    m_chartBar->addSeries(m_barSeries);
    m_chartBar->show();

    // 添加一个定时器来更新柱状图
    m_barTimer = new TpTimer(200);
    connect(m_barTimer, timeout, this, &MainWindowService::onBarTimeout);
    m_barTimer->start();

    m_timer = new TpTimer(50);
    connect(m_timer, timeout, this, &MainWindowService::onTimeout);
    m_timer->start();
}

void MainWindowService::onTimeout() {
    if (m_lineSeries1 && m_lineSeries2 && m_chartLine) {
        m_currentX += 0.1;

        float dynamicAmp1 = 100 + 50 * std::sin(m_currentX * 0.1);
        float yValue1 = dynamicAmp1 * std::sin(m_currentX * 0.8);

        float dynamicAmp2 = 80 + 30 * std::cos(m_currentX * 0.15);
        float yValue2 = dynamicAmp2 * std::cos(m_currentX * 1.5);

        m_lineSeries1->addPoint(m_currentX, yValue1);
        m_lineSeries2->addPoint(m_currentX, yValue2);

        m_chartLine->update();
    }
}

void MainWindowService::onBarTimeout() {
    if (m_barSeries && m_chartBar) {
        // 更新柱状图数据：每个柱子的值随时间变化（替换旧数据）
        m_barSeries->clear();
        for (int i = 0; i < 10; ++i) {
            double value = 50 + 30 * std::sin(i * 0.5 + m_currentX * 0.2);
            m_barSeries->addPoint(i, value);
        }
        m_chartBar->update();
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
