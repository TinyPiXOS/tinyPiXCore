#ifndef __MAIN_WINDOW_SERVICE_H
#define __MAIN_WINDOW_SERVICE_H

#include "tpFixScreen.h"
#include "examplesAppGlobal.h"
#include "tpButton.h"

class mainWindowService
    : public tpFixScreen
{
public:
    mainWindowService();
    virtual ~mainWindowService();

public:
    virtual bool appChange(int32_t id, int32_t pid, int32_t rotate, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require) override;

protected:
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool onRotateEvent(tpObjectRotateEvent *event) override;

    virtual bool onActiveEvent(tpObjectActiveEvent *event) override;

private:
    tpButton *testButton_;
};

#endif