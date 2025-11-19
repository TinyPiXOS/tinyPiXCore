#ifndef __TP_VBOX_LAYOUT_H
#define __TP_VBOX_LAYOUT_H

#include "TpBoxLayout.h"

class TpVBoxLayout
    : public TpBoxLayout
{
public:
    TpVBoxLayout(TpWidget *parent = nullptr);
    virtual ~TpVBoxLayout() = default;
};

#endif