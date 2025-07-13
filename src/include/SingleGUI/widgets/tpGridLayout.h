#ifndef __TP_GRID_LAYOUT_H
#define __TP_GRID_LAYOUT_H

#include "tpLayout.h"

TP_DEF_VOID_TYPE_VAR(ItpGridLayoutData);

class tpGridLayout : public tpLayout
{
public:
	tpGridLayout();
	virtual ~tpGridLayout();

	virtual void update() override;

private:
	ItpGridLayoutData *data_;
};

#endif
