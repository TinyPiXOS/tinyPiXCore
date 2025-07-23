/*///------------------------------------------------------------------------------------------------------------------------//
		网卡硬件接口
说 明 : 
日 期 : 2024.12.25

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
#include <sys/ioctl.h>				//ioctl
#include <arpa/inet.h>             //地址转换
#include <net/if.h>					//ifreq
#include <ifaddrs.h>
#include <linux/wireless.h>
#include <iomanip>
#include "network_conf.h"
#include "tpdbus/network_manager.h"
#include "tpNetworkInterface.h"



#define MAX_DNS_NUMBER		64
#define WPA_SUPPLICATION_CONFIG "/System/conf/wap_supplication/wap_supplication.conf"

#ifdef __cplusplus
extern "C"
{ //  告诉编译器下列代码要以C链接约定的模式进行链接
#endif
	int32_t Network_SockConf_Open();
	int32_t Network_Set_DHCP(const char *dev, uint8_t status);
	int32_t Network_Conf_Device(char *name, int32_t type, void *conf);
	void Network_Connect_Wireless(const char *dev, const char *ssid, const char *psk);
#ifdef __cplusplus
}
#endif


struct iw_essid
{
	uint16_t len;
	uint16_t flags;
	char __attribute__((aligned(8))) essid;
};

struct tpNetworkData
{
	tpString devname;		//网卡名称
	bool device_wl;			//网卡类型()

	std::thread thread_t;
	std::atomic<bool> scan_is_runing;	
	
	tpNetworkData(const tpString &name):devname(name) 
	{
		scan_is_runing=false;
	}
};


// 创建临时文件,
static tpString open_directories_temp()
{
    pid_t pid = getpid();                 // 获取进程 ID
    time_t t = time(NULL);                // 获取当前时间
    char *filename = (char *)malloc(256); // 分配内存
    if (filename == NULL)
    {
        perror("malloc");
        return nullptr;
    }
    snprintf(filename, 256, "/tmp/tmpfile_%d_%ld.tmp", pid, t);
    return filename;
}
// 删除临时文件
static int32_t close_directories_temp(char *file)
{
    if (remove(file) != 0)
    {
        perror("remove() error");
    }
    free(file);
    return 0;
}

tpString getVendorID(const tpString &iface)
{
	tpString path = "/sys/class/net/" + iface + "/device/vendor";
	std::ifstream vendorFile(path);
	tpString vendorID;

	if (vendorFile.is_open())
	{
		std::getline(vendorFile, vendorID);
		vendorFile.close();
	}
	else
	{
		std::cerr << "Failed to open vendor file." << std::endl;
	}

	return vendorID;
}

tpString getDeviceID(const tpString &iface)
{
	tpString path = "/sys/class/net/" + iface + "/device/device";
	std::ifstream deviceFile(path);
	tpString deviceID;

	if (deviceFile.is_open())
	{
		std::getline(deviceFile, deviceID);
		deviceFile.close();
	}
	else
	{
		std::cerr << "Failed to open device file." << std::endl;
	}

	return deviceID;
}

// 根据 vendor ID 和 device ID 查找制造商名称
tpString getManufacturer(const tpString &vendorID, const tpString &deviceID)
{
    // 这里用一个简单的映射表来查找制造商
    std::map<tpString, tpString> vendorMap = {
        {"0x14e4", "Broadcom"},
        {"0x8086", "Intel"},
        {"0x10ec", "Realtek"},
        // 添加更多制造商
    };

    // 组合 vendorID 和 deviceID 进行查找
    std::string key = vendorID; // 确保是以 0x 开头
                                //	std::cout << key << std::endl;
    if (vendorMap.find(key) != vendorMap.end())
    {
        return vendorMap[key];
    }

    return "Unknown Manufacturer";
}

// 判断是否是无限网卡
tpBool Network_Wl_Check(const char *dev)
{
	int32_t sock;
	struct iwreq wreq;
	tpBool type = TP_FALSE;
	memset(&wreq, 0, sizeof(wreq));
	strncpy(wreq.ifr_name, dev, IFNAMSIZ); // 接口名称
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (ioctl(sock, SIOCGIWNAME, &wreq) == 0)
	{
		printf("\nThe [%s] is a wireless interface. The protocol is %s\n", dev, wreq.u.name);
		type = TP_TRUE;
	}
	close(sock);
	return type;
}

//检查网线是否插入
tpBool Network_Flags_Check(const char *dev)
{
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, dev, IFNAMSIZ-1);
    if(ioctl(sock, SIOCGIFFLAGS, &ifr)<0)
	{
		close(sock);
        return TP_FALSE;
	}
    close(sock);
	//IFF_RUNNING 标志表示接口已分配资源并处于活动状态
	//IFF_UP 标志表示接口已被启用但可能未连接
    return (ifr.ifr_flags & IFF_RUNNING) ? TP_TRUE : TP_FALSE; // 检查接口是否启用
}

tpBool Network_Ping_Check(const char *dev, const char *addr)
{
	char command[512];
	snprintf(command,512,"ping -I %s -c 1 -w 1 %s > /dev/null 2>&1",dev,addr);
    int ret = system(command);
	return (ret==0) ? TP_TRUE : TP_FALSE;
}


tpNetworkInterface::tpNetworkInterface(const char *name)
{
	const tpString devname(name);
	data_=new tpNetworkData(devname);
}
tpNetworkInterface::tpNetworkInterface(const tpString& name)
{
	data_=new tpNetworkData(name);
}

tpNetworkInterface::~tpNetworkInterface()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);

	if(device->scan_is_runing)
	{
		device->scan_is_runing=false;
	}
	if (device->thread_t.joinable())
			device->thread_t.join(); // 等待线程完成

	if(device)
		delete(device);
}

tpList<tpNetworkInterface> tpNetworkInterface::getAllDevice()
{
	tpList<tpNetworkInterface> interface;
	struct ifaddrs *ifaddr;
	if (getifaddrs(&ifaddr) == -1)
	{
		std::cerr << "getifaddrs\n";
		return interface;
	}

	for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == nullptr)
			continue;
		if (ifa->ifa_name == nullptr || strlen(ifa->ifa_name) == 0) {
    		continue;
		}
		tpString name(ifa->ifa_name);
		auto it=std::find_if(interface.begin(),interface.end(),[&name](tpNetworkInterface &dev){return name==dev.getName();});
		if(it==interface.end())
		{
			interface.emplace_back(name);
			//std::cout << "Interface: " << name << std::endl;
		}
	}
	freeifaddrs(ifaddr);
	return std::move(interface);
}

tpString tpNetworkInterface::getName() const
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	return device->devname;
}

tpInt32 tpNetworkInterface::setConf(const tpString &name, uint16_t type, const tpString &addr)
{
    char *c_name = new char[name.length() + 1];
    char *c_addr = new char[addr.length() + 1];
    std::strcpy(c_name, name.c_str());
    std::strcpy(c_addr, addr.c_str());
    return Network_Conf_Device(c_name, type, c_addr);
}

tpInt32 tpNetworkInterface::getConf(const tpString &name, uint16_t type, tpString &addr)
{
    int32_t err;
    char *c_name = new char[name.length() + 1];
    char c_addr[32];
    std::memset(c_addr, 0, sizeof(c_addr));
    std::strcpy(c_name, name.c_str());
    if ((err = Network_Conf_Device(c_name, type, c_addr)) < 0)
        return err;
    addr = tpString(c_addr);
    return 0;
}

tpInt32 tpNetworkInterface::setStatus(bool status)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	char *dev=new char[device->devname.length() + 1];
	std::strcpy(dev, device->devname.c_str());

	int32_t sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		std::cerr << "socket error\n";
		return -1;
	}

	struct ifreq ifr;
	std::strncpy(ifr.ifr_name, dev, IFNAMSIZ);
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

//子网掩码转CIDR 前缀长度，如 "255.255.255.0" -> 24
int netmask_to_prefix(const char *netmask_str) {
    struct in_addr netmask;
    if (inet_pton(AF_INET, netmask_str, &netmask) != 1)
        return -1;

    uint32_t mask = ntohl(netmask.s_addr);
    int prefix = 0;
    while (mask & 0x80000000) {
        prefix++;
        mask <<= 1;
    }
    if (mask != 0) return -1; // 非连续1不是合法子网掩码
    return prefix;
}


tpInt32 tpNetworkInterface::getStatus(bool *status) 
{
	tpNetworkData *device = static_cast<tpNetworkData*>(data_);
	const std::string& devname = device->devname;

	// 创建临时字符数组存储设备名（确保不超过IFNAMSIZ限制）
	char dev[IFNAMSIZ] = {0};
	std::strncpy(dev, devname.c_str(), IFNAMSIZ - 1);

	// 创建socket（与setStatus保持一致）
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		std::cerr << "socket error: " << strerror(errno) << std::endl;
		return -1;
	}

	struct ifreq ifr;
	std::memset(&ifr, 0, sizeof(ifr));  // 初始化结构体
	std::strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	// 核心：获取网卡标志位[2,4](@ref)
	if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
		std::cerr << "ioctl(SIOCGIFFLAGS) error: " << strerror(errno) << std::endl;
		::close(sock);
		return -1;
	}

	// 检查IFF_UP标志位判断网卡状态[2,4,7](@ref)
	*status = (ifr.ifr_flags & IFF_UP) != 0;

	::close(sock);
	return 0;  // 成功返回0
}



// open
tpInt32 tpNetworkInterface::openDevice()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
    return setStatus(true);
}
// close
tpInt32 tpNetworkInterface::closeDevice()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
    return setStatus(false);
}

//网卡是否打开
tpBool tpNetworkInterface::isOpenDevice()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	bool status;
	if(getStatus(&status)<0 || status==false)
		return TP_FALSE;
	return TP_TRUE;
}

// 制造商
tpString tpNetworkInterface::getManu()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);

    tpString manufacturer;
    tpString vendorID = getVendorID(device->devname);
    tpString deviceID = getDeviceID(device->devname);

    if (!vendorID.empty() && !deviceID.empty())
    {
        manufacturer = getManufacturer(vendorID, deviceID);
    }
    else
    {
        // manufacturer="None";
    }
    return manufacturer;
}

//
tpBool tpNetworkInterface::isWireless()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);

	return Network_Wl_Check(device->devname.c_str());
}

//
tpBool tpNetworkInterface::isOnline()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);

	return Network_Flags_Check(device->devname.c_str());
}

tpBool tpNetworkInterface::isOnlineInternet()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	//ping baidu.com或8.8.8.8
	return Network_Ping_Check(device->devname.c_str(), "baidu.com");
}


tpInt32 tpNetworkInterface::setDhcp()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
//	return Network_Set_DHCP(device->devname.c_str(), 1);
//	return Network_Enable_DHCP(device->devname.c_str()); 
	return Network_Enable_DHCP_Command(device->devname.c_str());	//需要清设置的DNS
}

/*tpInt32 tpNetworkInterface::setStatic()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
//	return Network_Set_DHCP(device->devname.c_str(), 0);
	return  Network_Disable_DHCP(device->devname.c_str());
}*/




//关闭DHCP
tpInt32 tpNetworkInterface::setStatic(const tpString& ip, const tpString& gatway, const tpString& netmask,tpList<tpString>& dns)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	int prefix = netmask_to_prefix(netmask.c_str());
	if(prefix<0)
	{
		fprintf(stderr,"Netmask type is error\n");
		return -1;
	}
	uint8_t dns_flag=0;
	if(dns.size()==0)
		dns_flag=1;

	if (Network_Disable_DHCP_Command(device->devname.c_str(),ip.c_str(), prefix,gatway.c_str(),dns_flag)<0)
		return -1;
	if(dns_flag==1)
		return 0;
	if(setDns(TP_FALSE ,dns)<0)
		return -1;
	return 0;
}



tpBool tpNetworkInterface::isDhcp()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);

	//int ret=network_manager_get_dhcp_status(device->devname.c_str());
	int ret=Network_Is_DHCP_Command(device->devname.c_str());
	if(ret==0)
		return TP_TRUE;
	else if(ret<0)
		fprintf(stderr,"get dhcp status error\n");
	return TP_FALSE;
}


tpList<tpString> tpNetworkInterface::getDns()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	tpList<tpString> list;
	char *dns_c[MAX_DNS_NUMBER];
	int count=Network_Get_DNS_Command(device->devname.c_str(), dns_c, MAX_DNS_NUMBER);	//
	for(int i=0; i<count; i++)
	{
		tpString dns(dns_c[i]);
		list.emplace_back( dns);
		free(dns_c[i]);
	}
	return list;
}

tpInt32 tpNetworkInterface::setDns(tpBool autoDns, const tpList<tpString>& dnsList)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	if(autoDns)
		return Network_Set_Auto_DNS_Command(device->devname.c_str());
	
	int len=dnsList.size();
	char *dns_servers[len];
	int count=0;
	for(auto &dns: dnsList)
	{
		if(count==len)
			break;
		dns_servers[count]=strdup(dns.c_str());
		count++;	
	}
	tpInt32 err= Network_Set_DNS_Command(device->devname.c_str(),(const char **)dns_servers,count);
	do
	{
		count--;
		free(dns_servers[count]);
	}while(count);
	return err;
}

tpBool tpNetworkInterface::isStaticDns()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	if(Network_Is_Have_Static_DNS_Command(device->devname.c_str())>0)
		return TP_TRUE;
	return TP_FALSE;
}

tpString tpNetworkInterface::getGatway()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	char gw[64];
	Network_Get_Gateway_Command(device->devname.c_str(), gw, sizeof(gw));
	return tpString((const char *)gw);
}

tpInt32 tpNetworkInterface::setGatway(const tpString& gw)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	if(isDhcp())
		return -1;
	return Network_Set_Gateway_Command(device->devname.c_str(), gw.c_str());
}


tpString tpNetworkInterface::getAddr()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	tpString addr;
    getConf(device->devname, NET_G_ADDR, addr);
	return addr;
}

int32_t tpNetworkInterface::setAddr(const tpString &addr)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	if(isDhcp())
	{
		return -1;
	}
    return setConf(device->devname, NET_S_ADDR, addr);
}

tpString tpNetworkInterface::getMacAddr()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	tpString addr;
	getConf(device->devname, NET_G_HWADDR, addr);
	return addr;
}

int32_t tpNetworkInterface::setMacAddr(const tpString &addr)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
    return setConf(device->devname, NET_S_HWADDR, addr);
}

tpString tpNetworkInterface::getNetmask()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	tpString addr;
	getConf(device->devname, NET_G_NETMASK, addr);
	return addr;
}

int32_t tpNetworkInterface::setNetmask(const tpString &addr)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	if(isDhcp())
		return -1;
    return setConf(device->devname, NET_S_NETMASK, addr);
}

tpString tpNetworkInterface::getBroadAddr()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	tpString addr;
	getConf(device->devname, NET_G_BRDADDR, addr);
	return addr;
}

int32_t tpNetworkInterface::setBroadAddr(const tpString &addr)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
    return setConf(device->devname, NET_S_BRDADDR, addr);
}

tpString tpNetworkInterface::getAddrIpv6()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	tpString addr;
	getConf(device->devname, NET_G_ADDR6, addr);
	return addr;
}
int32_t tpNetworkInterface::setAddrIpv6(const tpString &addr)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
    return setConf(device->devname, NET_S_ADDR6, addr);
}

// 启动无线扫描
int32_t tpNetworkInterface::startScan()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	int32_t sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		std::cerr << "socket error\n";
		return 0;
	}

	uint8_t buff[128];
	struct iwreq wrq;
	std::memset(&wrq, 0, sizeof(wrq));
	std::strncpy(wrq.ifr_name, device->devname.c_str(), IFNAMSIZ); // wrq.ifr_ifrn.ifrn_name
	if (ioctl(sock, SIOCSIWSCAN, &wrq) < 0)
	{
		std::cerr << "ioctl (SIOCSIWSCAN)\n";
		::close(sock);
		return -1;
	}
	::close(sock);

	device->scan_is_runing=true;
	device->thread_t = std::thread(&tpNetworkInterface::threadScan, this, 1000);

	return 0;
}


int tpNetworkInterface::threadScan(tpUInt16 time)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	tpList<tpWirelessInfo> list_l;
	while(device->scan_is_runing)
	{
		tpList<tpWirelessInfo> list_n=getScan();
		for(const tpWirelessInfo &wlan_l:list_l)
		{
			auto it=find_if(list_n.begin(),list_n.end(),[wlan_l](const tpWirelessInfo &wlan_n){return (wlan_n.getSsid()==wlan_l.getSsid());});
			if(it==list_n.end())		//已经弹出
			{
				//printf("%s已减少\n",wlan_l.getSsid().c_str());
				wirelessRemove.emit(wlan_l.getSsid());
			}
		}

		for(const tpWirelessInfo &wlan_n:list_n)
		{
			auto it=find_if(list_l.begin(),list_l.end(),[wlan_n](const tpWirelessInfo &wlan_l){return (wlan_l.getSsid()==wlan_n.getSsid());});
			if(it==list_l.end())		//新设备
			{
				//printf("新网络\n",wlan_n.getSsid().c_str());
				tpWirelessInfo wlan=wlan_n;
				wirelessAdd.emit(wlan);
			}
		}

		list_l=std::move(list_n);
		/*for (auto &it : list_n)
		{
			if (it)
			{
				delete it;
			}
		}*/

		usleep(1000*time);
	}
	return 0;
}

int tpNetworkInterface::stopScan()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	device->scan_is_runing=false;
	return 0;
}


// 获取扫描<结果
tpList<tpWirelessInfo> tpNetworkInterface::getScan()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);

    int32_t buff_size = 4096;
    tpList<tpWirelessInfo> wlinfo;
    int32_t wifi_num = 0;
    int32_t sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket error\n";
        return wlinfo;
    }
MALLOC:
    buff_size *= 2;
    char *buff = (char *)malloc(buff_size);

    std::memset(buff, 0, buff_size);
    struct iwreq wrq;
    std::memset(&wrq, 0, sizeof(wrq));
    wrq.u.data.pointer = buff;
    wrq.u.data.length = buff_size;
    wrq.u.data.flags = 0;

    std::strncpy(wrq.ifr_name, device->devname.c_str(), IFNAMSIZ); // wrq.ifr_ifrn.ifrn_name
    // printf("network %s\n",dev);
SCAN:
    // 发送获取扫描结果请求
    if (ioctl(sock, SIOCGIWSCAN, &wrq) < 0)
    {
        if (errno == EAGAIN)
        {
            // printf("sleep\n");
            sleep(2);
            goto SCAN;
        }
        else if (errno == E2BIG)
        {
            //printf("buff is too short\n");
            goto MALLOC;
        }
        // close(sock);
    }

    struct iw_scan_req *scan_req = (struct iw_scan_req *)wrq.u.data.pointer;
    int32_t num_results = wrq.u.data.length;

    // 解析扫描结果
    struct iw_event *iwe;
    int32_t total = wrq.u.data.length; // 获取返回数据长度
                                       //    char* buffer = new char[total];
                                       //    std::memcpy(buff, wrq.u.data.pointer, total);
    printf("get length%d\n", total);
    int32_t offset = 0;
    while (offset < total)
    {
        // std::memcpy(&iwe, buff + offset, sizeof(iwe));
        iwe = (struct iw_event *)(buff + offset);
        // printf("len=%d  ,size=%ld  ",iwe->len,sizeof(iwe));
        // printf("cmd=%d,%04x\n",iwe->cmd,iwe->cmd);
        offset += iwe->len;

        if (iwe->cmd == SIOCGIWAP)
        { // mac
            wifi_num++;
            uint8_t *mac = (uint8_t *)iwe->u.ap_addr.sa_data;
            std::ostringstream mac_str;
            // printf("\nMAC: ");
            for (int32_t i = 0; i < 6; ++i)
            {
                mac_str << std::hex << std::setw(2) << std::setfill('0') << static_cast<int32_t>(mac[i]);
                if (i < 5)
                {
                    mac_str << ':';
                }
            }
            wlinfo.emplace_back(mac_str.str());
            // puts("");
        }
        if (wifi_num == 0)
            continue;
        if (iwe->cmd == SIOCGIWESSID)
        { // ssid
			tpString ssid;
            struct iw_essid *essid_p = (struct iw_essid *)&iwe->u.data;
            // printf("Len: %d\tflags: %d\n", essid_p->len, essid_p->flags);
            char *p = &(essid_p->essid);
            // printf("ESSID: ");
            int32_t i;
            for (i = 0; i < essid_p->len; ++i)
            {
               ssid += static_cast<char>(p[i]);
                // printf("%c", p[i]);
            }
			wlinfo[wifi_num - 1].setSsid(ssid);
            // puts("");
        }
        else if (iwe->cmd == SIOCGIWFREQ)
        {
            struct iw_freq *ap_freq = (struct iw_freq *)&(iwe->u.freq);
            double freq = (double)ap_freq->m * std::pow(10, ap_freq->e);
            if (freq > 1000)
            {
                // ap的工作频率
                wlinfo[wifi_num - 1].setFreq(freq);
                // printf("Frequency: %.3f\n", (float)freq / (1e9));
            }
            else
            {
                // AP的channel
                wlinfo[wifi_num - 1].setChannel((uint16_t)freq) ;
                // printf("Channel: %d\n", (int32_t)freq);
            }
        }
        else if (iwe->cmd == IWEVQUAL) // qual
        {
            struct iw_quality *qual = (struct iw_quality *)&(iwe->u.qual);
            int8_t level = qual->level;
            wlinfo[wifi_num - 1].setLevel(level);
        }
        else if (iwe->cmd == SIOCGIWENCODE)
        {
            // struct iw_point *data=&iwe->u.encoding;

            // printf("data=%s\n",(const char *)data->pointer);
        }
    }
    close(sock);
    free(buff);
    return wlinfo;
}




int32_t tpNetworkInterface::connectWireless(const tpString &ssid, const tpString &psk, tpUInt32 timeout)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	int32_t err=-1; 
    const char *name = ssid.c_str();
    const char *passwd = psk.c_str();
    char *buff = (char *)malloc(1024);
    char *buf = (char *)malloc(200);
    memset(buf, 0, 200);
    char *all = buf;
    FILE *outstream = NULL;
    sprintf(buf, "nmcli dev wifi con %s password %s name %s", name, passwd, name);
    if (system_cmd_timeout((const char *)buf,(uint32_t)timeout) < 0) // 执行all字符串所包含的命令
        goto EXIT_FREE;
    //	printf("1\n");
    sprintf(buf, "nmcli con mod %s wifi-sec.psk %s", name, passwd); // ke'yi'sheng'lue
    if (system_cmd_timeout((const char *)buf,(uint32_t)timeout) < 0)
	{
		goto EXIT_FREE;
	}
    //	printf("2\n");

    sprintf(buf, "nmcli con up %s", name); // 启用网络连接
    if ((outstream = popen(buf, "r")) == NULL)
    {
        std::cerr << "get command information error\n";
        goto EXIT_FREE;
    }
    while (fgets(buff, 1024, outstream) != NULL)
    {
        if (strstr(buff, "error") != NULL || strstr(buff, "错误") != NULL)
        {
			goto EXIT;
        }
    }
    //	printf("3\n");
    //sprintf(buf, "nmcli con mod %s connection.autoconnect yes", name); // 修改网络连接的单项参数，修改为自动连接
	if (passwd == NULL || strlen(passwd) == 0) {
    	sprintf(buf, "nmcli dev wifi connect '%s' name '%s'", name, name);
	} else {
		sprintf(buf, "nmcli dev wifi connect '%s' password '%s' name '%s'", name, passwd, name);
	}
    if (system_cmd_timeout((const char *)buf,(uint32_t)timeout) < -1)
        goto EXIT;
	
	err=0;
EXIT:
	pclose(outstream);
EXIT_FREE:
    free(buf);
    free(buff);
    return err;
}


tpString tpNetworkInterface::getWirelessSsid()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	char ssid[256];
	memset(ssid,0,sizeof(ssid));
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return nullptr;
    }

    struct iwreq iwr;
    strncpy(iwr.ifr_name, device->devname.c_str(), IFNAMSIZ - 1);
    iwr.u.essid.pointer = ssid;
    iwr.u.essid.length = sizeof(ssid);
    iwr.u.essid.flags = 0;

    if (ioctl(sock, SIOCGIWESSID, &iwr) < 0) {
        perror("ioctl");
        close(sock);
        return nullptr;
    }
    close(sock);
	return tpString(ssid);
}


int32_t tpNetworkInterface::disconnectWireless()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
    char buff[1024];
    sprintf(buff, "nmcli dev disconnect %s", device->devname.c_str());
    if (system(buff) < 0) //
        return -1;
    return 0;
}

tpInt32 tpNetworkInterface::setHotspotSsid(const tpString &ssid)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	if(!device)
		return -1;
	int err=0;
    int32_t sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
		fprintf(stderr,"get sock error\n");
        return -1;
    }
    struct iwreq wrq;
    std::memset(&wrq, 0, sizeof(wrq));
    std::strncpy(wrq.ifr_name, device->devname.c_str(), IFNAMSIZ);
    wrq.u.essid.length = ssid.length();
	wrq.u.essid.pointer = strdup(ssid.c_str());
    if (ioctl(sock, SIOCSIWESSID, &wrq) < 0)
    {
        fprintf(stderr,"ioctl SIOCSIWESSID\n");
        err=-1;
    }
    close(sock);
	free(wrq.u.essid.pointer);
    return err;
}

tpInt32 tpNetworkInterface::setHotspotPwd(const tpString &password)
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	if(!device)
		return -1;
    /*int32_t sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return -1;
    }
    // 设置密码（WPA2）
    struct iwreq wrq;
    std::memset(&wrq, 0, sizeof(wrq));
    std::strncpy(wrq.ifr_name, devname.c_str(), IFNAMSIZ);
    wrq.u.key.length = password.length();
    strncpy((char*)wrq.u.key.pointer, password.c_str(), password.length());
    wrq.u.key.flags = IW_ENCODE_SEQ_DEFAULT; // 默认序列
    if (ioctl(sock, SIOCSIWENCODE, &wrq) < 0) {
        perror("SIOCSIWENCODE");
        close(sock);
        return -1;
    }
    close(sock);*/
    return 0;
}

tpInt32 tpNetworkInterface::openHotspot()
{
	tpNetworkData *device=static_cast<tpNetworkData *>(data_);
	if(!device)
		return -1;
	int err=0;
    int32_t sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        return -1;
    }

    struct iwreq wrq;
    std::memset(&wrq, 0, sizeof(wrq));
    std::strncpy(wrq.ifr_name, device->devname.c_str(), IFNAMSIZ);
	wrq.u.mode = IW_MODE_MASTER; // IW_MODE_MASTER 表示热点模式
    if (ioctl(sock, SIOCSIWMODE, &wrq) < 0)
    {
        std::cerr << "SIOCSIWMODE\n";
        err=-1;
    }
    close(sock);
    return 0;
}
