#include "TpBattery.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpFont.h"
#include "TpPointF.h"
#include "TpRectF.h"

struct TpBatteryData
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

    TpBattery::BatteryStyle style = TpBattery::White;
};

TpBattery::TpBattery(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    data_ = new TpBatteryData();

    // refreshBaseCss();
}

TpBattery::~TpBattery()
{
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);
    if (batteryData)
    {
        delete batteryData;
        batteryData = nullptr;
        data_ = nullptr;
    }
}

void TpBattery::setStyle(const BatteryStyle &style)
{
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);
    batteryData->style = style;
    update();
}

TpBattery::BatteryStyle TpBattery::style()
{
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);
    return batteryData->style;
}

void TpBattery::setValue(const int32_t &value)
{
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);

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

int32_t TpBattery::value()
{
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);
    return batteryData->value;
}

void TpBattery::setAlamValue(const int32_t &value)
{
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);
    batteryData->alarmValue = value;
}

int32_t TpBattery::alamValue()
{
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);
    return batteryData->alarmValue;
}

void TpBattery::setAlamColor(const int32_t &color)
{
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);
    batteryData->alarmColor = color;
}

int32_t TpBattery::alamColor()
{
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);
    return batteryData->alarmColor;
}

bool TpBattery::onPaintEvent(TpPaintEvent *event)
{
    static uint64_t paintCount = 0;
    // std::cout << "TpBattery::onPaintEvent " << paintCount++ << std::endl;

    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);

    // TpChildWidget::onPaintEvent(event);

    int32_t powerColor = batteryData->style == TpBattery::White ? batteryData->whiteColor : batteryData->blackColor;
    int32_t borderColor = batteryData->style == TpBattery::White ? batteryData->whiteBorderColor : batteryData->blackBorderColor;
    int32_t fontColor = batteryData->style == TpBattery::White ? batteryData->blackColor : batteryData->whiteColor;

    TpPainter *painter = event->painter();
    double linew = batteryData->borderWidth;

    double headWidth = width() / 12;
    double batteryWidth = width() - headWidth;
    // TpRect batteryRect = TpRect(TpPoint(5, 5), TpPoint(batteryWidth, height() - 5));
    TpRect batteryRect = TpRect(TpPoint(0, 0), TpPoint(batteryWidth, height()));

    // 边框
    double borderRadius = batteryRect.height() * 0.3;

    painter->pen().setColor(borderColor);
    painter->pen().setWidth(linew);

    painter->drawRect(batteryRect.x(), batteryRect.y(), batteryRect.width(), batteryRect.height(), borderRadius);

    // 电量
    if (batteryData->value != 0)
    {
        int32_t powerColoer = batteryData->value > batteryData->alarmValue ? powerColor : batteryData->alarmColor;

        double margin = std::min(width(), height()) * 0.06;
        margin = std::max(margin, linew);

        double unit = (batteryRect.width() - (margin * 2) - linew * 2) / 100;
        TpPointF topLeft(batteryRect.left() + margin + linew, batteryRect.top() + margin + linew);
        TpPointF bottomRight(batteryData->value * unit + margin + linew, batteryRect.bottom() - margin - linew);
        TpRectF rect(topLeft, bottomRight);

        double bgRadius = rect.height() * 0.3;

        painter->pen().setColor(powerColoer);
        painter->setBrush(TpBrush(powerColoer));

        painter->drawRect(rect.x(), rect.y(), rect.width(), rect.height(), bgRadius);
    }

    // 绘制数值
    fontColor = batteryData->value > batteryData->alarmValue ? fontColor : batteryData->alarmColor;
    TpString text = TpString::number(batteryData->value);
    // 设置电量文字字体、大小
    // TpFont font(DEFAULT_FONT_FAMILY, batteryRect.w / 10);
    TpFont font(DEFAULT_FONT_FAMILY, height() / 2);
    font.setText(text);
    font.setFontColor(fontColor, fontColor);
    uint32_t textX = (batteryRect.width() - font.pixelWidth()) / 2.0;
    uint32_t textY = (batteryRect.height() - font.pixelHeight()) / 2.0 + 1;
    painter->drawText(font, textX, textY, text);

    // 绘制头部
    TpPointF headRectTopLeft(batteryRect.right(), height() / 3);
    TpPointF headRectBottomRight(width(), height() - height() / 3);
    TpRectF headRect(headRectTopLeft, headRectBottomRight);
    double headRadius = headRect.height() / 30;

    painter->pen().setColor(borderColor);
    painter->setBrush(TpBrush(borderColor));

    painter->drawRect(headRect.x(), headRect.y(), headRect.width(), headRect.height(), headRadius);

    return true;
}
