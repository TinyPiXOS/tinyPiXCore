#ifndef __TP_VFIX_SCREEN_H
#define __TP_VFIX_SCREEN_H

#include "tpScreen.h"

TP_DEF_VOID_TYPE_VAR(ItpFixScreenData);
class tpFixScreen
	: public tpScreen
{
public:
	enum
	{
		ITP_FULL_STYLE,
		ITP_POP_STYLE,
	};

public:
	tpFixScreen(const char *type = "tinyPiX_WM_Screen");
	virtual ~tpFixScreen();

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
	virtual void setRect(tpRect &rect) final;
	virtual void setRect(tpRect *rect) final;
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

private:
	virtual void setColorKey(bool enable = false, uint32_t colorKey = 0) final;
	virtual bool colorKeyEnable() final;
	virtual uint32_t colorKey() final;

private:
	virtual void setVarShape(void *shape) final;
	virtual void *varShape() final;

public:
	virtual bool onRotateEvent(tpObjectRotateEvent *event);
	virtual bool onActiveEvent(tpObjectActiveEvent *event);

private:
	ItpFixScreenData *data_;
};

#endif
