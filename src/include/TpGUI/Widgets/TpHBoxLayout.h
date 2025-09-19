#ifndef __TP_HBOX_LAYOUT_H
#define __TP_HBOX_LAYOUT_H

#include "TpBoxLayout.h"

class TpHBoxLayout 
    : public TpBoxLayout
{
public:
    TpHBoxLayout(TpChildWidget *parent = nullptr);
    virtual ~TpHBoxLayout() = default;
};

#endif