#ifndef TP_SPACER_ITEM_H
#define TP_SPACER_ITEM_H

#include "TpObject.h"

class TpSpacerItem
    : public TpObject
{
public:
    enum TpSizePolicy
    {
        Fixed,     // 固定尺寸（sizeHint为唯一尺寸）
        Minimum,   // sizeHint为最小尺寸
        Maximum,   // sizeHint为最大尺寸
        Expanding  // 尽可能扩展空间
    };

public:
    TpSpacerItem(const ItpSize &sizeHint,
                 TpSizePolicy hPolicy = Minimum,
                 TpSizePolicy vPolicy = Minimum);

    TpSpacerItem(const uint32_t &w, const uint32_t &h,
                 TpSizePolicy hPolicy = Minimum,
                 TpSizePolicy vPolicy = Minimum);

    // 尺寸策略访问器
    TpSizePolicy horizontalPolicy() const { return hPolicy_; }
    TpSizePolicy verticalPolicy() const { return vPolicy_; }

    // 尺寸约束访问器
    ItpSize sizeHint() const { return sizeHint_; }

private:
    ItpSize sizeHint_;
    TpSizePolicy hPolicy_;
    TpSizePolicy vPolicy_;
};

#endif // TP_SPACER_ITEM_H