#include "tpSpacerItem.h"

tpSpacerItem::tpSpacerItem(const ItpSize &sizeHint, tpSizePolicy hPolicy, tpSizePolicy vPolicy)
    : sizeHint_(sizeHint), hPolicy_(hPolicy), vPolicy_(vPolicy)
{
}

tpSpacerItem::tpSpacerItem(const uint32_t &w, const uint32_t &h, tpSizePolicy hPolicy, tpSizePolicy vPolicy)
    : sizeHint_(w, h), hPolicy_(hPolicy), vPolicy_(vPolicy)
{
}
