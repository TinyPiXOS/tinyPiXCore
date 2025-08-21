#ifndef __TP_IMAGE_PRIVATE_H
#define __TP_IMAGE_PRIVATE_H

#include "tpString.h"
#include "thorVG/thorvg.h"

struct TpImageData
{
    tpString fileName = "";
    tvg::Picture *tvgPicture = nullptr;

    ~TpImageData()
    {
        delete tvgPicture;
        tvgPicture = nullptr;
    }
};

#endif
