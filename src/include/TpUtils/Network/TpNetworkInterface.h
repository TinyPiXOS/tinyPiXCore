#ifndef __TP_NETWORK_INTERFACE_H
#define __TP_NETWORK_INTERFACE_H


#include <TpCore.h>
#include "TpSignalSlot.h"
#include "TpWirelessInfo.h"

TP_DEF_VOID_TYPE_VAR(ITpNetworkInterfaceData);

class TpNetworkInterface
{
public:
	TpNetworkInterface(const char *name);
	TpNetworkInterface(const TpString& name);
	~TpNetworkInterface();
public:
	/// @brief 获取所有网卡硬件
	/// @return 
	static TpList<TpNetworkInterface> getAllDevice();
	/// @brief 获取网卡接口名字
	/// @return 
	TpString getName() const;
	/// @brief 获取网卡制造商
	/// @return 
	TpString getManu();
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
	tpInt32 setStatic(const TpString& ip, const TpString& gatway, const TpString &netmask,TpList<TpString>& dns);
	/// @brief 判断自动DHCP是否打开
	/// @return 
	tpBool isDhcp();
	/// @brief 获取网关
	/// @return 
	TpString getGatway();
	/// @brief 设置网关
	/// @param gatway 
	/// @return 
	tpInt32 setGatway(const TpString& gatway);
	/// @brief 获取DNS
	/// @return 
	TpList<TpString> getDns();
	/// @brief 设置DNS
	/// @param dns_list dns列表
	/// @return 
	tpInt32 setDns(tpBool autoDns, const TpList<TpString>& dnsList = TpList<TpString>());
	/// @brief 是否是静态的DNS
	/// @return 
	tpBool isStaticDns();
	/// @brief 获取网卡IP地址
	/// @return 
	TpString getAddr();
	/// @brief 设置网卡IP地址
	/// @param addr 
	/// @return 
	tpInt32 setAddr(const TpString &addr);
	/// @brief 获取网卡MAC地址
	/// @return 
	TpString getMacAddr();
	/// @brief 设置网卡MAC地址
	/// @param addr 
	/// @return 
	tpInt32 setMacAddr(const TpString &addr);
	/// @brief 获取网卡子网掩码
	/// @return 
	TpString getNetmask();
	/// @brief 设置网卡子网掩码
	/// @param addr 
	/// @return 
	tpInt32 setNetmask(const TpString &addr);
	/// @brief 获取广播地址
	/// @return 
	TpString getBroadAddr(); 
	/// @brief 设置广播地址
	/// @param addr 
	/// @return 
	tpInt32 setBroadAddr(const TpString &addr);
	/// @brief 获取IPV6地址
	/// @return 
	TpString getAddrIpv6();
	/// @brief 设置IPV6地址
	/// @param addr 
	/// @return 
	tpInt32 setAddrIpv6(const TpString &addr);
	/// @brief 开始扫描无线网络
	/// @return 
	tpInt32 startScan();
	/// @brief 停止无线网络扫描
	/// @return 
	tpInt32 stopScan();
	/// @brief 获取扫描结果
	/// @return 
	TpList<TpWirelessInfo> getScan();

	/// @brief 连接到加密无线网络
	/// @param ssid wifi名称
	/// @param psk wifi密码
	/// @param timeout 超时时间，毫秒
	/// @return 
	tpInt32 connectWireless(const TpString &ssid, const TpString &psk="",tpUInt32 timeout=50000);
	/// @brief 断开无线网络的连接
	/// @return 
	tpInt32 disconnectWireless();
	/// @brief 获取连接的wifi的SSID
	/// @return 
	TpString getWirelessSsid();
	/// @brief 设置本机热点的的SSID
	/// @param ssid 
	/// @return 
	tpInt32 setHotspotSsid(const TpString &ssid);
	/// @brief 设置本机热点的密码
	/// @param password 
	/// @return 
	tpInt32 setHotspotPwd(const TpString &password);
	/// @brief 打开本机热点
	/// @return 
	tpInt32 openHotspot();

public
signals:
    declare_signal(wirelessRemove, TpString);
	declare_signal(wirelessAdd, TpWirelessInfo);


private:
	tpInt32 setConf(const TpString &name, uint16_t type, const TpString &addr);
	tpInt32 getConf(const TpString &name, uint16_t type, TpString &addr);
	tpInt32 setStatus(bool status);
	tpInt32 getStatus(bool *status);
	int threadScan(tpUInt16 time);
private:
	ITpNetworkInterfaceData *data_;
};




#endif
