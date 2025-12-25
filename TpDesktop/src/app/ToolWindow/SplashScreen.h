#ifndef __SPLASH_SCREEN_H
#define __SPLASH_SCREEN_H

#include "TpDialog.h"
#include "TpDateTime.h"
#include "TpTimer.h"
#include "TpAnimation.h"

class SplashScreen : public TpDialog
{
public:
    SplashScreen();
    virtual ~SplashScreen();

    /// @brief 动画显示开屏
    /// @param appRect 从应用图标位置作为动画起始位置
    void showSplashScreen(const TpRect &appRect);

    /// @brief 设置开屏背景图；未设置则纯色背景
    /// @param image 背景图
    void setScreenImage(const TpImage &image);
    /// @brief 设置开屏背景色；未设置则默认白色背景
    /// @param color
    void setScreenColor(const int32_t &color);

    virtual void setVisible(bool visible = true) override;

private:
    TpAnimation *scaleAnim_;
    TpTimer *waitTimer_;

    // 开屏窗口显示时间
    int64_t startTimeS_;
};

#endif
