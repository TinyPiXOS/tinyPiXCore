#ifndef __TP_FONT_PRIVATE_H
#define __TP_FONT_PRIVATE_H

#include "TpString.h"
#include "thorVG/thorvg.h"
#include "TpColors.h"
#include "TpSize.h"
#include "TpFontConfig.h"

struct TpFontData
{
    tvg::Text *tvgTextPtr = nullptr;

    TpString text = "";
    // 字体大小
    int32_t ptsize;

    bool italic = false;
    bool bold = false;
    bool wrap = false;

    TpSize wrapLayout = {100, 100};

    Tp::AlignmentFlag alignFlag;

    // 轮廓颜色
    int32_t fgColor;

    TpFontData()
    {
        tvgTextPtr = tvg::Text::gen();
    }
    ~TpFontData()
    {
        delete tvgTextPtr;
        tvgTextPtr = nullptr;
    }
};

#endif
