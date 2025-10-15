#ifndef __TP_IMAGE_PRIVATE_H
#define __TP_IMAGE_PRIVATE_H

#include "TpString.h"
#include "thorVG/thorvg.h"

struct TpImageData
{
    TpString fileName = "";
    tvg::Picture *tvgPicture = nullptr;

    uint32_t *loadBuffer = nullptr;

    // 图片实际尺寸
    int32_t actualWidth = 0;
    int32_t actualHeight = 0;

    ~TpImageData()
    {
        if (loadBuffer)
        {
            delete[] loadBuffer;
            loadBuffer = nullptr;
        }
        delete tvgPicture;
        tvgPicture = nullptr;
    }
};

#endif
