#include "TpColors.h"

TpColors::TpColors() : isNull_(true)
{
    data_ = _RGBA(0, 0, 0, 255);
}

TpColors::TpColors(int32_t colorSet) : isNull_(false)
{
    data_ = colorSet;
}

TpColors::TpColors(const TpColors &color) : isNull_(false)
{
    data_ = color.rgba();
}

TpColors::TpColors(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : isNull_(false)
{
    data_ = _RGBA(r, g, b, a);
}

TpColors::~TpColors()
{
}

void TpColors::setRed(uint8_t r)
{
    data_ = _RGBA(r, green(), blue(), alpha());
}

void TpColors::setGreen(uint8_t g)
{
    data_ = _RGBA(red(), g, blue(), alpha());
}

void TpColors::setBlue(uint8_t b)
{
    data_ = _RGBA(red(), green(), b, alpha());
}

void TpColors::setAlpha(uint8_t a)
{
    data_ = _RGBA(red(), green(), blue(), a);
}

inline void TpColors::setNull()
{
    isNull_ = true;
    data_ = _RGBA(0, 0, 0, 255);
}

inline bool TpColors::isNull()
{
    return isNull_;
}

void TpColors::setRgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    isNull_ = false;
    data_ = _RGBA(r, g, b, a);
}

TpColors TpColors::operator=(int32_t color)
{
    isNull_ = false;
    data_ = color;
    return *this;
}

TpColors TpColors::operator=(const TpColors &others)
{
    isNull_ = others.isNull_;
    data_ = others.data_;
    return *this;
}
