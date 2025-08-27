#include "TpButton.h"
#include "TpCanvas.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

TpButton::TpButton(TpChildWidget *parent)
    : ::TpButton(parent)
{
}

TpButton::~TpButton()
{
}

bool TpButton::onPaintEvent(TpObjectPaintEvent *event)
{
    ::TpButton::onPaintEvent(event);

    return true;
}

SMART_DEVICE_GUI_NAMESPACE_END
