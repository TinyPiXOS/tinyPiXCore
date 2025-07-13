#ifndef TP_SPACER_ITEM_H
#define TP_SPACER_ITEM_H

#include "tpObject.h"

class tpSpacerItem
    : public tpObject
{
public:
    enum tpSizePolicy
    {
        Fixed,     // 固定尺寸（sizeHint为唯一尺寸）
        Minimum,   // sizeHint为最小尺寸
        Maximum,   // sizeHint为最大尺寸
        Expanding  // 尽可能扩展空间
    };

public:
    tpSpacerItem(const ItpSize &sizeHint,
                 tpSizePolicy hPolicy = Minimum,
                 tpSizePolicy vPolicy = Minimum);

    tpSpacerItem(const uint32_t &w, const uint32_t &h,
                 tpSizePolicy hPolicy = Minimum,
                 tpSizePolicy vPolicy = Minimum);

    // 尺寸策略访问器
    tpSizePolicy horizontalPolicy() const { return hPolicy_; }
    tpSizePolicy verticalPolicy() const { return vPolicy_; }

    // 尺寸约束访问器
    ItpSize sizeHint() const { return sizeHint_; }

private:
    ItpSize sizeHint_;
    tpSizePolicy hPolicy_;
    tpSizePolicy vPolicy_;
};

#endif // TP_SPACER_ITEM_H