/***
 * @Author: hywang
 * @Date: 2024-08-07 10:03:04
 * @LastEditors: hywang
 * @LastEditTime: 2024-08-12 15:56:48
 * @FilePath: /pix-singlegui/PixSingleGUI/src/ExternUtils/TpCssParser.cpp
 * @Description:
 * @
 * @PiXOS
 */

#include "TpCssParser.h"

#include "Utils/JsonStructPackage/JsonStructPackageHeader.h"
#include "Utils/CSSParser/CSSParser/CSSParser.hpp"
#include "KeywordItem.hpp"
#include "gumbo.h"
#include "GumboInterface.h"
#include "HTMLCSSRefAdaptor.h"
#include "TpFile.h"
#include "TpRegex.h"
#include "TpDisplay.h"

#include <fstream>
#include <iostream>
#include <functional>
#include "TpJsonDocument.h"

typedef std::function<TpString(const TpString &)> CssParseFunc;

const char *HoveredStr = "hover";
const char *PressedStr = "active";
// const char *FocusedStr = "focus";
const char *CheckedStr = "checked";
const char *DisabledStr = "disabled";
const char *EnabledStr = "enabled";

const char *DefaultCssTypeName = "default";

struct TpCssParserData
{
    // css的字符串
    TpString cssStr;

    TpHash<TpString, TpVector<TpCssParser::BlockCssDataSPtr>> cssDataMap;

    TpHash<TpString, CssParseFunc> cssParseFuncMap;

    // <class, <type/type-hover, <cssName(Font-size), value(15)>>
    TpHash<TpString, TpHash<TpString, TpHash<TpString, TpString>>> cssOriginDataMap;

    TpCssParserData() : cssStr("")
    {
    }
};

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

TpCssParser::TpCssParser()
{
    data_ = new TpCssParserData();

    RegistCssParseFunc();
}

TpCssParser::TpCssParser(const TpString &_filePath)
{
    data_ = new TpCssParserData();

    RegistCssParseFunc();

    parseCss(_filePath);
}

void TpCssParser::parseCss(const TpString &_filePath)
{
    TpCssParserData *cssParserData = static_cast<TpCssParserData *>(data_);
    if (!cssParserData)
        return;

    future::CSSParser *parser = new future::CSSParser;

    TpString cssStr = _filePath;

    TpFile cssFile(_filePath);
    if (cssFile.exists())
    {
        cssFile.open(TpFile::ReadOnly);
        if (cssFile.isOpen())
        {
            cssStr = cssFile.readAll();
            // parser->parseByFile(_filePath);
        }
    }

    // 去除CSS中所有的注释
    TpRegex contentRegex(R"((/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*))");
    cssStr = cssStr.replace(contentRegex, "");

    cssStr = cssStr.simplified();
    cssStr = cssStr.replace("\\", "");
    // std::cout << " cssStr : " << cssStr << std::endl;

    cssParserData->cssStr = cssStr;
    parser->parseByString(cssStr);

    std::set<future::Selector *> selectors = parser->getSelectors();

    for (future::Selector *topSelector : selectors)
    {
        if (topSelector->getType() == future::Selector::SimpleSelectorSequence)
        {
            future::SequenceSelector *seqSelector = dynamic_cast<future::SequenceSelector *>(topSelector);
            if (!seqSelector)
                continue;

            BlockCssDataSPtr blockCssData = std::make_shared<BlockCssData>();
            ParseCssSelector(seqSelector, blockCssData);

            ParseCssStr(topSelector->getRuleData(), blockCssData);
        }
        else if (topSelector->getType() == future::Selector::TypeSelector)
        {
            future::TypeSelector *typeSelector = dynamic_cast<future::TypeSelector *>(topSelector);

            BlockCssDataSPtr blockCssData = std::make_shared<BlockCssData>();
            blockCssData->mouseStatus = TpCssParser::Normal;

            blockCssData->uiType = typeSelector->getTagName();

            // std::cout << "222  blockCssData->uiTyp" <<  blockCssData->uiType << std::endl;

            ParseCssStr(typeSelector->getRuleData(), blockCssData);

            cssParserData->cssDataMap[blockCssData->uiType].emplace_back(blockCssData);
        }
        else if (topSelector->getType() == future::Selector::UniversalSelector)
        {
            // 通用样式
            std::cout << " UniversalSelector" << std::endl;
        }
        else
        {
            std::cout << "*********************************" << std::endl;
            std::cout << "Type" << topSelector->getType() << std::endl;
            std::cout << "desc:" << topSelector->description() << "---" << std::endl;
            std::cout << "*********************************" << std::endl;
        }
    }

    delete parser;
    parser = nullptr;
}

TpString TpCssParser::cssStr()
{
    TpCssParserData *cssParserData = static_cast<TpCssParserData *>(data_);

    return cssParserData->cssStr;
}

tpShared<TpCssData> TpCssParser::readCss(const TpString &_className, const TpString &_uiType, const TpCssParser::MouseStatus &_status)
{
    TpCssParserData *cssParserData = static_cast<TpCssParserData *>(data_);

    if (!cssParserData->cssOriginDataMap.contains(_className))
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    auto &cssTypeMap = cssParserData->cssOriginDataMap[_className];

    TpString type = _uiType.empty() ? DefaultCssTypeName : _uiType;

    // 先取出当前类型CSS的基本数据
    auto cssDefauleData = cssTypeMap.value(type);

    // 根据鼠标状态，拼接类型后缀
    type = translateUiType(type, _status);

    // 取出当前后缀的css数据，覆盖掉基础数据的key值
    auto findTypeCssDataMap = cssTypeMap.value(type);
    for (const auto &findCssResDataIter : findTypeCssDataMap)
    {
        // 指定状态的参数覆盖掉默认样式参数
        cssDefauleData[findCssResDataIter.first] = findCssResDataIter.second;
    }

    tpShared<TpCssData> cssDataSPtr = tpMakeShared<TpCssData>(cssDefauleData);
    return cssDataSPtr;
}

void TpCssParser::ParseCssSelector(future::SequenceSelector *_seqSelector, BlockCssDataSPtr _blockCssData)
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

void TpCssParser::ParseCssStr(const TpString &_cssStr, BlockCssDataSPtr _blockCssData)
{
    TpCssParserData *cssParserData = static_cast<TpCssParserData *>(data_);
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

int32_t TpCssParser::TranslateColor(const TpString &_colorStr)
{
    // 颜色有两种机制
    int32_t resColor = 0;

    TpString colorDealStr = _colorStr.simplified();
    colorDealStr = colorDealStr.replace(" ", "");

    uint8_t red, green, blue;
    uint8_t alpha = 255;
    if (colorDealStr.contains("#"))
    {
        // 十六进制字符串
        if (colorDealStr.logicalLength() < 7)
            return resColor;

        // std::cout << "colorDealStr : " << colorDealStr << std::endl;

        red = colorDealStr.mid(1, 2).toInt(16);
        green = colorDealStr.mid(3, 2).toInt(16);
        blue = colorDealStr.mid(5, 2).toInt(16);

        // std::cout << "RGB : " << red << "  " << green << "  " << blue << "  " << std::endl;

        if (colorDealStr.logicalLength() > 8)
            alpha = colorDealStr.mid(7, 2).toInt(16);

        // 放在这设置，为了确保如果格式不匹配，color对象为null
        // resColor.setRgba(red, green, blue, alpha);
        resColor = _RGBA(red, green, blue, alpha);
    }
    else
    {
        // rgba rgb(1, 1, 1, 1)
        // 去除前缀，和后缀的右括号
        // std::cout << colorDealStr << std::endl;

        TpString resColorStr = colorDealStr.mid(colorDealStr.find("(") + 1, colorDealStr.find(")") - colorDealStr.find("(") - 1);
        // std::cout << resColorStr << std::endl;

        TpList<TpString> rgbaList = resColorStr.split(',');
        if (rgbaList.size() < 3)
            return resColor;

        // std::cout << rgbaList.at(0) << "-" << rgbaList.at(1) << "-" << rgbaList.at(2) << std::endl;

        red = rgbaList.at(0).toInt();
        green = rgbaList.at(1).toInt();
        blue = rgbaList.at(2).toInt();

        if (rgbaList.size() > 3)
        {
            alpha = rgbaList.at(3).toDouble() * 255;
        }

        resColor = _RGBA(red, green, blue, alpha);
        // resColor.setRgba(red, green, blue, alpha);
    }

    return resColor;
}

void TpCssParser::RegistCssParseFunc()
{
    TpCssParserData *cssParserData = static_cast<TpCssParserData *>(data_);
    if (!cssParserData)
        return;

    cssParserData->cssParseFuncMap["font-size"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["letter-spacing"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["line-height"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["height"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["min-height"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["max-height"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["width"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["min-width"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["max-width"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["border-width"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["gap"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding-left"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding-right"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding-top"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding-bottom"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["icon-size"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["margin-top"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["margin-bottom"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["sub-color"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["border-color"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["color"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["background"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["background-color"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["icon-background"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["font-family"] = std::bind(&TpCssParser::StrTypeCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["font-weight"] = std::bind(&TpCssParser::StrTypeCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["font-style"] = std::bind(&TpCssParser::StrTypeCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["border-radius"] = std::bind(&TpCssParser::BorderRadiusTypeCssFunc, this, std::placeholders::_1);
}

TpString TpCssParser::DpPxCssFunc(const TpString &_attrValue)
{
    TpString valueStr = _attrValue.replace("px", "");
    valueStr = valueStr.replace("dp", "");
    valueStr = valueStr.replace("sp", "");

    valueStr = valueStr.simplified();

    // dp转px
    // valueStr = TpString::number(TpDisplay::dp2Px(valueStr.toInt()));

    return valueStr;
}

TpString TpCssParser::ColorCssFunc(const TpString &_attrValue)
{
    return TpString::number(TranslateColor(_attrValue));
}

TpString TpCssParser::StrTypeCssFunc(const TpString &_attrValue)
{
    TpString valueStr = _attrValue.replace("\"", "");

    return valueStr;
}

TpString TpCssParser::BorderRadiusTypeCssFunc(const TpString &_attrValue)
{
    // if (_attrValue.compare("circle") == 0)
    //     return "0.5";

    // if (_attrValue.contains("%"))
    // {
    //     TpString dealValue = _attrValue.replace("%", "");
    //     dealValue = TpString::number(dealValue.toDouble() / 100.0);
    //     return dealValue;
    // }
    TpString valueStr = _attrValue;
    valueStr = valueStr.simplified();

    return valueStr;

    // return "0";
}
