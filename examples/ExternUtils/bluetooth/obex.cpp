//obex发送/接收测试程序
#include <iostream>
#include "tpBluetoothLocal.h"
#include "tpBluetoothDiscovery.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothTransfer.h"
#include "tpBluetoothTransferAgent.h"

int example_obex_send()
{
	tpBluetoothTransfer obex;
	obex.sendFile(tpBluetoothAddress(tpString("6C:D1:99:69:BF:F0")),"/home/jiyuchao/桌面/phone.wav");
	return 0;
}

int example_obex_agent()
{
	tpBluetoothTransferAgent agent;
	while(1);
}

int main()
{
//	example_obex_send();

	example_obex_agent();
}