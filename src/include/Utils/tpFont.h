#ifndef __TP_FONT_H
#define __TP_FONT_H

#include "tpUtils.h"
#include "tpSignalSlot.h"

#define TINY_FONT_NORMAL 0x00
#define TINY_FONT_ITALIC 0x01
#define TINY_FONT_BOLD 0x02
#define TINY_FONT_UNDERLINE 0x04
#define TINY_FONT_STROKELINE 0x08
#define TINY_FONT_TOPLINE 0x10
#define TINY_FONT_ALL 0x1f
#define DEFAULT_FONT_FAMILY "Source Han Sans CN"

TP_DEF_VOID_TYPE_VAR(ItpFontData);
TP_DEF_VOID_TYPE_VAR(IPiFontFamily);

class tpSurface;
class tpFontFamily;

class tpFont
{
public:
	typedef enum
	{
		TINY_FONT_DIRECTION_LTR,	  /*! Left to right */
		TINY_FONT_DIRECTION_RTL,	  /*! Right to left */
		TINY_FONT_DIRECTION_WEAK_LTR, /*! Left to right (weak) */
		TINY_FONT_DIRECTION_WEAK_RTL, /*! Right to left (weak) */
		TINY_FONT_DIRECTION_NEUTRAL	  /*! Neutral No direction specified, default*/
	} tpFontDirection;

	typedef enum
	{
		TINY_FONT_OPAQUE,
		TINY_FONT_TRANSPARENCY,
	} tpFontColorAttrib;

	typedef enum
	{
		TINY_FONT_ANTIALIAS_DEFAULT,  // default
		TINY_FONT_ANTIALIAS_NONE,	  // Use a bilevel alpha mask
		TINY_FONT_ANTIALIAS_GRAY,	  // Perform single-color antialiasing
		TINY_FONT_ANTIALIAS_SUBPIXEL, // Perform antialiasing by taking advantage of the order of subpixel elements on devices such as LCD panels
		TINY_FONT_ANTIALIAS_FAST,	  // Hint that the backend should perform some antialiasing but prefer speed over quality
		TINY_FONT_ANTIALIAS_GOOD,	  // The backend should balance quality against performance
		TINY_FONT_ANTIALIAS_BEST,	  // Hint that the backend should render at the highest quality, sacrificing speed if necessary, sometimes will occurs error!
	} tpFontFontAntialias;

	typedef enum
	{
		TINY_FONT_HINT_STYLE_DEFAULT, // default
		TINY_FONT_HINT_STYLE_NONE,	  // Do not hint outlines
		TINY_FONT_HINT_STYLE_SLIGHT,  // Hint outlines slightly to improve contrast while retaining good fidelity to the original shapes
		TINY_FONT_HINT_STYLE_MEDIUM,  // Hint outlines with medium strength giving a compromise between fidelity to the original shapes and contrast
		TINY_FONT_HINT_STYLE_FULL,	  // Hint outlines to maximize contrast
	} tpFontHinting;

	typedef enum
	{
		TINY_FONT_WEIGHT_THIN = 100,
		TINY_FONT_WEIGHT_ULTRALIGHT = 200,
		TINY_FONT_WEIGHT_LIGHT = 300,
		TINY_FONT_WEIGHT_SEMILIGHT = 350,
		TINY_FONT_WEIGHT_BOOK = 380,
		TINY_FONT_WEIGHT_NORMAL = 400,
		TINY_FONT_WEIGHT_MEDIUM = 500,
		TINY_FONT_WEIGHT_SEMIBOLD = 600,
		TINY_FONT_WEIGHT_BOLD = 700,
		TINY_FONT_WEIGHT_ULTRABOLD = 800,
		TINY_FONT_WEIGHT_HEAVY = 900,
		TINY_FONT_WEIGHT_ULTRAHEAVY = 1000,
	} tpFontWeight;

public:
	// tpFont(const char *family = "Sans", int32_t defaultPtSize = 12);
	tpFont(const char *family = DEFAULT_FONT_FAMILY, int32_t defaultPtSize = 12);

public:
	virtual ~tpFont();

public:
	virtual void setLanguage(const char *language);
	virtual void setAntialias(tpFontFontAntialias antialias);
	virtual void setHinting(tpFontHinting hinting);

public:
	virtual void setFontColorAttrib(tpFontColorAttrib attrib = TINY_FONT_TRANSPARENCY);
	virtual void setFontColor(int32_t fgColor, int32_t bgColor);
	virtual void setFontBackColor(int32_t bgcolor);
	virtual void setFontStyle(int32_t style = TINY_FONT_NORMAL);
	virtual void setFontUnderLineColor(int32_t color);
	virtual void setFontStrokeLineColor(int32_t color);
	virtual void setFontTopLineColor(int32_t color);
	virtual void setFontUnderLineWidth(int32_t width);
	virtual void setFontStrokeLineWidth(int32_t width);
	virtual void setFontTopLineWidth(int32_t width);
	virtual void setFontWeight(tpFontWeight weight);

public:
	virtual void setFontFamily(const char *family);
	virtual void setFontDPI(double Dpi);
	/// @brief 获取字体大小 px
	/// @return px值
	int32_t fontSize();
	/// @brief 设置字体大小
	/// @param ptsize px值
	virtual void setFontSize(const int32_t &ptsize);
	virtual void setFontForeColor(int32_t fgcolor);

public:
	virtual void setMinimumSize(int32_t width, int32_t height);
	virtual void setBaseDirection(tpFontDirection direction = TINY_FONT_DIRECTION_WEAK_LTR);

	virtual void setText(const tpString &text);

public:
	virtual void setMarkUp(const char *text);

public:
	virtual int32_t layoutWidth();
	virtual int32_t layoutHeight();
	virtual int32_t pixelWidth();
	virtual int32_t pixelHeight();
	virtual ItpSize pixelSize();

public:
	virtual tpSurface *createSurfaceDraw();
	virtual void render(tpSurface *surface, int32_t x, int32_t y);						   // render style or content by setText or setMarkUp
	virtual void renderText(tpSurface *surface, const char *text, int32_t x, int32_t y);   // do not use markup
	virtual void renderMarkUp(tpSurface *surface, const char *text, int32_t x, int32_t y); // use markup

public:
	static tpFontFamily *getSysFamilyFont();

	// public
	// signals:
	// 	/// @brief 字体大小改变信号
	// 	/// @param uint32_t 字体大小
	// 	declare_signal(onFontSizeChanged, uint32_t);

public:
	tpFont& operator=(const tpFont &others) = delete;

private:
	ItpFontData *data_;
};

class tpFontFamily
{
public:
	tpFontFamily();

public:
	virtual ~tpFontFamily();

public:
	virtual int32_t getFontFamilyNumbers();
	virtual const char *getFontFamilyName(int32_t index);

private:
	IPiFontFamily *familySet;
};

#endif
