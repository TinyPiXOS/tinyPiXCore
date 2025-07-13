#include "tpCanvas.h"
#include "tpSurface.h"
#include "tpFont.h"
#include "tpRect.h"
#include <SDL2_gfxPrimitives.h>
#include "tpDef.h"

#define OFFSET_X(set, x) (set->offsetX + x)
#define OFFSET_Y(set, y) (set->offsetY + y)

#define SET_CLIP_RECT(set, rect) (SDL_SetClipRect(set->surface, &rect) && SDL_RenderSetClipRect(set->render, &rect))

struct ItpCanvasSet
{
	SDL_Surface *surface;
	SDL_Renderer *render;
	tpSurface *tpsurface;
	cairo_surface_t *cairo_surface;
	int32_t offsetX, offsetY;
	bool beUsed;
	SDL_Rect clipRect;
};

// 绘制圆角图片资源数据
struct RoundSurfaceData
{
	tpShared<tpSurface> surface = nullptr;
	int32_t roundRad = 0;

	// 绘制坐标起始点
	int32_t x = 0;
	int32_t y = 0;

	// 偏移量
	int32_t offsetX = 0;
	int32_t offsetY = 0;
};

// 添加圆角矩形路径的辅助函数
static void addRoundedRectPath(cairo_t *cr, double x, double y, double width, double height, double radius)
{
	const double degrees = M_PI / 180.0;

	cairo_new_sub_path(cr);
	cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0);
	cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, 90 * degrees);
	cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path(cr);
}

// 公共掏空操作函数
static void applyHollowMask(cairo_t *cr, ItpCanvasSet *set, const HollowMask &hollowMaskData)
{
	// 矩形镂空
	tpVector<ItpRect> rectHollow = hollowMaskData.rectHollowList();
	for (const auto &hollowData : rectHollow)
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
		cairo_rectangle(cr, OFFSET_X(set, hollowData.x), OFFSET_Y(set, hollowData.y), hollowData.w, hollowData.h);
		cairo_fill(cr);
	}

	// 圆角矩形镂空
	tpVector<HollowMask::roundRectHollow> roundHollow = hollowMaskData.roundRectHollowList();
	for (const auto &hollowData : roundHollow)
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
		const double rectX = OFFSET_X(set, hollowData.region.x);
		const double rectY = OFFSET_Y(set, hollowData.region.y);
		const double rectWidth = hollowData.region.w;
		const double rectHeight = hollowData.region.h;
		const double cornerRadius = hollowData.round;

		addRoundedRectPath(cr, rectX, rectY, rectWidth, rectHeight, cornerRadius);
		cairo_fill(cr);
	}

	// 圆形镂空
	tpVector<HollowMask::circleHollow> circleHollow = hollowMaskData.circleHollowList();
	for (const auto &hollowData : circleHollow)
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
		const double centerX = OFFSET_X(set, hollowData.x);
		const double centerY = OFFSET_Y(set, hollowData.y);
		const double radius = hollowData.radius;

		cairo_arc(cr, centerX, centerY, radius, 0, 2 * M_PI);
		cairo_close_path(cr);
		cairo_fill(cr);
	}

	// 扇形镂空
	tpVector<HollowMask::pieHollow> pieHollowList = hollowMaskData.pieHollowList();
	for (const auto &hollowData : pieHollowList)
	{
		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
		const double centerX = OFFSET_X(set, hollowData.x);
		const double centerY = OFFSET_Y(set, hollowData.y);

		const double startAngleRad = -M_PI_2 + (hollowData.start * M_PI / 180.0);
		const double endAngleRad = -M_PI_2 + (hollowData.end * M_PI / 180.0);

		cairo_move_to(cr, centerX, centerY);
		cairo_line_to(cr,
					  centerX + hollowData.radius * cos(startAngleRad),
					  centerY + hollowData.radius * sin(startAngleRad));
		cairo_arc(cr, centerX, centerY, hollowData.radius,
				  startAngleRad, endAngleRad);
		cairo_close_path(cr);
		cairo_fill(cr);
	}
}

static inline int32_t cal_stride(int32_t width, int32_t depth)
{
	int32_t bpp = depth / 8;
	int32_t stride = width * bpp;

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

static inline cairo_surface_t *convertFromSDL_Surface(SDL_Surface *dstSurf)
{
	cairo_format_t format = CAIRO_FORMAT_INVALID;
	bool ret = false;

	int32_t Rmask = dstSurf->format->Rmask;
	int32_t Gmask = dstSurf->format->Gmask;
	int32_t Bmask = dstSurf->format->Bmask;
	int32_t Amask = dstSurf->format->Amask;

	switch (dstSurf->format->BitsPerPixel)
	{
	case 32:
	{
		ret = (((Rmask & 0x00ff0000) == 0x00ff0000) && ((Gmask & 0x0000ff00) == 0x0000ff00) && ((Bmask & 0x000000ff) == 0x000000ff) && ((Amask & 0xff000000) == 0xff000000));

		if (ret)
		{
			format = CAIRO_FORMAT_ARGB32;
		}
		else
		{
			if ((Amask & 0xff000000) == 0x00000000)
			{
				format = CAIRO_FORMAT_RGB24;
			}
		}
	}
	break;
	case 24:
	{
		ret = (((Rmask & 0x00ff0000) == 0x00ff0000) && ((Gmask & 0x0000ff00) == 0x0000ff00) && ((Bmask & 0x000000ff) == 0x000000ff) && ((Amask & 0x00000000) == 0x00000000));

		if (ret)
		{
			format = CAIRO_FORMAT_RGB24;
		}
	}
	break;
	case 16:
	{
		ret = (((Rmask & 0x0000f800) == 0x0000f800) && ((Gmask & 0x000007e0) == 0x000007e0) && ((Bmask & 0x0000001f) == 0x0000001f) && ((Amask & 0x00000000) == 0x00000000));

		if (ret)
		{
			format = CAIRO_FORMAT_RGB16_565;
		}
	}
	break;
	case 8:
	{
		format = CAIRO_FORMAT_A8;
	}
	break;
	default:
		return nullptr;
	}

	if (format == CAIRO_FORMAT_INVALID)
	{
		return nullptr;
	}

	return cairo_image_surface_create_for_data((uint8_t *)dstSurf->pixels, format, dstSurf->w, dstSurf->h, dstSurf->pitch);
}

static inline void drawCircleButton(tpCanvas *canvas, cairo_t *cr, cairo_surface_t *cairo_surface, int32_t offsetX, int32_t offsetY, void *args)
{
	RoundSurfaceData *set = (RoundSurfaceData *)args;

	cairo_surface_t *cairo_image = canvas->convertFromSurfaceToCairo(set->surface.get());

	if (cairo_image == nullptr)
	{
		return;
	}

	int width = cairo_image_surface_get_width(cairo_image);
	int height = cairo_image_surface_get_height(cairo_image);

	double radius = set->roundRad;

#if 0
	cairo_arc(cr, width / 2.0, height / 2.0, radius, 0, 2 * M_PI);
	cairo_clip(cr);
	cairo_new_path(cr); /* path not consumed by clip()*/

	cairo_scale(cr, 256.0 /width, 256.0 / height);

	cairo_set_source_surface(cr, cairo_image, set->offsetX + set->x, set->offsetY + set->y);

	cairo_paint(cr);

	cairo_surface_destroy(cairo_image);

#else
	cairo_surface_t *output = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

	if (output)
	{
		cairo_t *output_cr = cairo_create(output);
		// 清空背景
		cairo_set_source_rgba(output_cr, 0, 0, 0, 0);
		cairo_paint(output_cr);

		cairo_fill(output_cr);
		// cairo_arc(output_cr, width / 2.0, height / 2.0, radius, 0, 2 * M_PI);

		// 绘制圆角矩形路径
		cairo_move_to(output_cr, radius, 0);
		cairo_line_to(output_cr, width - radius, 0);
		cairo_arc(output_cr, width - radius, radius, radius, -M_PI / 2, 0);
		cairo_line_to(output_cr, width, height - radius);
		cairo_arc(output_cr, width - radius, height - radius, radius, 0, M_PI / 2);
		cairo_line_to(output_cr, radius, height);
		cairo_arc(output_cr, radius, height - radius, radius, M_PI / 2, M_PI);
		cairo_line_to(output_cr, 0, radius);
		cairo_arc(output_cr, radius, radius, radius, M_PI, 3 * M_PI / 2);
		cairo_close_path(output_cr);

		cairo_clip(output_cr);
		cairo_set_source_surface(output_cr, cairo_image, 0, 0);
		cairo_paint(output_cr);

		cairo_surface_destroy(cairo_image);
	}

	cairo_set_source_surface(cr, output, set->offsetX + set->x, set->offsetY + set->y);
	cairo_paint(cr);

	cairo_stroke(cr);
	cairo_surface_destroy(output);
#endif
}

tpCanvas::tpCanvas(tpSurface *surface, int32_t offsetX, int32_t offsetY)
{
	ItpCanvasSet *set = new ItpCanvasSet();

	if (set)
	{
		set->surface = nullptr;
		set->render = nullptr;
		set->beUsed = false;
		set->cairo_surface = nullptr;
		set->offsetX = offsetX;
		set->offsetY = offsetY;

		if (surface)
		{
			set->tpsurface = surface;
			set->surface = (SDL_Surface *)surface->surface();
			set->render = (SDL_Renderer *)surface->renderer();
			set->cairo_surface = convertFromSDL_Surface(set->surface);
			set->beUsed = (set->surface && set->render);
		}

		this->canvasSet = set;
	}
}

tpCanvas::~tpCanvas()
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set)
	{
		this->release();
		delete set;
	}
}

void tpCanvas::paintTest()
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
			return;

		// 创建临时图层
		cairo_push_group(cr);

		// 绘制半透明黑色遮罩
		cairo_set_source_rgba(cr, 0, 0, 0, 0.5); // 50% 不透明度
		cairo_rectangle(cr, 0, 0, 600, 600);
		cairo_fill(cr);

		// 挖空按钮区域
		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR); // 清除模式
		cairo_rectangle(cr, 250, 250, 100, 100);
		cairo_fill(cr);

		// 应用图层
		cairo_pop_group_to_source(cr);
		cairo_paint(cr);
	}
}

bool tpCanvas::setTarget(tpSurface *surface, int32_t offsetX, int32_t offsetY)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set)
	{
		if (surface == nullptr)
		{
			return false;
		}

		SDL_Surface *tmpS = (SDL_Surface *)surface->surface();
		SDL_Renderer *tmpR = (SDL_Renderer *)surface->renderer();

		if (tmpS &&
			tmpR)
		{
			if (set->beUsed)
			{
				this->release();
			}

			set->surface = tmpS;
			set->render = tmpR;
			set->tpsurface = surface;
			set->cairo_surface = convertFromSDL_Surface(set->surface);
			set->offsetX = offsetX;
			set->offsetY = offsetY;
			set->beUsed = true;
		}
	}

	return true;
}

void tpCanvas::setClipRect(tpRect &rect)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{
		set->tpsurface->setClipRect(&rect);

		set->clipRect.x = rect.X0();
		set->clipRect.y = rect.Y0();
		set->clipRect.w = rect.width();
		set->clipRect.h = rect.height();
	}
}

void tpCanvas::setClipRect(ItpRect *rect)
{
	if (rect)
	{
		tpRect r(rect);
		this->setClipRect(r);
	}
}

tpSurface *tpCanvas::surface()
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;
	tpSurface *surface = nullptr;

	if (set &&
		set->beUsed)
	{
		surface = set->tpsurface;
	}

	return surface;
}

static inline void draw_erase(ItpCanvasSet *set)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
		cairo_paint(cr);
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
		cairo_set_source_rgba(cr, 0, 0, 0, 0);
		cairo_fill(cr);

		cairo_destroy(cr);
	}
}

void tpCanvas::erase()
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{
		draw_erase(set);
	}
}

static inline void draw_pixel(ItpCanvasSet *set, int32_t x, int32_t y, int32_t color)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		cairo_set_source_rgba(cr, r, g, b, a);

		cairo_move_to(cr, x, y);
		cairo_line_to(cr, x, y);

		cairo_stroke(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::pixel(int32_t x, int32_t y, int32_t color)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{
		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		draw_pixel(set, x, y, color);
	}
}

static inline void draw_line(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_set_line_width(cr, width);

		cairo_move_to(cr, x1, y1);
		cairo_line_to(cr, x2, y2);

		cairo_stroke(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::hline(int32_t x1, int32_t x2, int32_t y, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x1 = OFFSET_X(set, x1);
		x2 = OFFSET_X(set, x2);
		y = OFFSET_Y(set, y);

		draw_line(set, x1, y, x2, y, color, width);
	}
}

void tpCanvas::vline(int32_t x, int32_t y1, int32_t y2, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{
		x = OFFSET_X(set, x);
		y1 = OFFSET_Y(set, y1);
		y2 = OFFSET_Y(set, y2);

		draw_line(set, x, y1, x, y2, color, width);
	}
}

void tpCanvas::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x1 = OFFSET_X(set, x1);
		y1 = OFFSET_Y(set, y1);
		x2 = OFFSET_X(set, x2);
		y2 = OFFSET_Y(set, y2);

		draw_line(set, x1, y1, x2, y2, color, width);
	}
}

static inline void draw_rectangle(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_set_line_width(cr, width);
		cairo_rectangle(cr, x1, y1, x2, y2);
		cairo_stroke(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x1 = OFFSET_X(set, x1);
		y1 = OFFSET_Y(set, y1);
		x2 = OFFSET_X(set, x2);
		y2 = OFFSET_Y(set, y2);

		draw_rectangle(set, x1, y1, x2, y2, color, width);
	}
}

static inline void draw_roundedRectangle(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, double width)
{
	if (!set->cairo_surface)
		return;

	double x = x1;
	double y = y1;
	double rectWidth = x2 - x1;
	double rectHeight = y2 - y1;

	if (rectWidth == 0 || rectHeight == 0)
		return;

	cairo_t *cr = cairo_create(set->cairo_surface);

	if (cr == nullptr)
		return;

	double aspect = 1.0;		/* 纵横比 */
	double corner_radius = rad; /* 拐角曲率半径 */

	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	// 圆角不能大于短边的一半
	double shortSide = rectWidth > rectHeight ? rectHeight : rectWidth;
	shortSide /= 2.0;
	if (radius > shortSide)
		radius = shortSide;

	cairo_new_sub_path(cr);
	cairo_arc(cr, x + rectWidth - radius, y + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc(cr, x + rectWidth - radius, y + rectHeight - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc(cr, x + radius, y + rectHeight - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path(cr);

	double r = _R(color) / 255.0;
	double g = _G(color) / 255.0;
	double b = _B(color) / 255.0;
	double a = _A(color) / 255.0;

	cairo_set_source_rgba(cr, r, g, b, a);
	cairo_set_line_width(cr, width);
	cairo_stroke(cr);
	cairo_destroy(cr);
}

void tpCanvas::roundedRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x1 = OFFSET_X(set, x1);
		y1 = OFFSET_Y(set, y1);
		x2 = OFFSET_X(set, x2);
		y2 = OFFSET_Y(set, y2);

		draw_roundedRectangle(set, x1, y1, x2, y2, rad, color, width);
	}
}

static inline void draw_box(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_rectangle(cr, x1, y1, x2 - x1, y2 - y1);
		cairo_fill(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::box(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x1 = OFFSET_X(set, x1);
		y1 = OFFSET_Y(set, y1);
		x2 = OFFSET_X(set, x2);
		y2 = OFFSET_Y(set, y2);

		draw_box(set, x1, y1, x2, y2, color);
	}
}

static inline void draw_roundedBox(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color)
{
	if (!set->cairo_surface)
		return;

	double x = x1;
	double y = y1;

	double rectWidth = x2 - x1;
	double rectHeight = y2 - y1;

	if (rectWidth == 0 || rectHeight == 0)
		return;

	cairo_t *cr = cairo_create(set->cairo_surface);

	if (cr == nullptr)
	{
		return;
	}

	SDL_Rect clipRect;
	SDL_GetClipRect(set->surface, &clipRect);
	cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
	cairo_clip(cr);
	cairo_new_path(cr);

	// 圆角不能大于短边的一半
	double radius = rad;
	double shortSide = rectWidth > rectHeight ? rectHeight : rectWidth;
	shortSide /= 2.0;
	if (radius > shortSide)
		radius = shortSide;

	double r = _R(color) / 255.0;
	double g = _G(color) / 255.0;
	double b = _B(color) / 255.0;
	double a = _A(color) / 255.0;

	cairo_set_source_rgba(cr, r, g, b, a);

	cairo_move_to(cr, x + radius, y);
	cairo_line_to(cr, x + rectWidth - radius, y);

	cairo_move_to(cr, x + rectWidth, y + radius);
	cairo_line_to(cr, x + rectWidth, y + rectHeight - radius);

	cairo_move_to(cr, x + rectWidth - radius, y + rectHeight);
	cairo_line_to(cr, x + radius, y + rectHeight);

	cairo_move_to(cr, x, y + rectHeight - radius);
	cairo_line_to(cr, x, y + radius);

	cairo_arc(cr, x + radius, y + radius, radius, M_PI, 3 * M_PI / 2.0);
	cairo_arc(cr, x + rectWidth - radius, y + radius, radius, 3 * M_PI / 2, 2 * M_PI);
	cairo_arc(cr, x + rectWidth - radius, y + rectHeight - radius, radius, 0, M_PI / 2);
	cairo_arc(cr, x + radius, y + rectHeight - radius, radius, M_PI / 2, M_PI);

	cairo_fill(cr);
	cairo_destroy(cr);
}

void tpCanvas::roundedBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x1 = OFFSET_X(set, x1);
		y1 = OFFSET_Y(set, y1);
		x2 = OFFSET_X(set, x2);
		y2 = OFFSET_Y(set, y2);

		draw_roundedBox(set, x1, y1, x2, y2, rad, color);
	}
}

static inline void draw_arc(ItpCanvasSet *set, int32_t x, int32_t y, int32_t rad, double start, double end, int32_t color, double width)
{
	if (!set->cairo_surface)
		return;

	cairo_t *cr = cairo_create(set->cairo_surface);

	if (cr == nullptr)
	{
		return;
	}

	SDL_Rect clipRect;
	SDL_GetClipRect(set->surface, &clipRect);
	cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
	cairo_clip(cr);
	cairo_new_path(cr);

	double r = _R(color) / 255.0;
	double g = _G(color) / 255.0;
	double b = _B(color) / 255.0;
	double a = _A(color) / 255.0;

	cairo_set_source_rgba(cr, r, g, b, a);
	cairo_set_line_width(cr, width);

	double start_p = start * M_PI / 180;
	double end_p = end * M_PI / 180;

	cairo_arc(cr, x, y, rad, start_p, end_p);

	cairo_stroke(cr);
	cairo_destroy(cr);
}

void tpCanvas::circle(int32_t x, int32_t y, int32_t rad, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		draw_arc(set, x, y, rad, 0, 360, color, width);
	}
}

void tpCanvas::arc(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		draw_arc(set, x, y, rad, start, end, color, width);
	}
}

static inline void draw_filledCircle(ItpCanvasSet *set, int32_t x, int32_t y, int32_t rad, double start, double end, int32_t color)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);

		double start_p = start * M_PI / 180;
		double end_p = end * M_PI / 180;

		cairo_arc(cr, x, y, rad, start, end);

		cairo_fill(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::filledCircle(int32_t x, int32_t y, int32_t rad, int32_t color)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		draw_filledCircle(set, x, y, rad, 0, 360, color);
	}
}

static inline void draw_ellipse(ItpCanvasSet *set, int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color, double width)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_set_line_width(cr, width);

		double scale = (double)ry / rx;

		cairo_scale(cr, 1, scale);
		cairo_arc(cr, x, y, rx, 0, 2 * M_PI);

		cairo_stroke(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::ellipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		draw_ellipse(set, x, y, rx, ry, color, width);
	}
}

static inline void draw_filledEllipse(ItpCanvasSet *set, int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;
		double scale = (double)ry / rx;

		cairo_set_source_rgba(cr, r, g, b, a);

		cairo_scale(cr, 1, scale);
		cairo_arc(cr, x, y, rx, 0, 2 * M_PI);

		cairo_fill(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::filledEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		draw_filledEllipse(set, x, y, rx, ry, color);
	}
}

static inline void draw_pie(ItpCanvasSet *set, int32_t x, int32_t y, int32_t rad, double start, double end, int32_t color, double width)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_set_line_width(cr, width);

		double start_p = start * M_PI / 180;
		double end_p = end * M_PI / 180;

		cairo_arc(cr, x, y, rad, start_p, end_p);
		cairo_move_to(cr, x, y);
		cairo_line_to(cr, x + rad * cos(start_p), y + rad * sin(start_p));
		cairo_line_to(cr, x + rad * cos(end_p), y + rad * sin(end_p));

		cairo_stroke(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::pie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{
		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		draw_pie(set, x, y, rad, start, end, color, width);
	}
}

static inline void draw_filledpie(ItpCanvasSet *set, int32_t x, int32_t y, int32_t rad, double start, double end, int32_t color)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);

		double start_p = start * M_PI / 180;
		double end_p = end * M_PI / 180;

		cairo_arc(cr, x, y, rad, start_p, end_p);
		cairo_move_to(cr, x, y);
		cairo_line_to(cr, x + rad * cos(start_p), y + rad * sin(start_p));
		cairo_line_to(cr, x + rad * cos(end_p), y + rad * sin(end_p));

		cairo_fill(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::filledPie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		draw_filledpie(set, x, y, rad, start, end, color);
	}
}

static inline void draw_trigon(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t color, double width)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_set_line_width(cr, width);

		cairo_move_to(cr, x1, y1);
		cairo_line_to(cr, x2, y2);

		cairo_move_to(cr, x2, y2);
		cairo_line_to(cr, x3, y3);

		cairo_move_to(cr, x3, y3);
		cairo_line_to(cr, x1, y1);

		cairo_stroke(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::trigon(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x1 = OFFSET_X(set, x1);
		y1 = OFFSET_Y(set, y1);
		x2 = OFFSET_X(set, x2);
		y2 = OFFSET_Y(set, y2);

		draw_trigon(set, x1, y1, x2, y2, x3, y3, color, width);
	}
}

static inline void draw_filledTrigon(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t color)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);

		cairo_move_to(cr, x1, y1);
		cairo_line_to(cr, x2, y2);

		cairo_move_to(cr, x2, y2);
		cairo_line_to(cr, x3, y3);

		cairo_move_to(cr, x3, y3);
		cairo_line_to(cr, x1, y1);

		cairo_fill(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::filledTrigon(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t color)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x1 = OFFSET_X(set, x1);
		y1 = OFFSET_Y(set, y1);
		x2 = OFFSET_X(set, x2);
		y2 = OFFSET_Y(set, y2);

		draw_filledTrigon(set, x1, y1, x2, y2, x3, y3, color);
	}
}

static inline void draw_polygon(ItpCanvasSet *set, short *vx, short *vy, int32_t n, int32_t color, double width)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_set_line_width(cr, width);

		cairo_move_to(cr, vx[0], vy[0]);

		int i;

		for (i = 1; i < n; ++i)
		{
			cairo_line_to(cr, vx[i], vy[i]);
			cairo_move_to(cr, vx[i], vy[i]);
		}

		cairo_line_to(cr, vx[0], vy[0]);

		cairo_stroke(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::polygon(short *vx, short *vy, int32_t n, int32_t color, double width)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		int32_t i;
		for (i = 0; i < n; i++)
		{
			vx[i] = OFFSET_X(set, vx[i]);
			vy[i] = OFFSET_Y(set, vy[i]);
		}

		draw_polygon(set, vx, vy, n, color, width);
	}
}

static inline void draw_filledPolygon(ItpCanvasSet *set, short *vx, short *vy, int32_t n, int32_t color)
{
	if (set->cairo_surface)
	{
		cairo_t *cr = cairo_create(set->cairo_surface);

		if (cr == nullptr)
		{
			return;
		}

		SDL_Rect clipRect;
		SDL_GetClipRect(set->surface, &clipRect);
		cairo_rectangle(cr, clipRect.x, clipRect.y, clipRect.w, clipRect.h);
		cairo_clip(cr);
		cairo_new_path(cr);

		double r = _R(color) / 255.0;
		double g = _G(color) / 255.0;
		double b = _B(color) / 255.0;
		double a = _A(color) / 255.0;

		cairo_set_source_rgba(cr, r, g, b, a);
		cairo_move_to(cr, vx[0], vy[0]);

		int i;

		for (i = 1; i < n; ++i)
		{
			cairo_line_to(cr, vx[i], vy[i]);
			cairo_move_to(cr, vx[i], vy[i]);
		}

		cairo_line_to(cr, vx[0], vy[0]);

		cairo_fill(cr);
		cairo_destroy(cr);
	}
}

void tpCanvas::filledPolygon(short *vx, short *vy, int32_t n, int32_t color)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		int32_t i;
		for (i = 0; i < n; i++)
		{
			vx[i] = OFFSET_X(set, vx[i]);
			vy[i] = OFFSET_Y(set, vy[i]);
		}

		draw_filledPolygon(set, vx, vy, n, color);
	}
}

void tpCanvas::hollowBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, const HollowMask &hollowMaskData)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;
	if (!set)
		return;

	if (!set->beUsed)
		return;

	x1 = OFFSET_X(set, x1);
	y1 = OFFSET_Y(set, y1);
	x2 = OFFSET_X(set, x2);
	y2 = OFFSET_Y(set, y2);

	if (!set->cairo_surface)
		return;

	cairo_t *cr = cairo_create(set->cairo_surface);

	if (cr == nullptr)
		return;

	// 创建临时图层
	cairo_push_group(cr);

	double r = _R(color) / 255.0;
	double g = _G(color) / 255.0;
	double b = _B(color) / 255.0;
	double a = _A(color) / 255.0;

	int32_t width = x2 - x1;
	int32_t height = y2 - y1;

	cairo_set_source_rgba(cr, r, g, b, a);
	cairo_rectangle(cr, x1, y1, width, height);
	cairo_fill(cr);

	// 应用所有掏空效果
	applyHollowMask(cr, set, hollowMaskData);

	// 应用图层
	cairo_pop_group_to_source(cr);
	cairo_paint(cr);
	cairo_destroy(cr);
}

void tpCanvas::hollowRoundedBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, const HollowMask &hollowMaskData)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;
	if (!set)
		return;

	if (!set->beUsed)
		return;

	x1 = OFFSET_X(set, x1);
	y1 = OFFSET_Y(set, y1);
	x2 = OFFSET_X(set, x2);
	y2 = OFFSET_Y(set, y2);

	cairo_t *cr = cairo_create(set->cairo_surface);
	if (!cr)
		return;

	// 创建临时图层
	cairo_push_group(cr);

	// 绘制圆角矩形遮罩
	double width = x2 - x1;
	double height = y2 - y1;

	// 计算实际圆角半径（不超过短边一半）
	double radius = rad;
	double minDimension = (width < height) ? width : height;
	if (radius > minDimension / 2)
	{
		radius = minDimension / 2;
	}

	double r = _R(color) / 255.0;
	double g = _G(color) / 255.0;
	double b = _B(color) / 255.0;
	double a = _A(color) / 255.0;

	cairo_set_source_rgba(cr, r, g, b, a);
	addRoundedRectPath(cr, x1, y1, width, height, radius);
	cairo_fill(cr);

	// 应用所有掏空效果
	applyHollowMask(cr, set, hollowMaskData);

	// 应用图层
	cairo_pop_group_to_source(cr);
	cairo_paint(cr);
	cairo_destroy(cr);
}

void tpCanvas::paintSurface(const tpShared<tpSurface> &surface, const tpRect &src_rect, const tpRect &dst_rect, bool strench)
{
	this->paintSurface(surface, &src_rect, &dst_rect, strench);
}

void tpCanvas::paintSurface(const tpShared<tpSurface> &surface, const tpRect *src_rect, const tpRect *dst_rect, bool strench)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;
	if (!set)
		return;

	if (!set->beUsed)
		return;

	SDL_Surface *srcSurf = (SDL_Surface *)surface->surface();

	if (srcSurf == nullptr)
		return;

	SDL_Rect srect, drect;

	if (src_rect)
	{
		srect.x = src_rect->X0();
		srect.y = src_rect->Y0();
		srect.w = src_rect->width();
		srect.h = src_rect->height();
	}
	else
	{
		srect.x = 0;
		srect.y = 0;
		srect.w = surface->width();
		srect.h = surface->height();
	}

	if (dst_rect)
	{
		drect.x = dst_rect->X0() + set->offsetX;
		drect.y = dst_rect->Y0() + set->offsetY;
		drect.w = dst_rect->width();
		drect.h = dst_rect->height();
	}
	else
	{
		drect.x = set->offsetX;
		drect.y = set->offsetY;
		drect.w = set->surface->w;
		drect.h = set->surface->h;
	}

	if (strench)
	{
		SDL_BlitScaled(srcSurf, &srect, set->surface, &drect);
	}
	else
	{
		SDL_BlitSurface(srcSurf, &srect, set->surface, &drect);
	}
}

void tpCanvas::paintSurface(const int32_t &x, const int32_t &y, const tpShared<tpSurface> &surface)
{
	tpRect srcRect(0, 0, surface->width(), surface->height());
	tpRect dstRect(x, y, surface->width(), surface->height());

	this->paintSurface(surface, srcRect, dstRect);
}

void tpCanvas::paintRoundSurface(const int32_t &x, const int32_t &y, int32_t rad, const tpShared<tpSurface> &surface)
{
	if (rad == 0)
	{
		this->paintSurface(x, y, surface);
		return;
	}

	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	RoundSurfaceData roundData;
	roundData.roundRad = rad;
	roundData.surface = surface;
	roundData.x = x;
	roundData.y = y;
	roundData.offsetX = set->offsetX;
	roundData.offsetY = set->offsetY;
	this->customizedCarioMethod(drawCircleButton, &roundData);
}

void tpCanvas::renderText(tpFont &font, int32_t x, int32_t y, const tpString &text)
{
	this->renderText(font, x, y, text.c_str());
}

void tpCanvas::renderText(tpFont &font, int32_t x, int32_t y, const char *text)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		font.renderText(set->tpsurface, text, x, y);
	}
}

void tpCanvas::renderMarkUp(tpFont &font, int32_t x, int32_t y, const tpString &text)
{
	this->renderMarkUp(font, x, y, text.c_str());
}

void tpCanvas::renderMarkUp(tpFont &font, int32_t x, int32_t y, const char *text)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		font.renderMarkUp(set->tpsurface, text, x, y);
	}
}

void tpCanvas::renderText(tpFont &font, int32_t x, int32_t y)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{

		x = OFFSET_X(set, x);
		y = OFFSET_Y(set, y);

		font.render(set->tpsurface, x, y);
	}
}

void tpCanvas::customizedCarioMethod(defDrawFunction func, void *args)
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set &&
		set->beUsed)
	{
		cairo_surface_t *cairo_surface = tpCanvas::convertFromSurfaceToCairo(set->tpsurface);

		if (cairo_surface == nullptr)
		{
			return;
		}

		cairo_t *cr = cairo_create(cairo_surface);

		if (cr)
		{
			SDL_Rect clipRect;
			SDL_GetClipRect(set->surface, &clipRect);
			cairo_rectangle(cr, (double)clipRect.x, (double)clipRect.y, (double)clipRect.w, (double)clipRect.h);
			cairo_clip(cr);
			cairo_new_path(cr);

			if (func)
			{
				func(this, cr, cairo_surface, set->offsetX, set->offsetY, args);
			}
		}

		cairo_surface_destroy(cairo_surface);
		cairo_destroy(cr);
	}
}

tpSurface *tpCanvas::convertFromCairoToSurface(cairo_surface_t *cairo_surface)
{
	if (cairo_surface == nullptr)
	{
		return nullptr;
	}

	void *addr = (void *)cairo_image_surface_get_data(cairo_surface);
	cairo_format_t fmt = cairo_image_surface_get_format(cairo_surface);
	int32_t width = cairo_image_surface_get_width(cairo_surface);
	int32_t height = cairo_image_surface_get_height(cairo_surface);
	int32_t stride = cairo_image_surface_get_stride(cairo_surface);

	if (addr == nullptr)
	{
		return nullptr;
	}

	ItpFormat format = TP_RGB_UNKOWN;
	int32_t Rmask = 0, Gmask = 0, Bmask = 0, Amask = 0;

	switch (fmt)
	{
	case CAIRO_FORMAT_ARGB32:
	case CAIRO_FORMAT_RGB24:
	{
		format = TP_RGB_32;

		Rmask = 0x00ff0000;
		Gmask = 0x0000ff00;
		Bmask = 0x000000ff;
		Amask = (fmt == CAIRO_FORMAT_RGB24) ? 0x00000000 : 0xff000000;
	}
	break;
	case CAIRO_FORMAT_RGB16_565:
	{
		format = TP_RGB_16;

		Rmask = 0x0000f800;
		Gmask = 0x000007e0;
		Bmask = 0x0000001f;
	}
	break;
	case CAIRO_FORMAT_A8:
	{
		format = TP_RGB_8;
	}
	break;
	default:
		return nullptr;
	}

	tpSurface *surface = new tpSurface();

	if (surface == nullptr)
	{
		return nullptr;
	}

	bool ret = surface->create(addr, width, height, format, stride, Rmask, Gmask, Bmask, Amask);

	if (ret == false)
	{
		delete surface;
		return nullptr;
	}

	return surface;
}

cairo_surface_t *tpCanvas::convertFromSurfaceToCairo(tpSurface *surface)
{
	if (surface == nullptr)
	{
		return nullptr;
	}

	SDL_Surface *dstSurf = (SDL_Surface *)surface->surface();

	if (dstSurf == nullptr)
	{
		return nullptr;
	}

	return convertFromSDL_Surface(dstSurf);
}

void tpCanvas::release()
{
	ItpCanvasSet *set = (ItpCanvasSet *)canvasSet;

	if (set)
	{
		if (set->beUsed)
		{
			set->surface = nullptr;
			set->render = nullptr;
			set->tpsurface = nullptr;
			set->cairo_surface = nullptr;
			set->beUsed = false;

			if (set->cairo_surface)
			{
				cairo_surface_destroy(set->cairo_surface);
			}
		}
	}
}

HollowMask::HollowMask()
{
}

HollowMask::~HollowMask()
{
}

void HollowMask::addRectHollow(const ItpRect &region)
{
	rectList_.emplace_back(region);
}

tpVector<ItpRect> HollowMask::rectHollowList() const
{
	return rectList_;
}

void HollowMask::addRoundRectHollow(const ItpRect &region, const uint32_t &round)
{
	addRoundRectHollow(HollowMask::roundRectHollow(region, round));
}

void HollowMask::addRoundRectHollow(const roundRectHollow &data)
{
	roundRectList_.emplace_back(data);
}

tpVector<HollowMask::roundRectHollow> HollowMask::roundRectHollowList() const
{
	return roundRectList_;
}

void HollowMask::addCircleHollow(const int32_t &x, const int32_t &y, const uint32_t &radius)
{
	addCircleHollow(HollowMask::circleHollow(x, y, radius));
}

void HollowMask::addCircleHollow(const circleHollow &data)
{
	circleList_.emplace_back(data);
}

tpVector<HollowMask::circleHollow> HollowMask::circleHollowList() const
{
	return circleList_;
}

void HollowMask::addPieHollow(const int32_t &x, const int32_t &y, const uint32_t &radius, const int32_t &start, const int32_t &end)
{
	addPieHollow(pieHollow(x, y, radius, start, end));
}

void HollowMask::addPieHollow(const pieHollow &data)
{
	pieList_.emplace_back(data);
}

tpVector<HollowMask::pieHollow> HollowMask::pieHollowList() const
{
	return pieList_;
}
