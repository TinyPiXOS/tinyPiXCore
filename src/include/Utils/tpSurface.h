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
	typedef enum
	{
		SAVE_BMP_FMT,
		SAVE_JPG_FMT,
		SAVE_PNG_FMT,
	} tpImageType;

public:
	tpSurface(IPiDSSurface *surface = nullptr); // only for tinypix, otherwise use nullptr
	tpSurface(const tpString& fileName, IPiDSSurface *surface = nullptr);

	virtual ~tpSurface();

public:
	/// @brief 加载图片资源文件，支持png, jpg, bmp, xpm, web, tiff, SVG
	/// @param filename 资源文件路径
	/// @param convertToFit 
	/// @return 加载结果
	virtual bool fromFile(const tpString &filename, bool convertToFit = true);

	/// @brief 指定size对图片进行缩放
	/// @param size 缩放后的尺寸
	/// @return 缩放后的资源对象
	tpShared<tpSurface> scaled(const ItpSize &size);
	/// @brief 指定size对图片进行缩放
	/// @param width 缩放后的宽度
	/// @param height 缩放后的高度
	/// @return  缩放后的资源对象
	tpShared<tpSurface> scaled(const uint32_t& width, const uint32_t& height);

public:
	virtual bool create(IPiDSSurface *surface); // only for tinypix
	// if format not be 32, or Amask = 0, canvas will be ineffective
	virtual bool create(void *address, int32_t width, int32_t height, int32_t format, int32_t stride,
						int32_t rmask = 0, int32_t gmask = 0, int32_t bmask = 0, int32_t amask = 0,
						uint8_t alpha = 0xff, bool enableColroKey = false, uint32_t colorKey = 0, tpRect *clip = nullptr, bool convertToFit = false);
	virtual bool create(tpSurface *surface, bool bShareMemoried = true); // if false, can not copy source data, only copy other parameters

public:
	virtual IPitpSurfacePtr *surface(); // real surface
	virtual IPiRendererPtr *renderer(); // real render

public:
	virtual void *matrix();

	/// @brief 对图片资源进行模糊处理(会对原始数据进行更新)
	/// @param radius 模糊半径
	void glassBlur(const int32_t& radius);

public:
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
	virtual bool hasImage();

public:
	virtual tpSurface *copy(tpRect &rect);								   // will be effected by clip rect
	virtual tpSurface *copy(int32_t x, int32_t y, int32_t w, int32_t h); // will be effected by clip rect

public:
	virtual void directBlitF(tpSurface &surface, tpRect &src, tpRect &dst); // from other surface
	virtual void directBlitT(tpSurface &surface, tpRect &src, tpRect &dst); // to other surface

public:
	virtual void strenchBlitF(tpSurface &surface, tpRect &src, tpRect &dst); // from other surface, can strench, have to zoom out will be effective
	virtual void strenchBlitT(tpSurface &surface, tpRect &src, tpRect &dst); // to other surface, can strench, have to zoom out will be effective

public:
	/// @brief 指定文件名保存资源文件数据  TODO，需要重构，可以支持不给入类型自动根据类型存储
	/// @param filename 
	/// @param type 
	/// @param jpguality 
	/// @return 
	virtual bool save(const tpString &filename, tpImageType type, int32_t jpguality = 100);
	virtual bool save(const tpString &filename, tpRect &rect, tpImageType type, int32_t jpguality = 100);

public:
	/// @brief 释放内部所有资源，释放后Surface即无效
	/// @return 释放结果
	virtual bool release();

private:
	tpShared<tpSurface> rotoZoomXY(double zx, double zy, double angle);

private:
	IPitpSurface *surfaceSet;
};

typedef tpSurface tpImage;

#endif
