#include "AppOperateMaskWindow.h"
#include "TpPainter.h"

AppOperateMaskWindow::AppOperateMaskWindow()
    : TpDialog()
{
    setWindowOpacity(0.8);
    setVisible(false);
}

AppOperateMaskWindow::~AppOperateMaskWindow()
{
}

void AppOperateMaskWindow::setAppRect(const TpRect &rect, const uint32_t &roundCorners)
{
    appRect_ = rect;
    roundCorners_ = roundCorners;
    update();
}

bool AppOperateMaskWindow::onMouseRleaseEvent(TpMouseEvent *event)
{
    close();
    return true;
}

bool AppOperateMaskWindow::onPaintEvent(TpPaintEvent *event)
{
    TpPainter *painter = event->painter();

    TpHollowMask hollowData;
    hollowData.addRectHollow(appRect_, roundCorners_);
    // painter->hollowBox(0, 0, width(), height(), _RGBA(255, 255, 255, 204), hollowData);

    painter->setPen(_RGB(255, 255, 255));
    painter->setBrush(TpBrush(_RGB(255, 255, 255)));
    painter->drawRect(0, 0, width(), height(), 0, hollowData);

    return true;
}
