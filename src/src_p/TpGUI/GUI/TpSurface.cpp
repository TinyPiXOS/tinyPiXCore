#include "TpSurface.h"
#include "TpRect.h"
#include <unistd.h>
#include <iostream>
#include "TpFileInfo.h"

#include <tinyPiXUtils.h>
#include "thorVG/thorvg.h"

#define DEFAULT_SVG_WIDTH 100
#define DEFAULT_SVG_HEIGHT 100

#define ARGB_A(pixel) (((pixel) >> 24) & 0xFF) // Alpha通道
#define ARGB_R(pixel) (((pixel) >> 16) & 0xFF) // Red通道
#define ARGB_G(pixel) (((pixel) >> 8) & 0xFF)  // Green通道
#define ARGB_B(pixel) ((pixel) & 0xFF)         // Blue通道
#define ARGB_PACK(a, r, g, b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

struct TpSurfaceData
{
    IPiWFSurface *pixWFSurface = nullptr;

    bool beUsed = false;

    // 画布矩形
    TpRect surfaceRect;
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

// TpSurface::TpSurface(IPiDSSurface *surface)
// {
//     TpSurfaceData *set = new TpSurfaceData();
//     set->beUsed = false;
//     this->data_ = set;

//     if (surface)
//     {
//         bool ret = this->create(surface);

//         if (ret == false)
//         {
//             std::cout << "TpSurface creates failed!" << std::endl;
//             std::exit(0);
//         }
//     }
// }

TpSurface::TpSurface(IPiDSSurface *surface, const TpRect &rect)
{
    TpSurfaceData *set = new TpSurfaceData();
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

        set->surfaceRect = rect;
    }
}

TpSurface::~TpSurface()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);

    if (set)
    {
        this->release();
        delete set;
    }

    useRef--;
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

    if (width == 0 || height == 0)
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

    TpRect clipRect(TpPoint(x, y), TpSize(w, h));

    return this->create(matrix, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask, alpha, enable, colorKey, clipRect);
}

bool TpSurface::create(void *address, int32_t width, int32_t height, int32_t format, int32_t stride,
                       int32_t rmask, int32_t gmask, int32_t bmask, int32_t amask,
                       uint8_t alpha, bool enableColroKey, uint32_t colorKey, const TpRect &clip)
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return false;

    IPiWFSurface *tmpSurface2 = nullptr;

    tmpSurface2 = tinyPiX_surface_create(address, width, height, format, rmask, gmask, bmask, amask);
    if (tmpSurface2 == nullptr)
        return false;

    if (alpha != 0xff)
    {
        // SDL_SetSurfaceAlphaMod(tmpSurface, alpha);
        // SDL_SetSurfaceBlendMode(tmpSurface, SDL_BLENDMODE_BLEND);

        tinyPiX_surface_set_alpha(tmpSurface2, alpha);
    }

    tinyPiX_surface_set_colorkey_enable(tmpSurface2, enableColroKey);
    tinyPiX_surface_set_colorkey(tmpSurface2, colorKey);

    if (clip.width() > 0 && clip.height() > 0)
    {
        tinyPiX_surface_set_cliprect(tmpSurface2, clip.x(), clip.y(), clip.width(), clip.height());
    }

    if (set->beUsed)
    {
        this->release();
    }

    set->pixWFSurface = tmpSurface2;

    set->beUsed = true;

    return true;
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

    TpRect TpR = surface->clipRect();

    return this->create(matrix, width, height, depth, stride, Rmask, Gmask, Bmask, Amask, alpha, enable, colorKey, TpR);
}

IPiDSSurface *TpSurface::surface()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);

    if (!set)
        return nullptr;

    if (!set->beUsed)
        return nullptr;

    return set->pixWFSurface;
}

void *TpSurface::matrix()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    // void *matrix = nullptr;

    if (!set)
        return nullptr;

    if (!set->beUsed)
        return nullptr;

    // std::cout << "Martix 偏移量 " << set->surfaceRect.x() << "  " << set->surfaceRect.y() << std::endl;

    // 原始buffer尺寸
    // uint32_t *matrix = (uint32_t *)tinyPiX_surface_get_matrix(set->pixWFSurface);
    // 偏移
    // matrix = matrix + (set->surfaceRect.x() + set->surfaceRect.y() * 1080);

    void * matrix = tinyPiX_surface_get_matrix(set->pixWFSurface);

    return matrix;
}

int32_t TpSurface::stride()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return 0;

    if (!set->beUsed)
        return 0;

    return tinyPiX_surface_get_stride(set->pixWFSurface);
}

int32_t TpSurface::width()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return 0;

    if (!set->beUsed)
        return 0;

    // return set->surfaceRect.width();
    return tinyPiX_surface_get_width(set->pixWFSurface);
}

int32_t TpSurface::height()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return 0;

    if (!set->beUsed)
        return 0;

    // return set->surfaceRect.height();
    return tinyPiX_surface_get_height(set->pixWFSurface);
}

int32_t TpSurface::format()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return 0;

    if (!set->beUsed)
        return 0;

    return tinyPiX_surface_get_format(set->pixWFSurface);
}

int32_t TpSurface::rmask()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return 0;

    if (!set->beUsed)
        return 0;

    return tinyPiX_surface_get_rmask(set->pixWFSurface);
}

int32_t TpSurface::gmask()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return 0;

    if (!set->beUsed)
        return 0;

    return tinyPiX_surface_get_gmask(set->pixWFSurface);
}

int32_t TpSurface::bmask()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return 0;

    if (!set->beUsed)
        return 0;

    return tinyPiX_surface_get_bmask(set->pixWFSurface);
}

int32_t TpSurface::amask()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return 0;

    if (!set->beUsed)
        return 0;

    return tinyPiX_surface_get_amask(set->pixWFSurface);
}

void TpSurface::setClipRect(const TpRect &rect)
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return;

    if (!set->beUsed)
        return;

    if (rect.width() == 0 || rect.height() == 0)
        return;

    tinyPiX_surface_set_cliprect(set->pixWFSurface, rect.x(), rect.y(), rect.width(), rect.height());
}

TpRect TpSurface::clipRect()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    if (!set)
        return TpRect();

    if (!set->beUsed)
        return TpRect();

    int32_t x = 0;
    int32_t y = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    tinyPiX_surface_get_cliprect(set->pixWFSurface, &x, &y, &width, &height);

    return TpRect(TpPoint(x, y), TpSize(width, height));
}

// void TpSurface::clear()
// {
//     this->fill(nullptr, _RGB(0, 0, 0));
// }

// void TpSurface::fill(TpRect *rect, int32_t color)
// {
//     TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
//     SDL_Rect *pFillRect = nullptr, fillRect;

//     if (rect)
//     {
//         fillRect.x = rect->x();
//         fillRect.y = rect->y();
//         fillRect.w = rect->width();
//         fillRect.h = rect->height();

//         pFillRect = &fillRect;
//     }

//     if (set)
//     {
//         uint8_t r = _R(color), g = _G(color), b = _B(color), a = _A(color);
//         color = SDL_MapRGBA(set->surface->format, r, g, b, a);
//         SDL_FillRect(set->surface, pFillRect, color);
//     }
// }

bool TpSurface::hasSurface()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);
    bool ret = false;

    if (set)
    {
        ret = set->beUsed;
    }

    return ret;
}

tpShared<TpSurface> TpSurface::copy(TpRect &rect)
{
    return this->copy(rect.x(), rect.y(), rect.width(), rect.height());
}

tpShared<TpSurface> TpSurface::copy(int32_t x, int32_t y, int32_t w, int32_t h)
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);

    if (!set)
        return nullptr;

    if (set->beUsed == false)
        return nullptr;

    int32_t width = w;
    int32_t height = h;

    int32_t depth = format();

    int32_t stride = cal_stride(width, depth);

    uint32_t Rmask;
    uint32_t Gmask;
    uint32_t Bmask;
    uint32_t Amask;
    tinyPiX_surface_get_rgba_mask(set->pixWFSurface, &Rmask, &Gmask, &Bmask, &Amask);

    uint8_t alpha = tinyPiX_surface_get_alpha(set->pixWFSurface);
    int32_t colorKey = tinyPiX_surface_get_colorkey(set->pixWFSurface);
    bool enable = tinyPiX_surface_get_colorkey_enable(set->pixWFSurface);

    tpShared<TpSurface> newSurf = tpMakeShared<TpSurface>();
    if (newSurf == nullptr)
        return nullptr;

    bool ret = newSurf->create(nullptr, width, height, depth, stride, Rmask, Gmask, Bmask, Amask, alpha, enable, colorKey);

    if (ret == false)
        return nullptr;

    // newSurf->setClipRect(TpRect(x, y, w, h));
    return newSurf;
}

void TpSurface::strenchBlitF(TpSurface &surface, const TpRect &src, const TpRect &dst)
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);

    if (set && set->beUsed)
    {
        // SDL_BlitScaled(srcSurf, src, set->surface, dst);
    }
}

void TpSurface::strenchBlitT(TpSurface &surface, const TpRect &src, const TpRect &dst)
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);

    if (set && set->beUsed)
    {
        // SDL_BlitScaled(set->surface, &srect, dstSurf, &drect);
    }
}

bool TpSurface::release()
{
    TpSurfaceData *set = static_cast<TpSurfaceData *>(data_);

    if (!set)
        return false;

    if (!set->beUsed)
        return false;

    if (set->pixWFSurface)
    {
        tinyPiX_surface_free(set->pixWFSurface);
    }

    set->pixWFSurface = nullptr;

    set->beUsed = false;

    return true;
}
