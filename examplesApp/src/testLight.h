#ifndef __TEST_LIGHT_H
#define __TEST_LIGHT_H

#include "TpChildWidget.h"
#include "TpEvent.h"

class testLight : public TpChildWidget
{
public:
    testLight(TpChildWidget *parent);

    virtual ~testLight();

public:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;

    virtual bool onPaintEvent(TpObjectPaintEvent *event) override;

private:
    int maxCount_;
    int count_;
};

#endif
