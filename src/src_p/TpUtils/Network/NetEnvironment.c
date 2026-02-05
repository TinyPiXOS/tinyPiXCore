/*///------------------------------------------------------------------------------------------------------------------------//
		网络环境检查
说 明 : 根据系统环境检查当前系统支持的网络配置工具
日 期 : 2026.2.2

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Network/NetEnvironment.h"

// 检查命令是否存在
static bool command_exists(const char *cmd) {
    char test_cmd[256];
    snprintf(test_cmd, sizeof(test_cmd), "command -v %s > /dev/null 2>&1", cmd);
    return system(test_cmd) == 0;
}

// 检查文件是否存在
static bool file_exists(const char *path) {
    return access(path, F_OK) == 0;
}

// 检查服务是否运行
static bool service_is_active(const char *service) {
    char cmd[256];
    
    // 检查systemd服务
    snprintf(cmd, sizeof(cmd), "systemctl is-active --quiet %s 2>/dev/null", service);
    if (system(cmd) == 0) return true;
    
    // 检查init.d服务
    snprintf(cmd, sizeof(cmd), "/etc/init.d/%s status 2>/dev/null | grep -q running", service);
    if (system(cmd) == 0) return true;
    
    return false;
}

// 检测DBus服务
static bool dbus_service_exists(const char *service) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), 
        "dbus-send --system --dest=org.freedesktop.DBus "
        "--type=method_call --print-reply /org/freedesktop/DBus "
        "org.freedesktop.DBus.NameHasOwner string:%s "
        "2>/dev/null | grep -q 'true'", service);
    return system(cmd) == 0;
}

// 检测所有网络工具
network_tools_t net_environment_detect_network_tools(void) {
    network_tools_t tools = {0};
    
    // 网络管理器检测
/*    tools.has_network_manager = (command_exists("nmcli") || 
                                dbus_service_exists("org.freedesktop.NetworkManager"));
    
    tools.has_connman = (command_exists("connmanctl") ||
                        dbus_service_exists("net.connman"));*/
    
    // DHCP客户端检测
    tools.has_dhcpcd = (command_exists("dhcpcd") ||
                       file_exists("/sbin/dhcpcd") ||
                       service_is_active("dhcpcd"));
    
    tools.has_dhclient = (command_exists("dhclient") ||
                         file_exists("/sbin/dhclient"));
    
    tools.has_systemd_networkd = (command_exists("networkctl") ||
                                 service_is_active("systemd-networkd"));
    
    // WiFi客户端检测
    tools.has_iwd = (command_exists("iwctl") ||
                    dbus_service_exists("net.connman.iwd") ||
                    service_is_active("iwd"));
    
    tools.has_wpa_supplicant = (command_exists("wpa_supplicant") ||
                               command_exists("wpa_cli") ||
                               service_is_active("wpa_supplicant"));
    
    // 热点和DHCP服务器检测
    tools.has_hostapd = (command_exists("hostapd") ||
                        file_exists("/usr/sbin/hostapd"));
    
    tools.has_dnsmasq = (command_exists("dnsmasq") ||
                        file_exists("/usr/sbin/dnsmasq"));
    
    // DNS管理器检测
    tools.has_systemd_resolved = (command_exists("resolvectl") ||
                                 service_is_active("systemd-resolved"));
    
    tools.has_resolvconf = command_exists("resolvconf");
    
    // 命令行工具检测
    tools.has_iproute2 = command_exists("ip");
    tools.has_iw = command_exists("iw");
    tools.has_iwconfig = command_exists("iwconfig");
    
    return tools;
}

