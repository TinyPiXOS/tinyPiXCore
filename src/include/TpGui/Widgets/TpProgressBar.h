#ifndef __TP_VPROGRESS_H
#define __TP_VPROGRESS_H

#include "TpChildWidget.h"
#include "TpColors.h"

TP_DEF_VOID_TYPE_VAR(IPitpProgressSet);

class TpFont;
class TpProgressBar : public TpChildWidget
{
public:
	TpProgressBar(TpChildWidget* parent);
	virtual ~TpProgressBar();

public:
	virtual void setRangeFrom(int32_t min = 0, int32_t max = 0);
	virtual void setPosition(int32_t pos);
	virtual void setPercent(double percent);

public:
	virtual int position();
	virtual double percent();

public:
	virtual TpFont *font();

public:
	virtual void setProgressBackColor(uint32_t bkColor = TpColors::Green);
	virtual void setProgressBackColor(TpColors &color);

public:
	virtual void setTopLeftLineColor(uint32_t color = TpColors::Black);
	virtual void setTopLeftLineColor(TpColors &color);

	virtual void setRightBottomLineColor(uint32_t color = TpColors::LightSlateGray);
	virtual void setRightBottomLineColor(TpColors &color);

public:
	virtual void setRect(const TpRect &rect);
	virtual void setRect(int32_t x, int32_t y, uint32_t w, uint32_t h);

public:
	virtual bool onPaintEvent(TpPaintEvent *event);

private:
	IPitpProgressSet *progressSet;
};

#endif
