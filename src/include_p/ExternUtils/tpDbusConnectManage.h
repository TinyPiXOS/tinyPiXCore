#ifndef __TP_DBUS_CONNECT_MANAGE_H
#define __TP_DBUS_CONNECT_MANAGE_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpDbusConnectManageData);

class tpDbusConnectManage{
public:
	static tpDbusConnectManage& instance();
	tpBool connection();
	tpBool isConnect();
	void disConnection();
private:
	tpDbusConnectManage();
	~tpDbusConnectManage();
	ItpDbusConnectManageData *data_;
};





#endif
