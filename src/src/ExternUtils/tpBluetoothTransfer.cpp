/*///------------------------------------------------------------------------------------------------------------------------//
		文件传输客户端接口
说 明 : 
日 期 : 2025.4.29

/*///------------------------------------------------------------------------------------------------------------------------//

#include "tpBluetoothTransfer.h"
#include "tpDbusConnectManage.h"
#include "bluetooth/include/blt_file.h"
#include "bluetooth/include/blt_dbussignal.h"

struct tpBluetoothTransferData{
	BluetObexPush *obex_push;

	BluetDbusSignal *sig_obj;
	BluetDbusSignal *sig_pro;
	tpBluetoothTransferData(){
		obex_push=NULL;
	};
};

tpBluetoothTransfer::tpBluetoothTransfer()
{
	data_ = new tpBluetoothTransferData();
	tpBluetoothTransferData *data = static_cast<tpBluetoothTransferData *>(data_);
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
		return ;

	data->obex_push=bluet_obex_push_creat();

	bluet_obex_hash_creat();
	if(!data->obex_push)
	{
		
		return;
	}

}

tpBluetoothTransfer::~tpBluetoothTransfer()
{
	tpBluetoothTransferData *data = static_cast<tpBluetoothTransferData *>(data_);
	if(!data)
		return ;
	bluet_obex_signal_subscribe_delete(data->sig_pro);
	bluet_obex_signal_subscribe_delete(data->sig_obj);

	bluet_obex_hash_free();

	bluet_obex_push_delete(data->obex_push);
}


int tpBluetoothTransfer::sendFile(tpBluetoothAddress address,const tpString &file)
{
	return sendFile(address,file.c_str());
}

int tpBluetoothTransfer::sendFile(tpBluetoothAddress address,const char *file)
{
	tpBluetoothTransferData *data = static_cast<tpBluetoothTransferData *>(data_);
	if(!data)
		return -1;

	bluet_obex_push_session_creat(data->obex_push,address.toString().c_str());

	data->sig_pro=bluet_obex_push_signal_subscribe_properties(data->obex_push,NULL);
	data->sig_obj=bluet_obex_push_signal_subscribe_objext_manager(data->obex_push,NULL);

	bluet_obex_send_file(data->obex_push,file);
	return 0;
}


int tpBluetoothTransfer::getStatus()
{
	return 0;
}

int tpBluetoothTransfer::getProgress()
{
	return 0;
}