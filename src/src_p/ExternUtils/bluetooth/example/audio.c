//音频播放测试程序
//"41:42:AE:49:83:B9"
#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blt_device.h"
#include "lib/bluez/device.h"

#include "lib/bluez_alsa.h"
#include "lib/freedesktop_systemd.h"
#include "blt_audio.h"

int main()
{
	GError *error = NULL;
	dbus_connect_init();

	if (!dbus_system_connect(&error))
    {
        g_printerr("Couldn't connect to DBus system bus: %s\n", error->message);
        exit(EXIT_FAILURE);
    }
	printf("dbus_system_connect ok\n");

	if (!dbus_system_connect(&error))
    {
        g_printerr("Couldn't connect to DBus system bus: %s\n", error->message);
        exit(EXIT_FAILURE);
    }
	printf("dbus_system_connect ok\n");

	DesktopSystem *system=desktop_system_creat();
	if(bluet_audio_blue_alsa_is_runing(system,&error)==0)
	{
		printf("启动bluez-alsa服务\n");
		bluet_audio_start_blue_alsa(system,NULL);
	}

	
	BluezAlsa *alsa=bluez_alsa_creat();
	bluez_alsa_get_adapters(alsa,NULL);
//	bluez_alsa_create_gdbus_proxy(alsa,NULL);
//	GDBusConnection *system_bus = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
	Adapter *adapter = find_adapter(NULL, &error);
	BluetDevice *device = bluet_device_creat(adapter, "41:42:AE:49:83:B9");
	Device *dev=bluet_device_get_device(device);
	device_set_trusted(dev,1,&error);
//	bluet_connect_remote_device(device,NULL);
	
	bluet_device_pair_with_remote(device,0);
	sleep(20);
	bluez_alsa_delete(alsa);
	desktop_system_delete(system);
	bluet_device_delete(device);
}