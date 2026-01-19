#ifndef __GATEWAY_SERVER_PUB_SUB_H
#define __GATEWAY_SERVER_PUB_SUB_H

#include <atomic>
#include <memory>
#include "TpString.h"

class TpGatewayServerPUBSUB
{
public:
    virtual ~TpGatewayServerPUBSUB() = default;

    virtual bool start(uint16_t tcp_port = 5555) = 0;
    virtual void stop() = 0;

    // virtual size_t getClientCount() const = 0;
    virtual size_t getMessageRate() = 0;
};
std::shared_ptr<TpGatewayServerPUBSUB> createGatewayServer();

#endif // GATEWAY_SERVER_H