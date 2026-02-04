#ifndef _NET_ENVIRONMENT_H_
#define _NET_ENVIRONMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// 网络工具可用性结构
typedef struct {
	// 网络管理器
	bool has_network_manager;   // NetworkManager
	bool has_connman;           // ConnMan

	// DHCP客户端
	bool has_dhcpcd;           // dhcpcd
	bool has_dhclient;         // dhclient
	bool has_systemd_networkd; // systemd-networkd (可做DHCP客户端)

	// WiFi客户端
	bool has_iwd;              // iwd
	bool has_wpa_supplicant;   // wpa_supplicant

	// 热点和DHCP服务器
	bool has_hostapd;          // hostapd
	bool has_dnsmasq;          // dnsmasq (也可做DNS)

	// DNS管理器
	bool has_systemd_resolved; // systemd-resolved
	bool has_resolvconf;       // resolvconf工具

	// 命令行工具
	bool has_iproute2;         // ip命令
	bool has_iw;               // iw工具
	bool has_iwconfig;         // iwconfig
} network_tools_t;


// 工具组合定义
typedef enum {
    TOOLSET_UNKNOWN = 0,
    TOOLSET_NETWORKMANAGER,     // NetworkManager
    TOOLSET_CONNMAN,            // ConnMan
    TOOLSET_SYSTEMD_IWD,        // systemd-networkd + iwd
    TOOLSET_SYSTEMD_WPA,        // systemd-networkd + wpa_supplicant
    TOOLSET_DHCPCD_IWD,         // dhcpcd + iwd
    TOOLSET_DHCPCD_WPA,         // dhcpcd + wpa_supplicant
    TOOLSET_EMBEDDED            // 自实现
} toolset_t;

network_tools_t net_environment_detect_network_tools(void);

#ifdef __cplusplus
}
#endif

#endif
