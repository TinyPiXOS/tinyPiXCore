#ifndef __MAIN_WINDOW_SERVICE_H
#define __MAIN_WINDOW_SERVICE_H

#include "TpMainWindow.h"
#include "ExamplesAppGlobal.h"
#include "TpTestLight.h"

class MainWindowService
    : public TpMainWindow
{
public:
    MainWindowService();
    virtual ~MainWindowService();

public:
    virtual bool appChange(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require) override;

protected:
    virtual bool onActiveEvent(TpActiveEvent *event) override;

    virtual bool onPaintEvent(TpPaintEvent *event) override;

private:
    TpTestLight *testLight_;
};

#endif