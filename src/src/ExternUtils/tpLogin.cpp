/*///------------------------------------------------------------------------------------------------------------------------//
		关机/重启/挂起
说 明 :	
日 期 : 2025.5.29

/*///------------------------------------------------------------------------------------------------------------------------//

#include "tpDbusConnectManage.h"
#include "tpdbus/logind.h"
#include "tpdbus/connect.h"
#include "tpLogin.h"


struct tpLoginData{
	Logind *login;
	tpLoginData()
	{
		login=NULL;
	}
};


tpLogin::tpLogin()
{
	data_ = new tpLoginData();
	tpLoginData* data = static_cast<tpLoginData*>(data_);

	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"connect to dbus error\n");
		return ;
	}
	data->login=logind_creat(system_conn);
	if(!data->login)
	{
		fprintf(stderr,"connect to dbus error\n");
		return ;
	}
}

tpLogin::~tpLogin()
{
	tpLoginData* data = static_cast<tpLoginData*>(data_);
	logind_delete(data->login);
	delete(data);
}

int tpLogin::powerOff()
{
	tpLoginData* data = static_cast<tpLoginData*>(data_);
	return logind_power_off(data->login);
}
int tpLogin::reboot()
{
	tpLoginData* data = static_cast<tpLoginData*>(data_);
	return logind_reboot(data->login);
}
int tpLogin::suspend()
{
	tpLoginData* data = static_cast<tpLoginData*>(data_);
	return logind_suspend(data->login);
}