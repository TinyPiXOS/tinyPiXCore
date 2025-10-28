
#ifndef __TP_VSCREEN_H
#define __TP_VSCREEN_H

#include "TpWidget.h"

class TpScreen
    : public TpWidget
{
public:
    TpScreen(const char *type = "tinyPiX_WM_Screen", int32_t x = 0, int32_t y = 0, uint32_t w = 0, uint32_t h = 0);
    virtual ~TpScreen();

public:
    virtual void setVisible(bool visible = true) override;

    virtual bool actived();

public:
    virtual void setText(const char *text = nullptr);
    virtual void setText(const TpString &text);

public:
    virtual void setRect(const TpRect &rect) override;
    virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h) override;

    /// @brief 设置窗口宽高
    /// @param width
    /// @param height
    virtual void setSize(const int32_t &width, const int32_t &height) override;

    /// @brief 设置窗口宽度
    /// @param width 宽度值，单位px
    virtual void setWidth(const int32_t &width) override;

    /// @brief 设置窗口高度
    /// @param height 高度值，单位px
    virtual void setHeight(const int32_t &height) override;

    virtual void move(int32_t x, int32_t y) override;

    /// @brief 获取窗口当前坐标
    /// @return 返回窗口当前坐标
    virtual const TpPoint pos() override;

public:
    virtual void setBeMoved(bool moved = false);
    virtual bool moved();

public:
    virtual void setAlpha(const uint8_t &alpha = 0xff) override;

public:
    virtual void bringToTop();
    virtual void bringToBottom();

public:
    virtual void update(int32_t x, int32_t y, int32_t w, int32_t h, bool onlyBlit = false) override;
    virtual void update(bool onlyBlit = false) override;

public:
    virtual Tp::TpObjectType objectType();
    virtual Tp::TpObjectSysLayer objectLayer();
    virtual int32_t objectSysID();
    virtual bool objectActive();

public:
    virtual void setParent(TpObject *parent);
    virtual TpObject *parent();

public:
    virtual TpObject *topObject();

    /// @brief
    virtual void deleteLater() override;

    virtual bool returns();

public:
    virtual TpSize screenSize();
    virtual int32_t screenWidth();
    virtual int32_t screenHeight();

public:
    /// @brief 外部禁止调用
    /// @param event
    /// @return
    int32_t dispatchEvent(void *event);
};

#endif
