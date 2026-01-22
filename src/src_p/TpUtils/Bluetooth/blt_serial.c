/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙SPP协议相关
说 明 : 
日 期 : 2025.8.6

/*///------------------------------------------------------------------------------------------------------------------------//

#include "blt_serial.h"
#include "bluetooth_inc.h"
#include "blt_dbussignal.h"













int bluet_serial_()
{

g_dbus_connection_signal_subscribe(
        system_conn,
        "org.bluez",
        BLUEZ_DBUS_INTERFACE_PROPERTIES,
        "PropertiesChanged",
        NULL,
        NULL,
        G_DBUS_SIGNAL_FLAGS_NONE,
        on_device_found,
        NULL,
        NULL
    );

 g_dbus_connection_signal_subscribe(
        system_conn,
        "org.bluez",
        NULL,
        "NewConnection",
        NULL,
        NULL,
        G_DBUS_SIGNAL_FLAGS_NONE,
        on_new_connection,
        NULL,
        NULL
    );

}


