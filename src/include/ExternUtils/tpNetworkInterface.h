#ifndef __TP_NETWORK_INTERFACE_H
#define __TP_NETWORK_INTERFACE_H


#include "tpUtils.h"
#include "tpSignalSlot.h"
#include "tpWirelessInfo.h"

TP_DEF_VOID_TYPE_VAR(ItpNetworkInterfaceData);

class tpNetworkInterface
{
public:
	tpNetworkInterface(const char *name);
	tpNetworkInterface(const tpString& name);
	~tpNetworkInterface();
public:
	/// @brief 获取所有网卡硬件
	/// @return 
	static tpList<tpNetworkInterface> getAllDevice();
	/// @brief 获取网卡接口名字
	/// @return 
	tpString getName() const;
	/// @brief 获取网卡制造商
	/// @return 
	tpString getManu();
	/// @brief 是否是无线网卡
	/// @return 
	tpBool isWireless();
	/// @brief 判断网络接口状态,有线网络和无线网络共用此接口，值代表已经插入网线或连接到无线网络，不代表真实上网状态
	/// @return 
	tpBool isOnline();
	/// @brief 互联网状态
	/// @return 
	tpBool isOnlineInternet();
	/// @brief 网卡打开
	/// @return 
	tpInt32 openDevice();
	/// @brief 网卡关闭
	/// @return 
	tpInt32 closeDevice();
	/// @brief 网卡是否打开
	/// @return 
	tpBool isOpenDevice();
	/// @brief 打开网卡自动DHCP
	/// @return 
	tpInt32 setDhcp();
	/// @brief 关闭网卡自动DHCP
	/// @return 
	/// tpInt32 setStatic();
	/// @brief 关闭DHCP，由于之前版本的DHCP存在问题，推荐使用此版本DHCP
	/// @param ip ip
	/// @param gatway 网关
	/// @param netmask 子网掩码 
	/// @param dns dns列表，列表为空自动进行DNS
	/// @return 
	tpInt32 setStatic(const tpString& ip, const tpString& gatway, const tpString &netmask,tpList<tpString>& dns);
	/// @brief 判断自动DHCP是否打开
	/// @return 
	tpBool isDhcp();
	/// @brief 获取网关
	/// @return 
	tpString getGatway();
	/// @brief 设置网关
	/// @param gatway 
	/// @return 
	tpInt32 setGatway(const tpString& gatway);
	/// @brief 获取DNS
	/// @return 
	tpList<tpString> getDns();
	/// @brief 设置DNS
	/// @param dns_list dns列表
	/// @return 
	tpInt32 setDns(tpBool autoDns, const tpList<tpString>& dnsList = tpList<tpString>());
	/// @brief 是否是静态的DNS
	/// @return 
	tpBool isStaticDns();
	/// @brief 获取网卡IP地址
	/// @return 
	tpString getAddr();
	/// @brief 设置网卡IP地址
	/// @param addr 
	/// @return 
	tpInt32 setAddr(const tpString &addr);
	/// @brief 获取网卡MAC地址
	/// @return 
	tpString getMacAddr();
	/// @brief 设置网卡MAC地址
	/// @param addr 
	/// @return 
	tpInt32 setMacAddr(const tpString &addr);
	/// @brief 获取网卡子网掩码
	/// @return 
	tpString getNetmask();
	/// @brief 设置网卡子网掩码
	/// @param addr 
	/// @return 
	tpInt32 setNetmask(const tpString &addr);
	/// @brief 获取广播地址
	/// @return 
	tpString getBroadAddr(); 
	/// @brief 设置广播地址
	/// @param addr 
	/// @return 
	tpInt32 setBroadAddr(const tpString &addr);
	/// @brief 获取IPV6地址
	/// @return 
	tpString getAddrIpv6();
	/// @brief 设置IPV6地址
	/// @param addr 
	/// @return 
	tpInt32 setAddrIpv6(const tpString &addr);
	/// @brief 开始扫描无线网络
	/// @return 
	tpInt32 startScan();
	/// @brief 停止无线网络扫描
	/// @return 
	tpInt32 stopScan();
	/// @brief 获取扫描结果
	/// @return 
	tpList<tpWirelessInfo> getScan();

	/// @brief 连接到加密无线网络
	/// @param ssid wifi名称
	/// @param psk wifi密码
	/// @param timeout 超时时间，毫秒
	/// @return 
	tpInt32 connectWireless(const tpString &ssid, const tpString &psk="",tpUInt32 timeout=50000);
	/// @brief 断开无线网络的连接
	/// @return 
	tpInt32 disconnectWireless();
	/// @brief 获取连接的wifi的SSID
	/// @return 
	tpString getWirelessSsid();
	/// @brief 设置本机热点的的SSID
	/// @param ssid 
	/// @return 
	tpInt32 setHotspotSsid(const tpString &ssid);
	/// @brief 设置本机热点的密码
	/// @param password 
	/// @return 
	tpInt32 setHotspotPwd(const tpString &password);
	/// @brief 打开本机热点
	/// @return 
	tpInt32 openHotspot();

public
signals:
    declare_signal(wirelessRemove, tpString);
	declare_signal(wirelessAdd, tpWirelessInfo);


private:
	tpInt32 setConf(const tpString &name, uint16_t type, const tpString &addr);
	tpInt32 getConf(const tpString &name, uint16_t type, tpString &addr);
	tpInt32 setStatus(bool status);
	tpInt32 getStatus(bool *status);
	int threadScan(tpUInt16 time);
private:
	ItpNetworkInterfaceData *data_;
};




#endif
