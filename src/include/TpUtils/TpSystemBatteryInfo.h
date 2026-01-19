#ifndef __TP_SYSTEM_BATTERY_INFO_H
#define __TP_SYSTEM_BATTERY_INFO_H

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ITpSystemBatteryInfoData);

class TpSystemBatteryInfo
{
public:
    enum SystemPowerState
    {
        TP_SYSTEM_POWER_UNKNOWN = 0, // 未知
        TP_SYSTEM_POWER_CHARGE,      // 充电中
        TP_SYSTEM_POWER_DISCHARGE,   // 放电中
        TP_SYSTEM_POWER_FULL         // 已充满
    };

public:
    TpSystemBatteryInfo();
    ~TpSystemBatteryInfo();

public:
    /// @brief 获取系统电源状态
    /// @return
    TpSystemBatteryInfo::SystemPowerState getState();
    /// @brief 获取电池电压(暂无法测试)
    /// @return
    double getVoltage();
    /// @brief 获取电池充/放电电流(暂不支持)
    /// @return
    double getCurrent();
    /// @brief 获取电池当前容量
    /// @return 返回百分比
    int getBatteryLevel();
    /// @brief 是否在充电
    /// @return
    tpBool isCharging();
    /// @brief 返回电池剩余容量(暂不支持)
    /// @return
    int remainingCapacity();

private:
    ITpSystemBatteryInfoData *data_;
    int getPath();
    int getType(void *prop);
};

#endif