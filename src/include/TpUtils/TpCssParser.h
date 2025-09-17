#ifndef __TP_CSS_PARSER_H
#define __TP_CSS_PARSER_H

#include <TpString.h>
#include <TpColors.h>
#include <TpVector.h>
#include <TpHash.h>
#include <memory>
#include "TpCssData.h"

TP_DEF_VOID_TYPE_VAR(ItpCssParserData);
/// @brief CSS解析器
class TpCssParser
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
        Normal // 没有状态的默认CSS数据
    };

public:
    TpCssParser();

    /// @brief 给入CSS文件路径或CSS字符串
    /// @param _filePath CSS文件路径或CSS字符串
    TpCssParser(const TpString &_filePath);

    /// @brief 清空当前缓存的所有CSS数据；一般用于完全重置UI样式时使用
    void clearCss();

    /// @brief 指定文件路径或者CSS字符串解析CSS数据，解析时只会覆盖给入CSs的key值，原有CSS数据不会被移除
    /// @param _filePath 文件路径或字符串
    void parseCss(const TpString &_filePath);

    /// @brief 获取CSS字符串
    /// @return
    TpString cssStr();

    /// @brief 指定类名、类型名、状态获取对应CSS数据结构
    /// @param _className 类名，例如 TpCombox
    /// @param _uiType CSS设置的类型
    /// @param _status 状态
    /// @return CSS数据指针
    tpShared<TpCssData> readCss(const TpString &_className, const TpString &_uiType, const TpCssParser::MouseStatus &_status);

private:
    // 根据属性名字，找到Data中变量赋值
    void RegistCssParseFunc();

    /// @brief 所有用dp或者px表示的数据类型
    /// @param _attrValue
    /// @return
    TpString DpPxCssFunc(const TpString &_attrValue);

    /// @brief 所有颜色解析函数
    /// @param _attrValue
    /// @return 颜色返回_RGB()整数转换为字符串
    TpString ColorCssFunc(const TpString &_attrValue);

    /// @brief 所有用字符串表示类型的CSS键值
    /// @param _attrValue
    /// @return
    TpString StrTypeCssFunc(const TpString &_attrValue);

private:
    ItpCssParserData *data_;
};

#endif
