#ifndef __TP_MAIN_WINDOW_H
#define __TP_MAIN_WINDOW_H

#include "TpScreen.h"

TP_DEF_VOID_TYPE_VAR(ITpMainWindowData);
class TpMainWindow
    : public TpScreen
{
public:
    TpMainWindow(const char *type = "tinyPiX_WM_Screen");
    virtual ~TpMainWindow();

public:
    virtual Tp::ItpObjectType objectType() final;

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
    ITpMainWindowData *data_;
};

#endif
