#include <iostream>
#include <stdio.h>
#include "tpBluetoothServiceDiscovery.h"
#include "tpBluetoothService.h"
#include "tpBluetoothUuid.h"

int example_service_registe()
{
	tpBluetoothService service_serial;
	service_serial.setServiceChannel(1);
	service_serial.setServiceUuid(tpBluetoothUuid(tpBluetoothUuid::TP_PROFILE_SERIAL_PORT));
	service_serial.setServiceName(tpString("Serial Port Profile"));
	tpBluetoothUuid uuid(tpBluetoothUuid::TP_PROFILE_SERIAL_PORT);

	service_serial.registerService();
	while(1);
	return 0;
}


int example_service_scan()
{
	tpBluetoothServiceDiscovery scan(tpBluetoothAddress("00:11:22:33:44:55"));
	scan.start();
	return 0;
}

int example_bluet_uuid()
{
	tpUInt16 num_other=0x1205;
	tpUInt16 num_16=0x1101;
	tpUInt32 num_32=0x00001101;
	printf("tpUInt16\n");
	tpBluetoothUuid uuid_16(num_16);
	printf("tpUInt32\n");
	tpBluetoothUuid uuid_32(num_32);
	printf("tpString\n");
	tpBluetoothUuid uuid_str(tpString("00001101-0000-1000-8000-00805F9B34FB"));
	printf("ok\n");

	printf("uuid_32:%08x\n",uuid_str.toUInt32());
	printf("uuid_16:%04x\n",uuid_str.toUInt16());
	printf("uuid128:%s\n",uuid_str.toString().c_str());

	if(uuid_16==uuid_32)
		printf("uuid_16==uuid_32\n");
	if(uuid_16==uuid_str)
		printf("uuid_16==uuid_str\n");
	
	uuid_str=num_other;
	printf("uuid_32:%08x\n",uuid_str.toUInt32());
	printf("uuid_16:%04x\n",uuid_str.toUInt16());
	printf("uuid128:%s\n",uuid_str.toString().c_str());
}

int main()
{

//	example_service_scan();
//	example_service_registe();
	example_bluet_uuid();
	return 0;
}