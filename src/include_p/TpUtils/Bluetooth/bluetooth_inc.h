#ifndef BLUETOOTH_INC_H
#define BLUETOOTH_INC_H

#define BLUEZ_DBUS_SERVICE_NAME "org.bluez"
#define BLUEZ_OBEX_DBUS_SERVICE_NAME "org.bluez.obex"
#define BLUEZ_DBUS_BASE_PATH "/org/bluez"
#define BLUEZ_OBEX_DBUS_BASE_PATH "/org/bluez/obex"

// DBUS的接口定义(仅定义了蓝牙需要使用的)
#define BLUEZ_DBUS_INTERFACE_OBJECT_MANAGER "org.freedesktop.DBus.ObjectManager"
#define BLUEZ_DBUS_INTERFACE_PROPERTIES "org.freedesktop.DBus.Properties"

#include "Dbus/properties.h"
#include "Dbus/manager.h"
#include "Dbus/connect.h"
#include "tools/utlist.h"
#include "tools/variable_array.h"
#include "bluez/Adapter.h"
#include "bluez/device.h"
#include "bluez/AgentManager.h"
#include "bluez/Profile.h"
#include "GUtils.h"
#include "obex/obex_object_push.h"
#include "obex/obex_client.h"
#include "obex/obex_session.h"
#include "obex/ObexAgentManager.h"
#include "obex/obex_agent.h"
#include "BluezAlsa.h"
#include "FreedesktopSystemd.h"
#include "service.h"
#include "spp/spp_client.h"
/*


#include "bluez/alert.h"
#include "bluez/alert_agent.h"
#include "bluez/cycling_speed.h"
#include "bluez/cycling_speed_manager.h"

#include "bluez/health_channel.h"
#include "bluez/health_device.h"
#include "bluez/health_manager.h"
#include "bluez/heart_rate.h"
#include "bluez/heart_rate_manager.h"
#include "bluez/media.h"
#include "bluez/media_control.h"
#include "bluez/media_player.h"
#include "bluez/network.h"
#include "bluez/network_server.h"
#include "bluez/obex/obex_agent_manager.h"
#include "bluez/obex/obex_client.h"
#include "bluez/obex/obex_file_transfer.h"
#include "bluez/obex/obex_message.h"
#include "bluez/obex/obex_message_access.h"
#include "bluez/obex/obex_object_push.h"
#include "bluez/obex/obex_phonebook_access.h"
#include "bluez/obex/obex_session.h"
#include "bluez/obex/obex_synchronization.h"
#include "bluez/obex/obex_transfer.h"
#include "bluez/profile_manager.h"
#include "bluez/proximity_monitor.h"
#include "bluez/proximity_reporter.h"
#include "bluez/sim_access.h"
#include "bluez/thermometer.h"
#include "bluez/thermometer_manager.h"
    */

#endif /* BLUEZ_H */
