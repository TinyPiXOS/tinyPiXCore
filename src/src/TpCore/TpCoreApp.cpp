#include "TpCoreApp.h"
#include <TpCoreApp_p.h>
#include <TpTimer.h>
// #include <csignal>

TpCoreApp::TpCoreApp(int32_t argc, char *argv[])
{
    // 注册信号处理
    // signal(SIGINT, coreAppSignalHandle);
    // signal(SIGTERM, coreAppSignalHandle);

    TpCoreAppData *coreData = new TpCoreAppData();
    coreData->mainThreadId = std::this_thread::get_id();

    coreData->running = true;
    coreData->waitRun = false;

    data_ = coreData;

    if (appPtr)
    {
        std::cout << "detects app instance more once!, exit......" << std::endl;
        std::exit(0);
    }
    appPtr = this;
}

TpCoreApp::~TpCoreApp()
{
    TpCoreAppData *coreData = static_cast<TpCoreAppData *>(data_);
    if (coreData)
    {
        delete coreData;
        coreData = nullptr;
        data_ = nullptr;
    }
}

TpCoreApp *TpCoreApp::Inst()
{
    return appPtr;
}

bool TpCoreApp::run()
{
    TpCoreAppData *coreAppData = static_cast<TpCoreAppData *>(TpCoreApp::data_);
    if (!coreAppData)
        return coreAppData->running;

    coreAppData->waitRun = true;

    while (coreAppData->running)
    {
        // 异步调用信号槽
        std::queue<std::function<void()>> cacheTaskList;

        {
            std::lock_guard<std::mutex> lock(coreAppData->queueSlotMutex_);
            cacheTaskList = coreAppData->slotTasks_;
            coreAppData->slotTasks_ = std::queue<std::function<void()>>();
        }

        while (!cacheTaskList.empty())
        {
            auto task = cacheTaskList.front();
            cacheTaskList.pop();
            task();
        }

        TpTimer::sleep(16);
    }

    return coreAppData->running;
}

bool TpCoreApp::isMainThread()
{
    TpCoreAppData *coreAppData = static_cast<TpCoreAppData *>(data_);
    return std::this_thread::get_id() == coreAppData->mainThreadId;
}

void TpCoreApp::postEvent(std::function<void()> task)
{
    TpCoreAppData *coreAppData = static_cast<TpCoreAppData *>(data_);

    if (!coreAppData->running)
        return;

    {
        std::lock_guard<std::mutex> lock(coreAppData->queueSlotMutex_);
        coreAppData->slotTasks_.push(task);
    }
}
