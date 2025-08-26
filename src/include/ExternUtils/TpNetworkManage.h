#ifndef __TP_NETWORK_MANAGE_H
#define __TP_NETWORK_MANAGE_H

#include "TpUtils.h"
#include "TpNetworkInfo.h"

TP_DEF_VOID_TYPE_VAR(ItpNetworkManageData);


class TpNetworkManage
{
public:
	TpNetworkManage();
	~TpNetworkManage();
public:
	TpList<TpNetworkInfo *> getList();//获取设备列表

private:
	ItpNetworkManageData *data_;
};




















#endif
