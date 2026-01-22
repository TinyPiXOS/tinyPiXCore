#include <iostream>
#include "Network/TpTcpServer.h"
#include "Network/TpTcpSocket.h"
#include "TpString.h"



void example_tcp_client()
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


int main()
{
	example_tcp_client();
	return 0;
}