#include "TpButton.h"
#include "TpCanvas.h"
#include "TpLinearGradient.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

TpButton::TpButton(TpChildWidget *parent)
    : ::TpButton(parent)
{
    setEnableBackGroundColor(false);

    setRoundCorners(20);
}

TpButton::~TpButton()
{
}

bool TpButton::onPaintEvent(TpObjectPaintEvent *event)
{
    TpCanvas *painter = event->canvas();

    if (checked())
    {
        // 绘制渐变背景
        // 设置渐变背景
        TpLinearGradient lineGradient(0, 0, width(), height());
        lineGradient.setColorAt(0, _RGB(107, 80, 246));
        lineGradient.setColorAt(1, _RGB(204, 143, 237));

        painter->setGradient(&lineGradient);
        std::cout << "roundCorners( " << roundCorners() << std::endl;
        painter->roundedBox(0, 0, width(), height(), roundCorners(), _RGB(204, 143, 237));

        // 重置渐变效果
        painter->setGradient(nullptr);
    }
    else
    {
        painter->roundedBox(0, 0, width(), height(), roundCorners(), _RGBA(60, 60, 67, 80));
    }

    ::TpButton::onPaintEvent(event);

    return true;
}

SMART_DEVICE_GUI_NAMESPACE_END
