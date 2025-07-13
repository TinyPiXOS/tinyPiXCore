/***
 * @Author: hywang
 * @Date: 2024-08-07 10:03:04
 * @LastEditors: hywang
 * @LastEditTime: 2024-08-12 15:56:48
 * @FilePath: /pix-singlegui/PixSingleGUI/src/ExternUtils/tpCssParser.cpp
 * @Description:
 * @
 * @PiXOS
 */

#include "tpCssParser.h"

#include "Utils/JsonStructPackage/JsonStructPackageHeader.h"
#include "Utils/CSSParser/CSSParser/CSSParser.hpp"
#include "KeywordItem.hpp"
#include "gumbo.h"
#include "GumboInterface.h"
#include "HTMLCSSRefAdaptor.h"
#include "tpFile.h"
#include "tpRegex.h"
#include "tpDisplay.h"

#include <fstream>
#include <iostream>
#include <functional>
#include "tpJsonDocument.h"

typedef std::function<tpString(const tpString &)> CssParseFunc;

const char *HoveredStr = "hover";
const char *PressedStr = "active";
// const char *FocusedStr = "focus";
const char *CheckedStr = "checked";
const char *DisabledStr = "disabled";
const char *EnabledStr = "enabled";

const char *DefaultCssTypeName = "default";

struct tpCssParserData
{
    // css的字符串
    tpString cssStr;

    tpHash<tpString, tpVector<tpCssParser::BlockCssDataSPtr>> cssDataMap;

    tpHash<tpString, CssParseFunc> cssParseFuncMap;

    // <class, <type/type-hover, <cssName(Font-size), value(15)>>
    tpHash<tpString, tpHash<tpString, tpHash<tpString, tpString>>> cssOriginDataMap;

    tpCssParserData() : cssStr("")
    {
    }
};

tpString translateUiType(const tpString &uiType, const tpCssParser::MouseStatus &mouseStatus)
{
    tpString typeStr(DefaultCssTypeName);
    if (!uiType.empty())
    {
        typeStr = uiType;
    }

    if (mouseStatus == tpCssParser::Hover)
        typeStr += "-" + tpString(HoveredStr);
    else if (mouseStatus == tpCssParser::Pressed)
        typeStr += "-" + tpString(PressedStr);
    else if (mouseStatus == tpCssParser::Checked)
        typeStr += "-" + tpString(CheckedStr);
    else if (mouseStatus == tpCssParser::Disabled)
        typeStr += "-" + tpString(DisabledStr);
    else if (mouseStatus == tpCssParser::Enabled)
        typeStr += "-" + tpString(EnabledStr);
    else
        typeStr = typeStr;

    return typeStr;
}

tpCssParser::tpCssParser()
{
    data_ = new tpCssParserData();

    RegistCssParseFunc();
}

tpCssParser::tpCssParser(const tpString &_filePath)
{
    data_ = new tpCssParserData();

    RegistCssParseFunc();

    parseCss(_filePath);
}

void tpCssParser::parseCss(const tpString &_filePath)
{
    tpCssParserData *cssParserData = static_cast<tpCssParserData *>(data_);
    if (!cssParserData)
        return;

    future::CSSParser *parser = new future::CSSParser;

    tpString cssStr = _filePath;

    tpFile cssFile(_filePath);
    if (cssFile.exists())
    {
        cssFile.open(tpFile::ReadOnly);
        if (cssFile.isOpen())
        {
            cssStr = cssFile.readAll();
            // parser->parseByFile(_filePath);
        }
    }

    // 去除CSS中所有的注释
    tpRegex contentRegex(R"((/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*))");
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
            blockCssData->mouseStatus = tpCssParser::Normal;

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

tpString tpCssParser::cssStr()
{
    tpCssParserData *cssParserData = static_cast<tpCssParserData *>(data_);

    return cssParserData->cssStr;
}

tpShared<tpCssData> tpCssParser::readCss(const tpString &_className, const tpString &_uiType, const tpCssParser::MouseStatus &_status)
{
    tpCssParserData *cssParserData = static_cast<tpCssParserData *>(data_);

    if (!cssParserData->cssOriginDataMap.contains(_className))
        return tpMakeShared<tpCssData>(tpHash<tpString, tpString>{});

    auto &cssTypeMap = cssParserData->cssOriginDataMap[_className];

    tpString type = _uiType.empty() ? DefaultCssTypeName : _uiType;

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

    tpShared<tpCssData> cssDataSPtr = tpMakeShared<tpCssData>(cssDefauleData);
    return cssDataSPtr;
}

void tpCssParser::ParseCssSelector(future::SequenceSelector *_seqSelector, BlockCssDataSPtr _blockCssData)
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
            // if (_blockCssData->uiType.compare("tpBattery") == 0)
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
                _blockCssData->mouseStatus = tpCssParser::Hover;
            }
            else if (pseName.compare(PressedStr) == 0)
            {
                _blockCssData->mouseStatus = tpCssParser::Pressed;
            }
            else if (pseName.compare(CheckedStr) == 0)
            {
                _blockCssData->mouseStatus = tpCssParser::Checked;
            }
            else if (pseName.compare(DisabledStr) == 0)
            {
                _blockCssData->mouseStatus = tpCssParser::Disabled;
            }
            else if (pseName.compare(EnabledStr) == 0)
            {
                _blockCssData->mouseStatus = tpCssParser::Enabled;
            }
            else
            {
                _blockCssData->mouseStatus = tpCssParser::Normal;
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

void tpCssParser::ParseCssStr(const tpString &_cssStr, BlockCssDataSPtr _blockCssData)
{
    tpCssParserData *cssParserData = static_cast<tpCssParserData *>(data_);
    if (!cssParserData)
        return;

    // 所有CSS具体参数的key value
    tpHash<tpString, tpString> cssValueMap;

    // 移除空格
    tpString simpliCssStr = _cssStr.simplified();

    simpliCssStr = simpliCssStr.replace("\\", "");

    // 按分号切割
    tpList<tpString> cssSingleStrList = simpliCssStr.split(';');

    for (const auto &singleCssStr : cssSingleStrList)
    {
        tpList<tpString> cssProperty = singleCssStr.split(':');
        if (cssProperty.size() < 2)
            continue;

        tpString propertyName = cssProperty.at(0);
        tpString propertyValue = cssProperty.at(1);

        propertyName = propertyName.simplified();
        propertyValue = propertyValue.simplified();

        tpString resultValue;
        // std::cout << propertyName << std::endl;
        if (cssParserData->cssParseFuncMap.contains(propertyName))
        {
            tpString resultValue = cssParserData->cssParseFuncMap.value(propertyName)(propertyValue);

            cssValueMap[propertyName] = resultValue;
        }
    }

    // 根据鼠标状态，转换uitype
    tpString typeStr = translateUiType(_blockCssData->attrValue, _blockCssData->mouseStatus);

    cssParserData->cssOriginDataMap[_blockCssData->uiType][typeStr] = cssValueMap;
}

int32_t tpCssParser::TranslateColor(const tpString &_colorStr)
{
    // 颜色有两种机制
    int32_t resColor = 0;

    tpString colorDealStr = _colorStr.simplified();
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

        tpString resColorStr = colorDealStr.mid(colorDealStr.find("(") + 1, colorDealStr.find(")") - colorDealStr.find("(") - 1);
        // std::cout << resColorStr << std::endl;

        tpList<tpString> rgbaList = resColorStr.split(',');
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

void tpCssParser::RegistCssParseFunc()
{
    tpCssParserData *cssParserData = static_cast<tpCssParserData *>(data_);
    if (!cssParserData)
        return;

    cssParserData->cssParseFuncMap["font-size"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["letter-spacing"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["line-height"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["height"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["min-height"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["max-height"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["width"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["min-width"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["max-width"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["border-width"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["gap"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding-left"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding-right"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding-top"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["padding-bottom"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["icon-size"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["margin-top"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["margin-bottom"] = std::bind(&tpCssParser::DpPxCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["sub-color"] = std::bind(&tpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["border-color"] = std::bind(&tpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["color"] = std::bind(&tpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["background"] = std::bind(&tpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["background-color"] = std::bind(&tpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["icon-background"] = std::bind(&tpCssParser::ColorCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["font-family"] = std::bind(&tpCssParser::StrTypeCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["font-weight"] = std::bind(&tpCssParser::StrTypeCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["font-style"] = std::bind(&tpCssParser::StrTypeCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["border-radius"] = std::bind(&tpCssParser::BorderRadiusTypeCssFunc, this, std::placeholders::_1);
}

tpString tpCssParser::DpPxCssFunc(const tpString &_attrValue)
{
    tpString valueStr = _attrValue.replace("px", "");
    valueStr = valueStr.replace("dp", "");
    valueStr = valueStr.replace("sp", "");

    valueStr = valueStr.simplified();

    // dp转px
    // valueStr = tpString::number(tpDisplay::dp2Px(valueStr.toInt()));

    return valueStr;
}

tpString tpCssParser::ColorCssFunc(const tpString &_attrValue)
{
    return tpString::number(TranslateColor(_attrValue));
}

tpString tpCssParser::StrTypeCssFunc(const tpString &_attrValue)
{
    tpString valueStr = _attrValue.replace("\"", "");

    return valueStr;
}

tpString tpCssParser::BorderRadiusTypeCssFunc(const tpString &_attrValue)
{
    // if (_attrValue.compare("circle") == 0)
    //     return "0.5";

    // if (_attrValue.contains("%"))
    // {
    //     tpString dealValue = _attrValue.replace("%", "");
    //     dealValue = tpString::number(dealValue.toDouble() / 100.0);
    //     return dealValue;
    // }
    tpString valueStr = _attrValue;
    valueStr = valueStr.simplified();

    return valueStr;

    // return "0";
}
