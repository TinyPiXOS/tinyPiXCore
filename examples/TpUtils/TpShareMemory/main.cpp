#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include "TpShareMemory.h"

// 配置数据结构
struct ConfigData
{
    int max_connections;
    double timeout_seconds;
    char server_ip[16];
    bool enable_logging;
};

int main(int argc, char *argv[])
{
    try
    {
        // 进程A创建共享内存（1KB大小）
        TpShareMemory config_share("app_config", 1024, true);

        if (!config_share.isMapped())
        {
            std::cerr << "Failed to create shared memory" << std::endl;
            return 1;
        }

        std::cout << "Process A: Shared memory created successfully" << std::endl;

        ConfigData config;
        config.max_connections = 100;
        config.timeout_seconds = 30.0;
        strcpy(config.server_ip, "192.168.1.1");
        config.enable_logging = true;

        // 写入配置数据
        if (config_share.writeData(&config, sizeof(config)))
        {
            std::cout << "Process A: Configuration written successfully" << std::endl;
            std::cout << "Current version: " << config_share.getVersion() << std::endl;
        }

        // 模拟配置更新（每5秒更新一次）
        for (int i = 0; i < 3; ++i)
        {
            std::this_thread::sleep_for(std::chrono::seconds(5));

            config.max_connections += 10;
            config.timeout_seconds += 5.0;

            if (config_share.writeData(&config, sizeof(config)))
            {
                std::cout << "Process A: Configuration updated to version "
                          << config_share.getVersion() << std::endl;
            }
        }

        std::cout << "Process A: Exiting..." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Process A Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
