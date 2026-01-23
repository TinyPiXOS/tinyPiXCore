/*///------------------------------------------------------------------------------------------------------------------------//
		dbus连接管理的接口
说 明 : 用于管理dbus连接
日 期 : 2025

/*///------------------------------------------------------------------------------------------------------------------------//
#include "TpDbusConnectManage.h"
#include "Dbus/connect.h"

struct TpDbusConnectManageData
{
    void *connect; // 暂时没有使用
    tpBool is_connect;
    TpDbusConnectManageData()
    {
        connect = NULL;
        is_connect = TP_FALSE;
    };
};

TpDbusConnectManage &TpDbusConnectManage::instance()
{
    static TpDbusConnectManage dbus;
    return dbus;
}

TpDbusConnectManage::TpDbusConnectManage()
{
    data_ = new TpDbusConnectManageData;
    TpDbusConnectManageData *conData = static_cast<TpDbusConnectManageData *>(data_);
    dbus_connect_init();
    if (dbus_system_connect(NULL) != TRUE)
    {
        fprintf(stderr, "[Error]:system dbus connect error\n");
        return;
    }

    /*if(dbus_session_connect(NULL) != TRUE)
    {
        fprintf(stderr,"[Error]:session dbus connect error\n");
        dbus_system_disconnect();
        return ;
    }*/
    printf("connect ok\n");
    conData->is_connect = TP_TRUE;
}

TpDbusConnectManage::~TpDbusConnectManage()
{
    TpDbusConnectManageData *conData = static_cast<TpDbusConnectManageData *>(data_);
    if (!conData)
        return;
    disConnection();
    delete (conData);
}

tpBool TpDbusConnectManage::isConnect()
{
    TpDbusConnectManageData *conData = static_cast<TpDbusConnectManageData *>(data_);
    return conData->is_connect;
}

tpBool TpDbusConnectManage::connection()
{
    TpDbusConnectManageData *conData = static_cast<TpDbusConnectManageData *>(data_);
    return conData->is_connect;
}

void TpDbusConnectManage::disConnection()
{
    TpDbusConnectManageData *conData = static_cast<TpDbusConnectManageData *>(data_);
    dbus_disconnect();
    conData->connect = NULL;
    conData->is_connect = TP_FALSE;
}