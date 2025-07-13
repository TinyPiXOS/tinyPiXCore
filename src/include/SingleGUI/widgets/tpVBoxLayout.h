#ifndef __TP_VBOX_LAYOUT_H
#define __TP_VBOX_LAYOUT_H

#include "tpBoxLayout.h"

class tpVBoxLayout
    : public tpBoxLayout
{
public:
    tpVBoxLayout(tpChildWidget *parent = nullptr);
    virtual ~tpVBoxLayout() = default;
};

#endif