#ifndef __TP_SURFACE_H
#define __TP_SURFACE_H

#include "tpUtils.h"
#include <tpString.h>
#include "tpGlobal.h"

/**strage 32bits and ARGB**/
TP_DEF_VOID_TYPE_VAR(IPitpSurface);
TP_DEF_VOID_TYPE_VAR(IPitpSurfacePtr);
TP_DEF_VOID_TYPE_VAR(IPiDSSurface);
TP_DEF_VOID_TYPE_VAR(IPiRendererPtr);

class tpRect;
class tpSurface
{
public:
    tpSurface(IPiDSSurface *surface = nullptr); // only for tinypix, otherwise use nullptr
    virtual ~tpSurface();

public:
    virtual bool create(IPiDSSurface *surface); // only for tinypix
    // if format not be 32, or Amask = 0, canvas will be ineffective
    virtual bool create(void *address, int32_t width, int32_t height, int32_t format, int32_t stride,
                        int32_t rmask = 0, int32_t gmask = 0, int32_t bmask = 0, int32_t amask = 0,
                        uint8_t alpha = 0xff, bool enableColroKey = false, uint32_t colorKey = 0, tpRect *clip = nullptr, bool convertToFit = false);
    virtual bool create(tpShared<tpSurface> surface, bool bShareMemoried = true); // if false, can not copy source data, only copy other parameters

public:
    virtual IPitpSurfacePtr *surface(); // real surface
    virtual IPiRendererPtr *renderer(); // real render

public:
    virtual void *matrix();

    virtual int32_t stride();

public:
    /// @brief 获取surface的宽度
    /// @return 宽度值
    virtual int32_t width();
    /// @brief 获取surface的高度
    /// @return 高度值
    virtual int32_t height();

public:
    virtual int32_t format();

public:
    virtual int32_t rmask();
    virtual int32_t gmask();
    virtual int32_t bmask();
    virtual int32_t amask();

public:
    virtual void setClipRect(tpRect *rect);
    virtual ItpRect clipRect();

public:
    virtual void clear();
    virtual void fill(tpRect *rect, int32_t color);

public:
    virtual bool hasSurface();

public:
    virtual tpShared<tpSurface> copy(tpRect &rect);                               // will be effected by clip rect
    virtual tpShared<tpSurface> copy(int32_t x, int32_t y, int32_t w, int32_t h); // will be effected by clip rect

public:
    virtual void directBlitF(tpShared<tpSurface> surface, tpRect &src, tpRect &dst); // from other surface
    virtual void directBlitT(tpShared<tpSurface> surface, tpRect &src, tpRect &dst); // to other surface

public:
    virtual void strenchBlitF(tpSurface &surface, tpRect &src, tpRect &dst); // from other surface, can strench, have to zoom out will be effective
    virtual void strenchBlitT(tpSurface &surface, tpRect &src, tpRect &dst); // to other surface, can strench, have to zoom out will be effective

public:
    /// @brief 释放内部所有资源，释放后Surface即无效
    /// @return 释放结果
    virtual bool release();

private:
    IPitpSurface *surfaceSet;
};

#endif
