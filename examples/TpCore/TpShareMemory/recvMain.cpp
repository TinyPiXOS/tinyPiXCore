#include "TpShareMemory.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

struct ConfigData
{
    int max_connections;
    double timeout_seconds;
    char server_ip[16];
    bool enable_logging;
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <process_name>" << std::endl;
        return 1;
    }

    std::string process_name = argv[1];

    try
    {
        // 进程B/C/D打开已存在的共享内存
        TpShareMemory config_share("app_config", 1024, false);

        if (!config_share.isMapped())
        {
            std::cerr << process_name << ": Failed to open shared memory" << std::endl;
            return 1;
        }

        std::cout << process_name << ": Connected to shared memory" << std::endl;

        uint32_t last_version = 0;
        ConfigData config;

        // 持续监控配置变化
        while (true)
        {
            uint32_t current_version = config_share.getVersion();

            if (current_version != last_version)
            {
                // 配置已更新，读取新配置
                if (config_share.readData(&config, sizeof(config)))
                {
                    std::cout << process_name << ": Config updated (v" << current_version << ")" << std::endl;
                    std::cout << "  Max connections: " << config.max_connections << std::endl;
                    std::cout << "  Timeout: " << config.timeout_seconds << "s" << std::endl;
                    std::cout << "  Server IP: " << config.server_ip << std::endl;
                    std::cout << "  Logging: " << (config.enable_logging ? "enabled" : "disabled") << std::endl;
                    std::cout << "------------------------" << std::endl;

                    last_version = current_version;
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << process_name << " Error: " << e.what() << std::endl;

        // 共享内存不存在，等待进程A创建
        if (std::string(e.what()).find("Failed to initialize") != std::string::npos)
        {
            std::cout << process_name << ": Shared memory not available, waiting for Process A..." << std::endl;
        }

        return 1;
    }

    return 0;
}