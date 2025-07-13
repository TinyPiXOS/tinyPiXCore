#ifndef __TEST_LIGHT_H
#define __TEST_LIGHT_H

#include "tpChildWidget.h"
#include "tpEvent.h"
#include "FrameworkGlobal.h"

EXAMPLE_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ItestLightData);
class testLight : public tpChildWidget
{
public:
    testLight(tpChildWidget *parent);

    virtual ~testLight();

public:
	virtual bool onMousePressEvent(tpMouseEvent *event) override;

    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

private:
    ItestLightData *data_;
};

EXAMPLE_GUI_NAMESPACE_END

#endif
