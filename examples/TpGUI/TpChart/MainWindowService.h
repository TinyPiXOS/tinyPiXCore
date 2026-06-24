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

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onActiveEvent(TpActiveEvent *event) override;

public:
    bool appChange(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require);

    void onTimeout();
    void onBarTimeout();

private:
    void initCharts();

private:
    TpChart* chartLine1_;
    TpChart* chartBar1_;
    TpChart* chartLine2_;
    TpChart* chartBar2_;
    TpLineSeries* lineSeries1_;
    TpLineSeries* lineSeries2_;
    TpBarSeries* barSeries1_;
    TpLineSeries* lineSeries3_;
    TpBarSeries* barSeries2_;
    double currentX_;
    TpTimer* timer_;
    TpTimer* barTimer_;
};

#endif // MAINWINDOWSERVICE_H
