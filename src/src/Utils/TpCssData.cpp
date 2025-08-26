#include "TpCssData.h"
#include "TpUtils.h"

TpCssData::TpCssData(const TpHash<TpString, TpString> &cssDataMap)
    : cssDataMap_(cssDataMap)
{
}

TpCssData::~TpCssData()
{
}

int32_t TpCssData::width()
{
    if (cssDataMap_.contains("width"))
        return cssDataMap_.value("width").toInt();
    return 10;
}

int32_t TpCssData::minimumWidth()
{
    if (cssDataMap_.contains("min-width"))
        return cssDataMap_.value("min-width").toInt();
    return 0;
}

int32_t TpCssData::maximumWidth()
{
    if (cssDataMap_.contains("max-width"))
        return cssDataMap_.value("max-width").toInt();
    return WIDGET_MAX_WIDTH;
}

int32_t TpCssData::height()
{
    if (cssDataMap_.contains("height"))
        return cssDataMap_.value("height").toInt();
    return 10;
}

int32_t TpCssData::minimumHeight()
{
    if (cssDataMap_.contains("min-height"))
        return cssDataMap_.value("min-height").toInt();
    return 0;
}

int32_t TpCssData::maximumHeight()
{
    if (cssDataMap_.contains("max-height"))
        return cssDataMap_.value("max-height").toInt();
    return WIDGET_MAX_HEIGHT;
}

int32_t TpCssData::color()
{
    if (cssDataMap_.contains("color"))
        return cssDataMap_.value("color").toInt();
    return 0;
}

int32_t TpCssData::subColor()
{
    if (cssDataMap_.contains("sub-color"))
        return cssDataMap_.value("sub-color").toInt();
    return 0;
}

int32_t TpCssData::backgroundColor()
{
    // int32_t color = 0;
    // if (cssDataMap_.contains("background"))
    // {
    //     color = cssDataMap_.value("background").toInt();
    //     std::cout << "background " << _R(color) << " " << _G(color) << " " << _B(color) << std::endl;
    // }
    // else if (cssDataMap_.contains("background-color"))
    // {
    //     color = cssDataMap_.value("background-color").toInt();
    //     std::cout << "background-color " << _R(color) << " " << _G(color) << " " << _B(color) << std::endl;
    // }

    if (cssDataMap_.contains("background"))
        return cssDataMap_.value("background").toInt();
    else if (cssDataMap_.contains("background-color"))
        return cssDataMap_.value("background-color").toInt();
    else
        return 0;
}

void TpCssData::setBackgroundColor(const int32_t &color)
{
    // std::cout << "setBackgroundColor " << std::endl;

    // std::cout << "setBackgroundColor " << _R(color) << " " << _G(color) << " " << _B(color) << std::endl;

    if (cssDataMap_.contains("background"))
        cssDataMap_["background"] = TpString::number(color);
    else if (cssDataMap_.contains("background-color"))
        cssDataMap_["background-color"] = TpString::number(color);
    else
        cssDataMap_["background"] = TpString::number(color);
}

int32_t TpCssData::borderColor()
{
    if (cssDataMap_.contains("border-color"))
        return cssDataMap_.value("border-color").toInt();
    return 0;
}

void TpCssData::setBorderColor(const int32_t &color)
{
    cssDataMap_["border-color"] = TpString::number(color);
}

int32_t TpCssData::borderWidth()
{
    if (cssDataMap_.contains("border-width"))
        return cssDataMap_.value("border-width").toInt();
    return 0;
}

int32_t TpCssData::fontSize()
{
    if (cssDataMap_.contains("font-size"))
        return cssDataMap_.value("font-size").toInt();
    return 10;
}

int32_t TpCssData::gap()
{
    if (cssDataMap_.contains("gap"))
        return cssDataMap_.value("gap").toInt();
    return 5;
}

int32_t TpCssData::padding()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();
    return 0;
}

int32_t TpCssData::paddingLeft()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();

    if (cssDataMap_.contains("padding-left"))
        return cssDataMap_.value("padding-left").toInt();
    return 0;
}

int32_t TpCssData::paddingRight()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();

    if (cssDataMap_.contains("padding-right"))
        return cssDataMap_.value("padding-right").toInt();
    return 0;
}

int32_t TpCssData::paddingTop()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();

    if (cssDataMap_.contains("padding-top"))
        return cssDataMap_.value("padding-top").toInt();
    return 0;
}

int32_t TpCssData::paddingBottom()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();

    if (cssDataMap_.contains("padding-bottom"))
        return cssDataMap_.value("padding-bottom").toInt();
    return 0;
}

uint32_t TpCssData::roundCorners()
{
    if (cssDataMap_.contains("border-radius"))
        return cssDataMap_.value("border-radius").toInt();
    return 0;
}

void TpCssData::setRoundCorners(const uint32_t &corners)
{
    cssDataMap_["border-radius"] = TpString::number(corners);
}

int32_t TpCssData::iconSize()
{
    if (cssDataMap_.contains("icon-size"))
        return cssDataMap_.value("icon-size").toInt();
    return 10;
}

// int32_t TpCssData::marginTop()
// {
//     if (cssDataMap_.contains("margin-top"))
//         return cssDataMap_.value("margin-top").toInt();
//     return 5;
// }

// int32_t TpCssData::marginBottom()
// {
//     if (cssDataMap_.contains("margin-bottom"))
//         return cssDataMap_.value("margin-bottom").toInt();
//     return 5;
// }

int32_t TpCssData::iconBackground()
{
    if (cssDataMap_.contains("icon-background"))
        return cssDataMap_.value("icon-background").toInt();
    return _RGB(255, 255, 255);
}
