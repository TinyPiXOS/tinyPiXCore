#ifndef _NETWORK_MID_INTERFACE_H_
#define _NETWORK_MID_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "Network/nm/NmConnection.h"



typedef union NetworkMidContext
{
	char *devname;
	int fd;
	NmConnection *nmc;
	void *ctx;
}NetworkMidContext;


struct NetworkMidInterface{
	NetworkMidContext context;
	/// @brief 设置静态关闭DHCP，
	int (*set_static_ipv4)(NetworkMidContext *context, const char *ip, int prefix, const char *gateway, bool dns_flag);

	/// @brief 设置ipv4 DHCP打开
	int (*set_dhcp_ipv4)(NetworkMidContext *context);

	/// @brief 设置ipv6静态关闭DHCP，
	int (*set_static_ipv6)(NetworkMidContext *context, const char *ip, int prefix, const char *gateway, bool dns_flag);

	/// @brief 设置ipv6 DHCP打开
	int (*set_dhcp_ipv6)(NetworkMidContext *context);

	/// @brief 设置ipv4DNS列表
	int (*set_dns_ipv4_list)(NetworkMidContext *context, const char **dns_list, uint32_t dns_count);

	/// @brief 设置ipv6DNS列表
	int (*set_dns_ipv6_list)(NetworkMidContext *context, const char **dns_list, uint32_t dns_count);

	/// @brief 设置ipv4的dns为自动
	int (*set_dns_ipv4_is_auto)(NetworkMidContext *context, bool is_auto);

	/// @brief 设置ipv6的dns为自动
	int (*set_dns_ipv6_is_auto)(NetworkMidContext *context, bool is_auto);

	/// @brief 获取ipv4的dhcp是否使能
	int (*get_dhcp_ipv4_is_enable)(NetworkMidContext *context);

	/// @brief 获取ipv4的dhcp是否使能
	int (*get_dhcp_ipv6_is_enable)(NetworkMidContext *context);

	/// @brief 获取ipv4的dns是否自动
	int (*get_dns_ipv4_auto_is_enable)(NetworkMidContext *context);

	/// @brief 获取ipv6的dns是否自动
	int (*get_dns_ipv6_auto_is_enable)(NetworkMidContext *context);

	/// @brief 获取ipv4DNS列表
	int (*get_dns_ipv4_list)(NetworkMidContext *context, char ***dns_list_out);

};


struct WirelessMidInterface{
	NetworkMidContext context;
	
	int (*connect_to_ssid)(NetworkMidContext *context, const char *ssid, const char *password); //connect_to_ssid
};


struct NetworkMidInterface *network_mid_interface_create(const char *devname);
void network_mid_interface_delete(struct NetworkMidInterface *self);





#ifdef __cplusplus
}
#endif

#endif