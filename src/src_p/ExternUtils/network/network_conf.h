#ifndef _NETWORK_CONF_H_
#define _NETWORK_CONF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>             //地址转换
#include <unistd.h>
#include <error.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>

#define ARPHRD_ETHER  1


#define MAX_NETWORK_THREAD 20	//最大网卡线程数量

#define PI_SCAN_MAX_DATA 4096


#define IP_FAMILY AF_INET

typedef enum {
	WIRED=1,	//有线网卡
	WIRELESS=2,	//无线网卡
	PLMN_4G=3	//４Ｇ网卡
//后续根据需要增加网卡类型
}NetworkDeviceType;

enum NETWORK
{
	//方便获取返回，最低位为1是获取，最低位是0是设置
	NET_G_ADDR    = 0x0001,	//g
	NET_S_ADDR    = 0x0002,
	NET_G_NETMASK = 0x0003,	//g
	NET_S_NETMASK = 0x0004,
	NET_G_BRDADDR = 0x0005,	//g
	NET_S_BRDADDR = 0x0006,
	NET_G_HWADDR  = 0x0007,	//g
	NET_S_HWADDR  = 0x0008,
	NET_G_METRIC  = 0x0009,	//g
	NET_S_METRIC  = 0x000A,
	NET_G_FLAGS   = 0x000B,	//g
	NET_S_FLAGS   = 0x000C,
	NET_G_DSTADDR = 0x000D,	//g
	NET_S_DSTADDR = 0x000E,
	NET_G_GATEWAY = 0x000F,
	NET_S_GATEWAY = 0x0010,
	NET_G_ADDR6   = 0x0011,	//g
	NET_S_ADDR6   = 0x0012,
	
	NET_MAX_CONF  = 0X0100,
	//wifi设备用
	NET_G_FREQ    = 0X0107,	//g
	NET_S_FREQ    = 0X0108,
	NET_G_NWID    = 0X0109,	//g
	NET_S_NWID    = 0X010A,
	NET_S_COMMIT  = 0X0102,
	NET_G_NAME    = 0X0103,	//g
	NET_G_MODE    = 0X0105,	//g
	NET_S_MODE    = 0X0106,
	NET_G_SENS    = 0X010B,	//g
	NET_S_SENS    = 0X010C,
	NET_MAX_WCONF = 0X0200,
	
	NET_S_CONNECT    =0X0201,
	NET_S_UPCONNECT  =0X0202,
	NET_S_DOWNCONNECT=0X0203,
	NET_S_DELCONNECT =0X0204
};
//本地网卡信息
typedef struct NetworkDevice{
	char name[30];//网卡名字
	char gateway[20];//网关
	int  type;	//网卡类型
	int  metric;	//优先级
	struct NetworkDevice *next;
}PiNetworkDevice;

//无线网络信息
typedef struct NetworkWireless{
	char SSID[30];	//ssid
	uint8_t inuse;	//是否正在使用
	uint8_t signal;	//信号强度
	uint8_t WPA1;	//安全WPA1的支持
	uint8_t WPA2;	//安全WPA2的支持
	struct NetworkWireless *next;
}PiNetworkWireless;

//无线连接信息
struct Connect_Info{
	char ssid[30];
	char passwd[30];
};
union Configure_Info{
	char data[30];
	int value;
	struct Connect_Info con;
};
struct NETWORK_SET{
	int fd;
	char dev[30];
	int command;
	uint8_t close;
	union Configure_Info conf;
}; 
extern struct NETWORK_SET userset[];

struct NetworkDevice *Network_List_CreatHead(); //初始化链表头
int Network_List_Free(struct NetworkDevice *head);
struct NetworkWireless *NetworkWireless_List_CreatHead(); //初始化链表头


int network_get_addr(int fd,char *name, struct sockaddr_in *addr);
int network_set_addr(int fd,char *name,struct sockaddr_in *addr);
int network_get_netmask(int fd,char *name,struct sockaddr_in *addr);
int network_set_netmask(int fd,char *name,struct sockaddr_in *addr);
int network_get_broadaddr(int fd,char *name,struct sockaddr_in *addr);
int network_set_broadaddr(int fd,char *name,struct sockaddr_in *addr);
int network_get_macaddr(int fd,char *name,struct sockaddr_in *addr);
int network_get_macaddr(int fd,char *name,struct sockaddr_in *addr);

int NetworkConf_Get_Device(struct NetworkDevice *card_head);
int NetworkConf_Get_Addr(int Netfd ,char *conf, char *name);
int NetworkConf_Set_Addr(int Netfd ,char *conf, char *name);
int NetworkConf_Get_AddrIpv6(int Netfd ,char *conf, char *name);
int NetworkConf_Set_AddrIpv6(int Netfd ,char *conf, char *name);
int NetworkConf_Get_DstAddr(int Netfd ,char *conf, char *name);
int NetworkConf_Set_DstAddr(int Netfd ,char *conf, char *name);
int NetworkConf_Get_Netmask(int Netfd,char *conf, char *name);
int NetworkConf_Set_Netmask(int Netfd,char *conf, char *name);
int NetworkConf_Get_BroadAddr(int Netfd,char *conf, char *name);
int NetworkConf_Set_BroadAddr(int Netfd,char *conf, char *name);
int NetworkConf_Get_MacAddr(int Netfd,char *conf, char *name);
int NetworkConf_Set_MacAddr(int Netfd,char *conf, char *name);
int NetworkConf_Get_Flags(int Netfd,short *flag, char *name);
int NetworkConf_Set_Flags(int Netfd,short flag, char *name);
int NetworkConf_Get_Mtu(int Netfd,int *mtu, char *name);
//-------以下为无限网卡专用配置-----
int NetworkConf_Get_Freq(int Netfd,struct iw_freq *freq, char *name);
int NetworkConf_Set_Freq(int Netfd,struct iw_freq freq, char *name);
int NetworkConf_Get_Nwid(int Netfd,struct iw_param *param, char *name);
int NetworkConf_Set_Nwid(int Netfd,struct iw_param param, char *name);
int NetworkConf_Get_Sens(int Netfd,struct iw_param *param, char *name);
int NetworkConf_Set_Sens(int Netfd,struct iw_param param, char *name);
int Network_Get_Wifi(struct NetworkWireless *head, char *name);

#ifdef __cplusplus
extern "C" {
#endif


int system_cmd_timeout(const char *cmd, uint32_t timeout);
//------------------------------------------
int Network_SockConf_Open() __attribute__((used));
int Network_Sock_Close(int) __attribute__((used));
int Network_Conf_Device(char *name,int type,void *conf) __attribute__((used));
int Network_Conf_Wireless(char *name,int type,void *conf) __attribute__((used));

//设置DHCP
int Network_Set_DHCP(const char* dev,uint8_t status) __attribute__((used));
int Network_Enable_DHCP(const char *ifname);
// 返回 0 成功，-1 失败
int Network_Disable_DHCP(const char *ifname) ;
int Network_Enable_DHCP_Command(const char *ifname);
int Network_Disable_DHCP_Command(const char *ifname,const char *ip, int prefix ,const char *gatewa,uint8_t dns_flag);
int Network_Get_DNS_Command(const char *ifname, char *dns_servers[], int max_servers);
int Network_Set_DNS_Command(const char *ifname, const char *dns_servers[], int dns_count);
int Network_Is_DHCP_Command(const char *ifname);
int Network_Set_Gateway_Command(const char *conn_name, const char *gateway);
int Network_Get_Gateway_Command(const char *conn_name, char *gateway, size_t len);
int Network_Is_Have_Static_DNS_Command(const char *ifname);
int Network_Is_Have_Server_DNS_Command(const char *ifname);
int Network_Set_Auto_DNS_Command(const char *ifname);
void Network_Connect_Wireless(const char* dev,const char* ssid, const char* psk) __attribute__((used));
#ifdef __cplusplus
}
#endif

#endif



