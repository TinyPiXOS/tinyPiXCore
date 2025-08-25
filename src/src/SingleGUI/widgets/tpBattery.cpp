#include "tpBattery.h"
#include "TpCanvas.h"
#include "tpEvent.h"
#include "tpFont.h"

struct tpBatteryData
{
    // int32_t borderColor = _RGB(80, 80, 80);
    // int32_t powerColor = _RGB(65, 205, 82);
    int32_t alarmColor = _RGB(250, 118, 113);

    int32_t alarmValue = 20;
    int32_t value = 0;

    int32_t borderWidth = 1;
    int32_t whiteBorderColor = _RGB(255, 255, 255);
    int32_t whiteColor = _RGB(255, 255, 255);
    int32_t blackBorderColor = _RGB(159, 159, 159);
    int32_t blackColor = _RGB(0, 0, 0);

    tpBattery::BatteryStyle style = tpBattery::White;
};

tpBattery::tpBattery(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    data_ = new tpBatteryData();

    // refreshBaseCss();
}

tpBattery::~tpBattery()
{
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);
    if (batteryData)
    {
        delete batteryData;
        batteryData = nullptr;
        data_ = nullptr;
    }
}

void tpBattery::setStyle(const BatteryStyle &style)
{
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);
    batteryData->style = style;
    update();
}

tpBattery::BatteryStyle tpBattery::style()
{
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);
    return batteryData->style;
}

void tpBattery::setValue(const int32_t &value)
{
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);

    if (value == batteryData->value)
        return;

    if (value < 0)
        batteryData->value = 0;
    else if (value > 100)
        batteryData->value = 100;
    else
        batteryData->value = value;

    update();
}

int32_t tpBattery::value()
{
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);
    return batteryData->value;
}

void tpBattery::setAlamValue(const int32_t &value)
{
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);
    batteryData->alarmValue = value;
}

int32_t tpBattery::alamValue()
{
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);
    return batteryData->alarmValue;
}

void tpBattery::setAlamColor(const int32_t &color)
{
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);
    batteryData->alarmColor = color;
}

int32_t tpBattery::alamColor()
{
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);
    return batteryData->alarmColor;
}

bool tpBattery::onPaintEvent(tpObjectPaintEvent *event)
{
    static uint64_t paintCount = 0;
    // std::cout << "tpBattery::onPaintEvent " << paintCount++ << std::endl;

    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);

    // tpChildWidget::onPaintEvent(event);

    int32_t powerColor = batteryData->style == tpBattery::White ? batteryData->whiteColor : batteryData->blackColor;
    int32_t borderColor = batteryData->style == tpBattery::White ? batteryData->whiteBorderColor : batteryData->blackBorderColor;
    int32_t fontColor = batteryData->style == tpBattery::White ? batteryData->blackColor : batteryData->whiteColor;

    TpCanvas *painter = event->canvas();
    double linew = batteryData->borderWidth;

    double headWidth = width() / 12;
    double batteryWidth = width() - headWidth;
    // ItpRect batteryRect = ItpRect(ItpPoint(5, 5), ItpPoint(batteryWidth, height() - 5));
    ItpRect batteryRect = ItpRect(ItpPoint(0, 0), ItpPoint(batteryWidth, height()));

    // 边框
    double borderRadius = batteryRect.h * 0.3;
    painter->roundedRectangle(batteryRect.x, batteryRect.y, batteryRect.x + batteryRect.w, batteryRect.y + batteryRect.h, borderRadius, borderColor, linew);

    // 电量
    if (batteryData->value != 0)
    {
        int32_t powerColoer = batteryData->value > batteryData->alarmValue ? powerColor : batteryData->alarmColor;

        double margin = std::min(width(), height()) * 0.06;
        margin = std::max(margin, linew);

        double unit = (batteryRect.w - (margin * 2) - linew * 2) / 100;
        ItpPointF topLeft(batteryRect.left() + margin + linew, batteryRect.top() + margin + linew);
        ItpPointF bottomRight(batteryData->value * unit + margin + linew, batteryRect.bottom() - margin - linew);
        ItpRectF rect(topLeft, bottomRight);

        double bgRadius = rect.h * 0.3;
        // painter->setBrush(powerColoer);
        painter->roundedBox(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, bgRadius, powerColoer);
    }

    // 绘制数值
    fontColor = batteryData->value > batteryData->alarmValue ? fontColor : batteryData->alarmColor;
    tpString text = tpString::number(batteryData->value);
    // 设置电量文字字体、大小
    // tpFont font(DEFAULT_FONT_FAMILY, batteryRect.w / 10);
    tpFont font(DEFAULT_FONT_FAMILY, height() / 2);
    font.setText(text);
    font.setFontColor(fontColor, fontColor);
    uint32_t textX = (batteryRect.w - font.pixelWidth()) / 2.0;
    uint32_t textY = (batteryRect.h - font.pixelHeight()) / 2.0 + 1;
    painter->renderText(font, textX, textY, text);

    // 绘制头部
    ItpPointF headRectTopLeft(batteryRect.right(), height() / 3);
    ItpPointF headRectBottomRight(width(), height() - height() / 3);
    ItpRectF headRect(headRectTopLeft, headRectBottomRight);
    double headRadius = headRect.h / 30;
    // painter->setPen(Qt::NoPen);
    // painter->setBrush(m_dPtr->borderColor);
    painter->roundedBox(headRect.x, headRect.y, headRect.x + headRect.w, headRect.y + headRect.h, headRadius, borderColor);

    return true;
}
