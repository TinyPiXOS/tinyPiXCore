#ifndef __TP_HARDWARE_DEVICE_H
#define __TP_HARDWARE_DEVICE_H

#include "TpUtils.h"

TP_DEF_VOID_TYPE_VAR(ITpHardwareDevice);

class TpHardwareDevice
{
public:
    virtual ~TpHardwareDevice() = default;

    // 核心功能接口
    virtual tpBool open() = 0;
    virtual void close() = 0;
    virtual ssize_t read(uint8_t* buffer, size_t length) = 0;
    virtual ssize_t write(const uint8_t* data, size_t length) = 0;

    // 可选：配置接口
    // virtual void setSpeed(uint32_t frequency_hz) { /* 默认实现可为空 */ }
    // virtual bool probeAddress(uint8_t address) = 0; // 可选：地址探测功能
};



#endif