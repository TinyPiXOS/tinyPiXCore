#ifndef MAINWINDOWSERVICEADVANCED_H
#define MAINWINDOWSERVICEADVANCED_H

#include <TpMainWindow.h>
#include <cstdint>

class TpPaintEvent;
class TpActiveEvent;
class TpChart;
class TpScatterSeries;
class TpPieSeries;

class MainWindowServiceAdvanced : public TpMainWindow {
public:
    MainWindowServiceAdvanced();
    virtual ~MainWindowServiceAdvanced();

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onActiveEvent(TpActiveEvent *event) override;

public:
    bool appChange(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require);

private:
    void initCharts();

private:
    TpChart* scatterChart_;
    TpChart* pieChart_;
    TpScatterSeries* scatterSeriesA_;
    TpScatterSeries* scatterSeriesB_;
    TpPieSeries* pieSeries_;
};

#endif // MAINWINDOWSERVICEADVANCED_H
