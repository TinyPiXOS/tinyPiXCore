#include <iostream>
#include "Network/TpNetworkInterface.h"
#include "Network/TpWirelessInfo.h"
#include "Network/TpTcpServer.h"
#include "Network/TpTcpSocket.h"
#include "Network/TpUdpSocket.h"
#include "TpString.h"


int32_t example_printf_device(void)
{

	TpList<std::shared_ptr<TpNetworkInterface>> interfaces = TpNetworkInterface::allDevice();
	TpString addr;
	for(const auto &iface : interfaces)    //printf device name         
	{
		std::cout << "device name: " << iface->name() << std::endl;
	}

	TpString ifname("ens33");
    TpNetworkInterface network(ifname);
	std::cout << "device Active:" << (network.isOpenDevice()==TP_TRUE ? "Yse" : "No") << std::endl;
    std::cout << "device name: " << network.name() << std::endl;
	std::cout << "device mac:"  << network.macAddr() << std::endl;
    std::cout << "device manu: " << network.manu() << std::endl;
    std::cout << "is wireless: " << (network.isWireless()==TP_TRUE ? "Yse" : "No") << std::endl;
	std::cout << "Physical interface status:" << (network.isOnline()==TP_TRUE ? "Online" : "No") << std::endl;
	std::cout << "Network Status:" << (network.isOnlineInternet()==TP_TRUE ? "OnlineInternet" : "No")  << std::endl;
	return 0;
}


int example_dhcp()
{
	TpNetworkInterface network("ens33");
	TpString ip("192.168.183.200");
	TpString gatway("192.168.183.1");
	TpString dns("192.168.183.2");
	TpString netmsk("255.255.255.0");


	printf("DHCP状态%d\n",network.isDhcp()==TP_TRUE ? 1:0);
	printf("\n\n设置DNS:\n");
	TpList<TpString> dns_list;
	dns_list.emplace_back("192.168.183.2");
	dns_list.emplace_back("8.8.8.8");
	dns_list.emplace_back("1.1.1.1");
	dns_list.emplace_back("6.6.6.6");
	network.setDns(TP_FALSE,dns_list);
	sleep(5);
	printf("\n\n设置打开DHCP:\n");
	network.setDhcp();
	sleep(5);
	printf("DHCP状态%d\n",network.isDhcp()==TP_TRUE ? 1:0);
	printf("\n\n设置关闭DHCP:\n");
	network.setStatic(ip, gatway, netmsk,dns_list);
	network.setDns(TP_TRUE);
	sleep(5);
	printf("DNS状态%s\n",network.isStaticDns()==TP_TRUE ? "静态":"动态");
	printf("DHCP状态%d\n",network.isDhcp()==TP_TRUE ? 1:0);
	printf("gateway:%s\n",network.gatway().c_str());
	printf("netmask:%s\n",network.netmask().c_str());
	printf("DNS:\n");
	TpList<TpString> list_=network.dns();
	for(TpString &it:list_)
	{
		printf("\t%s\n",it.c_str());
	}
	return 0;
}

int example_network()
{
	TpNetworkInterface network("ens33");
	network.setAddr(TpString("192.168.1.200"));
	printf("Arrd:%s\n",network.addr().c_str());
	
	network.setNetmask(TpString("255.255.255.0"));
	printf("Netmask:%s\n",network.netmask().c_str());

	network.setBroadAddr(TpString("192.168.1.255"));
	printf("BroadAddr:%s\n",network.broadAddr().c_str());
	return 0;
}

int example_is_net()
{
	TpNetworkInterface network("ens33");
	std::cout << "网络状态？" << std::endl;
	if(network.isOnlineInternet())
	{
		std::cout << "已连接网络" << std::endl;
	}
	else
		std::cout << "没有连接网络" << std::endl;
	return 0;
}

int32_t main(int32_t argc, char *argv[])
{
//	example_is_net();
//	example_printf_device();
//	example_dhcp();
//    example_printf_device();
    return 0;
}

