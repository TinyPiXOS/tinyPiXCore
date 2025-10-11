#ifndef __TEST_LIGHT_H
#define __TEST_LIGHT_H

#include "TpChildWidget.h"
#include "TpEvent.h"

class TpTestLight : public TpChildWidget
{
public:
    TpTestLight(TpChildWidget *parent);

    virtual ~TpTestLight();

public:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;

    virtual bool onPaintEvent(TpPaintEvent *event) override;

private:
    int maxCount_;
    int count_;
};

#endif
