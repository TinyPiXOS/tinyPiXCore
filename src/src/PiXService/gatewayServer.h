#ifndef GATEWAY_SERVER_H
#define GATEWAY_SERVER_H

#include <string>
#include <atomic>
#include <memory>

class GatewayServer
{
public:
    virtual ~GatewayServer() = default;

    virtual bool start(uint16_t tcp_port = 5555) = 0;
    virtual void stop() = 0;

    // virtual size_t getClientCount() const = 0;
    virtual size_t getMessageRate() = 0;
};

std::shared_ptr<GatewayServer> createGatewayServer();

#endif // GATEWAY_SERVER_H