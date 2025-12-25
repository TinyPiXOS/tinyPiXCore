#ifndef __INSTALL_BUTTON_H
#define __INSTALL_BUTTON_H

/*
    用于显示APP安装进度
*/

#include "TpVector.h"
#include "TpIconTopButton.h"

class HollowWidget;
class DesktopAppButton : public TpIconTopButton
{
public:
    DesktopAppButton(TpWidget *parent = nullptr);

    DesktopAppButton(const TpString &iconPath, const TpString &text, TpWidget *parent);

    virtual ~DesktopAppButton();

    /// @brief 设置边框圆角
    /// @param roundPercent px
    virtual void setRoundCorners(const uint32_t &round) override;

    /// @brief 设置安装进度
    /// @param progress 0-100
    void setInstallProgress(const uint32_t &progress);

protected:
    virtual bool onResizeEvent(TpResizeEvent *event) override;

private:
    HollowWidget *hollowWidget_;
};

class HollowWidget : public TpWidget
{
public:
    HollowWidget(TpWidget *parent = nullptr);
    ~HollowWidget();

    /// @brief 设置安装进度
    /// @param progress 0-100
    void setInstallProgress(const uint32_t &progress);

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;

private:
    uint32_t installProgress_;
};

#endif