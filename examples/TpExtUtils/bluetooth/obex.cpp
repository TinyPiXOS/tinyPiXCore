//obex发送/接收测试程序
#include <iostream>
#include "TpBluetoothLocal.h"
#include "TpBluetoothDiscovery.h"
#include "TpBluetoothDevice.h"
#include "TpBluetoothTransfer.h"
#include "TpBluetoothTransferAgent.h"

int example_obex_send()
{
	TpBluetoothTransfer obex;
	obex.sendFile(TpBluetoothAddress(TpString("6C:D1:99:69:BF:F0")),"/home/jiyuchao/桌面/phone.wav");
	return 0;
}

int example_obex_agent()
{
	TpBluetoothTransferAgent agent;
	while(1);
}

int main()
{
//	example_obex_send();

	example_obex_agent();
}