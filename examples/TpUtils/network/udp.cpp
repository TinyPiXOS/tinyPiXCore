#include <iostream>
#include "Network/TpUdpSocket.h"
#include "TpString.h"

void example_udp()
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

int main()
{
	example_udp();
	return 0;
}