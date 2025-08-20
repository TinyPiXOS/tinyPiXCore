#include "tpBluetoothServiceDiscovery.h"

int main()
{
	tpBluetoothServiceDiscovery scan(tpBluetoothAddress("00:11:22:33:44:55"));
	scan.start();
	return 0;
}