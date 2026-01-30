/*///------------------------------------------------------------------------------------------------------------------------//
        网卡配置接口
说 明 : 新版接口，提供对网卡配置的操作接口
日 期 : 2026.1.26

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <map>
#include <atomic>
#include <cmath>
#include <algorithm>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h> //ioctl
#include <arpa/inet.h> //地址转换
#include <net/if.h>    //ifreq
#include <ifaddrs.h>
#include <linux/wireless.h>
#include <iomanip>
#include "Network/NetworkConf.h"
#include "Network/NetworkAppConf.h"
#include "Network/NetworkManager.h"
#include "Network/NmConnection.h"
#include "Network/NmSettings.h"
#include "TpNetworkInterface.h"
#include "TpNetworkConfig.h"
#include "Dbus/connect.h"
#include "TpDbusConnectManage.h"

struct TpNetworkConfigData
{
	TpNetworkInterface interface;
	std::thread thread_t;
	int sockfd;
	TpString devname;		//设备名字，需要频繁使用
	TpString connpath;		//对应的配置路径，根据设备名生成
	NetworkManager *nm;
	NmConnection *nmc;
	NmSettings *nms;
	TpNetworkConfigData(TpString name):interface(name)
	{
		sockfd=-1;
		devname=name;
		nm=NULL;
		nmc=NULL;
		nms=NULL;
	}
};


TpNetworkConfig::TpNetworkConfig(const TpString &name)
{
    data_ = new TpNetworkConfigData(name);
	TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
	if(!device)
		return ;
	device->connpath = TpString(NETWORK_CONFIG_PREFIX)+ name;
	if (TpDbusConnectManage::instance().connection() != TP_TRUE)
    {
        fprintf(stderr, "[Error]: connect to dbus error\n");
		free(device);
        return;
    }
	device->nm = network_manager_create(system_conn);
	if(!device->nm)
		goto FREE;

	device->nms = nm_settings_create(system_conn,NULL);
	if(!device->nms)
		goto FREE;
	
	device->nmc = network_open_connection(system_conn, device->nms, device->connpath.c_str(), device->devname.c_str(), NULL);
	if(!device->nmc)
		goto FREE;
	return ;
FREE:

	if(!device->nms)
		nm_settings_delete(device->nms);

	if(!device->nm)
		network_manager_delete(device->nm);

	delete(device);
}

TpNetworkConfig::~TpNetworkConfig()
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
	if(!device)
		return;
	if(device->nmc)
		nm_connection_delete(device->nmc);
	if(!device->nms)
		nm_settings_delete(device->nms);
	if(!device->nm)
		network_manager_delete(device->nm);
	delete(device);
}


tpInt32 TpNetworkConfig::setStatus(bool status)
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
	TpString devname = device->interface.name();

    int32_t sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket error\n";
        return -1;
    }

    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, devname.c_str(), IFNAMSIZ);
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
    {
        std::cerr << "SIOCGIFFLAGS\n";
    }
    // 设置网卡状态
    if (status)
    {
        ifr.ifr_flags |= IFF_UP; // 启用
    }
    else
    {
        ifr.ifr_flags &= ~IFF_UP; // 禁用
    }

    if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0)
    {
        std::cerr << "ioctl error\n";
        ::close(sock);
        return -1;
    }

    ::close(sock);
    return 0;
}

// 子网掩码转CIDR 前缀长度，如 "255.255.255.0" -> 24
static int netmask_to_prefix(const char *netmask_str)
{
    struct in_addr netmask;
    if (inet_pton(AF_INET, netmask_str, &netmask) != 1)
        return -1;

    uint32_t mask = ntohl(netmask.s_addr);
    int prefix = 0;
    while (mask & 0x80000000)
    {
        prefix++;
        mask <<= 1;
    }
    if (mask != 0)
        return -1; // 非连续1不是合法子网掩码
    return prefix;
}

tpInt32 TpNetworkConfig::getStatus(bool *status)
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
    const TpString &devname = device->interface.name();

    // 创建临时字符数组存储设备名（确保不超过IFNAMSIZ限制）
    char dev[IFNAMSIZ] = {0};
    std::strncpy(dev, devname.c_str(), IFNAMSIZ - 1);

    // 创建socket（与setStatus保持一致）
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket error: " << strerror(errno) << std::endl;
        return -1;
    }

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr)); // 初始化结构体
    std::strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    // 核心：获取网卡标志位[2,4](@ref)
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
    {
        std::cerr << "ioctl(SIOCGIFFLAGS) error: " << strerror(errno) << std::endl;
        ::close(sock);
        return -1;
    }

    // 检查IFF_UP标志位判断网卡状态[2,4,7](@ref)
    *status = (ifr.ifr_flags & IFF_UP) != 0;

    ::close(sock);
    return 0; // 成功返回0
}

// open
tpInt32 TpNetworkConfig::openDevice()
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
    return setStatus(true);
}
// close
tpInt32 TpNetworkConfig::closeDevice()
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
    return setStatus(false);
}

// 网卡是否打开
tpBool TpNetworkConfig::isOpenDevice()
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
    bool status;
    if (getStatus(&status) < 0 || status == false)
        return TP_FALSE;
    return TP_TRUE;
}



TpString TpNetworkConfig::addr()
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
	char addr_c[32];
	memset(addr_c,0,sizeof(addr_c));
	NetworkConf_Get_Addr(device->sockfd,addr_c,device->devname.c_str());
	TpString addr(addr_c);
    return addr;
}

int32_t TpNetworkConfig::setAddr(const TpString &addr)
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
    if (isDhcp())
    {
        return -1;
    }
	return NetworkConf_Set_Addr(device->sockfd,addr.c_str(),device->devname.c_str());
}





tpInt32 TpNetworkConfig::setDhcp()
{
	TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
	nm_connection_set_ipv4_dhcp_is_enabled(device->nmc, true, NULL);
	return 0;
}


tpInt32 TpNetworkConfig::setStatic(const TpString &ip, const TpString &gatway, const TpString &netmask, TpList<TpString> &dns)
{
	TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);

    int prefix = netmask_to_prefix(netmask.c_str());
    if (prefix < 0)
    {
        fprintf(stderr, "Netmask type is error\n");
        return -1;
    }
    uint8_t dns_flag = 0;
    if (dns.size() == 0)
        dns_flag = 1;

    if (Network_Disable_DHCP_Command(device->devname.c_str(), ip.c_str(), prefix, gatway.c_str(), dns_flag) < 0)
        return -1;
    if (dns_flag == 1)
        return 0;
    if (setDns(TP_FALSE, dns) < 0)
        return -1;
    return 0;
}

tpBool TpNetworkConfig::isDhcp()
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);

    int ret = nm_connection_get_ipv4_dhcp_state(device->nmc,NULL);
    if (ret == 1)
        return TP_TRUE;
    else if(ret == 0)
		return TP_FALSE;
    fprintf(stderr, "get dhcp status error\n");
    return TP_FALSE;
}

TpList<TpString> TpNetworkConfig::dns()
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
    TpList<TpString> list;
    char **dns_c;
    int count = nm_connection_get_ipv4_dns_list(device->nmc, &dns_c, NULL); //
    for (int i = 0; i < count; i++)
    {
        TpString dns(dns_c[i]);
        list.emplace_back(dns);
    }
	nm_connection_free_ipv4_dns_list(dns_c);
	return list;
}

tpInt32 TpNetworkConfig::setDns(tpBool autoDns, const TpList<TpString> &dnsList)
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
    if (autoDns)
	{
        nm_connection_set_ipv4_dns_mode(device->nmc, true, NULL);
		return 0;
	}

	nm_connection_set_ipv4_dns_mode(device->nmc, false, NULL);
    int len = dnsList.size();
    char *dns_servers[len];
    int count = 0;
    for (auto &dns : dnsList)
    {
        if (count == len)
            break;
        dns_servers[count] = strdup(dns.c_str());
        count++;
    }
	
    nm_connection_set_ipv4_dns_list(device->nmc, (const char **)dns_servers, count, NULL);
    do
    {
        count--;
        free(dns_servers[count]);
    } while (count);
    return 0;
}

tpBool TpNetworkConfig::isStaticDns()
{
	TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
	int ret = nm_connection_get_ipv4_dns_mode(device->nmc,NULL);
	if(ret == 1)
		return TP_FALSE;
	else if(ret == 0)
		return TP_TRUE;
	fprintf(stderr,"get dns status error\n");
	return TP_TRUE;
}

