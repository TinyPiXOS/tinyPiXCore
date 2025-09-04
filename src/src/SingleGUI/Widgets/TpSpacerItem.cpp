#include "TpSpacerItem.h"

TpSpacerItem::TpSpacerItem(const TpSize &sizeHint, TpSizePolicy hPolicy, TpSizePolicy vPolicy)
    : sizeHint_(sizeHint), hPolicy_(hPolicy), vPolicy_(vPolicy)
{
}

TpSpacerItem::TpSpacerItem(const uint32_t &w, const uint32_t &h, TpSizePolicy hPolicy, TpSizePolicy vPolicy)
    : sizeHint_(w, h), hPolicy_(hPolicy), vPolicy_(vPolicy)
{
}
