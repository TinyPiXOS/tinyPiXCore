#ifndef MAINWINDOWSERVICE_H
#define MAINWINDOWSERVICE_H

#include <TpMainWindow.h>
#include <cstdint>

class TpPaintEvent;
class TpActiveEvent;
class TpChart;
class TpLineSeries;
class TpBarSeries;
class TpTimer;

class MainWindowService : public TpMainWindow {
public:
    MainWindowService();
    virtual ~MainWindowService();

    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onActiveEvent(TpActiveEvent *event) override;
    bool appChange(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require);

    void onTimeout();
    void onBarTimeout();

private:
    void initCharts();

private:
    TpChart* m_chartLine_1;
    TpChart* m_chartBar_1;
    TpChart* m_chartLine_2;
    TpChart* m_chartBar_2;
    TpLineSeries* m_lineSeries1;
    TpLineSeries* m_lineSeries2;
    TpBarSeries* m_barSeries_1;
    TpLineSeries* m_lineSeries3;
    TpBarSeries* m_barSeries_2;
    double m_currentX;
    TpTimer* m_timer;
    TpTimer* m_barTimer;
};

#endif // MAINWINDOWSERVICE_H
