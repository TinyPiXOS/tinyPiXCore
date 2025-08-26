#ifndef __MAIN_WINDOW_SERVICE_H
#define __MAIN_WINDOW_SERVICE_H

#include "TpFixScreen.h"
#include "examplesAppGlobal.h"
#include "TpButton.h"

class mainWindowService
    : public TpFixScreen
{
public:
    mainWindowService();
    virtual ~mainWindowService();

public:
    virtual bool appChange(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require) override;

protected:
    virtual bool onResizeEvent(TpObjectResizeEvent *event) override;

    virtual bool onActiveEvent(TpObjectActiveEvent *event) override;

    virtual bool onPaintEvent(TpObjectPaintEvent *event) override;

private:
    TpButton *testButton_;
};

#endif