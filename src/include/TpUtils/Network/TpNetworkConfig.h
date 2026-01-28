#ifndef __TP_NETWORK_CONFIG_H
#define __TP_NETWORK_CONFIG_H

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ITpNetworkConfigData);

class TpNetworkConfig
{
public:
	TpNetworkConfig(const TpString &name);
	~TpNetworkConfig();

public:
	tpInt32 openDevice();
	tpInt32 closeDevice();
	tpBool isOpenDevice();

	/// @brief 获取网卡IP地址
    /// @return
    TpString addr();
    /// @brief 设置网卡IP地址
    /// @param addr
    /// @return
    tpInt32 setAddr(const TpString &addr);

    /// @brief 获取网卡MAC地址
    /// @return
    TpString macAddr();
    /// @brief 设置网卡MAC地址
    /// @param addr
    /// @return
    tpInt32 setMacAddr(const TpString &addr);

    /// @brief 获取网卡子网掩码
    /// @return
    TpString netmask();
    /// @brief 设置网卡子网掩码
    /// @param addr
    /// @return
    tpInt32 setNetmask(const TpString &addr);

    /// @brief 获取广播地址
    /// @return
    TpString broadAddr();
    /// @brief 设置广播地址
    /// @param addr
    /// @return
    tpInt32 setBroadAddr(const TpString &addr);

    /// @brief 获取IPV6地址
    /// @return
    TpString addrIpv6();
    /// @brief 设置IPV6地址
    /// @param addr
    /// @return
    tpInt32 setAddrIpv6(const TpString &addr);

    /// @brief 获取网关
    /// @return
    TpString gatway();
    /// @brief 设置网关
    /// @param gatway
    /// @return
    tpInt32 setGatway(const TpString &gatway);

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
    tpInt32 setStatic(const TpString &ip, const TpString &gatway, const TpString &netmask, TpList<TpString> &dns);
	tpBool isDhcp();
	/// @brief 获取DNS
	/// @return
	TpList<TpString> dns();
	/// @brief 设置DNS
	/// @param dns_list dns列表
	/// @return
	tpInt32 setDns(tpBool autoDns, const TpList<TpString> &dnsList = TpList<TpString>());
	/// @brief 是否是静态的DNS
	/// @return
	tpBool isStaticDns();

private:
	tpInt32 setStatus(bool status);
	tpInt32 getStatus(bool* status);
private:
	ITpNetworkConfigData* data_;
};


#endif
