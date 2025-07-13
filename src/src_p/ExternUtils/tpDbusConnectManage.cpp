//dbus连接管理的接口

#include "tpDbusConnectManage.h"
#include "tpdbus/connect.h"

struct tpDbusConnectManageData{
	void* connect;			//暂时没有使用
	tpBool is_connect;
	tpDbusConnectManageData(){
		connect=NULL;
		is_connect=TP_FALSE;
	};
};



tpDbusConnectManage& tpDbusConnectManage::instance()
{
	static tpDbusConnectManage dbus;
	return dbus;
}

tpDbusConnectManage::tpDbusConnectManage()
{
	data_ = new tpDbusConnectManageData;
	tpDbusConnectManageData *conData = static_cast<tpDbusConnectManageData *>(data_);
	dbus_connect_init();
	if(dbus_system_connect(NULL)!= TRUE)
	{
		fprintf(stderr,"[Error]:system dbus connect error\n");
		return ;
	}

	/*if(dbus_session_connect(NULL) != TRUE)
	{
		fprintf(stderr,"[Error]:session dbus connect error\n");
		dbus_system_disconnect();
		return ;
	}*/
	printf("connect ok");
	conData->is_connect=TP_TRUE;
}

tpDbusConnectManage::~tpDbusConnectManage()
{
	tpDbusConnectManageData *conData = static_cast<tpDbusConnectManageData *>(data_);
	if(!conData)
		return ;
	disConnection();
	delete(conData);
}


tpBool tpDbusConnectManage::isConnect()
{
	tpDbusConnectManageData *conData = static_cast<tpDbusConnectManageData *>(data_);
	return conData->is_connect;
}

tpBool tpDbusConnectManage::connection() 
{
	tpDbusConnectManageData *conData = static_cast<tpDbusConnectManageData *>(data_);
    return conData->is_connect;
}

void tpDbusConnectManage::disConnection()
{
	tpDbusConnectManageData *conData = static_cast<tpDbusConnectManageData *>(data_);
	dbus_disconnect();
	conData->connect=NULL;
	conData->is_connect=TP_FALSE;
}