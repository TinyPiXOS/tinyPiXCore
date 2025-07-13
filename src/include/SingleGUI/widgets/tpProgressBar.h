#ifndef __TP_VPROGRESS_H
#define __TP_VPROGRESS_H

#include "tpChildWidget.h"
#include "tpColors.h"

TP_DEF_VOID_TYPE_VAR(IPitpProgressSet);

class tpFont;
class tpProgressBar : public tpChildWidget
{
public:
	tpProgressBar(tpChildWidget* parent);
	virtual ~tpProgressBar();

public:
	virtual void setRangeFrom(int32_t min = 0, int32_t max = 0);
	virtual void setPosition(int32_t pos);
	virtual void setPercent(double percent);

public:
	virtual int position();
	virtual double percent();

public:
	virtual tpFont *font();

public:
	virtual void setProgressBackColor(uint32_t bkColor = tpColors::Green);
	virtual void setProgressBackColor(tpColors &color);

public:
	virtual void setTopLeftLineColor(uint32_t color = tpColors::Black);
	virtual void setTopLeftLineColor(tpColors &color);

	virtual void setRightBottomLineColor(uint32_t color = tpColors::LightSlateGray);
	virtual void setRightBottomLineColor(tpColors &color);

public:
	virtual void setRect(tpRect &rect);
	virtual void setRect(tpRect *rect);
	virtual void setRect(ItpRect &rect);
	virtual void setRect(ItpRect *rect);
	virtual void setRect(int32_t x, int32_t y, uint32_t w, uint32_t h);

public:
	virtual bool onPaintEvent(tpObjectPaintEvent *event);

private:
	IPitpProgressSet *progressSet;
};

#endif
