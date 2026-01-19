#ifndef __TP_SVG_H
#define __TP_SVG_H

#include <TpCore.h>
#include "TpSize.h"
#include "TpFile.h"

TP_DEF_VOID_TYPE_VAR(ITpSVGData);

/// @brief SVG文件操作工具类
class TpSVG
{
public:
    /// @brief 默认构造函数
    TpSVG() noexcept;
    TpSVG(const TpString &filePath);
    ~TpSVG();

    /// @brief SVG文件的当前尺寸宽度
    /// @return 当前宽度
    int32_t width() const;

    /// @brief SVG文件的当前尺寸高度
    /// @return 当前高度
    int32_t height() const;

    /// @brief SVG文件的当前尺寸
    /// @return 当前尺寸
    TpSize size() const;

    /// @brief 加载SVG文件内容
    /// @param filePath SVG文件路径
    /// @return 是否加载成功
    bool load(const TpString &filePath);

    /// @brief 替换SVG中的颜色
    /// @param oldColor 需要替换的颜色值（支持#RRGGBB格式）
    /// @param newColor 新的颜色值（支持#RRGGBB格式）
    /// @param options 替换选项（0:默认只替换fill和stroke；1:替换所有颜色属性）
    void replaceColor(const TpString &oldColor, const TpString &newColor, int options = 0);

    /// @brief 替换SVG中的所有颜色（不指定旧颜色）
    /// @param newColor 新的颜色值（支持#RRGGBB格式）
    /// @param options 替换选项（0:默认只替换fill和stroke；1:替换所有颜色属性）
    void replaceColor(const TpString &newColor, int options = 0);

    /// @brief 获取当前SVG内容字符串
    /// @return SVG XML内容
    TpString svgValue() const;

    /// @brief 保存SVG到文件
    /// @param filePath 目标文件路径
    /// @return 保存成功返回true，否则返回false
    bool save(const TpString &filePath = "") const;

private:
    // 提取属性值
    TpString extractAttribute(const TpString &svgContent, const TpString &attributeName) const;

    // 替换属性值
    void replaceAttributeValue(TpString &content, const TpString &attribute, const TpString &oldValue, const TpString &newValue);

    // 替换属性值为新颜色（不指定旧颜色）
    void replaceAttributeValue(TpString &content, const TpString &attribute, const TpString &newColor);

    // 替换样式中的颜色值
    void replaceStyleColor(TpString &styleContent, const TpString &oldColor, const TpString &newColor);

    // 替换样式中的所有颜色值（不指定旧颜色）
    void replaceStyleColor(TpString &styleContent, const TpString &newColor);

private:
    ITpSVGData *data_;
};

#endif