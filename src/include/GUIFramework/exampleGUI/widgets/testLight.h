#ifndef __TEST_LIGHT_H
#define __TEST_LIGHT_H

#include "TpChildWidget.h"
#include "TpEvent.h"
#include "FrameworkGlobal.h"

EXAMPLE_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ItestLightData);
class testLight : public TpChildWidget
{
public:
    testLight(TpChildWidget *parent);

    virtual ~testLight();

public:
	virtual bool onMousePressEvent(TpMouseEvent *event) override;

    virtual bool onPaintEvent(TpObjectPaintEvent *event) override;

private:
    ItestLightData *data_;
};

EXAMPLE_GUI_NAMESPACE_END

#endif
