#include "tpCssData.h"
#include "tpUtils.h"

tpCssData::tpCssData(const tpHash<tpString, tpString> &cssDataMap)
    : cssDataMap_(cssDataMap)
{
}

tpCssData::~tpCssData()
{
}

int32_t tpCssData::width()
{
    if (cssDataMap_.contains("width"))
        return cssDataMap_.value("width").toInt();
    return 10;
}

int32_t tpCssData::minimumWidth()
{
    if (cssDataMap_.contains("min-width"))
        return cssDataMap_.value("min-width").toInt();
    return 0;
}

int32_t tpCssData::maximumWidth()
{
    if (cssDataMap_.contains("max-width"))
        return cssDataMap_.value("max-width").toInt();
    return WIDGET_MAX_WIDTH;
}

int32_t tpCssData::height()
{
    if (cssDataMap_.contains("height"))
        return cssDataMap_.value("height").toInt();
    return 10;
}

int32_t tpCssData::minimumHeight()
{
    if (cssDataMap_.contains("min-height"))
        return cssDataMap_.value("min-height").toInt();
    return 0;
}

int32_t tpCssData::maximumHeight()
{
    if (cssDataMap_.contains("max-height"))
        return cssDataMap_.value("max-height").toInt();
    return WIDGET_MAX_HEIGHT;
}

int32_t tpCssData::color()
{
    if (cssDataMap_.contains("color"))
        return cssDataMap_.value("color").toInt();
    return 0;
}

int32_t tpCssData::subColor()
{
    if (cssDataMap_.contains("sub-color"))
        return cssDataMap_.value("sub-color").toInt();
    return 0;
}

int32_t tpCssData::backgroundColor()
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

void tpCssData::setBackgroundColor(const int32_t &color)
{
    // std::cout << "setBackgroundColor " << std::endl;

    // std::cout << "setBackgroundColor " << _R(color) << " " << _G(color) << " " << _B(color) << std::endl;

    if (cssDataMap_.contains("background"))
        cssDataMap_["background"] = tpString::number(color);
    else if (cssDataMap_.contains("background-color"))
        cssDataMap_["background-color"] = tpString::number(color);
    else
        cssDataMap_["background"] = tpString::number(color);
}

int32_t tpCssData::borderColor()
{
    if (cssDataMap_.contains("border-color"))
        return cssDataMap_.value("border-color").toInt();
    return 0;
}

void tpCssData::setBorderColor(const int32_t &color)
{
    cssDataMap_["border-color"] = tpString::number(color);
}

int32_t tpCssData::borderWidth()
{
    if (cssDataMap_.contains("border-width"))
        return cssDataMap_.value("border-width").toInt();
    return 0;
}

int32_t tpCssData::fontSize()
{
    if (cssDataMap_.contains("font-size"))
        return cssDataMap_.value("font-size").toInt();
    return 10;
}

int32_t tpCssData::gap()
{
    if (cssDataMap_.contains("gap"))
        return cssDataMap_.value("gap").toInt();
    return 5;
}

int32_t tpCssData::padding()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();
    return 0;
}

int32_t tpCssData::paddingLeft()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();

    if (cssDataMap_.contains("padding-left"))
        return cssDataMap_.value("padding-left").toInt();
    return 0;
}

int32_t tpCssData::paddingRight()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();

    if (cssDataMap_.contains("padding-right"))
        return cssDataMap_.value("padding-right").toInt();
    return 0;
}

int32_t tpCssData::paddingTop()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();

    if (cssDataMap_.contains("padding-top"))
        return cssDataMap_.value("padding-top").toInt();
    return 0;
}

int32_t tpCssData::paddingBottom()
{
    if (cssDataMap_.contains("padding"))
        return cssDataMap_.value("padding").toInt();

    if (cssDataMap_.contains("padding-bottom"))
        return cssDataMap_.value("padding-bottom").toInt();
    return 0;
}

uint32_t tpCssData::roundCorners()
{
    if (cssDataMap_.contains("border-radius"))
        return cssDataMap_.value("border-radius").toInt();
    return 0;
}

void tpCssData::setRoundCorners(const uint32_t &corners)
{
    cssDataMap_["border-radius"] = tpString::number(corners);
}

int32_t tpCssData::iconSize()
{
    if (cssDataMap_.contains("icon-size"))
        return cssDataMap_.value("icon-size").toInt();
    return 10;
}

// int32_t tpCssData::marginTop()
// {
//     if (cssDataMap_.contains("margin-top"))
//         return cssDataMap_.value("margin-top").toInt();
//     return 5;
// }

// int32_t tpCssData::marginBottom()
// {
//     if (cssDataMap_.contains("margin-bottom"))
//         return cssDataMap_.value("margin-bottom").toInt();
//     return 5;
// }

int32_t tpCssData::iconBackground()
{
    if (cssDataMap_.contains("icon-background"))
        return cssDataMap_.value("icon-background").toInt();
    return _RGB(255, 255, 255);
}
