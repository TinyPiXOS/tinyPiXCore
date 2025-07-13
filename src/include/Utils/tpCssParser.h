/***
 * @Author: hywang
 * @Date: 2024-08-07 10:12:41
 * @LastEditors: hywang
 * @LastEditTime: 2024-08-15 11:30:14
 * @FilePath: /pix-singlegui/PixSingleGUI/include/ExternUtils/tpCssParser.h
 * @Description:
 * @
 * @PiXOS
 */

#ifndef __TP_CSS_PARSER_H
#define __TP_CSS_PARSER_H

#include <tpString.h>
#include <tpColors.h>
#include <tpVector.h>
#include <tpHash.h>
#include <memory>
#include "tpCssData.h"

namespace future
{
    class SequenceSelector;
};

TP_DEF_VOID_TYPE_VAR(ItpCssParserData);

/// @brief 
class tpCssParser
{
public:
    enum MouseStatus
    {
        Pressed,
        Hover,
        Disabled,
        Enabled,
        // Focused,
        Checked,
        Normal      // 没有状态的默认CSS数据
    };

    struct BlockCssData
    {
        tpString uiType;
        tpString attrName;
        tpString attrValue;

        MouseStatus mouseStatus = Normal;
        
        BlockCssData()
            : uiType(""), attrName(""), attrValue("")
        {
        }
    };
    typedef std::shared_ptr<BlockCssData> BlockCssDataSPtr;

public:
    tpCssParser();

    /// @brief 给入CSS文件路径或CSS字符串
    /// @param _filePath CSS文件路径或CSS字符串
    tpCssParser(const tpString &_filePath);
 
    /// @brief 指定文件路径或者CSS字符串解析CSS数据，解析时只会覆盖给入CSs的key值，原有CSS数据不会被移除
    /// @param _filePath 文件路径或字符串
    void parseCss(const tpString &_filePath);

    /// @brief 获取CSS字符串
    /// @return 
    tpString cssStr();
    
    /// @brief 指定类名、类型名、状态获取对应CSS数据结构
    /// @param _className 类名，例如 tpCombox
    /// @param _uiType CSS设置的类型
    /// @param _status 状态
    /// @return CSS数据指针
    tpShared<tpCssData> readCss(const tpString &_className, const tpString &_uiType, const tpCssParser::MouseStatus &_status);

private:
    // 递归解析selector
    void ParseCssSelector(future::SequenceSelector *_seqSelector, BlockCssDataSPtr _blockCssData);

    // 解析CSS字符串
    void ParseCssStr(const tpString &_cssStr, BlockCssDataSPtr _blockCssData);

    // 将颜色字符串转换为Color对象
    int32_t TranslateColor(const tpString &_colorStr);

private:
    // 根据属性名字，找到Data中变量赋值
    void RegistCssParseFunc();

    /// @brief 所有用dp或者px表示的数据类型
    /// @param _attrValue 
    /// @return 
    tpString DpPxCssFunc(const tpString &_attrValue);

    /// @brief 所有颜色解析函数
    /// @param _attrValue 
    /// @return 颜色返回_RGB()整数转换为字符串
    tpString ColorCssFunc(const tpString &_attrValue);

    /// @brief 所有用字符串表示类型的CSS键值
    /// @param _attrValue 
    /// @return 
    tpString StrTypeCssFunc(const tpString &_attrValue);

    /// @brief 圆角属性值读取
    /// @param _attrValue 
    /// @return 
    tpString BorderRadiusTypeCssFunc(const tpString &_attrValue);
    
private:
    ItpCssParserData *data_;
};

#endif
