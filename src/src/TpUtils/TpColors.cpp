/***
 * @Author: hywang
 * @Date: 2024-08-16 16:17:28
 * @LastEditors: hywang
 * @LastEditTime: 2024-08-16 16:19:28
 * @FilePath: /pix-singlegui/PixSingleGUI/include/Utils/TpColors.cpp
 * @Description:
 * @
 * @PiXOS
 */
#include "TpColors.h"

inline void TpColors::setNull()
{
    isNull_ = true;
    this->colorSet_ = _RGBA(0, 0, 0, 255);
}

inline bool TpColors::isNull()
{
    return isNull_;
}

void TpColors::setRgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    isNull_ = false;
    this->colorSet_ = _RGBA(r, g, b, a);
}