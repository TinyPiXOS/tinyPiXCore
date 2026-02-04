/*///------------------------------------------------------------------------------------------------------------------------//
		网卡的中间层接口
说 明 : 用于整合底层各个网络管理工具或库的接口
日 期 : 2026.2.3

/*///------------------------------------------------------------------------------------------------------------------------//

#include "Network/NetworkMidInterface.h"
#include "Network/nm/NmConnection.h"
#include "Network/NetworkAppConf.h"
#include "Network/NetEnvironment.h"


static int set_use_network_manager(struct NetworkMidInterface *self)
{
	self->set_static_ipv4 = network_set_connection_static_ipv4;
	self->set_dhcp_ipv4 = network_set_connection_ipv4_dhcp;

	self->set_static_ipv6=NULL;
	self->set_dhcp_ipv6=NULL;

	//设置ipv4DNS列表
	self->set_dns_ipv4_list = network_set_ipv4_dns_list;

	//设置ipv6DNS列表
	self->set_dns_ipv6_list=NULL;

	//设置ipv4的dns是否为自动
	self->set_dns_ipv4_is_auto = network_set_ipv4_dns_mode;

	//设置ipv6的dns是否为自动
	self->set_dns_ipv6_is_auto = network_set_ipv6_dns_mode;

	//获取ipv4的dhcp是否使能
	self->get_dhcp_ipv4_is_enable = network_get_ipv4_dhcp_state;

	//获取ipv4的dhcp是否使能
	self->get_dhcp_ipv6_is_enable = NULL;

	//获取ipv4的dns是否自动
	self->get_dns_ipv4_auto_is_enable = network_get_ipv4_dns_mode;

	//获取ipv6的dns是否自动
	self->get_dns_ipv6_auto_is_enable = network_get_ipv6_dns_mode;

	//获取ipv4DNS列表
	self->get_dns_ipv4_list = network_get_ipv4_dns_list;
}

static int set_use_connman(struct NetworkMidInterface *self)
{
	//暂不支持
	return -1;
}

static int set_use_systemd_networkd(struct NetworkMidInterface *self)
{
	//暂不支持
	return -1;
}

static int set_use_wpa_supplicant(struct NetworkMidInterface *self)
{
	//暂不支持
	return -1;
}

static int set_use_dhcpcd(struct NetworkMidInterface *self)
{
	
}


static int set_use_iwd(struct NetworkMidInterface *self)
{
	//暂不支持
	return -1;
}


// 选择最佳工具组合
static int select_best_toolset(struct NetworkMidInterface *self, const network_tools_t *tools) {
    if (!tools) return TOOLSET_EMBEDDED;
    
    // 优先级1: NetworkManager (最完整)
    if (tools->has_network_manager) {
		printf("完整网络管理解决方案，支持GUI和DBus API\n");
		set_use_network_manager(self);
        return 0;
    }
    
    // 优先级2: ConnMan (嵌入式网络管理)
    if (tools->has_connman) {
		printf("嵌入式网络管理器，支持DBus API\n");
		set_use_connman(self);
        return 0;
    }
    
    // 优先级3: systemd-networkd + iwd (现代组合)
    if (tools->has_systemd_networkd && tools->has_iwd) {
		printf("现代组合：systemd-networkd + iwd，轻量高效\n");
		set_use_systemd_networkd(self);
		set_use_iwd(self);
        return 0;
    }
    
    // 优先级4: systemd-networkd + wpa_supplicant
    if (tools->has_systemd_networkd && tools->has_wpa_supplicant) {
		printf("传统组合：systemd-networkd + wpa_supplicant\n");
		set_use_systemd_networkd(self);
		set_use_wpa_supplicant(self);
        return 0;
    }
    
    // 优先级5: dhcpcd + iwd
    if (tools->has_dhcpcd && tools->has_iwd) {
		printf("传统组合：dhcpcd + iwd\n");
		set_use_iwd(self);
		set_use_dhcpcd(self);
        return 0;
    }
    
    // 优先级6: dhcpcd + wpa_supplicant (传统组合)
    if (tools->has_dhcpcd && tools->has_wpa_supplicant) {
		printf("最广泛支持：dhcpcd + wpa_supplicant\n");
		set_use_dhcpcd(self);
		set_use_wpa_supplicant(self);
        return 0;
    }
    
    // 最低优先级: 自实现
	printf("自实现最小网络栈\n");
    return 0;
}



struct NetworkMidInterface *network_mid_interface_create()
{
	struct NetworkMidInterface *self = malloc(sizeof(struct NetworkMidInterface));
	if(!self)
		return NULL;
	self->set_static_ipv4 = NULL;
	self->set_dhcp_ipv4 = NULL;
	self->set_static_ipv6=NULL;
	self->set_dhcp_ipv6=NULL;
	self->set_dns_ipv4_list = NULL;
	self->set_dns_ipv6_list=NULL;
	self->set_dns_ipv4_is_auto = NULL;
	self->set_dns_ipv6_is_auto = NULL;
	self->get_dhcp_ipv4_is_enable = NULL;
	self->get_dhcp_ipv6_is_enable = NULL;
	self->get_dns_ipv4_auto_is_enable = NULL;
	self->get_dns_ipv6_auto_is_enable = NULL;
	self->get_dns_ipv4_list = NULL;
	network_tools_t  envir = net_environment_detect_network_tools();
	select_best_toolset(self, &envir);

	return self;
}
void network_mid_interface_delete(struct NetworkMidInterface *self)
{
	if(!self)
		return;

	free(self);
}