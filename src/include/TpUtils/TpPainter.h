#ifndef __TP_CANVAS_H
#define __TP_CANVAS_H

#include "TpUtils.h"
#include "TpString.h"
#include "TpVector.h"
#include "TpGlobal.h"
#include "TpImage.h"
#include "TpGradient.h"
#include "TpPen.h"
#include "TpBrush.h"
#include "TpPainterPath.h"

class TpPainter;
class TpSurface;
class TpRect;
class TpFont;

/// @brief 绘制镂空遮罩
class TpHollowMask
{
public:
    /// @brief 矩形信息
    struct RectHollow
    {
        TpRect region;
        uint32_t round = 0;

        RectHollow(const TpRect &region, const uint32_t &round = 0)
            : region(region), round(round)
        {
        }
    };

    /// @brief 圆形信息
    struct CircleHollow
    {
        int32_t x;
        int32_t y;
        uint32_t radius;

        CircleHollow(const int32_t &x, const int32_t &y, const uint32_t &radius)
            : x(x), y(y), radius(radius)
        {
        }
    };

    /// @brief 扇形信息
    struct PieHollow
    {
        int32_t x;
        int32_t y;
        int32_t start; // 起始角度 0-360
        int32_t end;   // 终止角度 0-360
        uint32_t radius;

        PieHollow()
        {
        }
        PieHollow(const int32_t &x, const int32_t &y, const int32_t &start, const int32_t &end, const uint32_t &radius)
            : x(x), y(y), start(start), end(end), radius(radius)
        {
        }
    };

    /// @brief 多边形信息
    struct PolygonHollow
    {
        TpVector<TpPoint> posintList;

        PolygonHollow()
        {
        }
    };

public:
    TpHollowMask();
    ~TpHollowMask();

    /// @brief 添加矩形镂空
    /// @param region 矩形区域
    /// @param round 圆角值
    void addRectHollow(const TpRect &region, const uint32_t &round = 0);
    /// @brief 添加矩形镂空
    /// @param data 矩形镂空参数
    void addRectHollow(const RectHollow &data);
    /// @brief 获取矩形镂空列表
    /// @return 矩形镂空列表
    TpVector<RectHollow> rectHollowList() const;

    /// @brief 添加圆形镂空
    /// @param x 圆心X坐标
    /// @param y 圆心Y坐标
    /// @param radius 半径
    void addCircleHollow(const int32_t &x, const int32_t &y, const uint32_t &radius);
    /// @brief 添加圆形镂空
    /// @param data 圆形镂空参数
    void addCircleHollow(const CircleHollow &data);
    /// @brief 获取圆形镂空列表
    /// @return 圆形镂空列表
    TpVector<CircleHollow> circleHollowList() const;

    /// @brief 添加扇形镂空
    /// @param x 圆心X坐标
    /// @param y 圆心Y坐标
    /// @param radius 半径
    /// @param start 起始角度0 -360
    /// @param end 终止角度
    void addPieHollow(const int32_t &x, const int32_t &y, const uint32_t &radius, const int32_t &start, const int32_t &end);
    /// @brief 添加扇形镂空
    /// @param data 扇形镂空参数
    void addPieHollow(const PieHollow &data);
    /// @brief 获取扇形镂空列表
    /// @return 扇形镂空列表
    TpVector<PieHollow> pieHollowList() const;

    /// @brief 添加多边形镂空
    /// @param polygon 多边形镂空信息
    void addPolygonHollow(const PolygonHollow &polygon);
    /// @brief 获取多边形镂空列表
    /// @return 多边形镂空列表
    TpVector<PolygonHollow> polygonHollowList() const;

private:
    TpVector<RectHollow> rectList_;
    TpVector<CircleHollow> circleList_;
    TpVector<PieHollow> pieList_;
    TpVector<PolygonHollow> polygonList_;
};

TP_DEF_VOID_TYPE_VAR(ITpCanvasData);
/// @brief 绘制模块类，用于绘制各种形状。资源等
/// @brief 所有的颜色值，均使用_RGB或_RGBA宏给入十进制值（0-255），例如_RGB(128,128,128)或_RGBA(128,128,128,120)
class TpPainter
{
public:
    TpPainter(tpShared<TpSurface> surface, int32_t offsetX, int32_t offsetY, int32_t width, int32_t height);
    virtual ~TpPainter();

    void paintTest();

public:
    /// @brief 设置画笔颜色
    /// @param color 画笔颜色
    void setPen(const TpColors &color);
    /// @brief 设置画笔
    /// @param pen 画笔对象
    void setPen(const TpPen &pen);
    /// @brief 获取当前画笔对象引用
    /// @return 画笔对象引用
    TpPen &pen() const;

    /// @brief 设置画刷
    /// @param brush 画刷对象
    void setBrush(const TpBrush &brush);
    /// @brief 获取当前画刷对象引用
    /// @return 画刷对象引用
    TpBrush &brush() const;

public:
    /**color all are RRGGBBAA**/

    /// @brief 绘制一个像素点
    /// @param x x坐标
    /// @param y y坐标
    virtual void drawPoint(int32_t x, int32_t y);
    /// @brief 绘制一个点
    /// @param point 点坐标
    virtual void drawPoint(const TpPoint &point);

    /// @brief 绘制一条水平线
    /// @param x1 第一个点的X坐标
    /// @param x2 第二个点的X坐标
    /// @param y 两个点的Y坐标
    virtual void drawHLine(int32_t x1, int32_t x2, int32_t y);
    /// @brief 绘制一条垂直线
    /// @param x 两个点的X坐标
    /// @param y1 第一个点的Y坐标
    /// @param y2 第二个点的Y坐标
    virtual void drawVLine(int32_t x, int32_t y1, int32_t y2);
    /// @brief 绘制一条线
    /// @param x1 第一个点的X坐标
    /// @param y1 第一个点的Y坐标
    /// @param x2 第二个点的X坐标
    /// @param y2 第二个点的Y坐标
    virtual void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    /// @brief 绘制一条线
    /// @param point1 第一个点的坐标
    /// @param point2 第二个点的坐标
    virtual void drawLine(const TpPoint &point1, const TpPoint &point2);

    /// @brief 绘制矩形; rad = 0 则为直角矩形
    /// @param x1 矩形左上角顶点X坐标
    /// @param y1 矩形左上角顶点Y坐标
    /// @param w 矩形宽度
    /// @param h 矩形高度
    /// @param rad 圆角值
    /// @param hollowMaskData 掏空属性
    virtual void drawRect(int32_t x1, int32_t y1, int32_t w, int32_t h, int32_t rad = 0, const TpHollowMask &hollowMaskData = TpHollowMask());
    /// @brief 绘制矩形
    /// @param rect 矩形尺寸
    /// @param rad 圆角值
    /// @param hollowMaskData 掏空属性
    virtual void drawRect(const TpRect &rect, int32_t rad = 0, const TpHollowMask &hollowMaskData = TpHollowMask());

    /// @brief 绘制椭圆；长轴半径和短轴半径相等时为正圆
    /// @param x 圆心坐标X
    /// @param y 圆心坐标Y
    /// @param rx 长轴半径
    /// @param ry 短轴半径
    virtual void drawEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, const TpHollowMask &hollowMaskData = TpHollowMask());
    /// @brief 绘制椭圆；长轴半径和短轴半径相等时为正圆
    /// @param center 圆心坐标
    /// @param rx 长轴半径
    /// @param ry 短轴半径
    virtual void drawEllipse(const TpPoint &center, int32_t rx, int32_t ry, const TpHollowMask &hollowMaskData = TpHollowMask());

    /// @brief 绘制圆环;起始角顺时针绘制至终止角，终止角需大于起始角；
    /// @brief 0度：指向右侧（正X轴方向）
    /// @brief 90度：指向下方（正Y轴方向）
    /// @brief 180度：指向左侧（负X轴方向）
    /// @brief 270度：指向上方（负Y轴方向）
    /// @param x 圆心坐标
    /// @param y 圆心坐标
    /// @param rad 半径
    /// @param start 起始角度
    /// @param end 终止角度
    virtual void drawArc(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end);
    /// @brief 绘制圆环;起始角顺时针绘制至终止角，终止角需大于起始角；
    /// @param center 圆心坐标
    /// @param rad 半径
    /// @param start 起始角度
    /// @param end 终止角度
    virtual void drawArc(const TpPoint &center, int32_t rad, int32_t start, int32_t end);

    /// @brief 绘制扇形;起始角顺时针绘制至终止角，终止角需大于起始角；
    /// @brief 0度：指向右侧（正X轴方向）
    /// @brief 90度：指向下方（正Y轴方向）
    /// @brief 180度：指向左侧（负X轴方向）
    /// @brief 270度：指向上方（负Y轴方向）
    /// @param x 圆心坐标X
    /// @param y 圆心坐标Y
    /// @param rad 半径
    /// @param start 起始角度
    /// @param end 终止角度
    virtual void drawPie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, const TpHollowMask &hollowMaskData = TpHollowMask());
    /// @brief 绘制扇形;起始角顺时针绘制至终止角，终止角需大于起始角；
    /// @param center 圆心坐标
    /// @param rad 半径
    /// @param start 起始角度
    /// @param end 终止角度
    virtual void drawPie(const TpPoint &center, int32_t rad, int32_t start, int32_t end, const TpHollowMask &hollowMaskData = TpHollowMask());

    /// @brief 绘制多边形
    /// @param pointList 所有顶点坐标；数量为1则画点，为2则画线
    virtual void drawPolygon(const TpVector<TpPoint> &pointList, const TpHollowMask &hollowMaskData = TpHollowMask());

    /// @brief 绘制三次贝塞尔曲线
    /// @param startX 起始点
    /// @param startY
    /// @param cx1 第一个控制点
    /// @param cy1
    /// @param cx2 第二个控制点
    /// @param cy2
    /// @param endX 终点
    /// @param endY
    virtual void drawCubic(int32_t startX, int32_t startY, int32_t cx1, int32_t cy1, int32_t cx2, int32_t cy2, int32_t endX, int32_t endY);
    /// @brief 绘制三次贝塞尔曲线
    /// @param startPoint 起始点
    /// @param cPoint 第一个控制点
    /// @param c2Point 第二个控制点
    /// @param endPoint 终止点
    virtual void drawCubic(const TpPoint &startPoint, const TpPoint &cPoint, const TpPoint &c2Point, const TpPoint &endPoint);

    /// @brief 绘制图片
    /// @param x 绘制X坐标
    /// @param y 绘制Y坐标
    /// @param image 资源对象
    /// @param roundRad 圆角值
    virtual void drawImage(const int32_t &x, const int32_t &y, const TpImage &image, int32_t roundRad = 0);
    /// @brief 绘制图片
    /// @param point 绘制坐标
    /// @param image 资源对象
    /// @param roundRad 圆角值
    virtual void drawImage(const TpPoint &point, const TpImage &image, int32_t roundRad = 0);

    /// @brief 绘制文本
    /// @param font 文本字体
    /// @param x X坐标
    /// @param y Y坐标
    /// @param text 文本字符串内容
    virtual void drawText(TpFont &font, int32_t x, int32_t y, const TpString &text);
    /// @brief 绘制文本
    /// @param font 文本字体；内部需设置文本字符串
    /// @param x X坐标
    /// @param y Y坐标
    virtual void drawText(TpFont &font, int32_t x, int32_t y);

    /// @brief 绘制自定义路径
    /// @param path 自定义路径
    virtual void drawPath(const TpPainterPath& path);

public:
    /// @brief 设置裁剪矩形；只显示裁剪区域内容
    /// @param rect 裁剪矩形
    virtual void setClipRect(const TpRect &rect);

    /// @brief 获取裁剪区域
    /// @return 裁剪矩形
    virtual TpRect clipRect();

    /// @brief 清理画布；清除所有绘制对象
    virtual void erase();

public:
    /// @brief 用户无需调用
    void addScene(void *canvas, void *scene);

    /// @brief 绘图同步；用户无需调用
    void sync(void* object);

private:
    ITpCanvasData *data_;
};

#endif
