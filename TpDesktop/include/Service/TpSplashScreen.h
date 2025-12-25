#ifndef __TP_SPLASH_SCREEN_H
#define __TP_SPLASH_SCREEN_H

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ITpSplashScreenData);

class TpSplashScreen
{
public:
    /// @brief 获取唯一单例
    /// @return 实例指针
    static TpSplashScreen *Instance();

public:
    /// @brief 应用启动完成后应在onVisibleEvent中手动关闭开屏窗口
    void closeSplashScreen();

public:
    /// @brief 禁用拷贝构造
    TpSplashScreen(const TpSplashScreen &) = delete;
    /// @brief 禁用赋值构造
    TpSplashScreen &operator=(const TpSplashScreen &) = delete;

private:
    TpSplashScreen();
    virtual ~TpSplashScreen();

private:
    ITpSplashScreenData *data_;
};

#endif
