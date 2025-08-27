#ifndef __TP_VFIX_SCREEN_H
#define __TP_VFIX_SCREEN_H

#include "TpScreen.h"

TP_DEF_VOID_TYPE_VAR(ItpFixScreenData);
class TpFixScreen
	: public TpScreen
{
public:
	enum
	{
		ITP_FULL_STYLE,
		ITP_POP_STYLE,
	};

public:
	TpFixScreen(const char *type = "tinyPiX_WM_Screen");
	virtual ~TpFixScreen();

public:
	virtual ItpObjectType objectType() final;

public:
	/// @brief 设置桌面颜色，该树形会下发至应用的appchanged
	/// @param alpha 
	/// @param color 
	/// @param screenAttr 
	/// @return 
	virtual int setVScreenAttribute(uint8_t alpha, uint32_t color, int screenAttr);

private:
	virtual void setRect(TpRect &rect) final;
	virtual void setRect(TpRect *rect) final;
	virtual void setRect(ItpRect &rect) final;
	virtual void setRect(ItpRect *rect) final;
	virtual void setRect(int x, int y, int w, int h) final;

private:
	virtual void setPosition(int x, int y) final;

private:
	virtual void setBeMoved(bool moved = false) final;
	virtual bool moved() final;

private:
	virtual void setAlpha(const uint8_t& alpha = 0xff) final;
	virtual uint8_t alpha() final;

public:
	virtual bool onActiveEvent(TpObjectActiveEvent *event);

private:
	ItpFixScreenData *data_;
};

#endif
