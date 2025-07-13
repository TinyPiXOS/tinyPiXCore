#ifndef __TEST_LIGHT_H
#define __TEST_LIGHT_H

#include "tpChildWidget.h"
#include "tpEvent.h"

class testLight : public tpChildWidget
{
public:
    testLight(tpChildWidget *parent);

    virtual ~testLight();

public:
    virtual bool onMousePressEvent(tpMouseEvent *event) override;

    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

private:
    int maxCount_;
    int count_;
};

#endif
