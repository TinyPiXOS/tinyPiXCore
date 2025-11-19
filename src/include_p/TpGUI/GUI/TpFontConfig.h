#ifndef __TP_FONT_CONFIG_H
#define __TP_FONT_CONFIG_H

#include <TpGUI.h>
#include <TpFont.h>
#include <TpMap.h>
#include <TpString.h>
#include <TpVector.h>
#include <TpList.h>

/// @brief 字重
enum FontWeight
{
    FONT_WEIGHT_NORMAL,
    FONT_WEIGHT_BOLD,
    FONT_WEIGHT_LIGHT,
    FONT_WEIGHT_EXTRALIGHT,
    FONT_WEIGHT_HEAVY,
    FONT_WEIGHT_MEDIUM,
    FONT_WEIGHT_SEMIBOLD,
};

class TpFontConfig
{
public:
    ~TpFontConfig();

    /// @brief 获取字体库管理单例指针
    /// @return 指针对象
    static tpShared<TpFontConfig> Inst();

    /// @brief 字重类型获取字体格式
    /// @param fontName 字库名称；ex: SourceHanSerifCN
    /// @param fontWeight 字重枚举
    /// @return 字体名字字符串
    TpString fontName(const TpString &fontName, FontWeight fontWeight);

    /// @brief 是否加载了该字库
    /// @param fontName 字库名称；ex: SourceHanSerifCN
    /// @return 存在返回true，否则返回false
    bool containsFont(const TpString &fontName);

    /// @brief 所有字体名称列表
    /// @return 字体列表
    TpList<TpString> families();

private:
    TpFontConfig();

private:
    TpMap<TpString, TpVector<TpString>> fontMap;
};

#endif
