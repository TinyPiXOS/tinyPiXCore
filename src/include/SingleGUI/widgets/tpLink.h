#ifndef __TP_VLINK_H
#define __TP_VLINK_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpColors.h"
#include <string>

TP_DEF_VOID_TYPE_VAR(IPitpLinkSet);

class tpFont;
class tpLink : public tpChildWidget
{
public:
	tpLink(tpChildWidget *parent);
	virtual ~tpLink();

public:
	virtual void setAutoFit(bool enable = true);

public:
	virtual void setNormalColor(int32_t color = tpColors::Blue);
	virtual void setNormalColor(tpColors &color);

public:
	virtual void setHoverColor(int32_t color = tpColors::Red);
	virtual void setHoverColor(tpColors &color);

public:
	virtual void setDownColor(int32_t color = tpColors::Black);
	virtual void setDownColor(tpColors &color);

public:
	virtual void setText(const char *text);
	virtual void setText(std::string &text);

public:
	virtual tpFont *font();

public:
	virtual void setRect(tpRect &rect);
	virtual void setRect(tpRect *rect);
	virtual void setRect(ItpRect &rect);
	virtual void setRect(ItpRect *rect);
	virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h);

public:
	declare_signal(onClicked, bool);

public:
    virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
	virtual bool onMouseMoveEvent(tpMouseEvent *event);
	virtual bool onLeaveEvent(tpObjectLeaveEvent *event);
	virtual bool onPaintEvent(tpObjectPaintEvent *event);

private:
	IPitpLinkSet *linkSet;
};

#endif
