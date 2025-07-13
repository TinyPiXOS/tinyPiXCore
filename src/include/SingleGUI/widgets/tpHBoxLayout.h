#ifndef __TP_HBOX_LAYOUT_H
#define __TP_HBOX_LAYOUT_H

#include "tpBoxLayout.h"

class tpHBoxLayout 
    : public tpBoxLayout
{
public:
    tpHBoxLayout(tpChildWidget *parent = nullptr);
    virtual ~tpHBoxLayout() = default;
};

#endif