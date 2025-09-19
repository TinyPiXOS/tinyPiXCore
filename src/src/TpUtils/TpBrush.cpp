#include "TpBrush.h"

// 画刷数据实现结构体
struct TpBrushData
{
    Tp::BrushStyle style; // 画刷样式
    TpColors color;            // 画刷颜色
    TpGradient *gradient;      // 渐变对象指针（如果是渐变画刷）

    /// @brief 默认构造函数
    TpBrushData()
        : style(Tp::NoBrush),
          color(TpColors())
    {
    }

    /// @brief 带参数的构造函数
    /// @param bs 画刷样式
    /// @param c 画刷颜色
    TpBrushData(Tp::BrushStyle bs, const TpColors &c = TpColors())
        : style(bs),
          color(c)
    {
    }

    /// @brief 渐变画刷构造函数
    /// @param g 渐变对象
    TpBrushData(TpGradient *g)
        : style(Tp::LinearGradientPattern),
          gradient(g)
    {
    }

    /// @brief 复制构造函数
    /// @param other 要复制的数据对象
    TpBrushData(const TpBrushData &other)
        : style(other.style),
          color(other.color),
          gradient(other.gradient)
    {
    }

    /// @brief 赋值运算符重载
    /// @param other 要赋值的对象
    /// @return 当前对象的引用
    TpBrushData &operator=(const TpBrushData &other)
    {
        if (this != &other)
        {
            style = other.style;
            color = other.color;
            gradient = other.gradient;
        }
        return *this;
    }
};

// 默认构造函数
TpBrush::TpBrush()
    : data_(new TpBrushData())
{
}

// 使用指定样式创建画刷
TpBrush::TpBrush(Tp::BrushStyle bs)
    : data_(new TpBrushData(bs))
{
}

// 使用指定颜色和样式创建画刷
TpBrush::TpBrush(const TpColors &color, Tp::BrushStyle bs)
    : data_(new TpBrushData(bs, color))
{
}

// 复制构造函数
TpBrush::TpBrush(const TpBrush &brush)
    : data_(new TpBrushData(*static_cast<TpBrushData *>(brush.data_)))
{
}

// 使用渐变创建画刷
TpBrush::TpBrush(TpGradient *gradient)
    : data_(new TpBrushData(gradient))
{
}

// 析构函数
TpBrush::~TpBrush()
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    if (brushData)
    {
        delete brushData;
        brushData = nullptr;
        data_ = nullptr;
    }
}

// 赋值运算符重载
TpBrush &TpBrush::operator=(const TpBrush &brush)
{
    if (this != &brush)
    {
        TpBrushData *brushData = static_cast<TpBrushData *>(data_);
        TpBrushData *otherData = static_cast<TpBrushData *>(brush.data_);

        // 复制新数据
        *brushData = *otherData;
    }
    return *this;
}

// 获取画刷样式
Tp::BrushStyle TpBrush::style() const
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    return brushData->style;
}

// 设置画刷样式
void TpBrush::setStyle(Tp::BrushStyle bs)
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    brushData->style = bs;
}

// 获取画刷颜色
const TpColors &TpBrush::color() const
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    return brushData->color;
}

// 设置画刷颜色
void TpBrush::setColor(const TpColors &color)
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    brushData->color = color;
}

// 获取渐变对象
TpGradient *TpBrush::gradient() const
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    return brushData->gradient;
}