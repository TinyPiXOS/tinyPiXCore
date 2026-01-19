#include "TpBrush.h"
#include "TpLinearGradient.h"
#include "TpRadialGradient.h"

// 画刷数据实现结构体
struct TpBrushData
{
    Tp::BrushStyle style;           // 画刷样式
    TpColors color;                 // 画刷颜色
    TpGradient *gradient = nullptr; // 渐变对象指针（如果是渐变画刷）

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
        : style(Tp::LinearGradientPattern)
    {
        copyGradient(g);
    }

    /// @brief 复制构造函数
    /// @param other 要复制的数据对象
    TpBrushData(const TpBrushData &other)
        : style(other.style),
          color(other.color)
    {
        copyGradient(other.gradient);
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
            copyGradient(other.gradient);
        }
        return *this;
    }

    ~TpBrushData()
    {
        if (gradient)
        {
            delete gradient;
            gradient = nullptr;
        }
    }

private:
    void copyGradient(TpGradient *others)
    {
        // 先释放已存在的渐变对象
        if (gradient)
        {
            delete gradient;
            gradient = nullptr;
        }

        if (!others)
            return;

        if (others->gradientType() == TpGradient::LinearGradient)
        {
            TpLinearGradient *inputGradient = dynamic_cast<TpLinearGradient *>(others);
            if (!inputGradient)
                return;

            TpLinearGradient *lineGradient = new TpLinearGradient();
            *lineGradient = *inputGradient;
            gradient = lineGradient;
        }
        else if (others->gradientType() == TpGradient::RadialGradient)
        {
            TpRadialGradient *inputGradient = dynamic_cast<TpRadialGradient *>(others);
            if (!inputGradient)
                return;

            TpRadialGradient *radialGradient = new TpRadialGradient();
            *radialGradient = *inputGradient;
            gradient = radialGradient;
        }
        else
        {
        }
    }
};

TpBrush::TpBrush()
    : data_(new TpBrushData())
{
}

TpBrush::TpBrush(Tp::BrushStyle bs)
    : data_(new TpBrushData(bs))
{
}

TpBrush::TpBrush(const TpColors &color, Tp::BrushStyle bs)
    : data_(new TpBrushData(bs, color))
{
}

TpBrush::TpBrush(const TpBrush &brush)
    : data_(new TpBrushData(*static_cast<TpBrushData *>(brush.data_)))
{
}

TpBrush::TpBrush(TpGradient *gradient)
    : data_(new TpBrushData(gradient))
{
}

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

TpBrush &TpBrush::operator=(const TpBrush &brush)
{
    if (this != &brush)
    {
        TpBrushData *brushData = static_cast<TpBrushData *>(data_);
        delete brushData;
        brushData = nullptr;

        TpBrushData *otherData = static_cast<TpBrushData *>(brush.data_);

        // 3. 根据 brush 的 data_ 创建一个全新的 TpBrushData 副本
        //    这里调用了 TpBrushData 的拷贝构造函数，您已经正确实现了它
        TpBrushData *newData = new TpBrushData(*otherData);
        data_ = newData;

        // 复制新数据
        // *brushData = *otherData;
    }
    return *this;
}

Tp::BrushStyle TpBrush::style() const
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    return brushData->style;
}

void TpBrush::setStyle(Tp::BrushStyle bs)
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    brushData->style = bs;
}

const TpColors &TpBrush::color() const
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    return brushData->color;
}

void TpBrush::setColor(const TpColors &color)
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    brushData->color = color;
}

TpGradient *TpBrush::gradient() const
{
    TpBrushData *brushData = static_cast<TpBrushData *>(data_);
    return brushData->gradient;
}