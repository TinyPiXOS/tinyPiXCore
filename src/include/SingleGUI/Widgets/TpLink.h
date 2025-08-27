#ifndef __TP_VLINK_H
#define __TP_VLINK_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpColors.h"
#include <string>

TP_DEF_VOID_TYPE_VAR(IPitpLinkSet);

class TpFont;
class TpLink : public TpChildWidget
{
public:
	TpLink(TpChildWidget *parent);
	virtual ~TpLink();

public:
	virtual void setAutoFit(bool enable = true);

public:
	virtual void setNormalColor(int32_t color = TpColors::Blue);
	virtual void setNormalColor(TpColors &color);

public:
	virtual void setHoverColor(int32_t color = TpColors::Red);
	virtual void setHoverColor(TpColors &color);

public:
	virtual void setDownColor(int32_t color = TpColors::Black);
	virtual void setDownColor(TpColors &color);

public:
	virtual void setText(const char *text);
	virtual void setText(std::string &text);

public:
	virtual TpFont *font();

public:
	virtual void setRect(TpRect &rect);
	virtual void setRect(TpRect *rect);
	virtual void setRect(ItpRect &rect);
	virtual void setRect(ItpRect *rect);
	virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h);

public:
	declare_signal(onClicked, bool);

public:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
	virtual bool onMouseMoveEvent(TpMouseEvent *event);
	virtual bool onLeaveEvent(TpObjectLeaveEvent *event);
	virtual bool onPaintEvent(TpObjectPaintEvent *event);

private:
	IPitpLinkSet *linkSet;
};

#endif
