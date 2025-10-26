#ifndef __TP_PEN_H
#define __TP_PEN_H

#include "TpUtils.h"
#include "TpColors.h"

class TpBrush;
TP_DEF_VOID_TYPE_VAR(ITpPenData);
/// @brief 画笔工具类，用于定义绘图时的线条属性
class TpPen
{
public:
    /// @brief 默认构造函数
    TpPen();
    /// @brief 拷贝构造函数
    TpPen(const TpPen &other);
    TpPen(const TpColors &color);
    TpPen(const TpColors &color, int32_t width);
    /// @brief 析构函数
    ~TpPen();

    /// @brief 获取画笔样式
    /// @return 当前画笔样式 (实线/虚线等)
    Tp::PenStyle style() const;

    /// @brief 设置画笔样式
    /// @param style 要设置的画笔样式
    void setStyle(Tp::PenStyle style);

    /// @brief 获取虚线偏移量
    /// @return 当前虚线偏移量
    float dashOffset() const;

    /// @brief 设置虚线偏移量
    /// @param doffset 新的虚线偏移量
    void setDashOffset(float doffset);

    /// @brief 获取画笔宽度
    /// @return 当前画笔宽度(像素)
    int32_t width() const;

    /// @brief 设置画笔宽度
    /// @param width 新的画笔宽度(像素)
    void setWidth(int32_t width);

    /// @brief 获取画笔颜色
    /// @return 当前画笔颜色对象
    TpColors color() const;

    /// @brief 设置画笔颜色
    /// @param color 新的颜色对象
    void setColor(const TpColors &color);

    /// @brief 获取线帽样式
    /// @return 当前线帽样式
    Tp::PenCapStyle capStyle() const;

    /// @brief 设置线帽样式
    /// @param pcs 新的线帽样式
    void setCapStyle(Tp::PenCapStyle pcs);

    /// @brief 获取连接点样式
    /// @return 当前连接点样式
    Tp::PenJoinStyle joinStyle() const;

    /// @brief 设置连接点样式
    /// @param pcs 新的连接点样式
    void setJoinStyle(Tp::PenJoinStyle pcs);

    /// @brief 设置画笔的画刷；可以设置渐变等填充效果
    /// @param brush 画刷
    void setBrush(const TpBrush &brush);
    /// @brief 获取当前画笔画刷
    /// @return 画刷
    TpBrush brush();

     /// @brief 赋值运算符重载
    TpPen& operator=(const TpPen &other);
    
private:
    ITpPenData *data_;
};

#endif