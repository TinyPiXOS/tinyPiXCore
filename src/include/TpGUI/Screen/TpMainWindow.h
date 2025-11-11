#ifndef __TP_MAIN_WINDOW_H
#define __TP_MAIN_WINDOW_H

#include "TpScreen.h"

TP_DEF_VOID_TYPE_VAR(ITpMainWindowData);
/// @brief 应用主窗体，每个应用只能拥有一个TpMainWindow
class TpMainWindow
    : public TpScreen
{
public:
    TpMainWindow(const char *type = "tinyPiX_USE_Float");
    virtual ~TpMainWindow();

public:
    virtual Tp::TpObjectType objectType() final override;

    /// @brief 组件类名，子类实现，返回子类类名字符串，用于匹配CSS中对应样式
    /// @return 类名字符串
    virtual TpString pluginType() override { return TO_STRING(TpMainWindow); }

    virtual void setBackGroundColor(const TpColors &color, bool enable = true) override;
    virtual void setBackGroundColor(int32_t color, bool enable = true) override;
    virtual void setBackGroundColor(const TpBrush &bgBrush, bool enable = true) override;
    virtual void setEnableBackGroundColor(bool enable) override;

    virtual void setBorderColor(const TpColors &color, bool enable = true) override;
    virtual void setBorderColor(int32_t color, bool enable = true) override;
    virtual void setBorderColor(const TpBrush &borderBrush, bool enable = true) override;
    virtual void setEnabledBorderColor(bool enable) override;

protected:
    /// @brief TpMainWindow无resize事件
    virtual bool onResizeEvent(TpResizeEvent *event) final override { return true; };

private:
    virtual void setVisible(bool visible = true) final override {};

    virtual void setRect(const TpRect &rect) final override {};
    virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h) final override {};

private:
    virtual void setBeMoved(bool moved = false) final override {};
    virtual bool moved() final override { return false; };

private:
    virtual void setWindowOpacity(float opacity) final override {};
    virtual float windowOpacity() final override { return 1.0; };
};

#endif
