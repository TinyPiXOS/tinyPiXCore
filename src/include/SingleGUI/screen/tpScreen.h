
#ifndef __TP_VSCREEN_H
#define __TP_VSCREEN_H

#include "tpChildWidget.h"

class tpScreen
	: public tpChildWidget
{
public:
	tpScreen(const char *type = "tinyPiX_WM_Screen", int32_t x = 0, int32_t y = 0, uint32_t w = 0, uint32_t h = 0);
	virtual ~tpScreen();

public:
	virtual void setVisible(bool visible = true) override;

	virtual bool actived();

public:
	virtual void setText(const char *text = nullptr);
	virtual void setText(const tpString &text);

public:
	virtual void setRotateEnable(bool enabled = false);
	virtual bool rotateEnable();
	virtual void setRotate(ItpRotateType type);
	virtual ItpRotateType rotate();

public:
	virtual void setRect(const int32_t &x, const int32_t &y, const uint32_t &w, const uint32_t &h) override;

	/// @brief 设置窗口宽高
	/// @param width
	/// @param height
	virtual void setSize(const uint32_t &width, const uint32_t &height) override;

	/// @brief 设置窗口宽度
	/// @param width 宽度值，单位px
	virtual void setWidth(const uint32_t &width) override;

	/// @brief 设置窗口高度
	/// @param height 高度值，单位px
	virtual void setHeight(const uint32_t &height) override;

	virtual void move(int32_t x, int32_t y) override;

public:
	virtual void setBeMoved(bool moved = false);
	virtual bool moved();

public:
	virtual void setAlpha(const uint8_t &alpha = 0xff) override;

public:
	virtual void setColorKey(bool enable = false, uint32_t colorKey = 0);
	virtual void setColorKeyEnable(bool enable = false);

public:
	virtual void bringToTop();
	virtual void bringToBottom();

public:
	virtual void update(tpRect &rect, bool clip = true, bool onlyBlit = false, bool sync = false);								 // must override
	virtual void update(ItpRect &rect, bool clip = true, bool onlyBlit = false, bool sync = false);								 // must override
	virtual void update(ItpRect *rect, bool clip = true, bool onlyBlit = false, bool sync = false);								 // must override
	virtual void update(int32_t x, int32_t y, int32_t w, int32_t h, bool clip = true, bool onlyBlit = false, bool sync = false); // must override
	virtual void update(bool clip = true, bool onlyBlit = false, bool sync = false);											 // must override
	virtual void syncUpdate(bool clip = true, bool onlyBlit = false);

public:
	virtual tpList<tpObject *> &objectList();
	virtual IPitpObject *objectSets();

public:
	virtual ItpObjectType objectType();
	virtual ItpObjectSysLayer objectLayer();
	virtual int32_t objectSysID();
	virtual bool objectActive();

public:
	virtual void setParent(tpObject *parent);
	virtual tpObject *parent();

public:
	virtual tpObject *topObject();

	/// @brief
	virtual void deleteLater() override;

	virtual bool returns();

public:
	virtual ItpSize screenSize();
	virtual int32_t screenWidth();
	virtual int32_t screenHeight();

public:
	/// @brief 外部禁止调用
	/// @param event
	/// @return
	int32_t dispatchEvent(void *event);
};

#endif
