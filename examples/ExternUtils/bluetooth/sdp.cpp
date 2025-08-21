#include <iostream>
#include <stdio.h>
#include "tpBluetoothServiceDiscovery.h"
#include "tpBluetoothService.h"


int example_service_registe()
{
	tpBluetoothService service_serial;
	service_serial.setServiceChannel(1);
	service_serial.setServiceUuid(tpBluetoothUuid(tpBluetoothUuid::TP_PROFILE_SERIAL_PORT));
	service_serial.setServiceName(tpString("Serial Port Profile"));
	tpBluetoothUuid uuid(tpBluetoothUuid::TP_PROFILE_SERIAL_PORT);

	service_serial.registerService();
	return 0;
}


int example_service_scan()
{
	tpBluetoothServiceDiscovery scan(tpBluetoothAddress("00:11:22:33:44:55"));
	scan.start();
	return 0;
}

int main()
{
//	example_service_scan();
	example_service_registe();
	return 0;
}