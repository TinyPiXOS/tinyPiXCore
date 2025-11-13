#include "TpCssParser.h"
#include "TpCore/JsonStructPackage/JsonStructPackageHeader.h"
#include "TpGUI/CSSParser/CSSParser/CSSParser.hpp"
#include "KeywordItem.hpp"
#include "gumbo.h"
#include "GumboInterface.h"
#include "HTMLCSSRefAdaptor.h"
#include "TpFile.h"
#include "TpRegex.h"
#include "TpDisplay.h"
#include "TpCssParser_p.h"
#include "TpJsonDocument.h"

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

void TpCssParser::clearCss()
{
    TpCssParserData *cssParserData = static_cast<TpCssParserData *>(data_);
    if (!cssParserData)
        return;

    cssParserData->cssStr = "";

    cssParserData->cssDataMap.clear();
    cssParserData->cssOriginDataMap.clear();
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

            ParseCssStr(data_, topSelector->getRuleData(), blockCssData);
        }
        else if (topSelector->getType() == future::Selector::TypeSelector)
        {
            future::TypeSelector *typeSelector = dynamic_cast<future::TypeSelector *>(topSelector);

            BlockCssDataSPtr blockCssData = std::make_shared<BlockCssData>();
            blockCssData->mouseStatus = TpCssParser::Normal;

            blockCssData->uiType = typeSelector->getTagName();

            // std::cout << "222  blockCssData->uiTyp" <<  blockCssData->uiType << std::endl;

            ParseCssStr(data_, typeSelector->getRuleData(), blockCssData);

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

    cssParserData->cssParseFuncMap["border-radius"] = std::bind(&TpCssParser::DpPxCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["sub-color"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["border-color"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["color"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["background"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["background-color"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["icon-background"] = std::bind(&TpCssParser::ColorCssFunc, this, std::placeholders::_1);

    cssParserData->cssParseFuncMap["font-family"] = std::bind(&TpCssParser::StrTypeCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["font-weight"] = std::bind(&TpCssParser::StrTypeCssFunc, this, std::placeholders::_1);
    cssParserData->cssParseFuncMap["font-style"] = std::bind(&TpCssParser::StrTypeCssFunc, this, std::placeholders::_1);
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
    // 颜色分为普通颜色和渐变颜色；有十进制和十六进制两种机制
    TpString resColorStr = "";
    TpString defaultColorStr = TpString(_RGB(0, 0, 0));

    if (_attrValue.contains("gradient"))
    {
        /*  CSS角度定义
            ​​0度​​：表示从下到上（垂直向上）
            ​​90度​​：表示从左到右（水平向右）
            ​​180度​​：表示从上到下（垂直向下）
            ​​270度​​：表示从右到左（水平向左）
        */

        // 渐变颜色

        // 判断渐变类型
        if (_attrValue.contains("linear-gradient"))
        {
            // 线性渐变
            resColorStr = parseLinearGradient(_attrValue);
        }
        else if (_attrValue.contains("radial-gradient"))
        {
            // 径向渐变 TODO  当前仅支持线性渐变
            // resColorStr = parseRadialGradient(_attrValue);
            resColorStr = TpString::number(_RGB(0, 0, 0));
        }
        else
        {
            // 暂不支持的渐变类型
            resColorStr = TpString::number(_RGB(0, 0, 0));
        }
    }
    else
    {
        // 普通颜色
        resColorStr = TpString::number(parseColorValue(_attrValue));
    }

    return resColorStr;
}

TpString TpCssParser::StrTypeCssFunc(const TpString &_attrValue)
{
    TpString valueStr = _attrValue.replace("\"", "");

    return valueStr;
}
