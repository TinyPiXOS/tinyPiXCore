#ifndef __GATEWAY_SERVER_REP_H
#define __GATEWAY_SERVER_REP_H

#include <string>
#include <atomic>
#include <memory>

class TpGatewayServerREP
{
public:
    virtual ~TpGatewayServerREP() = default;

    virtual bool start(uint16_t tcp_port = 6555) = 0;
    virtual void stop() = 0;

    virtual int32_t getRequestCount() const = 0;
};
std::shared_ptr<TpGatewayServerREP> createGatewayREPServer();

#endif // GATEWAY_SERVER_H