#ifndef __TP_CANVAS_H
#define __TP_CANVAS_H

#include "TpUtils.h"
#include "TpString.h"
#include "TpVector.h"
#include "TpGlobal.h"
#include "TpImage.h"
#include "TpGradient.h"

class TpCanvas;
class TpSurface;
class TpRect;
class TpFont;

/// @brief 绘制镂空遮罩
class HollowMask
{
public:
    /// @brief 矩形信息
    struct RectHollow
    {
        ItpRect region;
        uint32_t round = 0;

        RectHollow(const ItpRect &region, const uint32_t &round = 0)
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
        TpVector<ItpPoint> posintList;

        PolygonHollow()
        {
        }
    };

public:
    HollowMask();
    ~HollowMask();

    /// @brief 添加矩形镂空
    /// @param region 矩形区域
    /// @param round 圆角值
    void addRectHollow(const ItpRect &region, const uint32_t &round = 0);
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
class TpCanvas
{
public:
    TpCanvas(tpShared<TpSurface> surface = nullptr, int32_t offsetX = 0, int32_t offsetY = 0);
    virtual ~TpCanvas();

    void paintTest();

public:
    /// @brief 重设绘制画布
    /// @param surface 画布指针
    /// @param offsetX X偏移量
    /// @param offsetY Y偏移量
    /// @return 设置结果；成功返回true；否则返回false
    virtual bool setTarget(tpShared<TpSurface> surface, int32_t offsetX = 0, int32_t offsetY = 0);
    /// @brief 获取当前画图
    /// @return 返回画布指针；无则返回nullptr
    virtual tpShared<TpSurface> surface();

public:
    /// @brief 设置裁剪矩形；只显示裁剪区域内容
    /// @param rect 裁剪矩形
    virtual void setClipRect(const ItpRect &rect);

    /// @brief 获取裁剪区域
    /// @return 裁剪矩形
    virtual ItpRect clipRect();

public:
    /// @brief 清理画布；清除所有绘制对象
    virtual void erase();

public:
    /**color all are RRGGBBAA**/

    /// @brief 设置填充渐变；设置后绘制的所有形状颜色均为渐变色;绘制接口给入的颜色将失效
    /// @param gradient 渐变属性类；取消渐变则传入nullptr
    virtual void setGradient(TpGradient *gradient);

    /// @brief 绘制一个像素点
    /// @param x x坐标
    /// @param y y坐标
    /// @param color 颜色值
    virtual void pixel(int32_t x, int32_t y, int32_t color);

    /// @brief 绘制一条水平线
    /// @param x1 第一个点的X坐标
    /// @param x2 第二个点的X坐标
    /// @param y 两个点的Y坐标
    /// @param color 颜色
    virtual void hline(int32_t x1, int32_t x2, int32_t y, int32_t color, double width = 1.0);
    /// @brief 绘制一条垂直线
    /// @param x 两个点的X坐标
    /// @param y1 第一个点的Y坐标
    /// @param y2 第二个点的Y坐标
    /// @param color 颜色
    virtual void vline(int32_t x, int32_t y1, int32_t y2, int32_t color, double width = 1.0);
    /// @brief 绘制一条线
    /// @param x1 第一个点的X坐标
    /// @param y1 第一个点的Y坐标
    /// @param x2 第二个点的X坐标
    /// @param y2 第二个点的Y坐标
    /// @param color 颜色
    /// @param width 线宽
    virtual void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width = 1.0);

    /// @brief 绘制矩形
    /// @param x1 矩形左上角顶点X坐标
    /// @param y1 矩形左上角顶点Y坐标
    /// @param x2 矩形右下角顶点X坐标
    /// @param y2 矩形右下角顶点Y坐标
    /// @param color 颜色
    /// @param width 画笔宽度
    virtual void rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width = 1.0);
    /// @brief 绘制圆角矩形
    /// @param x1 矩形左上角顶点X坐标
    /// @param y1 矩形左上角顶点Y坐标
    /// @param x2 矩形右下角顶点X坐标
    /// @param y2 矩形右下角顶点Y坐标
    /// @param rad 圆角值
    /// @param color 颜色
    /// @param width 画笔宽度
    virtual void roundedRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, double width = 1.0);

    /// @brief 绘制矩形填充，第一个点坐标必须为左上点，第二个点坐标为右下点
    /// @param x1 左上角点X坐标
    /// @param y1 左上角点Y坐标
    /// @param x2 右下角点X坐标
    /// @param y2 右下角点Y坐标
    /// @param color 颜色_RGB
    /// @param HollowMask 镂空形状数据；以(x1, y1)为零点坐标
    virtual void box(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, const HollowMask &hollowMaskData = HollowMask());
    /// @brief 绘制圆角矩形填充，第一个点坐标必须为左上点，第二个点坐标为右下点
    /// @param x1 左上角点X坐标
    /// @param y1 左上角点Y坐标
    /// @param x2 右下角点X坐标
    /// @param y2 右下角点Y坐标
    /// @param rad 圆角值
    /// @param color 颜色_RGB
    /// @param HollowMask 镂空形状数据；以(x1, y1)为零点坐标
    virtual void roundedBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, const HollowMask &hollowMaskData = HollowMask());

    /// @brief 绘制圆形
    /// @param x 圆心坐标X
    /// @param y 圆心坐标Y
    /// @param rad 半径
    /// @param color 颜色
    /// @param width 画笔宽度
    virtual void circle(int32_t x, int32_t y, int32_t rad, int32_t color, double width = 1.0);
    /// @brief 绘制填充圆形
    /// @param x 圆心坐标X
    /// @param y 圆心坐标Y
    /// @param rad 半径
    /// @param color 颜色
    /// @param HollowMask 镂空形状数据；以(x - rad, y - rad)为零点坐标
    virtual void filledCircle(int32_t x, int32_t y, int32_t rad, int32_t color, const HollowMask &hollowMaskData = HollowMask());

    /// @brief 绘制椭圆
    /// @param x 圆心坐标X
    /// @param y 圆心坐标Y
    /// @param rx 长轴
    /// @param ry 短轴
    /// @param color 颜色
    /// @param width 线宽
    virtual void ellipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color, double width = 1.0);
    /// @brief 绘制填充椭圆
    /// @param x 圆心坐标X
    /// @param y 圆心坐标Y
    /// @param rx 长轴
    /// @param ry 短轴
    /// @param color 颜色
    /// @param HollowMask 镂空形状数据；以(x - rx, y - ry)为零点坐标
    virtual void filledEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color, const HollowMask &hollowMaskData = HollowMask());

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
    /// @param color 颜色
    /// @param width 画笔宽度
    /// @param isRound 顶点位置是否圆角处理
    virtual void arc(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width = 1.0, const bool &isRound = false);

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
    /// @param color 颜色
    /// @param width 线宽
    virtual void pie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width = 1.0);
    /// @brief 绘制填充扇形;起始角顺时针绘制至终止角，终止角需大于起始角；
    /// @brief 0度：指向右侧（正X轴方向）
    /// @brief 90度：指向下方（正Y轴方向）
    /// @brief 180度：指向左侧（负X轴方向）
    /// @brief 270度：指向上方（负Y轴方向）
    /// @param x 圆心坐标X
    /// @param y 圆心坐标Y
    /// @param rad 半径
    /// @param start 起始角度
    /// @param end 终止角度
    /// @param color 颜色
    /// @param HollowMask 镂空形状数据；以(x - rad, y - rad)为零点坐标
    virtual void filledPie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, const HollowMask &hollowMaskData = HollowMask());

    /// @brief 绘制多边形
    /// @param pointList 所有顶点坐标；数量为1则画点，为2则画线
    /// @param color 颜色
    /// @param width 线宽
    virtual void polygon(const TpVector<ItpPoint> &pointList, int32_t color, double width = 1.0);
    /// @brief 绘制填充多边形
    /// @param pointList 所有顶点坐标；数量为1则画点，为2则画线
    /// @param color 颜色
    /// @param HollowMask 镂空形状数据；以第一个点为零点坐标
    virtual void filledPolygon(const TpVector<ItpPoint> &pointList, int32_t color, const HollowMask &hollowMaskData = HollowMask());

    /// @brief 绘制三次贝塞尔曲线
    /// @param startX 起始点
    /// @param startY
    /// @param cx1 第一个控制点
    /// @param cy1
    /// @param cx2 第二个控制点
    /// @param cy2
    /// @param endX 终点
    /// @param endY
    /// @param color 颜色
    /// @param width 线宽
    virtual void cubicTo(int32_t startX, int32_t startY, int32_t cx1, int32_t cy1, int32_t cx2, int32_t cy2, int32_t endX, int32_t endY, int32_t color, double width = 1.0);
    /// @brief 绘制三次贝塞尔曲线
    /// @param startPoint 起始点
    /// @param cPoint 第一个控制点
    /// @param c2Point 第二个控制点
    /// @param endPoint 终止点
    /// @param color 颜色
    /// @param width 线宽
    virtual void cubicTo(ItpPoint startPoint, ItpPoint cPoint, ItpPoint c2Point, ItpPoint endPoint, int32_t color, double width = 1.0);

public:
    /// @brief 绘制图片资源
    /// @param x 绘制X坐标
    /// @param y 绘制Y坐标
    /// @param image 资源对象
    /// @param roundRad
    virtual void paintImage(const int32_t &x, const int32_t &y, const TpImage &image, int32_t roundRad = 0);

public:
    /// @brief 绘制文本
    /// @param font 文本字体
    /// @param x X坐标
    /// @param y Y坐标
    /// @param text 文本字符串内容
    virtual void renderText(TpFont &font, int32_t x, int32_t y, const TpString &text);
    /// @brief 绘制文本
    /// @param font 文本字体；内部需设置文本字符串
    /// @param x X坐标
    /// @param y Y坐标
    virtual void renderText(TpFont &font, int32_t x, int32_t y);

public:
    void addScene(void *canvas, void *scene);

    /// @brief 绘图同步；用户无需调用
    void sync();

private:
    ITpCanvasData *data_;
};

#endif
