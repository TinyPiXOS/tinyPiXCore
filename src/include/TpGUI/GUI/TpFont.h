#ifndef __TP_FONT_H
#define __TP_FONT_H

#include <TpCore.h>
#include <TpList.h>

#define DEFAULT_FONT_FAMILY "SourceHanSerifCN"

TP_DEF_VOID_TYPE_VAR(ITpFontData);
TP_DEF_VOID_TYPE_VAR(ITpFontFamilyData);

class TpSurface;
class TpFontFamily;
class TpColors;
class TpSize;

class TpFont
{
public:
    TpFont(const TpString &language = DEFAULT_FONT_FAMILY, int32_t defaultPtSize = 12);
    virtual ~TpFont();

    virtual void setLanguage(const TpString &language);

    // 临时接口，兼容以前写法
    virtual void setFontColor(int32_t color, int32_t color2);

    /// @brief 设置字体轮廓和填充颜色
    /// @param color 颜色值
    virtual void setFontColor(int32_t color);
    /// @brief 设置字体轮廓和填充颜色
    /// @param color 颜色
    virtual void setFontColor(const TpColors &color);
    /// @brief 设置字体填充颜色
    /// @param bgcolor 颜色值
    virtual void setFontBackColor(int32_t bgColor);
    /// @brief 设置字体轮廓颜色
    /// @param fgcolor 颜色值
    virtual void setFontForeColor(int32_t fgColor);

    /// @brief 设置是否斜体
    /// @param italic 是否斜体
    void setItalic(bool italic);

    /// @brief 获取是否是斜体
    /// @return 斜体为true
    bool italic();

    /// @brief 设置是否加粗
    /// @param bold 是否加粗
    void setBold(bool bold);

    /// @brief 获取是否是加粗字体
    /// @return 加粗为true
    bool bold();

    /// @brief 设置是否自动换行
    /// @param wrap 是否换行
    void setWrap(bool wrap);
    bool wrap();

    /// @brief 布局宽高；根据布局宽高实现自动换行和居中
    /// @param w 宽度值
    /// @param h 高度值
    void setLayout(int32_t w, int32_t h);

    int32_t layoutWidth();
    int32_t layoutHeight();

    /// @brief 设置基于Layout的居中属性
    /// @param align 布局属性
    void setAlign(Tp::AlignmentFlag align = Tp::AlignLeft);

    Tp::AlignmentFlag align();

public:
    /// @brief 获取字体大小 px
    /// @return px值
    int32_t fontSize();
    /// @brief 设置字体大小
    /// @param ptsize px值
    void setFontSize(const int32_t &ptsize);

    /// @brief 设置字体内文本
    /// @param text 文本
    virtual void setText(const TpString &text);
    /// @brief 获取文本字符串
    /// @return 文本字符串
    TpString text() const;

public:
    virtual int32_t pixelWidth();
    virtual int32_t pixelHeight();
    virtual TpSize pixelSize();

    friend class TpPainter;

private:
    ITpFontData *data_;
};

class TpFontFamily
{
public:
    TpFontFamily();
    virtual ~TpFontFamily();

    static TpList<TpString> families();

private:
    ITpFontFamilyData *familySet;
};

#endif
