#ifndef __TP_IMAGE_PRIVATE_H
#define __TP_IMAGE_PRIVATE_H

#include "TpString.h"
#include "thorVG/thorvg.h"

struct TpImageData
{
    TpString fileName = "";
    tvg::Picture *tvgPicture = nullptr;

    // 图片实际尺寸
    float actualWidth = 0;
    float actualHeight = 0;

    ~TpImageData()
    {
        delete tvgPicture;
        tvgPicture = nullptr;
    }
};

#endif
