/*///------------------------------------------------------------------------------------------------------------------------//
		无线网卡配置接口
说 明 : 
日 期 : 2026.2.27

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
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
#include "NetworkConf.h"
//#include "Network/NetworkManager.h"
#include "TpWirelessConfig.h"
#include "Network/iwd/IwdManager.h"
#include "Network/NetworkMidInterface.h"
#include "Network/nm/NmWireless.h"


struct TpWirelessConfigData
{
	std::thread thread_t;
	TpString devname;		//设备名字，需要频繁使用
	NetworkMidContext *net_ctx;
};


TpWirelessConfig::TpWirelessConfig()
{

}


TpWirelessConfig::~TpWirelessConfig()
{
	
}

// 启动无线扫描
int32_t TpWirelessConfig::startScan()
{
    TpWirelessConfigData *device = static_cast<TpWirelessConfigData *>(data_);
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

    device->scan_is_runing = true;
    device->thread_t = std::thread(&TpWirelessConfig::threadScan, this, 1000);

    return 0;
}

int TpWirelessConfig::threadScan(tpUInt16 time)
{
    TpWirelessConfigData *device = static_cast<TpWirelessConfigData *>(data_);
    TpList<TpWirelessInfo> list_l;
    while (device->scan_is_runing)
    {
        TpList<TpWirelessInfo> list_n = scanList();
        for (const TpWirelessInfo &wlan_l : list_l)
        {
            auto it = find_if(list_n.begin(), list_n.end(), [wlan_l](const TpWirelessInfo &wlan_n)
                              { return (wlan_n.getSsid() == wlan_l.getSsid()); });
            if (it == list_n.end()) // 已经弹出
            {
                // printf("%s已减少\n",wlan_l.getSsid().c_str());
                wirelessRemove.emit(wlan_l.getSsid());
            }
        }

        for (const TpWirelessInfo &wlan_n : list_n)
        {
            auto it = find_if(list_l.begin(), list_l.end(), [wlan_n](const TpWirelessInfo &wlan_l)
                              { return (wlan_l.getSsid() == wlan_n.getSsid()); });
            if (it == list_l.end()) // 新设备
            {
                // printf("新网络\n",wlan_n.getSsid().c_str());
                TpWirelessInfo wlan = wlan_n;
                wirelessAdd.emit(wlan);
            }
        }

        list_l = std::move(list_n);
        /*for (auto &it : list_n)
        {
            if (it)
            {
                delete it;
            }
        }*/

        usleep(1000 * time);
    }
    return 0;
}

int TpWirelessConfig::stopScan()
{
    TpWirelessConfigData *device = static_cast<TpWirelessConfigData *>(data_);
    device->scan_is_runing = false;
    return 0;
}

// 获取扫描<结果
TpList<TpWirelessInfo> TpWirelessConfig::scanList()
{
    TpWirelessConfigData *device = static_cast<TpWirelessConfigData *>(data_);

    int32_t buff_size = 4096;
    TpList<TpWirelessInfo> wlinfo;
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
            // printf("buff is too short\n");
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
            TpString ssid;
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
                wlinfo[wifi_num - 1].setChannel((uint16_t)freq);
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

int32_t TpWirelessConfig::connectWireless(const TpString &ssid, const TpString &psk, tpUInt32 timeout)
{
   return nm_wireless_connect_ssid(ssid.c_str(), psk.c_str(), timeout);
}

TpString TpWirelessConfig::getWirelessSsid()
{
    TpWirelessConfigData *device = static_cast<TpWirelessConfigData *>(data_);
    char ssid[256];
    memset(ssid, 0, sizeof(ssid));
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return nullptr;
    }

    struct iwreq iwr;
    strncpy(iwr.ifr_name, device->devname.c_str(), IFNAMSIZ - 1);
    iwr.u.essid.pointer = ssid;
    iwr.u.essid.length = sizeof(ssid);
    iwr.u.essid.flags = 0;

    if (ioctl(sock, SIOCGIWESSID, &iwr) < 0)
    {
        perror("ioctl");
        close(sock);
        return nullptr;
    }
    close(sock);
    return TpString(ssid);
}

int32_t TpWirelessConfig::disconnectWireless()
{
    TpWirelessConfigData *device = static_cast<TpWirelessConfigData *>(data_);
    return nmcli_disconnect_wireless(device->devname.c_str());
}

tpInt32 TpWirelessConfig::setHotspotSsid(const TpString &ssid)
{
    TpWirelessConfigData *device = static_cast<TpWirelessConfigData *>(data_);
    if (!device)
        return -1;
    int err = 0;
    int32_t sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        fprintf(stderr, "get sock error\n");
        return -1;
    }
    struct iwreq wrq;
    std::memset(&wrq, 0, sizeof(wrq));
    std::strncpy(wrq.ifr_name, device->devname.c_str(), IFNAMSIZ);
    wrq.u.essid.length = ssid.length();
    wrq.u.essid.pointer = strdup(ssid.c_str());
    if (ioctl(sock, SIOCSIWESSID, &wrq) < 0)
    {
        fprintf(stderr, "ioctl SIOCSIWESSID\n");
        err = -1;
    }
    close(sock);
    free(wrq.u.essid.pointer);
    return err;
}

tpInt32 TpWirelessConfig::setHotspotPwd(const TpString &password)
{
    TpWirelessConfigData *device = static_cast<TpWirelessConfigData *>(data_);
    if (!device)
        return -1;
	//暂不支持设置密码
    return 0;
}

tpInt32 TpWirelessConfig::openHotspot()
{
    TpWirelessConfigData *device = static_cast<TpWirelessConfigData *>(data_);
    if (!device)
        return -1;
    int err = 0;
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
        err = -1;
    }
    close(sock);
    return 0;
}
