#ifndef __TP_LOTTIR_ANIMATION_H
#define __TP_LOTTIR_ANIMATION_H

#include "TpChildWidget.h"
#include "TpString.h"

TP_DEF_VOID_TYPE_VAR(ITpLottieAnimationData);
class TpLottieAnimation : public TpChildWidget
{
public:
    TpLottieAnimation(TpChildWidget *parent = nullptr);
    TpLottieAnimation(const TpString &lottieFile, TpChildWidget *parent = nullptr);
    ~TpLottieAnimation();

    /// @brief 加载lottie动画
    /// @param lottieFile 动画文件绝对路径
    /// @return 加载成功返回true，否则返回false
    bool load(const TpString &lottieFile);

    /// @brief 设置窗口显隐
    /// @param visible true显示，false隐藏
    virtual void setVisible(bool visible = true) override;

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;

private:
    ITpLottieAnimationData *data_;
};

#endif