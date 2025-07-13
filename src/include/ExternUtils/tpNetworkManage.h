#ifndef __TP_NETWORK_MANAGE_H
#define __TP_NETWORK_MANAGE_H

#include "tpUtils.h"
#include "tpNetworkInfo.h"

TP_DEF_VOID_TYPE_VAR(ItpNetworkManageData);


class tpNetworkManage
{
public:
	tpNetworkManage();
	~tpNetworkManage();
public:
	tpList<tpNetworkInfo *> getList();//获取设备列表

private:
	ItpNetworkManageData *data_;
};




















#endif
