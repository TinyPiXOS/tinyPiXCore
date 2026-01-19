/*///------------------------------------------------------------------------------------------------------------------------//
		关机/重启/挂起
说 明 :	
日 期 : 2025.5.29

/*///------------------------------------------------------------------------------------------------------------------------//

#include "TpDbusConnectManage.h"
#include "Dbus/logind.h"
#include "Dbus/connect.h"
#include "TpLogin.h"


struct TpLoginData{
	Logind *login;
	TpLoginData()
	{
		login=NULL;
	}
};


TpLogin::TpLogin()
{
	data_ = new TpLoginData();
	TpLoginData* data = static_cast<TpLoginData*>(data_);

	if(TpDbusConnectManage::instance().connection()!=TP_TRUE)
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

TpLogin::~TpLogin()
{
	TpLoginData* data = static_cast<TpLoginData*>(data_);
	logind_delete(data->login);
	delete(data);
}

int TpLogin::powerOff()
{
	TpLoginData* data = static_cast<TpLoginData*>(data_);
	return logind_power_off(data->login);
}
int TpLogin::reboot()
{
	TpLoginData* data = static_cast<TpLoginData*>(data_);
	return logind_reboot(data->login);
}
int TpLogin::suspend()
{
	TpLoginData* data = static_cast<TpLoginData*>(data_);
	return logind_suspend(data->login);
}