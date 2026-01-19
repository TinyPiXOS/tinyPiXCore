#ifndef __TP_LOGIN_H
#define __TP_LOGIN_H

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ITpLoginData);

class TpLogin
{
public:
    TpLogin();
    ~TpLogin();

public:
    /// @brief 关机
    /// @return
    int powerOff();
    /// @brief 重启
    /// @return
    int reboot();
    /// @brief 挂起
    /// @return
    int suspend();

private:
    ITpLoginData *data_;
};

#endif