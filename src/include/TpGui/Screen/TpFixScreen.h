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
    virtual tinyPiX::ItpObjectType objectType() final;

public:
    /// @brief 设置桌面颜色，该树形会下发至应用的appchanged
    /// @param alpha
    /// @param color
    /// @param screenAttr
    /// @return
    virtual int setVScreenAttribute(uint8_t alpha, uint32_t color, int32_t screenAttr);

private:
    virtual void setRect(const TpRect &rect) final {};
    virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h) final {};

private:
    virtual void setBeMoved(bool moved = false) final {};
    virtual bool moved() final { return false; };

private:
    virtual void setAlpha(const uint8_t &alpha = 0xff) final {};
    virtual uint8_t alpha() final { return 0xff; };

public:
    virtual bool onActiveEvent(TpActiveEvent *event);

private:
    ItpFixScreenData *data_;
};

#endif
