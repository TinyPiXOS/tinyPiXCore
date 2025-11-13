/*///------------------------------------------------------------------------------------------------------------------------//
        文件传输服务端代理接口
说 明 : 使用注意：在ubuntu上运行需要先使用sudo pkill -9 obexd杀掉当前系统已经注册的obex服务，然后使用sudo /usr/lib/bluetooth/obexd -n -d
        运行安装的obexd
日 期 : 2025.4.29

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include "TpBluetoothTransferAgent.h"
#include "TpDbusConnectManage.h"
#include "include/blt_file.h"
#include "include/blt_dbussignal.h"

struct TpBluetoothTransferAgentData
{
    BluetObexAgent *obex_agent;
    BluetDbusSignal *sig_obj;
    BluetDbusSignal *sig_pro;
    TpBluetoothTransferAgentData()
    {
        obex_agent = NULL;
        sig_obj = NULL;
        sig_pro = NULL;
    };
};

TpBluetoothTransferAgent::TpBluetoothTransferAgent()
{
    data_ = new TpBluetoothTransferAgentData();
    TpBluetoothTransferAgentData *data = static_cast<TpBluetoothTransferAgentData *>(data_);
    if (TpDbusConnectManage::instance().connection() != TP_TRUE)
        return;

    data->obex_agent = bluet_obex_agent_creat(NULL);
    if (!data->obex_agent)
    {

        return;
    }
    data->sig_obj = bluet_obex_agent_signal_subscribe_objext_manager(NULL, NULL);
    data->sig_pro = bluet_obex_agent_signal_subscribe_properties(NULL, NULL);
}

TpBluetoothTransferAgent::~TpBluetoothTransferAgent()
{
    TpBluetoothTransferAgentData *data = static_cast<TpBluetoothTransferAgentData *>(data_);
    if (!data)
        return;
    bluet_obex_signal_subscribe_delete(data->sig_obj);
    bluet_obex_signal_subscribe_delete(data->sig_pro);
    bluet_obex_agent_delete(data->obex_agent);
    delete (data);
}

int TpBluetoothTransferAgent::startServer()
{
    return 0;
}

int TpBluetoothTransferAgent::stopServer()
{
    return 0;
}

void TpBluetoothTransferAgent::setSaveDirectory(const TpString &directory)
{
    return;
}
