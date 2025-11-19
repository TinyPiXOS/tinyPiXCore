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

    int32_t borderWidth = 2;
    int32_t whiteBorderColor = _RGB(255, 255, 255);
    int32_t whiteColor = _RGB(255, 255, 255);
    int32_t blackBorderColor = _RGB(159, 159, 159);
    int32_t blackColor = _RGB(0, 0, 0);

    TpBattery::BatteryStyle style = TpBattery::White;
};

TpBattery::TpBattery(TpWidget *parent)
    : TpWidget(parent)
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
    TpBatteryData *batteryData = static_cast<TpBatteryData *>(data_);

    // TpWidget::onPaintEvent(event);

    int32_t powerColor = batteryData->style == TpBattery::White ? batteryData->whiteColor : batteryData->blackColor;
    int32_t borderColor = batteryData->style == TpBattery::White ? batteryData->whiteBorderColor : batteryData->blackBorderColor;
    int32_t fontColor = batteryData->style == TpBattery::White ? batteryData->blackColor : batteryData->whiteColor;

    TpPainter *painter = event->painter();
    double linew = batteryData->borderWidth;

    double headWidth = width() / 12;
    double batteryWidth = width() - headWidth;
    // 起始坐标偏移线宽，保证绘制在区域内
    TpRect batteryRect = TpRect(TpPoint(linew, linew), TpPoint(batteryWidth, height() - linew));

    // 绘制边框
    double borderRadius = batteryRect.height() / 30;
    painter->pen().setColor(borderColor);
    painter->pen().setWidth(linew);
    painter->drawRect(batteryRect, borderRadius);

    // 电量
    if (batteryData->value != 0)
    {
        int32_t powerColoer = batteryData->value > batteryData->alarmValue ? powerColor : batteryData->alarmColor;

        double margin = std::min(width(), height()) / 50.0;
        margin = std::max(margin, linew);
        // margin=0;

        // double linew = 0;

        double unit = (batteryRect.width() - (margin * 2)) / 100;
        TpPointF topLeft(batteryRect.left() + margin, batteryRect.top() + margin);
        TpPointF bottomRight(batteryData->value * unit + margin + 2, batteryRect.bottom() - margin);
        TpRectF rect(topLeft, bottomRight);

        double bgRadius = rect.height() / 30.0;

        painter->pen().setColor(powerColoer);
        painter->setBrush(TpBrush(powerColoer));

        painter->drawRect(rect.x(), rect.y(), rect.width(), rect.height(), borderRadius);
    }

    // 绘制数值
    fontColor = batteryData->value > batteryData->alarmValue ? fontColor : batteryData->alarmColor;
    TpString text = TpString::number(batteryData->value);
    // 设置电量文字字体、大小
    // TpFont font(DEFAULT_FONT_FAMILY, batteryRect.w / 10);
    TpFont font(DEFAULT_FONT_FAMILY, batteryRect.height() * 0.55);
    font.setText(text);
    font.setFontColor(fontColor, fontColor);
    uint32_t textX = batteryRect.left() + (batteryRect.width() - font.pixelWidth()) / 2.0;
    uint32_t textY = batteryRect.top() + (batteryRect.height() - font.pixelHeight()) / 2.0 + 1;
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
