#include "TpFont.h"
#include "TpFont_p.h"

TpFont::TpFont(const TpString &language, int32_t defaultPtSize)
{
    TpFontData *fontData = new TpFontData();
    data_ = fontData;

    if (TpFontConfig::Inst()->containsFont(language))
    {
        fontData->tvgTextPtr->font(TpFontConfig::Inst()->fontName(language, FONT_WEIGHT_NORMAL).c_str());
    }
    else
    {
        fontData->tvgTextPtr->font(TpFontConfig::Inst()->fontName(DEFAULT_FONT_FAMILY, FONT_WEIGHT_NORMAL).c_str());
    }

    fontData->ptsize = defaultPtSize;
    fontData->tvgTextPtr->size(defaultPtSize);
    setAlign(Tp::AlignLeft);

    setFontColor(_RGB(0, 0, 0));
}

TpFont::~TpFont()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    if (fontData)
    {
        delete fontData;
        fontData = nullptr;
        data_ = nullptr;
    }
}

void TpFont::setLanguage(const TpString &language)
{
    if (TpFontConfig::Inst()->containsFont(language))
    {
        TpFontData *fontData = static_cast<TpFontData *>(data_);
        fontData->tvgTextPtr->font(language.c_str());
    }
}

void TpFont::setFontColor(int32_t color, int32_t color2)
{
    setFontForeColor(color);
    setFontBackColor(color2);
}

void TpFont::setFontColor(int32_t color)
{
    setFontForeColor(color);
    setFontBackColor(color);
}

void TpFont::setFontColor(const TpColors &color)
{
    setFontForeColor(color.rgba());
    setFontBackColor(color.rgba());
}

void TpFont::setFontBackColor(int32_t bgColor)
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    fontData->tvgTextPtr->fill(_R(bgColor), _G(bgColor), _B(bgColor));
}

void TpFont::setFontForeColor(int32_t fgColor)
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    fontData->fgColor = fgColor;
    fontData->tvgTextPtr->outline(0, _R(fgColor), _G(fgColor), _B(fgColor));

    // 轮廓颜色变化，要根据是否有加粗效果调整轮廓
    setBold(bold());
}

void TpFont::setItalic(bool italic)
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    fontData->italic = italic;
    fontData->tvgTextPtr->italic(italic ? 0.18f : 0);
}

bool TpFont::italic()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    return fontData->italic;
}

void TpFont::setBold(bool bold)
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    fontData->bold = bold;
    fontData->tvgTextPtr->outline(bold ? 1 : 0, _R(fontData->fgColor), _G(fontData->fgColor), _B(fontData->fgColor));
}

bool TpFont::bold()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    return fontData->bold;
}

void TpFont::setWrap(bool wrap)
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    fontData->wrap = wrap;
    fontData->tvgTextPtr->wrap(wrap ? tvg::TextWrap::Smart : tvg::TextWrap::None);
}

bool TpFont::wrap()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    return fontData->wrap;
}

void TpFont::setLayout(int32_t w, int32_t h)
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    fontData->tvgTextPtr->layout(w, h);

    fontData->wrapLayout.setWidth(w);
    fontData->wrapLayout.setHeight(h);
    setAlign(align());
}

void TpFont::setLayout(const TpSize &size)
{
    setLayout(size.width(), size.height());
}

int32_t TpFont::layoutWidth()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    return fontData->wrapLayout.width();
}

int32_t TpFont::layoutHeight()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    return fontData->wrapLayout.height();
}

TpSize TpFont::layoutSize()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    return fontData->wrapLayout;
}

void TpFont::setAlign(Tp::Alignment align)
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    fontData->alignFlag = align;

    float x = 0;
    float y = 0;
    // 处理水平对齐
    if (align & Tp::AlignLeft)
    {
        x = 0.0f;
    }
    else if (align & Tp::AlignRight)
    {
        x = 0.97f;
    }
    else if (align & Tp::AlignHCenter)
    {
        x = 0.5f;
    }
    else
    {
    }

    // 处理垂直对齐
    if (align & Tp::AlignTop)
    {
        y = 0.0f;
    }
    else if (align & Tp::AlignBottom)
    {
        y = 0.97f;
    }
    else if (align & Tp::AlignVCenter)
    {
        y = 0.5f;
    }
    else
    {
    }

    fontData->tvgTextPtr->align(x, y);
}

Tp::Alignment TpFont::align()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    return fontData->alignFlag;
}

int32_t TpFont::fontSize()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    return fontData->ptsize;
}

void TpFont::setFontSize(const int32_t &ptsize)
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    fontData->ptsize = TP_MAX(ptsize, 0);
    fontData->tvgTextPtr->size(fontData->ptsize);

    setAlign(align());
}

void TpFont::setText(const TpString &text)
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    fontData->text = text;
    fontData->tvgTextPtr->text(text.c_str());

    setAlign(align());
}

TpString TpFont::text() const
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    return fontData->text;
}

int32_t TpFont::pixelWidth()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    if (fontData->text.empty())
        return 0;

    float x, y, w, h;
    fontData->tvgTextPtr->bounds(&x, &y, &w, &h);
    return w + 1;
}

int32_t TpFont::pixelHeight()
{
    TpFontData *fontData = static_cast<TpFontData *>(data_);
    if (fontData->text.empty())
        return 0;

    float x, y, w, h;
    fontData->tvgTextPtr->bounds(&x, &y, &w, &h);
    return h + 1;
}

TpSize TpFont::pixelSize()
{
    TpSize size = {0, 0};

    TpFontData *fontData = static_cast<TpFontData *>(data_);
    if (fontData->text.empty())
        return size;

    float x, y, w, h;
    fontData->tvgTextPtr->bounds(&x, &y, &w, &h);

    size.setWidth(w + 1);
    size.setHeight(h + 1);

    return size;
}

TpFontFamily::TpFontFamily()
{
}

TpFontFamily::~TpFontFamily()
{
}

TpList<TpString> TpFontFamily::families()
{
    return TpFontConfig::Inst()->families();
}

bool TpFontFamily::loadFont(const TpString &fontPath)
{
    return TpFontConfig::Inst()->loadFont(fontPath);
}
