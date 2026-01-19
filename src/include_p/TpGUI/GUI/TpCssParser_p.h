#ifndef __TP_CSSPARSER_PRIVATE_H
#define __TP_CSSPARSER_PRIVATE_H

#include <TpCore.h>
#include "TpString.h"
#include "TpList.h"
#include "TpCssParser.h"
#include <functional>
#include <fstream>
#include <iostream>

namespace future
{
    class SequenceSelector;
};

typedef std::function<TpString(const TpString &)> CssParseFunc;

struct BlockCssData
{
    TpString uiType;
    TpString attrName;
    TpString attrValue;

    TpCssParser::MouseStatus mouseStatus = TpCssParser::Normal;

    BlockCssData()
        : uiType(""), attrName(""), attrValue("")
    {
    }
};
typedef std::shared_ptr<BlockCssData> BlockCssDataSPtr;

struct TpCssParserData
{
    // css的字符串
    TpString cssStr;

    TpHash<TpString, TpVector<BlockCssDataSPtr>> cssDataMap;

    TpHash<TpString, CssParseFunc> cssParseFuncMap;

    // <class, <type/type-hover, <cssName(Font-size), value(15)>>
    TpHash<TpString, TpHash<TpString, TpHash<TpString, TpString>>> cssOriginDataMap;

    TpCssParserData() : cssStr("")
    {
    }
};

const char *HoveredStr = "hover";
const char *PressedStr = "active";
// const char *FocusedStr = "focus";
const char *CheckedStr = "checked";
const char *DisabledStr = "disabled";
const char *EnabledStr = "enabled";

const char *DefaultCssTypeName = "default";

TpString translateUiType(const TpString &uiType, const TpCssParser::MouseStatus &mouseStatus)
{
    TpString typeStr(DefaultCssTypeName);
    if (!uiType.empty())
    {
        typeStr = uiType;
    }

    if (mouseStatus == TpCssParser::Hover)
        typeStr += "-" + TpString(HoveredStr);
    else if (mouseStatus == TpCssParser::Pressed)
        typeStr += "-" + TpString(PressedStr);
    else if (mouseStatus == TpCssParser::Checked)
        typeStr += "-" + TpString(CheckedStr);
    else if (mouseStatus == TpCssParser::Disabled)
        typeStr += "-" + TpString(DisabledStr);
    else if (mouseStatus == TpCssParser::Enabled)
        typeStr += "-" + TpString(EnabledStr);
    else
        typeStr = typeStr;

    return typeStr;
}

// 递归解析selector
void ParseCssSelector(future::SequenceSelector *_seqSelector, BlockCssDataSPtr _blockCssData)
{
    for (const auto &childSelector : _seqSelector->getContrains())
    {
        if (childSelector->getType() == future::Selector::IDSelector)
        {
            future::IdSelector *idSelector = dynamic_cast<future::IdSelector *>(childSelector);
            if (idSelector)
            {
                std::string idName = idSelector->getIdIdentifier();

                _blockCssData->attrName = "id";
                _blockCssData->attrValue = idName;
            }
        }
        else if (childSelector->getType() == future::Selector::TypeSelector)
        {
            future::TypeSelector *typeSelector = dynamic_cast<future::TypeSelector *>(childSelector);

            _blockCssData->uiType = typeSelector->getTagName();
            // std::cout << "_blockCssData->uiType " << _blockCssData->uiType << std::endl;
            // if (_blockCssData->uiType.compare("TpBattery") == 0)
            // {
            //     int a=0;
            // }
        }
        else if (childSelector->getType() == future::Selector::AttributeSelector)
        {
            future::AttributeSelector *attrSelector = dynamic_cast<future::AttributeSelector *>(childSelector);

            std::string keyStr = attrSelector->getKey();
            std::string valueStr = attrSelector->getValue();

            _blockCssData->attrName = keyStr;
            _blockCssData->attrValue = valueStr;
        }
        else if (childSelector->getType() == future::Selector::SimpleSelectorSequence)
        {
            future::SequenceSelector *seqSelector = dynamic_cast<future::SequenceSelector *>(childSelector);
            ParseCssSelector(seqSelector, _blockCssData);
        }
        else if (childSelector->getType() == future::Selector::PseudoSelector)
        {
            future::PseudoSelector *pseSelector = dynamic_cast<future::PseudoSelector *>(childSelector);
            std::string pseName = pseSelector->getPseudoData();

            if (pseName.compare(HoveredStr) == 0)
            {
                _blockCssData->mouseStatus = TpCssParser::Hover;
            }
            else if (pseName.compare(PressedStr) == 0)
            {
                _blockCssData->mouseStatus = TpCssParser::Pressed;
            }
            else if (pseName.compare(CheckedStr) == 0)
            {
                _blockCssData->mouseStatus = TpCssParser::Checked;
            }
            else if (pseName.compare(DisabledStr) == 0)
            {
                _blockCssData->mouseStatus = TpCssParser::Disabled;
            }
            else if (pseName.compare(EnabledStr) == 0)
            {
                _blockCssData->mouseStatus = TpCssParser::Enabled;
            }
            else
            {
                _blockCssData->mouseStatus = TpCssParser::Normal;
            }
        }
        else if (childSelector->getType() == future::Selector::ClassSelector)
        {
        }
        else
        {
            std::cout << "Type22: " << childSelector->getType() << std::endl;
        }
    }
}

// 解析CSS字符串
void ParseCssStr(ITpCssParserData *data, const TpString &_cssStr, BlockCssDataSPtr _blockCssData)
{
    TpCssParserData *cssParserData = static_cast<TpCssParserData *>(data);
    if (!cssParserData)
        return;

    // 所有CSS具体参数的key value
    TpHash<TpString, TpString> cssValueMap;

    // 移除空格
    TpString simpliCssStr = _cssStr.simplified();

    simpliCssStr = simpliCssStr.replace("\\", "");

    // 按分号切割
    TpList<TpString> cssSingleStrList = simpliCssStr.split(';');

    for (const auto &singleCssStr : cssSingleStrList)
    {
        TpList<TpString> cssProperty = singleCssStr.split(':');
        if (cssProperty.size() < 2)
            continue;

        TpString propertyName = cssProperty.at(0);
        TpString propertyValue = cssProperty.at(1);

        propertyName = propertyName.simplified();
        propertyValue = propertyValue.simplified();

        TpString resultValue;
        // std::cout << propertyName << std::endl;
        if (cssParserData->cssParseFuncMap.contains(propertyName))
        {
            TpString resultValue = cssParserData->cssParseFuncMap.value(propertyName)(propertyValue);

            cssValueMap[propertyName] = resultValue;
        }
    }

    // 根据鼠标状态，转换uitype
    TpString typeStr = translateUiType(_blockCssData->attrValue, _blockCssData->mouseStatus);

    cssParserData->cssOriginDataMap[_blockCssData->uiType][typeStr] = cssValueMap;
}

// RGBA或#解析为颜色值
int32_t parseColorValue(const TpString &colorStr)
{
    int32_t resColor = 0;

    TpString colorDealStr = colorStr.simplified();
    colorDealStr = colorDealStr.replace(" ", "");

    uint8_t red, green, blue;
    uint8_t alpha = 255;
    if (colorDealStr.contains("#"))
    {
        // 十六进制字符串
        if (colorDealStr.logicalLength() < 7)
            return resColor;

        red = colorDealStr.mid(1, 2).toInt(16);
        green = colorDealStr.mid(3, 2).toInt(16);
        blue = colorDealStr.mid(5, 2).toInt(16);

        if (colorDealStr.logicalLength() > 8)
            alpha = colorDealStr.mid(7, 2).toInt(16);

        // 放在这设置，为了确保如果格式不匹配，color对象为null
        resColor = _RGBA(red, green, blue, alpha);
    }
    else
    {
        TpString resColorStr = colorDealStr.mid(colorDealStr.find("(") + 1, colorDealStr.find(")") - colorDealStr.find("(") - 1);
        TpList<TpString> rgbaList = resColorStr.split(',');
        if (rgbaList.size() < 3)
            return resColor;

        red = rgbaList.at(0).toInt();
        green = rgbaList.at(1).toInt();
        blue = rgbaList.at(2).toInt();

        if (rgbaList.size() > 3)
        {
            alpha = rgbaList.at(3).toDouble() * 255;
        }

        resColor = _RGBA(red, green, blue, alpha);
    }

    return resColor;
}

// 辅助函数：解析位置字符串（支持百分比和小数）
double parsePosition(const TpString &posStr)
{
    TpString simplified = posStr.simplified();
    if (simplified.endsWith('%'))
    {
        // 百分比格式：去掉%并转换为小数
        TpString numStr = simplified.mid(0, simplified.length() - 1).simplified();
        return numStr.toDouble() / 100.0;
    }
    // 直接转换为小数
    return simplified.toDouble();
}

// 线性渐变解析函数
TpString parseLinearGradient(const TpString &gradientStr)
{
    // 简化输入字符串并移除外层括号
    TpString input = gradientStr.simplified();

    // 先移除无关字符，只保留数据
    input = input.replace("linear-gradient(", "");

    // 移除最后一个括号
    if (input.endsWith(");"))
        input = input.mid(0, input.logicalLength() - 2);
    else
        input = input.mid(0, input.logicalLength() - 1);

    // 分割角度和颜色部分
    int depth = 0;
    int splitIndex = -1;
    for (int i = 0; i < input.length(); ++i)
    {
        char c = input[i];
        if (c == '(')
            depth++;
        else if (c == ')')
            depth--;
        else if (c == ',' && depth == 0)
        {
            splitIndex = i;
            break;
        }
    }
    if (splitIndex == -1)
        return ""; // 格式错误

    TpString anglePart = input.mid(0, splitIndex).simplified();
    TpString colorsPart = input.mid(splitIndex + 1).simplified();

    // 解析角度（支持deg单位）
    int angle = 0;
    if (anglePart.endsWith("deg"))
    {
        anglePart = anglePart.mid(0, anglePart.length() - 3).simplified();
    }
    angle = anglePart.toInt();

    // 分割颜色标记（考虑括号嵌套）
    TpList<TpString> colorTokens;
    depth = 0;
    TpString currentToken;
    for (int i = 0; i < colorsPart.length(); ++i)
    {
        char c = colorsPart[i];
        if (c == '(')
            depth++;
        else if (c == ')')
            depth--;

        if (c == ',' && depth == 0)
        {
            colorTokens.append(currentToken.simplified());
            currentToken = "";
        }
        else
        {
            currentToken.append(c);
        }
    }
    if (!currentToken.empty())
    {
        colorTokens.append(currentToken.simplified());
    }

    // 解析每个颜色标记
    struct ColorStop
    {
        TpString color;
        double position;
        bool hasPosition;
    };
    TpList<ColorStop> stops;

    for (int i = 0; i < colorTokens.size(); ++i)
    {
        TpString token = colorTokens[i];
        ColorStop stop;
        stop.hasPosition = false;

        // 处理rgb颜色
        if (token.startsWith("rgb("))
        {
            int endIndex = -1;
            int depth = 0;
            for (int j = 0; j < token.length(); ++j)
            {
                if (token[j] == '(')
                    depth++;
                else if (token[j] == ')')
                {
                    depth--;
                    if (depth == 0)
                    {
                        endIndex = j;
                        break;
                    }
                }
            }
            if (endIndex == -1)
                continue; // 格式错误

            stop.color = token.mid(0, endIndex + 1); // 包含整个rgb(...)
            TpString rest = token.mid(endIndex + 1).simplified();
            if (!rest.empty())
            {
                stop.position = parsePosition(rest);
                stop.hasPosition = true;
            }
        }
        // 处理十六进制颜色
        else
        {
            int spaceIndex = -1;
            for (int j = 0; j < token.length(); ++j)
            {
                if (token[j] == ' ')
                {
                    spaceIndex = j;
                    break;
                }
            }

            if (spaceIndex != -1)
            {
                stop.color = token.mid(0, spaceIndex);
                TpString posPart = token.mid(spaceIndex + 1).simplified();
                stop.position = parsePosition(posPart);
                stop.hasPosition = true;
            }
            else
            {
                stop.color = token; // 无位置信息
            }
        }
        stops.append(stop);
    }

    // 处理缺失的位置信息
    if (stops.size() > 0)
    {
        bool allHavePosition = true;
        for (int i = 0; i < stops.size(); ++i)
        {
            if (!stops[i].hasPosition)
            {
                allHavePosition = false;
                break;
            }
        }

        if (!allHavePosition)
        {
            // 只有两个颜色时：第一个0，第二个1
            if (stops.size() == 2)
            {
                stops[0].position = 0.0;
                stops[1].position = 1.0;
            }
            // 多个颜色时均匀分布
            else
            {
                for (int i = 0; i < stops.size(); ++i)
                {
                    stops[i].position = static_cast<double>(i) / (stops.size() - 1);
                }
            }
        }
    }

    // 构建结果字符串
    TpString result = "linear-gradient," + TpString::number(angle) + ",";
    for (int i = 0; i < stops.size(); ++i)
    {
        TpString colorValue = TpString::number(parseColorValue(stops[i].color));
        TpString posStr = TpString::number(stops.at(i).position, 1); // 保留1位小数
        result += colorValue + "|" + posStr;
        if (i < stops.size() - 1)
        {
            result += ",";
        }
    }

    return result;
}

// 径向渐变解析函数
TpString parseRadialGradient(const TpString &gradientStr)
{
    return "";
}

#endif
