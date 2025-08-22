#include "TpSurface.h"
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
#define ARGB_B(pixel) ((pixel) & 0xFF)         // Blue通道
#define ARGB_PACK(a, r, g, b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

struct TpSurfaceData
{
    SDL_Surface *surface = nullptr;
    SDL_Renderer *render = nullptr;

    bool beUsed;
};

static int32_t useRef = 0;
static bool inited = false;

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

TpSurface::TpSurface(IPiDSSurface *surface)
{
    TpSurfaceData *set = new TpSurfaceData();

    if (set)
    {
        set->surface = nullptr;
        set->render = nullptr;
        set->beUsed = false;

        this->data_ = set;

        if (surface)
        {
            bool ret = this->create(surface);

            if (ret == false)
            {
                std::cout << "TpSurface creates failed!" << std::endl;
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

TpSurface::~TpSurface()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;

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

bool TpSurface::create(IPiDSSurface *surface)
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

bool TpSurface::create(void *address, int32_t width, int32_t height, int32_t format, int32_t stride,
                       int32_t rmask, int32_t gmask, int32_t bmask, int32_t amask,
                       uint8_t alpha, bool enableColroKey, uint32_t colorKey, tpRect *clip, bool convertToFit)
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

bool TpSurface::create(tpShared<TpSurface> surface, bool bShareMemoried)
{
    if (surface == nullptr)
        return false;

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

IPitpSurfacePtr *TpSurface::surface()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
    SDL_Surface *surface = nullptr;

    if (!set)
        return surface;

    if (!set->beUsed)
        return surface;

    surface = set->surface;

    return surface;
}

IPiRendererPtr *TpSurface::renderer()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
    SDL_Renderer *render = nullptr;

    if (!set)
        return render;

    if (!set->beUsed)
        return render;

    render = set->render;

    return render;
}

void *TpSurface::matrix()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
    void *matrix = nullptr;

    if (!set)
        return matrix;

    if (!set->beUsed)
        return matrix;

    matrix = set->surface->pixels;

    return matrix;
}

int32_t TpSurface::stride()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

int32_t TpSurface::width()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;

    int32_t width = 0;
    if (!set)
        return width;

    if (!set->beUsed)
        return width;

    width = set->surface->w;

    return width;
}

int32_t TpSurface::height()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

int32_t TpSurface::format()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

int32_t TpSurface::rmask()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

int32_t TpSurface::gmask()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

int32_t TpSurface::bmask()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

int32_t TpSurface::amask()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

void TpSurface::setClipRect(tpRect *rect)
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

ItpRect TpSurface::clipRect()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
    SDL_Rect rect = {0};

    if (!set)
        return ItpRect();

    if (!set->beUsed)
        return ItpRect();

    SDL_GetClipRect(set->surface, &rect);

    ItpRect result(rect.x, rect.y, rect.w, rect.h);

    return result;
}

void TpSurface::clear()
{
    this->fill(nullptr, _RGB(0, 0, 0));
}

void TpSurface::fill(tpRect *rect, int32_t color)
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
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

bool TpSurface::hasSurface()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;
    bool ret = false;

    if (set)
    {
        ret = set->beUsed;
    }

    return ret;
}

tpShared<TpSurface> TpSurface::copy(tpRect &rect)
{
    return this->copy(rect.X0(), rect.Y0(), rect.width(), rect.height());
}

tpShared<TpSurface> TpSurface::copy(int32_t x, int32_t y, int32_t w, int32_t h)
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;

    if (!set)
        return nullptr;

    if (set->beUsed == false)
        return nullptr;

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

    tpShared<TpSurface> newSurf = tpMakeShared<TpSurface>();

    if (newSurf == nullptr)
        return nullptr;

    bool ret = newSurf->create(nullptr, width, height, depth, stride, Rmask, Gmask, Bmask, Amask, alpha, enable, colorKey, nullptr);

    if (ret == false)
    {
        return nullptr;
    }

    SDL_Surface *dstSurf = (SDL_Surface *)newSurf->surface();

    if (dstSurf)
    {
        SDL_BlitSurface(set->surface, &cpRect, dstSurf, nullptr);
    }

    return newSurf;
}

void TpSurface::directBlitF(tpShared<TpSurface> surface, tpRect &src, tpRect &dst)
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

    SDL_Surface *srcSurf = (SDL_Surface *)surface->surface();

    if (srcSurf)
    {
        TpSurfaceData *set = (TpSurfaceData *)this->data_;

        if (set && set->beUsed)
        {
            SDL_BlitSurface(srcSurf, &srect, set->surface, &drect);
        }
    }
}

void TpSurface::directBlitT(tpShared<TpSurface> surface, tpRect &src, tpRect &dst)
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

    SDL_Surface *dstSurf = (SDL_Surface *)surface->surface();

    if (dstSurf)
    {
        TpSurfaceData *set = (TpSurfaceData *)this->data_;

        if (set && set->beUsed)
        {
            SDL_BlitSurface(set->surface, &srect, dstSurf, &drect);
        }
    }
}

void TpSurface::strenchBlitF(TpSurface &surface, tpRect &src, tpRect &dst)
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
        TpSurfaceData *set = (TpSurfaceData *)this->data_;

        if (set &&
            set->beUsed)
        {
            SDL_BlitScaled(srcSurf, &srect, set->surface, &drect);
        }
    }
}

void TpSurface::strenchBlitT(TpSurface &surface, tpRect &src, tpRect &dst)
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
        TpSurfaceData *set = (TpSurfaceData *)this->data_;

        if (set &&
            set->beUsed)
        {
            SDL_BlitScaled(set->surface, &srect, dstSurf, &drect);
        }
    }
}

bool TpSurface::release()
{
    TpSurfaceData *set = (TpSurfaceData *)this->data_;

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

    set->render = nullptr;
    set->beUsed = false;

    return true;
}
