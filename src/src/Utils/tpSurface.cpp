#include "tpSurface.h"
#include "tpRect.h"
#include <unistd.h>
#include <iostream>
#include "tpFileInfo.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL2_rotozoom.h>
#include <SDL2_gfxPrimitives.h>
#include <tinyPiXUtils.h>
#include "librsvg/rsvg.h"

#define DEFAULT_SVG_WIDTH 100
#define DEFAULT_SVG_HEIGHT 100

#define ARGB_A(pixel) (((pixel) >> 24) & 0xFF) // Alpha通道
#define ARGB_R(pixel) (((pixel) >> 16) & 0xFF) // Red通道
#define ARGB_G(pixel) (((pixel) >> 8) & 0xFF)  // Green通道
#define ARGB_B(pixel) ((pixel) & 0xFF)		   // Blue通道
#define ARGB_PACK(a, r, g, b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

typedef struct
{
	SDL_Surface *surface = nullptr;
	SDL_Renderer *render = nullptr;

	bool beUsed;

	// SVG的相关数据
	RsvgHandle *handle = nullptr;
	uint32_t svgWidth = DEFAULT_SVG_WIDTH;
	uint32_t svgHeight = DEFAULT_SVG_HEIGHT;
} ItpSurfaceSet;

static int32_t useRef = 0;
static bool inited = false;

static inline bool file_exist(const char *filename)
{
	return (access(filename, F_OK) != -1);
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

// 初始化SVG的数据
static void initSvgData(tpSurface *thisPtr, ItpSurfaceSet *data)
{
	cairo_surface_t *cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, data->svgWidth, data->svgHeight);

	if (!cairo_surface)
		return;

	cairo_t *cr = cairo_create(cairo_surface);

	if (!cr)
	{
		cairo_surface_destroy(cairo_surface);
		return;
	}

	cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

	RsvgRectangle rectangle = {0, 0, (double)data->svgWidth, (double)data->svgHeight};

	rsvg_handle_render_document(data->handle, cr, &rectangle, nullptr);

	int32_t stride = cairo_image_surface_get_stride(cairo_surface);

	if (!stride)
	{
		cairo_destroy(cr);
		cairo_surface_destroy(cairo_surface);
		return;
	}

	bool ret = thisPtr->create(nullptr, data->svgWidth, data->svgHeight, TP_RGB_32, stride, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	// printf("ret=%d, width=%d,height=%d,stride=%d\n", ret, data->svgWidth, data->svgHeight, stride);

	if (ret)
	{
		void *matrix = (void *)cairo_image_surface_get_data(cairo_surface);

		if (matrix)
		{
			void *addr = thisPtr->matrix();

			if (addr)
			{
				memcpy(addr, matrix, stride * data->svgHeight);
			}
		}
	}

	cairo_destroy(cr);
	cairo_surface_destroy(cairo_surface);
}

tpSurface::tpSurface(IPiDSSurface *surface)
{
	ItpSurfaceSet *set = new ItpSurfaceSet();

	if (set)
	{
		set->surface = nullptr;
		set->render = nullptr;
		set->beUsed = false;

		this->surfaceSet = set;

		if (surface)
		{
			bool ret = this->create(surface);

			if (ret == false)
			{
				std::cout << "tpSurface creates failed!" << std::endl;
				std::exit(0);
			}
		}
	}

	if (useRef == 0 &&
		inited == false)
	{
		inited = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
	}

	if (inited)
	{
		useRef++;
	}
}

tpSurface::tpSurface(const tpString &fileName, IPiDSSurface *surface)
{
	ItpSurfaceSet *set = new ItpSurfaceSet();

	if (set)
	{
		set->surface = nullptr;
		set->render = nullptr;
		set->beUsed = false;

		this->surfaceSet = set;

		if (surface)
		{
			bool ret = this->create(surface);

			if (ret == false)
			{
				std::cout << "tpSurface creates failed!" << std::endl;
				std::exit(0);
			}
		}
	}

	if (useRef == 0 &&
		inited == false)
	{
		inited = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
	}

	if (inited)
	{
		useRef++;
	}

	fromFile(fileName);
}

tpSurface::~tpSurface()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;

	if (set)
	{
		this->release();
		delete set;
	}

	useRef--;

	if (inited &&
		useRef == 0)
	{
		IMG_Quit();
		inited = false;
	}
}

bool tpSurface::fromFile(const tpString &filename, bool convertToFit)
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	bool ret = false;

	if (!set)
		return false;

	if (filename.empty())
		return false;

	tpFileInfo surfaceFile(filename);
	if (!surfaceFile.exists())
		return false;

	if ((surfaceFile.suffix().compare("svg") == 0) || (surfaceFile.suffix().compare("SVG") == 0))
	{
		RsvgHandle *tmp_handle = rsvg_handle_new_from_file(filename.c_str(), nullptr);

		if (tmp_handle == nullptr)
			return false;

		if (set->handle)
		{
			g_object_unref(set->handle);
		}

		set->handle = tmp_handle;

		initSvgData(this, set);
	}
	else
	{
		// 非SVG文件重置SVG指针
		if (set->handle)
		{
			g_object_unref(set->handle);
		}

		SDL_Surface *tmpSurface = IMG_Load(filename.c_str());

		if (tmpSurface == NULL)
		{
			return false;
		}

		if (convertToFit)
		{
			SDL_Surface *tmp = SDL_ConvertSurfaceFormat(tmpSurface, SDL_PIXELFORMAT_ARGB8888, 0);
			SDL_FreeSurface(tmpSurface);

			if (tmp)
			{
				tmpSurface = tmp;
			}
			else
			{
				return false;
			}
		}

		SDL_Renderer *tmpRenderer = SDL_CreateSoftwareRenderer(tmpSurface);

		if (tmpRenderer == nullptr)
		{
			SDL_FreeSurface(tmpSurface);
			return false;
		}

		if (set->beUsed)
		{
			this->release();
		}

		SDL_SetSurfaceBlendMode(tmpSurface, SDL_BLENDMODE_BLEND);

		set->surface = tmpSurface;
		set->render = tmpRenderer;

		ret = true;
		set->beUsed = ret;
	}

	return ret;
}

tpShared<tpSurface> tpSurface::scaled(const ItpSize &size)
{
	return scaled(size.w, size.h);
}

tpShared<tpSurface> tpSurface::scaled(const uint32_t &width, const uint32_t &height)
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;

	if (!set)
		return nullptr;

	if (set->handle)
	{
		set->svgWidth = width;
		set->svgHeight = height;

		tpShared<tpSurface> scaledRes = tpMakeShared<tpSurface>();
		initSvgData(scaledRes.get(), set);

		return scaledRes;
	}
	else
	{
		double zx = 1;
		if (this->width() != 0)
			zx = 1.0 * width / this->width();

		double zy = 1;
		if (this->height() != 0)
			zy = 1.0 * height / this->height();

		return this->rotoZoomXY(zx, zy, 0);
	}
}

bool tpSurface::create(IPiDSSurface *surface)
{
	if (surface == nullptr)
	{
		return false;
	}

	void *matrix = tinyPiX_surface_get_matrix(surface);

	if (matrix == nullptr)
	{
		return false;
	}

	int32_t width = tinyPiX_surface_get_width(surface);
	int32_t height = tinyPiX_surface_get_height(surface);

	if (width == 0 ||
		height == 0)
	{
		return false;
	}

	int32_t depth = (int32_t)tinyPiX_surface_get_format(surface);

	switch (depth)
	{
	case 8:
	case 16:
	case 24:
	case 32:
		break;
	default:
		return false;
	}

	int32_t pitch = tinyPiX_surface_get_stride(surface);

	if (pitch != cal_stride(width, depth))
	{
		return false;
	}

	uint8_t alpha = tinyPiX_surface_get_alpha(surface);
	int32_t colorKey = tinyPiX_surface_get_colorkey(surface);
	bool enable = tinyPiX_surface_get_colorkey_enable(surface);

	int32_t Rmask = tinyPiX_surface_get_rmask(surface);
	int32_t Gmask = tinyPiX_surface_get_gmask(surface);
	int32_t Bmask = tinyPiX_surface_get_bmask(surface);
	int32_t Amask = tinyPiX_surface_get_amask(surface);

	int32_t x = 0, y = 0;
	uint32_t w = 0, h = 0;

	tinyPiX_surface_get_cliprect(surface, &x, &y, &w, &h);

	tpRect clipRect(x, y, w, h);

	return this->create(matrix, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask, alpha, enable, colorKey, &clipRect);
}

bool tpSurface::create(void *address, int32_t width, int32_t height, int32_t format, int32_t stride,
					   int32_t rmask, int32_t gmask, int32_t bmask, int32_t amask,
					   uint8_t alpha, bool enableColroKey, uint32_t colorKey, tpRect *clip, bool convertToFit)
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	bool ret = false;

	if (set)
	{
		SDL_Surface *tmpSurface = nullptr;

		if (address)
		{
			tmpSurface = SDL_CreateRGBSurfaceFrom(address, width, height, format, stride, rmask, gmask, bmask, amask);
		}
		else
		{
			tmpSurface = SDL_CreateRGBSurface(0, width, height, format, rmask, gmask, bmask, amask);
		}

		if (tmpSurface == nullptr)
		{
			return false;
		}

		if (convertToFit)
		{
			if (tmpSurface->format->BytesPerPixel < TP_RGB_32 ||
				tmpSurface->format->Amask == 0)
			{
				SDL_Surface *tmp = SDL_ConvertSurfaceFormat(tmpSurface, SDL_PIXELFORMAT_ARGB32, 0);
				SDL_FreeSurface(tmpSurface);

				if (tmp)
				{
					tmpSurface = tmp;
				}
				else
				{
					return false;
				}
			}
		}

		SDL_Renderer *tmpRenderer = SDL_CreateSoftwareRenderer(tmpSurface);

		if (tmpRenderer == nullptr)
		{
			SDL_FreeSurface(tmpSurface);
			return false;
		}

		if (alpha != 0xff)
		{
			SDL_SetSurfaceAlphaMod(tmpSurface, alpha);
			SDL_SetSurfaceBlendMode(tmpSurface, SDL_BLENDMODE_BLEND);
		}

		SDL_SetColorKey(tmpSurface, (int32_t)enableColroKey, colorKey);

		if (clip)
		{
			if (clip->width() > 0 &&
				clip->height() > 0)
			{

				SDL_Rect rect;

				rect.x = clip->X0();
				rect.y = clip->Y0();
				rect.w = clip->width();
				rect.h = clip->height();

				SDL_SetClipRect(tmpSurface, &rect);
				SDL_RenderSetClipRect(tmpRenderer, &rect);
			}
		}

		if (set->beUsed)
		{
			this->release();
		}

		set->surface = tmpSurface;
		set->render = tmpRenderer;

		ret = true;
		set->beUsed = ret;
	}

	return ret;
}

bool tpSurface::create(tpSurface *surface, bool bShareMemoried)
{
	if (surface == nullptr)
	{
		return false;
	}

	void *matrix = bShareMemoried ? surface->matrix() : nullptr;

	int32_t width = surface->width();
	int32_t height = surface->height();
	int32_t stride = surface->stride();

	int32_t depth = surface->format();

	int32_t Rmask = surface->rmask();
	int32_t Gmask = surface->gmask();
	int32_t Bmask = surface->bmask();
	int32_t Amask = surface->amask();

	uint8_t alpha = 255;
	uint32_t colorKey = 0;

	bool enable = true;

	ItpRect tpr = surface->clipRect();
	tpRect rect = tpr;

	return this->create(matrix, width, height, depth, stride, Rmask, Gmask, Bmask, Amask, alpha, enable, colorKey, &rect);
}

IPitpSurfacePtr *tpSurface::surface()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	SDL_Surface *surface = nullptr;

	if (!set)
		return surface;

	if (!set->beUsed)
		return surface;

	surface = set->surface;

	return surface;
}

IPiRendererPtr *tpSurface::renderer()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	SDL_Renderer *render = nullptr;

	if (!set)
		return render;

	if (!set->beUsed)
		return render;

	render = set->render;

	return render;
}

void *tpSurface::matrix()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	void *matrix = nullptr;

	if (!set)
		return matrix;

	if (!set->beUsed)
		return matrix;

	matrix = set->surface->pixels;

	return matrix;
}

void tpSurface::glassBlur(const int32_t &radius)
{
	int width = this->width();
	int height = this->height();
	uint32_t *matrix = (uint32_t *)this->matrix();

	if (!matrix)
		return;

	// 初始化随机种子
	srand(time(NULL));
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// 生成随机偏移（范围: [-radius, radius]）
			int dx = (rand() % (2 * radius + 1)) - radius;
			int dy = (rand() % (2 * radius + 1)) - radius;

			// 边界检查
			int nx = x + dx;
			int ny = y + dy;
			nx = (nx < 0) ? 0 : (nx >= width) ? width - 1
											  : nx;
			ny = (ny < 0) ? 0 : (ny >= height) ? height - 1
											   : ny;

			// 提取当前像素Alpha
			uint32_t current_pixel = matrix[y * width + x];
			uint8_t alpha = ARGB_A(current_pixel);

			// 获取邻域像素RGB
			uint32_t neighbor_pixel = matrix[ny * width + nx];
			uint8_t r = ARGB_R(neighbor_pixel);
			uint8_t g = ARGB_G(neighbor_pixel);
			uint8_t b = ARGB_B(neighbor_pixel);

			// 替换RGB并保留Alpha
			matrix[y * width + x] = ARGB_PACK(alpha, r, g, b);
		}
	}
}

int32_t tpSurface::stride()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	int32_t stride = 0;

	if (set)
	{
		if (set->beUsed)
		{
			stride = set->surface->pitch;
		}
	}

	return stride;
}

int32_t tpSurface::width()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;

	int32_t width = 0;
	if (!set)
		return width;

	if (!set->beUsed)
		return width;

	width = set->surface->w;

	return width;
}

int32_t tpSurface::height()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	int32_t height = 0;

	if (set)
	{
		if (set->beUsed)
		{
			height = set->surface->h;
		}
	}

	return height;
}

int32_t tpSurface::format()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	int32_t format = 0;

	if (set)
	{
		if (set->beUsed)
		{
			format = set->surface->format->BitsPerPixel;
		}
	}

	return format;
}

int32_t tpSurface::rmask()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	int32_t rmask = 0;

	if (set)
	{
		if (set->beUsed)
		{
			rmask = set->surface->format->Rmask;
		}
	}

	return rmask;
}

int32_t tpSurface::gmask()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	int32_t gmask = 0;

	if (set)
	{
		if (set->beUsed)
		{
			gmask = set->surface->format->Gmask;
		}
	}

	return gmask;
}

int32_t tpSurface::bmask()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	int32_t bmask = 0;

	if (set)
	{
		if (set->beUsed)
		{
			bmask = set->surface->format->Bmask;
		}
	}

	return bmask;
}

int32_t tpSurface::amask()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	int32_t amask = 0;

	if (set)
	{
		if (set->beUsed)
		{
			amask = set->surface->format->Amask;
		}
	}

	return amask;
}

void tpSurface::setClipRect(tpRect *rect)
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	SDL_Rect *pClipRect = nullptr, clipRect;

	if (rect)
	{
		clipRect.x = rect->X0();
		clipRect.y = rect->Y0();
		clipRect.w = rect->width();
		clipRect.h = rect->height();

		pClipRect = &clipRect;
	}

	if (set)
	{
		if (set->beUsed)
		{
			SDL_SetClipRect(set->surface, pClipRect);
			SDL_RenderSetClipRect(set->render, pClipRect);
		}
	}
}

ItpRect tpSurface::clipRect()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	SDL_Rect rect = {0};

	if (!set)
		return ItpRect();

	if (!set->beUsed)
		return ItpRect();

	SDL_GetClipRect(set->surface, &rect);

	ItpRect result(rect.x, rect.y, rect.w, rect.h);

	return result;
}

void tpSurface::clear()
{
	this->fill(nullptr, _RGB(0, 0, 0));
}

void tpSurface::fill(tpRect *rect, int32_t color)
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	SDL_Rect *pFillRect = nullptr, fillRect;

	if (rect)
	{
		fillRect.x = rect->X0();
		fillRect.y = rect->Y0();
		fillRect.w = rect->width();
		fillRect.h = rect->height();

		pFillRect = &fillRect;
	}

	if (set)
	{
		uint8_t r = _R(color), g = _G(color), b = _B(color), a = _A(color);
		color = SDL_MapRGBA(set->surface->format, r, g, b, a);
		SDL_FillRect(set->surface, pFillRect, color);
	}
}

bool tpSurface::hasSurface()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	bool ret = false;

	if (set)
	{
		ret = set->beUsed;
	}

	return ret;
}

bool tpSurface::hasImage()
{
	return this->hasSurface();
}

tpSurface *tpSurface::copy(tpRect &rect)
{
	return this->copy(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

tpSurface *tpSurface::copy(int32_t x, int32_t y, int32_t w, int32_t h)
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	tpSurface *result = nullptr;

	if (set)
	{

		if (set->beUsed == false)
		{
			return nullptr;
		}

		SDL_Rect cpRect;

		cpRect.x = x;
		cpRect.y = y;
		cpRect.w = w;
		cpRect.h = h;

		int32_t width = cpRect.w;
		int32_t height = cpRect.h;

		int32_t depth = set->surface->format->BitsPerPixel;

		int32_t stride = cal_stride(width, depth);

		int32_t Rmask = set->surface->format->Rmask;
		int32_t Gmask = set->surface->format->Gmask;
		int32_t Bmask = set->surface->format->Bmask;
		int32_t Amask = set->surface->format->Amask;

		uint8_t alpha = 0xff;
		SDL_GetSurfaceAlphaMod(set->surface, &alpha);

		uint32_t colorKey = 0;
		SDL_GetColorKey(set->surface, &colorKey);

		bool enable = SDL_HasColorKey(set->surface);

		tpSurface *newSurf = new tpSurface();

		if (newSurf == nullptr)
		{
			return nullptr;
		}

		bool ret = newSurf->create(nullptr, width, height, depth, stride, Rmask, Gmask, Bmask, Amask, alpha, enable, colorKey, nullptr);

		if (ret == false)
		{
			delete newSurf;
			return nullptr;
		}

		SDL_Surface *dstSurf = (SDL_Surface *)newSurf->surface();

		if (dstSurf)
		{
			SDL_BlitSurface(set->surface, &cpRect, dstSurf, nullptr);
		}

		result = newSurf;
	}

	return result;
}

void tpSurface::directBlitF(tpSurface &surface, tpRect &src, tpRect &dst)
{
	SDL_Rect srect, drect;

	srect.x = src.X0();
	srect.y = src.Y0();
	srect.w = src.width();
	srect.h = src.height();

	drect.x = dst.X0();
	drect.y = dst.Y0();
	drect.w = dst.width();
	drect.h = dst.height();

	SDL_Surface *srcSurf = (SDL_Surface *)surface.surface();

	if (srcSurf)
	{
		ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;

		if (set &&
			set->beUsed)
		{
			SDL_BlitSurface(srcSurf, &srect, set->surface, &drect);
		}
	}
}

void tpSurface::directBlitT(tpSurface &surface, tpRect &src, tpRect &dst)
{
	SDL_Rect srect, drect;

	srect.x = src.X0();
	srect.y = src.Y0();
	srect.w = src.width();
	srect.h = src.height();

	drect.x = dst.X0();
	drect.y = dst.Y0();
	drect.w = dst.width();
	drect.h = dst.height();

	SDL_Surface *dstSurf = (SDL_Surface *)surface.surface();

	if (dstSurf)
	{
		ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;

		if (set &&
			set->beUsed)
		{
			SDL_BlitSurface(set->surface, &srect, dstSurf, &drect);
		}
	}
}

void tpSurface::strenchBlitF(tpSurface &surface, tpRect &src, tpRect &dst)
{
	SDL_Rect srect, drect;

	srect.x = src.X0();
	srect.y = src.Y0();
	srect.w = src.width();
	srect.h = src.height();

	drect.x = dst.X0();
	drect.y = dst.Y0();
	drect.w = dst.width();
	drect.h = dst.height();

	SDL_Surface *srcSurf = (SDL_Surface *)surface.surface();

	if (srcSurf)
	{
		ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;

		if (set &&
			set->beUsed)
		{
			SDL_BlitScaled(srcSurf, &srect, set->surface, &drect);
		}
	}
}

void tpSurface::strenchBlitT(tpSurface &surface, tpRect &src, tpRect &dst)
{
	SDL_Rect srect, drect;

	srect.x = src.X0();
	srect.y = src.Y0();
	srect.w = src.width();
	srect.h = src.height();

	drect.x = dst.X0();
	drect.y = dst.Y0();
	drect.w = dst.width();
	drect.h = dst.height();

	SDL_Surface *dstSurf = (SDL_Surface *)surface.surface();

	if (dstSurf)
	{
		ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;

		if (set &&
			set->beUsed)
		{
			SDL_BlitScaled(set->surface, &srect, dstSurf, &drect);
		}
	}
}

bool tpSurface::save(const tpString &filename, tpImageType type, int32_t jpguality)
{
	if (filename.empty())
		return false;

	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;

	if (!set)
		return false;

	if (!set->beUsed)
		return false;

	switch (type)
	{
	case SAVE_BMP_FMT:
	{
		SDL_SaveBMP(set->surface, filename.c_str());
	}
	break;
	case SAVE_JPG_FMT:
	{
		IMG_SaveJPG(set->surface, filename.c_str(), jpguality);
	}
	break;
	case SAVE_PNG_FMT:
	{
		IMG_SavePNG(set->surface, filename.c_str());
	}
	break;
	default:
		return false;
	}

	return true;
}

bool tpSurface::save(const tpString &filename, tpRect &rect, tpImageType type, int32_t jpguality)
{
	if (filename.empty())
		return false;

	tpSurface *tmp = this->copy(rect);
	bool ret = false;

	if (!tmp)
		return false;

	ret = tmp->save(filename, type, jpguality);
	delete tmp;

	return ret;
}

bool tpSurface::release()
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;

	if (!set)
		return false;

	if (!set->beUsed)
		return false;

	if (set->surface)
	{
		SDL_FreeSurface(set->surface);
	}

	set->surface = nullptr;

	if (set->render)
	{
		SDL_DestroyRenderer(set->render);
	}

	if (set->handle)
	{
		g_object_unref(set->handle);
	}

	set->render = nullptr;
	set->beUsed = false;

	return true;
}

tpShared<tpSurface> tpSurface::rotoZoomXY(double zx, double zy, double angle)
{
	ItpSurfaceSet *set = (ItpSurfaceSet *)this->surfaceSet;
	tpShared<tpSurface> newSurf = nullptr;

	if (set)
	{
		if (set->beUsed)
		{
			SDL_Surface *tmp = rotozoomSurfaceXY(set->surface, -angle, zx, zy, true);

			if (tmp == nullptr)
			{
				return nullptr;
			}

			int32_t width = tmp->w;
			int32_t height = tmp->h;

			int32_t depth = tmp->format->BitsPerPixel;

			int32_t stride = cal_stride(width, depth);

			int32_t Rmask = tmp->format->Rmask;
			int32_t Gmask = tmp->format->Gmask;
			int32_t Bmask = tmp->format->Bmask;
			int32_t Amask = tmp->format->Amask;

			newSurf = tpMakeShared<tpSurface>();

			if (newSurf == nullptr)
			{
				SDL_FreeSurface(tmp);
				return nullptr;
			}

			bool ret = newSurf->create(nullptr, width, height, depth, stride, Rmask, Gmask, Bmask, Amask);

			if (ret == false)
			{
				SDL_FreeSurface(tmp);
				newSurf = nullptr;
			}

			void *matrix = newSurf->matrix();

			if (matrix)
			{
				memcpy(matrix, tmp->pixels, stride * height);
			}

			SDL_FreeSurface(tmp);
		}
	}

	return newSurf;
}