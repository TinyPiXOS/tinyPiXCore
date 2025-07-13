#ifndef __TP_LOGIN_H
#define __TP_LOGIN_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpLoginData);

class tpLogin
{
public:
	tpLogin();
	~tpLogin();
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
	ItpLoginData *data_;
};



#endif