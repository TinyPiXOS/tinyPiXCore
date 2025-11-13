#ifndef __TP_SURFACE_H
#define __TP_SURFACE_H

#include <TpCore.h>
#include <TpString.h>
#include "TpGlobal.h"
#include "TpRect.h"

/**strage 32bits and ARGB**/
TP_DEF_VOID_TYPE_VAR(ITpSurfaceData);
TP_DEF_VOID_TYPE_VAR(IPiDSSurface);

class TpRect;
/// @brief 内部类，用户无需调用
class TpSurface
{
public:
    // TpSurface(IPiDSSurface *surface = nullptr); // only for tinypix, otherwise use nullptr
    // only for tinypix, otherwise use nullptr
    TpSurface(IPiDSSurface *surface = nullptr, const TpRect& rect = TpRect()); 

    virtual ~TpSurface();

public:
    virtual bool create(IPiDSSurface *surface); // only for tinypix
    // if format not be 32, or Amask = 0, canvas will be ineffective
    virtual bool create(void *address, int32_t width, int32_t height, int32_t format, int32_t stride,
                        int32_t rmask = 0, int32_t gmask = 0, int32_t bmask = 0, int32_t amask = 0,
                        uint8_t alpha = 0xff, bool enableColroKey = false, uint32_t colorKey = 0, const TpRect &clip = TpRect());
    virtual bool create(tpShared<TpSurface> surface, bool bShareMemoried = true); // if false, can not copy source data, only copy other parameters

public:
    /// @brief 获取当前绘制画布指针
    /// @return 画布指针
    virtual IPiDSSurface *surface();

public:
    /// @brief 获取画布指针
    /// @return uint32_t*
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
    virtual void setClipRect(const TpRect &rect);
    virtual TpRect clipRect();

public:
    // virtual void clear();
    // virtual void fill(TpRect *rect, int32_t color);

public:
    virtual bool hasSurface();

public:
    virtual tpShared<TpSurface> copy(TpRect &rect);                               // will be effected by clip rect
    virtual tpShared<TpSurface> copy(int32_t x, int32_t y, int32_t w, int32_t h); // will be effected by clip rect

public:
    virtual void directBlitF(tpShared<TpSurface> surface, const TpRect &src, const TpRect &dst); // from other surface

    /// @brief 将自己的surface数据拷贝至目标surface
    /// @param surface
    /// @param src
    /// @param dst
    virtual void directBlitT(tpShared<TpSurface> surface, const TpRect &src, const TpRect &dst);

public:
    virtual void strenchBlitF(TpSurface &surface, const TpRect &src, const TpRect &dst); // from other surface, can strench, have to zoom out will be effective
    virtual void strenchBlitT(TpSurface &surface, const TpRect &src, const TpRect &dst); // to other surface, can strench, have to zoom out will be effective

public:
    /// @brief 释放内部所有资源，释放后Surface即无效
    /// @return 释放结果
    virtual bool release();

private:
    ITpSurfaceData *data_;
};

#endif
