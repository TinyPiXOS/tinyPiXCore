#include <iostream>
#include "TpNetworkInterface.h"
#include "TpWirelessInfo.h"
#include "TpTcpServer.h"
#include "TpTcpSocket.h"
#include "TpUdpSocket.h"
#include "TpString.h"


int example_wireless()
{
	TpNetworkInterface device("wlx502b73e07098");
	if(device.isWireless())
		std::cout << "this is wireless" << std::endl;
	else	
		return 0;
	if(device.isOnline())
	{
		std::cout << "已连接网络" << std::endl;
	}

	if(device.startScan()<0)
		std::cout << "不能扫描\n";

	connect(&device, wirelessRemove, [=](TpString name)
            { std::cout << "[Signal]网络消失：" << name<< std::endl; });

	connect(&device, wirelessAdd, [=](TpWirelessInfo wifi)
            { std::cout << "[Signal]新添网络：" << wifi.getSsid() << "  \tLevel: "<< wifi.getLevel()<<std::endl; });

	while(1);

//	std::cout << "网络SSID:" << device.getWirelessSsid()<< std::endl;
	std::cout << "准备连接\n";
	device.connectWireless("哲思亿佳","zerseager@");
	std::cout << "连接完成\n";
	while(1);
}


int32_t example_printf_device(void)
{
	auto interfaces = TpNetworkInterface::getAllDevice();
	TpString addr;
	for(auto &iface : interfaces)    //printf device name         
	{
		std::cout << "device name: " << iface.getName() << std::endl;
	}

	TpString ifname("ens33");
    TpNetworkInterface network(ifname);
	std::cout << "device Active:" << (network.isOpenDevice()==TP_TRUE ? "Yse" : "No") << std::endl;
    std::cout << "device name: " << network.getName() << std::endl;
	std::cout << "device mac:"  << network.getMacAddr() << std::endl;
    std::cout << "device manu: " << network.getManu() << std::endl;
    std::cout << "is wireless: " << (network.isWireless()==TP_TRUE ? "Yse" : "No") << std::endl;
	std::cout << "Physical interface status:" << (network.isOnline()==TP_TRUE ? "Online" : "No") << std::endl;
	std::cout << "Network Status:" << (network.isOnlineInternet()==TP_TRUE ? "OnlineInternet" : "No")  << std::endl;
	return 0;
}

int32_t example_udp()
{
    TpUdpSocket udp_s,udp_r;
    tpUInt8 send_buf[20]="test data";
    tpUInt8 recv_buf[1024];
    TpString addr_s="0.0.0.0";
    TpString addr_d="192.168.1.32";
    TpString addr_r="000.000.000.000";
    uint16_t port_r;
    udp_r.bind(addr_s,8000);
    if(udp_s.sendTo(send_buf,10,addr_d,8001)<0)
        std::cout << "send data error" << std::endl;
    /*while(1)
    {
        if(udp_r.recvFrom(recv_buf,sizeof(recv_buf),addr_r,&port_r)>0)
        {
            std::cout << "recv:" << recv_buf <<std::endl;
            if(udp_s.sendTo(send_buf,10,addr_d,8001)<0)
                std::cout << "send data error" << std::endl;
        }
    }*/

	connect(&udp_r, TpUdpSocket::readyRead, [&]() {
        while (udp_r.hasPendingDatagrams()) {
            auto datagram = udp_r.recvDatagram(1024);
            std::cout << "Local " << datagram.destinationAddress()<< ":" << datagram.destinationPort()<<std::endl;
			std::cout << "Received from " << datagram.senderAddress()<< ":" << datagram.senderPort()<<std::endl;
			std::cout << "Received data " << datagram.size()<< ":" << datagram.data()<<std::endl;
			std::cout << std::endl;
        }
    });
	while (1);
}
 


int32_t example_tcp_server()
{
    TpTcpServer tcp_s;
	TpList<TpTcpSocket *> client_list;
    TpString addr_s="0.0.0.0";
    tpUInt8 recv_buf[1024];
    tpUInt8 send_buf[20]="recv data";
    tcp_s.listen(addr_s,8001);

	connect(&tcp_s, TpTcpServer::newConnection, [&]() {
		TpTcpSocket *tcp_c=tcp_s.nextPendingConnection();
		if (tcp_c) 
		{
			std::cout << "New client from " << tcp_c->getPeerAddress() << ":" << tcp_c->getPeerAddress() << std::endl;

			connect(tcp_c, TpTcpSocket::readyRead, [=](TpTcpSocket *client) {
				tpUInt8 buf[1024];
				buf[20]='\0';
				tpInt64 n = client->recv(buf, sizeof(buf));
				if (n > 0) {
					std::cout << "Received: " << buf << std::endl;
				}
			});
			connect(tcp_c, TpTcpSocket::disconnected, [=](TpTcpSocket *client) {
				std::cout << "Client disconnected: "
							<< client->getPeerAddress() << ":" << client->getPeerPort() << std::endl;
			});
		}


    });
	while(1);
    tcp_s.close();
}

int32_t example_tcp_client()
{
    TpTcpSocket tcp_c;
    TpString addr_s="192.168.1.32";
    tpUInt8 send_buf[20]="client data";
	tcp_c.connectToHost(addr_s,8000);
	connect(&tcp_c,TpTcpSocket::connected,[](){
		std::cout << "Client connected ok" << std::endl;
	});
	
    while(1)
    {
        if(tcp_c.send(send_buf,11)<0)
            std::cout << "send data error" << std::endl;
		
		sleep(1);
    }
    tcp_c.close();
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
	printf("gateway:%s\n",network.getGatway().c_str());
	printf("netmask:%s\n",network.getNetmask().c_str());
	printf("DNS:\n");
	TpList<TpString> list_=network.getDns();
	for(TpString &it:list_)
	{
		printf("\t%s\n",it.c_str());
	}
}

int example_network()
{
	TpNetworkInterface network("ens33");
	network.setAddr(TpString("192.168.1.200"));
	TpString getAddr();
	printf("Arrd:%s\n",network.getAddr().c_str());
	
	network.setNetmask(TpString("255.255.255.0"));
	printf("Netmask:%s\n",network.getNetmask().c_str());

	network.setBroadAddr(TpString("192.168.1.255"));
	printf("BroadAddr:%s\n",network.getBroadAddr().c_str());

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
}

int example_hotspot()
{
	TpNetworkInterface device("wlx502b73e07098");
	TpString ssid("tinyPiX WIFI");
	TpString pwd("tinyPiX");
	printf("设置ssid\n");
	device.setHotspotSsid(ssid);
	printf("设置pwd\n");
	device.setHotspotPwd(pwd);
	printf("热点开启\n");
	device.openHotspot();
	
}

int32_t main(int32_t argc, char *argv[])
{
//	example_is_net();
//	example_hotspot();
//	example_printf_device();
//	example_dhcp();
//    example_printf_device();
//	example_wireless();//
//    example_udp();
    example_tcp_server();
//   example_tcp_client();
    return 0;
}

