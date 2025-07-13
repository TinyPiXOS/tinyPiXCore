#ifndef __TP_CANVAS_H
#define __TP_CANVAS_H

#include "tpUtils.h"
#include "tpString.h"
#include "tpVector.h"
#include "tpGlobal.h"
#include <cairo.h>
TP_DEF_VOID_TYPE_VAR(ItpCanvasData);

class tpCanvas;
class tpSurface;
class tpRect;
class tpFont;
class HollowMask;

typedef void (*defDrawFunction)(tpCanvas *, cairo_t *cr, cairo_surface_t *cairo_surface, int32_t offsetX, int32_t offsetY, void *args);

/// @brief 绘制模块类，用于绘制各种形状。资源等
/// @brief 所有的颜色值，均使用_RGB或_RGBA宏给入十进制值（0-255），例如_RGB(128,128,128)或_RGBA(128,128,128,120)
class tpCanvas
{
public:
	tpCanvas(tpSurface *surface = nullptr, int32_t offsetX = 0, int32_t offsetY = 0); // set target and initalized
	virtual ~tpCanvas();

	void paintTest();

public:
	virtual bool setTarget(tpSurface *surface, int32_t offsetX = 0, int32_t offsetY = 0); // nullptr will be canceled

public:
	virtual void setClipRect(tpRect &rect);
	virtual void setClipRect(ItpRect *rect);

public:
	virtual tpSurface *surface();

public:
	virtual void erase(); // fill transparency color

public:
	/**color all are RRGGBBAA**/

	/// @brief 绘制一个像素点
	/// @param x x坐标
	/// @param y y坐标
	/// @param color 颜色值
	virtual void pixel(int32_t x, int32_t y, int32_t color);

	/// @brief 绘制一条水平线
	/// @param x1 第一个点的X坐标
	/// @param x2 第二个点的X坐标
	/// @param y 两个点的Y坐标
	/// @param color 颜色值
	virtual void hline(int32_t x1, int32_t x2, int32_t y, int32_t color, double width = 1.0);

	/// @brief 绘制一条垂直线
	/// @param x 两个点的X坐标
	/// @param y1 第一个点的Y坐标
	/// @param y2 第二个点的Y坐标
	/// @param color 颜色值
	virtual void vline(int32_t x, int32_t y1, int32_t y2, int32_t color, double width = 1.0);
	virtual void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width = 1.0);

	virtual void rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width = 1.0);
	virtual void roundedRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, double width = 1.0);

	virtual void box(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color);
	/// @brief 绘制圆角矩形，第一个点坐标必须为左上点，第二个点坐标为右下点
	/// @param x1 左上角点X坐标
	/// @param y1 左上角点Y坐标
	/// @param x2 右下角点X坐标
	/// @param y2 右下角点Y坐标
	/// @param rad 圆角值
	/// @param color 颜色_RGB
	virtual void roundedBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color);

	/// @brief 绘制圆形
	/// @param x 圆心坐标X
	/// @param y 圆心坐标Y
	/// @param rad 半径
	/// @param color 颜色
	/// @param width 画笔宽度
	virtual void circle(int32_t x, int32_t y, int32_t rad, int32_t color, double width = 1.0);
	virtual void arc(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width = 1.0);
	/// @brief 绘制填充圆形
	/// @param x 圆心坐标X
	/// @param y 圆心坐标Y
	/// @param rad 半径
	/// @param color 颜色
	virtual void filledCircle(int32_t x, int32_t y, int32_t rad, int32_t color);

	virtual void ellipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color, double width = 1.0);
	virtual void filledEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color);

	/// @brief 绘制扇形
	/// @param x 圆心坐标X
	/// @param y 圆心坐标Y
	/// @param rad 半径
	/// @param start 起始角度
	/// @param end 终止角度
	/// @param color 颜色
	/// @param width 线宽
	virtual void pie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width = 1.0);
	/// @brief 绘制填充扇形
	/// @param x 圆心坐标X
	/// @param y 圆心坐标Y
	/// @param rad 半径
	/// @param start 起始角度
	/// @param end 终止角度
	/// @param color 颜色
	virtual void filledPie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color);

	virtual void trigon(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t color, double width = 1.0);
	virtual void filledTrigon(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t color);

	virtual void polygon(short *vx, short *vy, int32_t n, int32_t color, double width = 1.0);
	virtual void filledPolygon(short *vx, short *vy, int32_t n, int32_t color);

public:
	/// @brief 绘制镂空矩形
	/// @param x1 矩形左上点X坐标
	/// @param y1 矩形左上点Y坐标
	/// @param x2 矩形右下点X坐标
	/// @param y2 矩形右下点Y坐标
	/// @param color 填充颜色
	/// @param HollowMask 镂空形状数据
	virtual void hollowBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, const HollowMask &hollowMaskData);
	/// @brief 绘制镂空圆角矩形，第一个点坐标必须为左上点，第二个点坐标为右下点
	/// @param x1 左上角点X坐标
	/// @param y1 左上角点Y坐标
	/// @param x2 右下角点X坐标
	/// @param y2 右下角点Y坐标
	/// @param rad 圆角值
	/// @param color 颜色_RGB
	/// @param HollowMask 镂空形状数据
	virtual void hollowRoundedBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, const HollowMask &hollowMaskData);

public:
	virtual void paintSurface(const tpShared<tpSurface> &surface, const tpRect &src_rect, const tpRect &dst_rect, bool strench = false);
	virtual void paintSurface(const tpShared<tpSurface> &surface, const tpRect *src_rect, const tpRect *dst_rect, bool strench = false);
	/// @brief 绘制图片资源
	/// @param x 绘制X坐标
	/// @param y Y坐标
	/// @param surface 资源指针
	virtual void paintSurface(const int32_t &x, const int32_t &y, const tpShared<tpSurface> &surface);
	/// @brief 绘制圆角图片
	/// @param x X坐标
	/// @param y Y坐标
	/// @param rad 圆角弧度
	/// @param surface 资源指针
	virtual void paintRoundSurface(const int32_t &x, const int32_t &y, int32_t rad, const tpShared<tpSurface> &surface);

public:
	/**font render for mark up**/
	virtual void renderText(tpFont &font, int32_t x, int32_t y, const tpString &text);
	virtual void renderText(tpFont &font, int32_t x, int32_t y, const char *text);

	virtual void renderMarkUp(tpFont &font, int32_t x, int32_t y, const tpString &text);
	virtual void renderMarkUp(tpFont &font, int32_t x, int32_t y, const char *text);
	/**font render normally by font setText or setMarkUp**/
	virtual void renderText(tpFont &font, int32_t x, int32_t y);

public:
	/**customized function**/
	virtual void customizedCarioMethod(defDrawFunction func, void *args); // must be ARGB for 32 or 24, 565 for 16. or will not be ran

public:
	virtual void release();

public:
	static tpSurface *convertFromCairoToSurface(cairo_surface_t *cairo_surface);
	static cairo_surface_t *convertFromSurfaceToCairo(tpSurface *surface);

private:
	ItpCanvasData *canvasSet;
};

/// @brief 绘制镂空遮罩
class HollowMask
{
public:
	struct roundRectHollow
	{
		ItpRect region;
		uint32_t round = 0;

		roundRectHollow(const ItpRect &region, const uint32_t &round)
			: region(region), round(round)
		{
		}
	};

	struct circleHollow
	{
		int32_t x;
		int32_t y;
		uint32_t radius;

		circleHollow(const int32_t &x, const int32_t &y, const uint32_t &radius)
			: x(x), y(y), radius(radius)
		{
		}
	};

	struct pieHollow
	{
		int32_t x;
		int32_t y;
		int32_t start; // 起始角度 0-360
		int32_t end;   // 终止角度 0-360
		uint32_t radius;

		pieHollow()
		{
		}
		pieHollow(const int32_t &x, const int32_t &y, const int32_t &start, const int32_t &end, const uint32_t &radius)
			: x(x), y(y), start(start), end(end), radius(radius)
		{
		}
	};

public:
	HollowMask();
	~HollowMask();

	/// @brief 添加矩形镂空
	/// @param region 矩形区域
	void addRectHollow(const ItpRect &region);
	/// @brief 获取所有矩形镂空数据
	/// @return 矩形镂空列表
	tpVector<ItpRect> rectHollowList() const;

	/// @brief 添加圆角矩形镂空
	/// @param region 矩形区域
	/// @param round 圆角值
	void addRoundRectHollow(const ItpRect &region, const uint32_t &round);
	/// @brief 添加圆角矩形镂空
	/// @param data 矩形镂空参数
	void addRoundRectHollow(const roundRectHollow &data);
	/// @brief 获取圆角矩形镂空列表
	/// @return 圆角矩形镂空列表
	tpVector<roundRectHollow> roundRectHollowList() const;

	/// @brief 添加圆形镂空
	/// @param x 圆心X坐标
	/// @param y 圆心Y坐标
	/// @param radius 半径
	void addCircleHollow(const int32_t &x, const int32_t &y, const uint32_t &radius);
	/// @brief 添加圆形镂空
	/// @param data 圆形镂空参数
	void addCircleHollow(const circleHollow &data);
	/// @brief 获取圆形镂空列表
	/// @return 圆形镂空列表
	tpVector<circleHollow> circleHollowList() const;

	/// @brief 添加扇形镂空
	/// @param x 圆心X坐标
	/// @param y 圆心Y坐标
	/// @param radius 半径
	/// @param start 起始角度0 -360
	/// @param end 终止角度
	void addPieHollow(const int32_t &x, const int32_t &y, const uint32_t &radius, const int32_t &start, const int32_t &end);
	/// @brief 添加扇形镂空
	/// @param data 扇形镂空参数
	void addPieHollow(const pieHollow &data);
	/// @brief 获取扇形镂空列表
	/// @return 扇形镂空列表
	tpVector<pieHollow> pieHollowList() const;

	// 通用多边形镂空（扩展用）
	// void addPolygonHollow(const Polygon& points);

private:
	tpVector<ItpRect> rectList_;
	tpVector<roundRectHollow> roundRectList_;
	tpVector<circleHollow> circleList_;
	tpVector<pieHollow> pieList_;
};

#endif
