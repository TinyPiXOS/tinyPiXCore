#include "tpBattery.h"
#include "tpCanvas.h"
#include "tpEvent.h"
#include "tpFont.h"

struct tpBatteryData
{
    // int32_t borderColor = _RGB(80, 80, 80);
    // int32_t powerColor = _RGB(65, 205, 82);
    int32_t alarmColor = _RGB(250, 118, 113);

    int32_t alarmValue = 20;
    int32_t value = 0;
};

tpBattery::tpBattery(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    data_ = new tpBatteryData();

    refreshBaseCss();
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
    tpBatteryData *batteryData = static_cast<tpBatteryData *>(data_);

    tpChildWidget::onPaintEvent(event);

    int32_t powerColor = currentStatusCss()->color();
    int32_t borderColor = currentStatusCss()->borderColor();

    tpCanvas *painter = event->canvas();
    double linew = 1;

    double headWidth = width() / 12;
    double batteryWidth = width() - headWidth;
    // ItpRect batteryRect = ItpRect(ItpPoint(5, 5), ItpPoint(batteryWidth, height() - 5));
    ItpRect batteryRect = ItpRect(ItpPoint(0, 0), ItpPoint(batteryWidth, height()));

    // 边框
    double borderRadius = batteryRect.h / 30;
    painter->roundedRectangle(batteryRect.x, batteryRect.y, batteryRect.x + batteryRect.w, batteryRect.y + batteryRect.h, borderRadius, borderColor, linew);

    // 电量
    if (batteryData->value != 0)
    {
        int32_t powerColoer = batteryData->value > batteryData->alarmValue ? powerColor : batteryData->alarmColor;

        double margin = std::min(width(), height()) / 50.0;
        margin = std::max(margin, linew);

        double unit = (batteryRect.w - (margin * 2)) / 100;
        ItpPointF topLeft(batteryRect.left() + margin, batteryRect.top() + margin);
        ItpPointF bottomRight(batteryData->value * unit + margin, batteryRect.bottom() - margin);
        ItpRectF rect(topLeft, bottomRight);

        double bgRadius = rect.h / 30;
        // painter->setBrush(powerColoer);
        painter->roundedBox(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, bgRadius, powerColoer);
    }

    // 绘制数值
    int32_t fontColoer = batteryData->value > batteryData->alarmValue ? _RGB(64, 65, 66) : batteryData->alarmColor;
    tpString text = tpString::number(batteryData->value);
    // 设置电量文字字体、大小
    // tpFont font(DEFAULT_FONT_FAMILY, batteryRect.w / 10);
    tpFont font(DEFAULT_FONT_FAMILY, height() / 2);
    font.setText(text);
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
