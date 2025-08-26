#ifndef __TP_IMAGE_PRIVATE_H
#define __TP_IMAGE_PRIVATE_H

#include "TpString.h"
#include "thorVG/thorvg.h"

struct TpImageData
{
    TpString fileName = "";
    tvg::Picture *tvgPicture = nullptr;

    ~TpImageData()
    {
        delete tvgPicture;
        tvgPicture = nullptr;
    }
};

#endif
