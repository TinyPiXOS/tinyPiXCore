#include <iostream>
#include "Network/TpTcpServer.h"
#include "Network/TpTcpSocket.h"
#include "TpString.h"


void example_tcp_server()
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

int main()
{

	example_tcp_server();
	return 0;
}