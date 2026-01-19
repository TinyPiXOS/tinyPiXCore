#ifndef __TP_FONT_H
#define __TP_FONT_H

#include <TpCore.h>
#include <TpList.h>

// #define DEFAULT_FONT_FAMILY "MiSans"
#define DEFAULT_FONT_FAMILY "WqyZenhei"

TP_DEF_VOID_TYPE_VAR(ITpFontData);
TP_DEF_VOID_TYPE_VAR(ITpFontFamilyData);

class TpFontFamily;
class TpColors;
class TpSize;

class TpFont
{
public:
    TpFont(const TpString &language = DEFAULT_FONT_FAMILY, int32_t defaultPtSize = 12);
    virtual ~TpFont();

    /// @brief 设置字体名称；如果字体不存在则使用默认
    /// @param language 字体名称
    virtual void setLanguage(const TpString &language);

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

    /// @brief 布局宽高；根据布局宽高实现自动换行和居中模式
    /// @param w 宽度值
    /// @param h 高度值
    void setLayout(int32_t w, int32_t h);
    /// @brief 布局宽高；根据布局宽高实现自动换行和居中模式
    /// @param size 布局尺寸
    void setLayout(const TpSize &size);

    /// @brief 获取布局宽度
    /// @return 布局宽度
    int32_t layoutWidth();
    /// @brief 获取布局高度
    /// @return 布局高度
    int32_t layoutHeight();
    /// @brief 获取布局尺寸
    /// @return 布局尺寸
    TpSize layoutSize();

    /// @brief 设置基于Layout的居中模式
    /// @param align 布局属性
    void setAlign(Tp::Alignment align = Tp::AlignLeft);
    /// @brief 获取字体居中模式
    /// @return 居中模式
    Tp::Alignment align();

    /// @brief 设置是否为垂直文本
    /// @param isVertical 是否垂直
    void setVerticalText(bool isVertical);
    /// @brief 获取是否为垂直文本
    /// @return 垂直文本为true
    bool isVerticalText();

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

    /// @brief 获取当前字体宽度
    /// @return 宽度值
    virtual int32_t pixelWidth();
    /// @brief 获取当前字体高度
    /// @return 高度值
    virtual int32_t pixelHeight();
    /// @brief 获取当前字体尺寸
    /// @return 尺寸
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

    /// @brief 获取字体家族名称
    /// @return 字体名称列表
    static TpList<TpString> families();

    /// @brief 指定字体文件路径加载字库;使用该字体时字体名需与文件名保持一致
    /// @param fontPath 字体(TTF)文件绝对路径
    /// @return 加载结果
    static bool loadFont(const TpString &fontPath);

private:
    ITpFontFamilyData *familySet;
};

#endif
