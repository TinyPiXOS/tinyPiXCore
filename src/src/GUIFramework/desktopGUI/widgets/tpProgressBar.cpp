#include "GUIFramework/desktopGUI/widgets/tpProgressBar.h"
#include "tpCanvas.h"
#include "tpFont.h"

#include <sstream>
#include <iomanip>

DESKTOP_GUI_NAMESPACE_BEGIN

tpProgressBar::tpProgressBar(tpChildWidget*parent)
    : tpChildWidget(parent), textVisible_(true), max_(100), min_(0), value_(0), bgColor_(_RGB(231, 231, 231)), fillColor_(_RGB(0, 82, 217)), isNoBorder_(true)
{
    setVisible(true);
}

tpProgressBar::~tpProgressBar()
{
}

void tpProgressBar::setTextVisible(const bool &visible)
{
    textVisible_ = visible;
}

void tpProgressBar::setMax(const uint32_t &max)
{
    max_ = max;
}

void tpProgressBar::setMin(const uint32_t &min)
{
    min_ = min;
}

void tpProgressBar::setValue(const uint32_t &value)
{
    value_ = value;
    if (value_ > max_)
        value_ = max_;

    update();
}

void tpProgressBar::setBackgroundColor(const int32_t &color)
{
    bgColor_ = color;

    update();
}

void tpProgressBar::setFillColor(const int32_t &color)
{
    fillColor_ = color;

    update();
}

void tpProgressBar::setNoBorder(const bool &noBorder)
{
    isNoBorder_ = noBorder;
}

void tpProgressBar::setBorderColor(const int32_t &color)
{
    borderColor_ = color;
}

bool tpProgressBar::onPaintEvent(tpObjectPaintEvent *event)
{
    tpCanvas *paintCanvas = event->canvas();

    // 计算百分比
    double percentValue = 1.0 * (value_ - min_) / (max_ - min_);

    uint32_t barWidth = rect().w - 2;

    // 文字显示，进度条宽度要短一点，流出文本位置
    if (textVisible_)
    {
        uint32_t percentValueInt = 100 * percentValue;

        tpString percentText = std::to_string(percentValueInt) + "%  ";
        tpFont textFont;
        textFont.setFontForeColor(_RGB(0, 0, 0));
        textFont.setText(percentText);

        barWidth -= textFont.pixelWidth();
        uint32_t textY = (rect().h - 2 - textFont.pixelHeight()) / 2.0;

        // paintCanvas->renderText(textFont, barWidth, textY, percentText);
        paintCanvas->renderText(textFont, rect().w - textFont.pixelWidth(), 0, percentText);
    }

    // 绘制底色
    paintCanvas->roundedBox(1, 1, barWidth - 1, rect().h - 2, (rect().h - 2) * 0.45, bgColor_);

    // 绘制填充色
    uint32_t fillWidth = barWidth * percentValue;
    if (fillWidth != 0)
        paintCanvas->roundedBox(1, 1, fillWidth, rect().h - 2, (rect().h - 2) * 0.45, fillColor_);

    // 绘制边框
    if (!isNoBorder_)
    {
        // paintCanvas->rectangle(1, 1, barWidth - 1, rect().h - 1, borderColor_);
        paintCanvas->roundedRectangle(0, 0, barWidth, rect().h - 1, (rect().h - 1) * 0.45, borderColor_);
    }

    return true;
}

DESKTOP_GUI_NAMESPACE_END