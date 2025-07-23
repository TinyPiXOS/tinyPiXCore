#include "tpFont.h"
#include "tpColors.h"
#include "tpRect.h"
#include "tpSize.h"
#include "tpSurface.h"
#include "tpCanvas.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <cairo.h>
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

struct tpFontFamilySet
{
	int32_t numbers;
	PangoFontFamily **families;

	tpFontFamilySet() : families(nullptr)
	{
	}

	~tpFontFamilySet()
	{
		if (families)
		{
			delete families;
			families = nullptr;
		}
	}
};

struct tpSurfaceArgs
{
	int32_t depth = 0;
	int32_t Rmask = 0;
	int32_t Gmask = 0;
	int32_t Bmask = 0;
	int32_t Amask = 0;
};

struct tpFontData
{
	PangoContext *context = nullptr;
	PangoFontMap *font_map = nullptr;
	PangoFontDescription *font_desc = nullptr;
	PangoLayout *layout = nullptr;

	tpSurfaceArgs surface_args;
	int32_t min_width = 0;
	int32_t min_height = 0;
	int32_t fgcolor = 0;
	int32_t bgcolor = 0;
	tpFont::tpFontColorAttrib attrib;
	tpFont::tpFontFontAntialias antialias;
	tpFont::tpFontHinting hinting;
	int32_t underline = 0;
	int32_t undercolor, strokecolor, topcolor;
	int32_t underwidth, strokewidth, topwidth;
	bool useMarkUp;

	// 字体大小
	int32_t ptsize = 12;

	std::mutex pangoMutex; // 递归锁支持重入
	// std::mutex getSizeMutex;

	tpFontData()
	{
	}

	// tpFontData &operator=(const tpFontData &others)
	// {
	// 	this->surface_args = others.surface_args;
	// 	this->min_width = others.min_width;
	// 	this->min_height = others.min_height;
	// 	this->fgcolor = others.fgcolor;
	// 	this->bgcolor = others.bgcolor;
	// 	this->attrib = others.attrib;
	// 	this->antialias = others.antialias;
	// 	this->hinting = others.hinting;
	// 	this->underline = others.underline;
	// 	this->undercolor = others.undercolor;
	// 	this->strokecolor = others.strokecolor;
	// 	this->topcolor = others.topcolor;
	// 	this->underwidth = others.underwidth;
	// 	this->strokewidth = others.strokewidth;
	// 	this->topwidth = others.topwidth;
	// 	this->useMarkUp = others.useMarkUp;
	// 	this->ptsize = others.ptsize;

	// 	return *this;
	// }
};

static inline unsigned int cal_stride(int width, int depth)
{
	std::cout << "-----------------------depth  " << depth << std::endl;
	int bpp = depth / 8;
	unsigned int stride = width * bpp;

	switch (depth)
	{
	case 4:
		stride = (stride + 1) / 2;
		break;
	default:
		break;
	}

	return ((stride + 3) & ~3);
}

static inline void _setSurfaceCreateArgs(tpFontData *context, int32_t depth,
										 int32_t Rmask, int32_t Gmask, int32_t Bmask, int32_t Amask)
{
	context->surface_args.depth = depth;
	context->surface_args.Rmask = Rmask;
	context->surface_args.Gmask = Gmask;
	context->surface_args.Bmask = Bmask;
	context->surface_args.Amask = Amask;
}

static inline tpFontData *_createContext(const char *family, int32_t ptsize)
{
	// tpFontData *context = (tpFontData *)g_malloc(sizeof(tpFontData));
	tpFontData *context = new tpFontData();

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
	context->fgcolor = tpColors::Black;
	context->bgcolor = tpColors::White;
	context->attrib = tpFont::TINY_FONT_TRANSPARENCY;
	context->antialias = tpFont::TINY_FONT_ANTIALIAS_DEFAULT;
	context->hinting = tpFont::TINY_FONT_HINT_STYLE_DEFAULT;
	context->underline = TINY_FONT_NORMAL;
	context->undercolor = tpColors::Black;
	context->strokecolor = tpColors::Black;
	context->topcolor = tpColors::Black;
	context->useMarkUp = false;
	context->underwidth = 1;
	context->strokewidth = 1;
	context->topwidth = 1;

	_setSurfaceCreateArgs(context, DEFAULT_DEPTH, DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);

	return context;
}

static inline void _freeContext(tpFontData *context)
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

static inline void _setMinimumSize(tpFontData *context, int32_t width, int32_t height)
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

static inline int32_t _getLayoutWidth(tpFontData *context)
{
	PangoRectangle logical_rect;
	pango_layout_get_extents(context->layout, nullptr, &logical_rect);
	// pango_layout_get_pixel_extents (context->layout, nullptr, &logical_rect);

	// return logical_rect.width;
	return PANGO_PIXELS(logical_rect.width);
}

static inline int32_t _getLayoutHeight(tpFontData *context)
{
	PangoRectangle logical_rect;
	pango_layout_get_extents(context->layout, nullptr, &logical_rect);

	return PANGO_PIXELS(logical_rect.height);
}

static inline void _setMarkup(tpFontData *context, const char *markup, int32_t length)
{
	context->useMarkUp = true;
	pango_layout_set_markup(context->layout, markup, length);
	pango_layout_set_auto_dir(context->layout, true);
	pango_layout_set_alignment(context->layout, PANGO_ALIGN_LEFT);
	pango_layout_set_font_description(context->layout, context->font_desc);
}

static inline void _setText(tpFontData *context, const char *text, int32_t length)
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

static inline void _setDpi(tpFontData *context, double dpi)
{
	pango_cairo_context_set_resolution(context->context, dpi);
}

static inline void _setFontSize(tpFontData *context, int32_t ptsize)
{
	pango_font_description_set_size(context->font_desc, ptsize * PANGO_SCALE);
}

static inline void _setFontFamily(tpFontData *context, const char *family)
{
	pango_font_description_set_family(context->font_desc, family);
}

static inline void _setFontStyle(tpFontData *context, int32_t style)
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

static inline void _setFontUnderLineColor(tpFontData *context, int32_t color)
{
	context->undercolor = color;
}

static inline void _setFontStrokeLineColor(tpFontData *context, int32_t color)
{
	context->strokecolor = color;
}

static inline void _setFontTopLineColor(tpFontData *context, int32_t color)
{
	context->topcolor = color;
}

static inline void _setFontUnderLineWidth(tpFontData *context, int32_t width)
{
	context->underwidth = width;
}

static inline void _setFontStrokeLineWidth(tpFontData *context, int32_t width)
{
	context->strokewidth = width;
}

static inline void _setFontTopLineWidth(tpFontData *context, int32_t width)
{
	context->topwidth = width;
}

static inline void _setFontWeight(tpFontData *context, tpFont::tpFontWeight weight)
{
	pango_font_description_set_weight(context->font_desc, (PangoWeight)weight);
}

static inline void _setLanguage(tpFontData *context, const char *language_tag)
{
	pango_context_set_language(context->context, pango_language_from_string(language_tag));
}

static inline void _setFontColorsAttrib(tpFontData *context, tpFont::tpFontColorAttrib attrib)
{
	context->attrib = attrib;
}

static inline void _setAntialias(tpFontData *context, tpFont::tpFontFontAntialias antialias)
{
	context->antialias = antialias;
}

static inline void _setHinting(tpFontData *context, tpFont::tpFontHinting hinting)
{
	context->hinting = hinting;
}

static inline void _setFontForeColor(tpFontData *context, int32_t fgcolor)
{
	context->fgcolor = fgcolor;
}

static inline void _setFontBackColor(tpFontData *context, int32_t bgcolor)
{
	context->bgcolor = bgcolor;
}

static inline void _setFontColor(tpFontData *context, int32_t fgcolor, int32_t bgcolor)
{
	context->fgcolor = fgcolor;
	context->bgcolor = bgcolor;
}

static inline void _setBaseDirection(tpFontData *context, tpFont::tpFontDirection direction)
{
	PangoDirection pango_dir = PANGO_DIRECTION_LTR;

	switch (direction)
	{
	case tpFont::TINY_FONT_DIRECTION_RTL:
		pango_dir = PANGO_DIRECTION_RTL;
		break;
	case tpFont::TINY_FONT_DIRECTION_LTR:
		pango_dir = PANGO_DIRECTION_WEAK_LTR;
		break;
	case tpFont::TINY_FONT_DIRECTION_WEAK_RTL:
		pango_dir = PANGO_DIRECTION_WEAK_RTL;
		break;
	case tpFont::TINY_FONT_DIRECTION_NEUTRAL:
		pango_dir = PANGO_DIRECTION_NEUTRAL;
		break;
	}

	pango_context_set_base_dir(context->context, pango_dir);
}

static inline PangoFontMap *_getPangoFontMap(tpFontData *context)
{
	return context->font_map;
}

static inline PangoFontDescription *_getPangoFontDescription(tpFontData *context)
{
	return context->font_desc;
}

static inline PangoLayout *_getPangoLayout(tpFontData *context)
{
	return context->layout;
}

static inline int32_t _getPixelWidth(tpFontData *context)
{
	std::lock_guard<std::mutex> lock(context->pangoMutex);

	int32_t width = 0;
	pango_layout_set_font_description(context->layout, context->font_desc);
	pango_layout_get_pixel_size(context->layout, &width, nullptr);
	return width;
}

static inline int32_t _getPixelHeight(tpFontData *context)
{
	std::lock_guard<std::mutex> lock(context->pangoMutex);

	int32_t height = 0;
	pango_layout_set_font_description(context->layout, context->font_desc);
	pango_layout_get_pixel_size(context->layout, nullptr, &height);
	return height;
}

static inline ItpSize _getPixelSize(tpFontData *context)
{
	std::lock_guard<std::mutex> lock(context->pangoMutex);

	ItpSize size;
	pango_layout_set_font_description(context->layout, context->font_desc);
	pango_layout_get_pixel_size(context->layout, (int32_t *)&size.w, (int32_t *)&size.h);
	return size;
}

static inline void _surfaceToSurface(tpSurface *src, tpSurface *dst, int x, int y)
{
	if (dst == nullptr)
	{
		return;
	}

	unsigned int sw = src->width(), sh = src->height();
	unsigned int dw = dst->width(), dh = dst->height();
	unsigned int final_w = 0, final_h = 0;
	int sx = 0, sy = 0, dx = x, dy = y;

	// if (dx > dw ||
	// 	dy > dh)
	// {
	// 	return;
	// }

	if (dx < 0)
	{
		sx -= dx;
		sw += dx;
		dx = 0;
	}
	else
	{
		dw -= dx;
	}

	if (dy < 0)
	{
		sy -= dy;
		sh += dy;
		dy = 0;
	}
	else
	{
		dh -= dy;
	}

	final_w = TP_MIN(sw, dw);
	final_h = TP_MIN(sh, dh);

	tpRect srect(sx, sy, final_w, final_h);
	tpRect drect(dx, dy, final_w, final_h);

	src->directBlitT(*dst, srect, drect);
}

static inline void _destroySurface(tpSurface *surface)
{
	if (surface)
	{
		delete surface;
	}
}

static inline void _complexDraw(tpFontData *context, tpSurface *surface, int32_t x, int32_t y)
{
	ItpSize size = _getPixelSize(context);

	if (size.w == 0 || size.h == 0)
		return;

	cairo_surface_t *surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size.w, size.h);
	if (surf == nullptr)
		return;

	cairo_t *cr = cairo_create(surf);
	if (!cr)
		return;

	cairo_font_options_t *options = cairo_font_options_create();

	if (options)
	{
		cairo_antialias_t antialias_t = CAIRO_ANTIALIAS_DEFAULT;

		switch (context->antialias)
		{
		case tpFont::TINY_FONT_ANTIALIAS_NONE:
			antialias_t = CAIRO_ANTIALIAS_NONE;
			break;
		case tpFont::TINY_FONT_ANTIALIAS_GRAY:
			antialias_t = CAIRO_ANTIALIAS_GRAY;
			break;
		case tpFont::TINY_FONT_ANTIALIAS_SUBPIXEL:
			antialias_t = CAIRO_ANTIALIAS_SUBPIXEL;
			break;
		case tpFont::TINY_FONT_ANTIALIAS_FAST:
			antialias_t = CAIRO_ANTIALIAS_FAST;
			break;
		case tpFont::TINY_FONT_ANTIALIAS_GOOD:
			antialias_t = CAIRO_ANTIALIAS_GOOD;
			break;
		case tpFont::TINY_FONT_ANTIALIAS_BEST:
			antialias_t = CAIRO_ANTIALIAS_BEST;
			break;
		}

		cairo_hint_style_t hint_style_t = CAIRO_HINT_STYLE_DEFAULT;

		switch (context->hinting)
		{
		case tpFont::TINY_FONT_HINT_STYLE_NONE:
			hint_style_t = CAIRO_HINT_STYLE_NONE;
			break;
		case tpFont::TINY_FONT_HINT_STYLE_SLIGHT:
			hint_style_t = CAIRO_HINT_STYLE_SLIGHT;
			break;
		case tpFont::TINY_FONT_HINT_STYLE_MEDIUM:
			hint_style_t = CAIRO_HINT_STYLE_MEDIUM;
			break;
		case tpFont::TINY_FONT_HINT_STYLE_FULL:
			hint_style_t = CAIRO_HINT_STYLE_FULL;
			break;
		}

		cairo_font_options_set_antialias(options, antialias_t);
		cairo_font_options_set_hint_style(options, hint_style_t);

		pango_cairo_context_set_font_options(context->context, options);
	}

	double fr = _R(context->fgcolor) / 255.0;
	double fg = _G(context->fgcolor) / 255.0;
	double fb = _B(context->fgcolor) / 255.0;
	double fa = _A(context->fgcolor) / 255.0;

	if (context->useMarkUp == false)
	{
		double br = _R(context->bgcolor) / 255.0;
		double bg = _G(context->bgcolor) / 255.0;
		double bb = _B(context->bgcolor) / 255.0;
		double ba = _A(context->bgcolor) / 255.0;

		switch (context->attrib)
		{
		case tpFont::TINY_FONT_OPAQUE:
		{
			ba = 1.0;
		}
		break;
		case tpFont::TINY_FONT_TRANSPARENCY:
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
	pango_cairo_update_context(cr, context->context);
	pango_cairo_update_layout(cr, context->layout);
	pango_cairo_show_layout(cr, context->layout);

	tpSurface *srcSurf = tpCanvas::convertFromCairoToSurface(surf);

	if (srcSurf == nullptr)
	{
		goto failed;
	}

	if (context->underline &&
		context->useMarkUp == false)
	{
		tpCanvas *canvas = new tpCanvas(nullptr);
		if (canvas)
		{
			canvas->setTarget(srcSurf);

			if ((context->underline & TINY_FONT_UNDERLINE) == TINY_FONT_UNDERLINE)
			{
				int32_t bottom_x = 0, bottom_y = size.h - size.h / 6;
				// canvas->thickLine(bottom_x, bottom_y, size.w, bottom_y - context->underwidth, context->underwidth, context->undercolor);
			}

			if ((context->underline & TINY_FONT_STROKELINE) == TINY_FONT_STROKELINE)
			{
				int32_t bottom_x = 0, bottom_y = size.h / 2;
				// canvas->thickLine(bottom_x, bottom_y - context->strokewidth / 2, size.w, bottom_y + context->strokewidth / 2, context->strokewidth, context->strokecolor);
			}

			if ((context->underline & TINY_FONT_TOPLINE) == TINY_FONT_TOPLINE)
			{
				int32_t bottom_x = 0, bottom_y = size.h / 5;
				// canvas->thickLine(bottom_x, bottom_y, size.w, bottom_y + context->topwidth, context->topwidth, context->topcolor);
			}

			delete canvas;
		}
	}

	_surfaceToSurface(srcSurf, surface, x, y);
	_destroySurface(srcSurf);
failed:
	cairo_font_options_destroy(options);
	cairo_surface_destroy(surf);
	cairo_destroy(cr);
}

static inline tpSurface *_createSurfaceDraw(tpFontData *context)
{
	ItpSize size = _getPixelSize(context);

	if (size.w == 0 ||
		size.h == 0)
	{
		return nullptr;
	}

	tpSurface *surface = new tpSurface();

	if (surface)
	{
		unsigned int depth = (ItpFormat)context->surface_args.depth;
		unsigned int stride = cal_stride(size.w, depth);
		bool ret = surface->create(nullptr, size.w, size.h,
								   depth,
								   stride,
								   context->surface_args.Rmask,
								   context->surface_args.Gmask,
								   context->surface_args.Bmask,
								   context->surface_args.Amask);

		_complexDraw(context, surface, 0, 0);
	}

	return surface;
}

tpFontFamily::tpFontFamily()
{
	tpFontFamilySet *set = new tpFontFamilySet();

	if (set)
	{
		PangoFontMap *fontmap = pango_cairo_font_map_get_default();
		pango_font_map_list_families(fontmap, &set->families, &set->numbers);

		this->familySet = set;
	}
}

int32_t tpFontFamily::getFontFamilyNumbers()
{
	int32_t numbers = 0;
	tpFontFamilySet *set = (tpFontFamilySet *)this->familySet;

	if (set)
	{
		numbers = set->numbers;
	}

	return numbers;
}

const char *tpFontFamily::getFontFamilyName(int32_t index)
{
	const char *result = nullptr;
	tpFontFamilySet *set = (tpFontFamilySet *)this->familySet;

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

tpFontFamily::~tpFontFamily()
{
	tpFontFamilySet *set = (tpFontFamilySet *)this->familySet;

	if (set)
	{
		// g_free(set->families);
		delete set;
		set = nullptr;
	}
}

tpFontFamily *tpFont::getSysFamilyFont()
{
	return (new tpFontFamily());
}

tpFont::tpFont(const char *family, int32_t defaultPtSize): data_(nullptr)
{
	this->data_ = _createContext(family, defaultPtSize);
}

void tpFont::setLanguage(const char *language)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setLanguage(set, language);
	}
}

void tpFont::setFontWeight(tpFontWeight weight)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontWeight(set, weight);
	}
}

void tpFont::setAntialias(tpFontFontAntialias antialias)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setAntialias(set, antialias);
	}
}

void tpFont::setHinting(tpFontHinting hinting)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setHinting(set, hinting);
	}
}

void tpFont::setFontColorAttrib(tpFontColorAttrib attrib)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontColorsAttrib(set, attrib);
	}
}

void tpFont::setFontColor(int32_t fg_color, int32_t bg_color)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontColor(set, fg_color, bg_color);
	}
}

void tpFont::setFontForeColor(int32_t fg_color)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontForeColor(set, fg_color);
	}
}

void tpFont::setFontBackColor(int32_t bg_color)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontBackColor(set, bg_color);
	}
}

void tpFont::setFontStyle(int32_t style)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontStyle(set, style);
	}
}

void tpFont::setFontDPI(double Dpi)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setDpi(set, Dpi);
	}
}

int32_t tpFont::fontSize()
{
	tpFontData *set = (tpFontData *)this->data_;
	if (!set)
		return 0;

	return set->ptsize;
}

void tpFont::setFontSize(const int32_t &ptsize)
{
	tpFontData *set = (tpFontData *)this->data_;
	if (!set)
		return;

	set->ptsize = TP_MAX(ptsize, 0);
	_setFontSize(set, set->ptsize);
}

void tpFont::setFontFamily(const char *family)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontFamily(set, family);
	}
}

void tpFont::setFontUnderLineColor(int32_t color)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontUnderLineColor(set, color);
	}
}

void tpFont::setFontStrokeLineColor(int32_t color)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontStrokeLineColor(set, color);
	}
}

void tpFont::setFontTopLineColor(int32_t color)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontTopLineColor(set, color);
	}
}

void tpFont::setFontUnderLineWidth(int32_t width)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontUnderLineWidth(set, width);
	}
}

void tpFont::setFontStrokeLineWidth(int32_t width)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontStrokeLineWidth(set, width);
	}
}

void tpFont::setFontTopLineWidth(int32_t width)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setFontTopLineWidth(set, width);
	}
}

void tpFont::setMinimumSize(int32_t width, int32_t height)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setMinimumSize(set, width, height);
	}
}

void tpFont::setBaseDirection(tpFont::tpFontDirection direction)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_setBaseDirection(set, direction);
	}
}

void tpFont::setMarkUp(const char *text)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		if (text == nullptr ||
			strlen(text) == 0)
		{
			return;
		}

		_setMarkup(set, text, strlen(text));
	}
}

void tpFont::setText(const tpString &text)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (!set)
		return;

	if (text.empty())
		return;

	_setText(set, text.c_str(), text.length());
}

int32_t tpFont::layoutWidth()
{
	tpFontData *set = (tpFontData *)this->data_;
	int32_t width = 0;

	if (set)
	{
		width = _getLayoutWidth(set);
	}

	return width;
}

int32_t tpFont::layoutHeight()
{
	tpFontData *set = (tpFontData *)this->data_;
	int32_t height = 0;

	if (set)
	{
		height = _getLayoutHeight(set);
	}

	return height;
}

int32_t tpFont::pixelWidth()
{
	tpFontData *set = (tpFontData *)this->data_;
	int32_t width = 0;

	if (set)
	{
		// std::lock_guard<std::mutex> lock_g(set->getSizeMutex);

		width = _getPixelWidth(set);
	}

	return width;
}

int32_t tpFont::pixelHeight()
{
	tpFontData *set = (tpFontData *)this->data_;
	int32_t height = 0;

	if (set)
	{
		height = _getPixelHeight(set);
	}

	return height;
}

ItpSize tpFont::pixelSize()
{
	tpFontData *set = (tpFontData *)this->data_;
	ItpSize size = {0, 0};

	if (set)
	{
		size = _getPixelSize(set);
	}

	return size;
}

tpSurface *tpFont::createSurfaceDraw()
{
	tpFontData *set = (tpFontData *)this->data_;
	tpSurface *surface = nullptr;

	if (set)
	{
		surface = _createSurfaceDraw(set);
	}

	return surface;
}

void tpFont::render(tpSurface *surface, int32_t x, int32_t y)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		_complexDraw(set, surface, x, y);
	}
}

void tpFont::renderText(tpSurface *surface, const char *text, int32_t x, int32_t y)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		if (text == nullptr ||
			strlen(text) == 0)
		{
			return;
		}

		_setText(set, text, strlen(text));
		_complexDraw(set, surface, x, y);
	}
}

void tpFont::renderMarkUp(tpSurface *surface, const char *text, int32_t x, int32_t y)
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		if (text == nullptr ||
			strlen(text) == 0)
		{
			return;
		}

		_setMarkup(set, text, strlen(text));
		_complexDraw(set, surface, x, y);
	}
}

tpFont::~tpFont()
{
	tpFontData *set = (tpFontData *)this->data_;

	if (set)
	{
		std::lock_guard<std::mutex> lock(set->pangoMutex);

		_freeContext(set);
		set = nullptr;
	}
}
