#include "TpFont.h"
#include "TpColors.h"
#include "TpRect.h"
#include "TpSize.h"
#include "TpSurface.h"
#include "TpCanvas.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <iostream>

#define DEFAULT_FONT_SIZE 12
#define DEFAULT_DPI 96
#define DEFAULT_DEPTH 32
#define _MAKE_FONT_NAME(family, size) family " " #size
#define MAKE_FONT_NAME(family, size) _MAKE_FONT_NAME(family, size)
#define DEFAULT_FONT_NAME_LENGTH 1024

#define DEFAULT_AMASK 0xff000000
#define DEFAULT_RMASK 0x00ff0000
#define DEFAULT_GMASK 0x0000ff00
#define DEFAULT_BMASK 0x000000ff

struct TpFontFamilySet
{
    int32_t numbers;
    PangoFontFamily **families;

    TpFontFamilySet() : families(nullptr)
    {
    }

    ~TpFontFamilySet()
    {
        if (families)
        {
            delete families;
            families = nullptr;
        }
    }
};

struct TpSurfaceArgs
{
    int32_t depth = 0;
    int32_t Rmask = 0;
    int32_t Gmask = 0;
    int32_t Bmask = 0;
    int32_t Amask = 0;
};

struct TpFontData
{
    TpString text = "";

    PangoContext *context = nullptr;
    PangoFontMap *font_map = nullptr;
    PangoFontDescription *font_desc = nullptr;
    PangoLayout *layout = nullptr;

    TpSurfaceArgs surface_args;
    int32_t min_width = 0;
    int32_t min_height = 0;
    int32_t fgcolor = 0;
    int32_t bgcolor = 0;
    TpFont::TpFontColorAttrib attrib;
    TpFont::TpFontFontAntialias antialias;
    TpFont::TpFontHinting hinting;
    int32_t underline = 0;
    int32_t undercolor, strokecolor, topcolor;
    int32_t underwidth, strokewidth, topwidth;
    bool useMarkUp;

    // 字体大小
    int32_t ptsize = 12;

    std::mutex pangoMutex; // 递归锁支持重入

    TpFontData()
    {
    }
};

static inline void _setSurfaceCreateArgs(TpFontData *context, int32_t depth,
                                         int32_t Rmask, int32_t Gmask, int32_t Bmask, int32_t Amask)
{
    context->surface_args.depth = depth;
    context->surface_args.Rmask = Rmask;
    context->surface_args.Gmask = Gmask;
    context->surface_args.Bmask = Bmask;
    context->surface_args.Amask = Amask;
}

static inline TpFontData *_createContext(const char *family, int32_t ptsize)
{
    // TpFontData *context = (TpFontData *)g_malloc(sizeof(TpFontData));
    TpFontData *context = new TpFontData();

    if (context == nullptr)
        return nullptr;

    const char *charset;
    context->font_map = pango_cairo_font_map_new();
    context->context = pango_font_map_create_context(PANGO_FONT_MAP(context->font_map));

    if (context->context == nullptr)
    {
        delete context;
        context = nullptr;
        // g_free(context);
        return nullptr;
    }

    pango_cairo_context_set_resolution(context->context, DEFAULT_DPI);
    g_get_charset(&charset);
    pango_context_set_language(context->context, pango_language_from_string(charset));
    pango_context_set_base_dir(context->context, PANGO_DIRECTION_LTR);

    char fontNameString[DEFAULT_FONT_NAME_LENGTH] = {0};
    sprintf(fontNameString, "%s %d", family, ptsize);

    context->font_desc = pango_font_description_from_string(fontNameString);

    if (context->font_desc == nullptr)
    {
        context->font_desc = pango_font_description_from_string(MAKE_FONT_NAME(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    }

    pango_font_description_set_weight(context->font_desc, PANGO_WEIGHT_NORMAL);
    pango_context_set_base_dir(context->context, PANGO_DIRECTION_NEUTRAL);

    context->layout = pango_layout_new(context->context);
    context->min_height = 0;
    context->min_width = 0;
    context->fgcolor = TpColors::Black;
    context->bgcolor = TpColors::White;
    context->attrib = TpFont::TINY_FONT_TRANSPARENCY;
    context->antialias = TpFont::TINY_FONT_ANTIALIAS_DEFAULT;
    context->hinting = TpFont::TINY_FONT_HINT_STYLE_DEFAULT;
    context->underline = TINY_FONT_NORMAL;
    context->undercolor = TpColors::Black;
    context->strokecolor = TpColors::Black;
    context->topcolor = TpColors::Black;
    context->useMarkUp = false;
    context->underwidth = 1;
    context->strokewidth = 1;
    context->topwidth = 1;

    _setSurfaceCreateArgs(context, DEFAULT_DEPTH, DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);

    return context;
}

static inline void _freeContext(TpFontData *context)
{
    if (!context)
        return;

    // 按正确顺序释放Pango对象
    if (context->layout)
    {
        g_object_unref(context->layout);
        context->layout = nullptr;
    }
    if (context->font_desc)
    {
        pango_font_description_free(context->font_desc);
        context->font_desc = nullptr;
    }
    if (context->context)
    {
        g_object_unref(context->context);
        context->context = nullptr;
    }

    if (context->font_map)
    {
        g_object_unref(context->font_map);
        context->font_map = nullptr;
    }

    delete context;
    // g_free(context);
    context = nullptr;
}

static inline void _setMinimumSize(TpFontData *context, int32_t width, int32_t height)
{
    int32_t pango_width;
    if (width > 0)
    {
        pango_width = width * PANGO_SCALE;
    }
    else
    {
        pango_width = -1;
        pango_layout_set_width(context->layout, pango_width);
    }

    context->min_width = width;
    context->min_height = height;
}

static inline int32_t _getLayoutWidth(TpFontData *context)
{
    PangoRectangle logical_rect;
    pango_layout_get_extents(context->layout, nullptr, &logical_rect);
    // pango_layout_get_pixel_extents (context->layout, nullptr, &logical_rect);

    // return logical_rect.width;
    return PANGO_PIXELS(logical_rect.width);
}

static inline int32_t _getLayoutHeight(TpFontData *context)
{
    PangoRectangle logical_rect;
    pango_layout_get_extents(context->layout, nullptr, &logical_rect);

    return PANGO_PIXELS(logical_rect.height);
}

static inline void _setText(TpFontData *context, const char *text, int32_t length)
{
    // std::cout << " ******************* text " << text << std::endl;
    std::lock_guard<std::mutex> lock(context->pangoMutex);

    context->useMarkUp = false;
    pango_layout_set_attributes(context->layout, nullptr);
    pango_layout_set_text(context->layout, text, length);
    pango_layout_set_auto_dir(context->layout, true);
    pango_layout_set_alignment(context->layout, PANGO_ALIGN_LEFT);
    pango_layout_set_font_description(context->layout, context->font_desc);
}

static inline void _setDpi(TpFontData *context, double dpi)
{
    pango_cairo_context_set_resolution(context->context, dpi);
}

static inline void _setFontSize(TpFontData *context, int32_t ptsize)
{
    pango_font_description_set_size(context->font_desc, ptsize * PANGO_SCALE);
}

static inline void _setFontFamily(TpFontData *context, const char *family)
{
    pango_font_description_set_family(context->font_desc, family);
}

static inline void _setFontStyle(TpFontData *context, int32_t style)
{
    if (style == TINY_FONT_NORMAL)
    {
        pango_font_description_set_weight(context->font_desc, PANGO_WEIGHT_NORMAL);
        pango_font_description_set_style(context->font_desc, PANGO_STYLE_NORMAL);
    }
    else
    {

        if ((style & TINY_FONT_ITALIC) == TINY_FONT_ITALIC)
        {
            pango_font_description_set_style(context->font_desc, PANGO_STYLE_ITALIC);
        }

        if ((style & TINY_FONT_BOLD) == TINY_FONT_BOLD)
        {
            pango_font_description_set_weight(context->font_desc, PANGO_WEIGHT_BOLD);
        }

        context->underline = (style & TINY_FONT_UNDERLINE) | (style & TINY_FONT_STROKELINE) | (style & TINY_FONT_TOPLINE);
    }
}

static inline void _setFontUnderLineColor(TpFontData *context, int32_t color)
{
    context->undercolor = color;
}

static inline void _setFontStrokeLineColor(TpFontData *context, int32_t color)
{
    context->strokecolor = color;
}

static inline void _setFontTopLineColor(TpFontData *context, int32_t color)
{
    context->topcolor = color;
}

static inline void _setFontUnderLineWidth(TpFontData *context, int32_t width)
{
    context->underwidth = width;
}

static inline void _setFontStrokeLineWidth(TpFontData *context, int32_t width)
{
    context->strokewidth = width;
}

static inline void _setFontTopLineWidth(TpFontData *context, int32_t width)
{
    context->topwidth = width;
}

static inline void _setFontWeight(TpFontData *context, TpFont::TpFontWeight weight)
{
    pango_font_description_set_weight(context->font_desc, (PangoWeight)weight);
}

static inline void _setLanguage(TpFontData *context, const char *language_tag)
{
    pango_context_set_language(context->context, pango_language_from_string(language_tag));
}

static inline void _setFontColorsAttrib(TpFontData *context, TpFont::TpFontColorAttrib attrib)
{
    context->attrib = attrib;
}

static inline void _setAntialias(TpFontData *context, TpFont::TpFontFontAntialias antialias)
{
    context->antialias = antialias;
}

static inline void _setHinting(TpFontData *context, TpFont::TpFontHinting hinting)
{
    context->hinting = hinting;
}

static inline void _setFontForeColor(TpFontData *context, int32_t fgcolor)
{
    context->fgcolor = fgcolor;
}

static inline void _setFontBackColor(TpFontData *context, int32_t bgcolor)
{
    context->bgcolor = bgcolor;
}

static inline void _setFontColor(TpFontData *context, int32_t fgcolor, int32_t bgcolor)
{
    context->fgcolor = fgcolor;
    context->bgcolor = bgcolor;
}

static inline void _setBaseDirection(TpFontData *context, TpFont::TpFontDirection direction)
{
    PangoDirection pango_dir = PANGO_DIRECTION_LTR;

    switch (direction)
    {
    case TpFont::TINY_FONT_DIRECTION_RTL:
        pango_dir = PANGO_DIRECTION_RTL;
        break;
    case TpFont::TINY_FONT_DIRECTION_LTR:
        pango_dir = PANGO_DIRECTION_WEAK_LTR;
        break;
    case TpFont::TINY_FONT_DIRECTION_WEAK_RTL:
        pango_dir = PANGO_DIRECTION_WEAK_RTL;
        break;
    case TpFont::TINY_FONT_DIRECTION_NEUTRAL:
        pango_dir = PANGO_DIRECTION_NEUTRAL;
        break;
    }

    pango_context_set_base_dir(context->context, pango_dir);
}

static inline PangoFontMap *_getPangoFontMap(TpFontData *context)
{
    return context->font_map;
}

static inline PangoFontDescription *_getPangoFontDescription(TpFontData *context)
{
    return context->font_desc;
}

static inline PangoLayout *_getPangoLayout(TpFontData *context)
{
    return context->layout;
}

static inline int32_t _getPixelWidth(TpFontData *context)
{
    std::lock_guard<std::mutex> lock(context->pangoMutex);

    int32_t width = 0;
    pango_layout_set_font_description(context->layout, context->font_desc);
    pango_layout_get_pixel_size(context->layout, &width, nullptr);
    return width;
}

static inline int32_t _getPixelHeight(TpFontData *context)
{
    std::lock_guard<std::mutex> lock(context->pangoMutex);

    int32_t height = 0;
    pango_layout_set_font_description(context->layout, context->font_desc);
    pango_layout_get_pixel_size(context->layout, nullptr, &height);
    return height;
}

static inline ItpSize _getPixelSize(TpFontData *context)
{
    std::lock_guard<std::mutex> lock(context->pangoMutex);

    ItpSize size;
    pango_layout_set_font_description(context->layout, context->font_desc);
    pango_layout_get_pixel_size(context->layout, (int32_t *)&size.w, (int32_t *)&size.h);
    return size;
}

TpFontFamily::TpFontFamily()
{
    TpFontFamilySet *set = new TpFontFamilySet();

    if (set)
    {
        PangoFontMap *fontmap = pango_cairo_font_map_get_default();
        pango_font_map_list_families(fontmap, &set->families, &set->numbers);

        this->familySet = set;
    }
}

int32_t TpFontFamily::getFontFamilyNumbers()
{
    int32_t numbers = 0;
    TpFontFamilySet *set = (TpFontFamilySet *)this->familySet;

    if (set)
    {
        numbers = set->numbers;
    }

    return numbers;
}

const char *TpFontFamily::getFontFamilyName(int32_t index)
{
    const char *result = nullptr;
    TpFontFamilySet *set = (TpFontFamilySet *)this->familySet;

    if (set)
    {
        if (index >= 0 &&
            index <= set->numbers)
        {
            result = pango_font_family_get_name(set->families[index]);
        }
    }

    return result;
}

TpFontFamily::~TpFontFamily()
{
    TpFontFamilySet *set = (TpFontFamilySet *)this->familySet;

    if (set)
    {
        // g_free(set->families);
        delete set;
        set = nullptr;
    }
}

TpFont::TpFont(const char *family, int32_t defaultPtSize) : data_(nullptr)
{
    this->data_ = _createContext(family, defaultPtSize);
}

void TpFont::setLanguage(const char *language)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setLanguage(set, language);
    }
}

void TpFont::setFontWeight(TpFontWeight weight)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontWeight(set, weight);
    }
}

void TpFont::setAntialias(TpFontFontAntialias antialias)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setAntialias(set, antialias);
    }
}

void TpFont::setHinting(TpFontHinting hinting)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setHinting(set, hinting);
    }
}

void TpFont::setFontColorAttrib(TpFontColorAttrib attrib)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontColorsAttrib(set, attrib);
    }
}

void TpFont::setFontColor(int32_t fg_color, int32_t bg_color)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontColor(set, fg_color, bg_color);
    }
}

void TpFont::setFontForeColor(int32_t fg_color)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontForeColor(set, fg_color);
    }
}

void TpFont::setFontBackColor(int32_t bg_color)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontBackColor(set, bg_color);
    }
}

void TpFont::setFontStyle(int32_t style)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontStyle(set, style);
    }
}

void TpFont::setFontDPI(double Dpi)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setDpi(set, Dpi);
    }
}

int32_t TpFont::fontSize()
{
    TpFontData *set = (TpFontData *)this->data_;
    if (!set)
        return 0;

    return set->ptsize;
}

void TpFont::setFontSize(const int32_t &ptsize)
{
    TpFontData *set = (TpFontData *)this->data_;
    if (!set)
        return;

    set->ptsize = TP_MAX(ptsize, 0);
    _setFontSize(set, set->ptsize);
}

void TpFont::setFontFamily(const char *family)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontFamily(set, family);
    }
}

void TpFont::setFontUnderLineColor(int32_t color)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontUnderLineColor(set, color);
    }
}

void TpFont::setFontStrokeLineColor(int32_t color)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontStrokeLineColor(set, color);
    }
}

void TpFont::setFontTopLineColor(int32_t color)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontTopLineColor(set, color);
    }
}

void TpFont::setFontUnderLineWidth(int32_t width)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontUnderLineWidth(set, width);
    }
}

void TpFont::setFontStrokeLineWidth(int32_t width)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontStrokeLineWidth(set, width);
    }
}

void TpFont::setFontTopLineWidth(int32_t width)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setFontTopLineWidth(set, width);
    }
}

void TpFont::setMinimumSize(int32_t width, int32_t height)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setMinimumSize(set, width, height);
    }
}

void TpFont::setBaseDirection(TpFont::TpFontDirection direction)
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        _setBaseDirection(set, direction);
    }
}

void TpFont::setText(const TpString &text)
{
    TpFontData *set = (TpFontData *)this->data_;
    if (!set)
        return;

    if (text.empty())
        return;

    set->text = text;
    _setText(set, text.c_str(), text.length());
}

TpString TpFont::text()
{
    TpFontData *set = static_cast<TpFontData *>(data_);
    if (!set)
        return "";
    return set->text;
}

int32_t TpFont::layoutWidth()
{
    TpFontData *set = (TpFontData *)this->data_;
    int32_t width = 0;

    if (set)
    {
        width = _getLayoutWidth(set);
    }

    return width;
}

int32_t TpFont::layoutHeight()
{
    TpFontData *set = (TpFontData *)this->data_;
    int32_t height = 0;

    if (set)
    {
        height = _getLayoutHeight(set);
    }

    return height;
}

int32_t TpFont::pixelWidth()
{
    TpFontData *set = (TpFontData *)this->data_;
    int32_t width = 0;

    if (set)
    {
        // std::lock_guard<std::mutex> lock_g(set->getSizeMutex);

        width = _getPixelWidth(set);
    }

    return width;
}

int32_t TpFont::pixelHeight()
{
    TpFontData *set = (TpFontData *)this->data_;
    int32_t height = 0;

    if (set)
    {
        height = _getPixelHeight(set);
    }

    return height;
}

ItpSize TpFont::pixelSize()
{
    TpFontData *set = (TpFontData *)this->data_;
    ItpSize size = {0, 0};

    if (set)
    {
        size = _getPixelSize(set);
    }

    return size;
}

TpFontFamily *TpFont::getSysFamilyFont()
{
    return (new TpFontFamily());
}

uint32_t *TpFont::renderText(const TpString &text)
{
    TpFontData *set = static_cast<TpFontData *>(data_);
    if (!set)
        return nullptr;

    if (text.empty())
        return nullptr;

    _setText(set, text.c_str(), text.length());

    ItpSize size = _getPixelSize(set);

    if (size.w == 0 || size.h == 0)
        return nullptr;

    uint32_t *buffer = new uint32_t[size.w * size.h];
    memset(buffer, 0, size.w * size.h * sizeof(uint32_t));

    cairo_surface_t *surf = cairo_image_surface_create_for_data(
        (unsigned char *)buffer,
        CAIRO_FORMAT_ARGB32,
        size.w, size.h,
        size.w * 4);

    // cairo_surface_t *surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size.w, size.h);
    if (surf == nullptr)
        return nullptr;

    cairo_t *cr = cairo_create(surf);
    if (!cr)
        return nullptr;

    cairo_font_options_t *options = cairo_font_options_create();

    if (options)
    {
        cairo_antialias_t antialias_t = CAIRO_ANTIALIAS_DEFAULT;

        switch (set->antialias)
        {
        case TpFont::TINY_FONT_ANTIALIAS_NONE:
            antialias_t = CAIRO_ANTIALIAS_NONE;
            break;
        case TpFont::TINY_FONT_ANTIALIAS_GRAY:
            antialias_t = CAIRO_ANTIALIAS_GRAY;
            break;
        case TpFont::TINY_FONT_ANTIALIAS_SUBPIXEL:
            antialias_t = CAIRO_ANTIALIAS_SUBPIXEL;
            break;
        case TpFont::TINY_FONT_ANTIALIAS_FAST:
            antialias_t = CAIRO_ANTIALIAS_FAST;
            break;
        case TpFont::TINY_FONT_ANTIALIAS_GOOD:
            antialias_t = CAIRO_ANTIALIAS_GOOD;
            break;
        case TpFont::TINY_FONT_ANTIALIAS_BEST:
            antialias_t = CAIRO_ANTIALIAS_BEST;
            break;
        }

        cairo_hint_style_t hint_style_t = CAIRO_HINT_STYLE_DEFAULT;

        switch (set->hinting)
        {
        case TpFont::TINY_FONT_HINT_STYLE_NONE:
            hint_style_t = CAIRO_HINT_STYLE_NONE;
            break;
        case TpFont::TINY_FONT_HINT_STYLE_SLIGHT:
            hint_style_t = CAIRO_HINT_STYLE_SLIGHT;
            break;
        case TpFont::TINY_FONT_HINT_STYLE_MEDIUM:
            hint_style_t = CAIRO_HINT_STYLE_MEDIUM;
            break;
        case TpFont::TINY_FONT_HINT_STYLE_FULL:
            hint_style_t = CAIRO_HINT_STYLE_FULL;
            break;
        }

        cairo_font_options_set_antialias(options, antialias_t);
        cairo_font_options_set_hint_style(options, hint_style_t);

        pango_cairo_context_set_font_options(set->context, options);
    }

    double fr = _R(set->fgcolor) / 255.0;
    double fg = _G(set->fgcolor) / 255.0;
    double fb = _B(set->fgcolor) / 255.0;
    double fa = _A(set->fgcolor) / 255.0;

    if (set->useMarkUp == false)
    {
        double br = _R(set->bgcolor) / 255.0;
        double bg = _G(set->bgcolor) / 255.0;
        double bb = _B(set->bgcolor) / 255.0;
        double ba = _A(set->bgcolor) / 255.0;

        switch (set->attrib)
        {
        case TpFont::TINY_FONT_OPAQUE:
        {
            ba = 1.0;
        }
        break;
        case TpFont::TINY_FONT_TRANSPARENCY:
        {
            ba = 0.0;
        }
        break;
        }
        cairo_set_source_rgba(cr, br, bg, bb, ba);
        cairo_rectangle(cr, 0, 0, size.w, size.h);
        cairo_fill(cr);
    }

    cairo_set_source_rgba(cr, fr, fg, fb, fa);
    pango_cairo_update_context(cr, set->context);
    pango_cairo_update_layout(cr, set->layout);
    pango_cairo_show_layout(cr, set->layout);

    cairo_font_options_destroy(options);
    cairo_surface_destroy(surf);
    cairo_destroy(cr);

    return buffer;
}

TpFont::~TpFont()
{
    TpFontData *set = (TpFontData *)this->data_;

    if (set)
    {
        std::lock_guard<std::mutex> lock(set->pangoMutex);

        _freeContext(set);
        set = nullptr;
    }
}
