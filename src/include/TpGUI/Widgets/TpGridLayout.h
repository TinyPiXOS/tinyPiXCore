#ifndef __TP_GRID_LAYOUT_H
#define __TP_GRID_LAYOUT_H

#include "TpLayout.h"

TP_DEF_VOID_TYPE_VAR(ITpGridLayoutData);

class TpGridLayout : public TpLayout
{
public:
	TpGridLayout();
	virtual ~TpGridLayout();

	virtual void update() override;

private:
	ITpGridLayoutData *data_;
};

#endif
