#ifndef __TP_PIXMAP_H
#define __TP_PIXMAP_H

#include "tpObject.h"
#include "tpString.h"
#include "tpSurface.h"

TP_DEF_VOID_TYPE_VAR(ItpPixmapData);

class tpPixmap
{
public:
    tpPixmap();
    tpPixmap(const tpString &pixPath);
    tpPixmap(const tpPixmap &pixmap);

    ~tpPixmap();

private:
    ItpPixmapData *data_;
};

#endif
