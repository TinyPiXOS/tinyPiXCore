#ifndef __TP_FIXSCREEN_H
#define __TP_FIXSCREEN_H

#include "TpScreen.h"

TP_DEF_VOID_TYPE_VAR(ITpFixScreenData);
/// @brief 物理屏幕
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
    virtual Tp::TpObjectType objectType() final override;

    virtual int setVScreenAttribute(uint8_t alpha, uint32_t color, int32_t screenAttr);

protected:
    virtual bool onActiveEvent(TpActiveEvent *event) override;

private:
    virtual void setRect(const TpRect &rect) final override {};
    virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h) final override {};

private:
    virtual void setBeMoved(bool moved = false) final override {};
    virtual bool moved() final override { return false; };

private:
    virtual void setWindowOpacity(float opacity) final override {};
    virtual float windowOpacity() final override { return 0xff; };

private:
    ITpFixScreenData *data_;
};

#endif
