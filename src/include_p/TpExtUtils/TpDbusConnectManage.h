#ifndef __TP_DBUS_CONNECT_MANAGE_H
#define __TP_DBUS_CONNECT_MANAGE_H

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ItpDbusConnectManageData);

class TpDbusConnectManage{
public:
	static TpDbusConnectManage& instance();
	tpBool connection();
	tpBool isConnect();
	void disConnection();
private:
	TpDbusConnectManage();
	~TpDbusConnectManage();
	ItpDbusConnectManageData *data_;
};





#endif
