#ifndef __TEST_LIGHT_H
#define __TEST_LIGHT_H

#include "TpWidget.h"
#include "TpEvent.h"

class TpTestLight : public TpWidget
{
public:
    TpTestLight(TpWidget *parent);

    virtual ~TpTestLight();

public:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;

    virtual bool onPaintEvent(TpPaintEvent *event) override;

private:
    int maxCount_;
    int count_;
};

#endif
