#include "TpTimer.h"
#include <TpCore.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <functional>

struct TpTimerData
{
    // 定时器是否正在执行
    std::atomic<bool> active;
    // 定时器间隔事件Ms
    std::atomic<uint32_t> intervalMs;
    // 定时器类型
    std::atomic<TpTimer::TimerType> type;
    // 定时器全局ID，进程内唯一
    uint32_t timerId;

    std::thread timerThread;
    std::chrono::steady_clock::time_point nextTriggerTime;

    std::mutex threadMutex;

    virtual ~TpTimerData()
    {
        if (timerThread.joinable())
        {
            timerThread.join(); // 等待线程结束
            // 或 t.detach();  // 分离线程（后台运行）
        }
    }
};

TpTimer::TpTimer(int32_t msec)
{
    timerSet_ = new TpTimerData();

    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);

    timerData->active = false;
    timerData->intervalMs = msec;
    timerData->type = TpTimer::CoarseTimer;
    timerData->timerId = generateTimerId();
}

TpTimer::~TpTimer()
{
    if (isActive())
        stop();

    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);

    if (timerData)
    {
        delete timerData;
        timerData = nullptr;
        timerSet_ = nullptr;
    }
}

bool TpTimer::isActive() const
{
    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);
    if (!timerData)
        return false;

    return timerData->active.load();
}

uint32_t TpTimer::timerId() const
{
    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);
    if (!timerData)
        return 0;

    return timerData->timerId;
}

void TpTimer::setInterval(uint32_t msec)
{
    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);
    if (!timerData)
        return;

    timerData->intervalMs.store(msec);
}

uint32_t TpTimer::interval() const
{
    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);
    if (!timerData)
        return 0;

    return timerData->intervalMs.load();
}

void TpTimer::setTimerType(TpTimer::TimerType atype)
{
    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);
    if (!timerData)
        return;

    timerData->type.store(atype);
}

TpTimer::TimerType TpTimer::timerType()
{
    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);
    if (!timerData)
        return TpTimer::CoarseTimer;

    return timerData->type.load();
}

void TpTimer::start(int32_t msec)
{
    setInterval(msec);
    start();
}

void TpTimer::start()
{
    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);
    if (!timerData)
        return;

    // 先完全停止并清理旧线程
    this->stop();

    std::lock_guard<std::mutex> lock(timerData->threadMutex);

    if (!timerData->active.load())
    {
        // 确保线程已停止
        int32_t sleepMaxWait = 1000; 
        while (timerData->timerThread.joinable() && sleepMaxWait-- > 0)
        {
            this->sleep(2);
        }

        timerData->active.store(true);
        timerData->nextTriggerTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(timerData->intervalMs.load());
        // std::cout << "线程状态： " << timerData->timerThread.joinable() << std::endl;
        timerData->timerThread = std::thread(&TpTimer::timerFunction, this);
        if (!timerData->timerThread.joinable())
        {
            // 线程创建失败
            std::cerr << "Failed to create timer thread." << std::endl;
            timerData->active.store(false);
        }
    }
}

void TpTimer::stop()
{
    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);
    if (!timerData)
        return;

    std::lock_guard<std::mutex> lock(timerData->threadMutex);

    if (timerData->active.load())
    {
        timerData->active.store(false);
        if (timerData->timerThread.joinable())
        {
            // stop可能在timer的线程函数中执行，不能直接join会死锁，在新线程等待
            // 判断当前线程是否是定时器线程
            if (std::this_thread::get_id() == timerData->timerThread.get_id())
            {
                // 在独立线程中执行 join()
                std::thread([=]()
                            {
                    if (timerData->timerThread.joinable()) {
                        timerData->timerThread.join();
                    } })
                    .detach();
            }
            else
            {
                timerData->timerThread.join();
            }
        }
    }
}

void TpTimer::sleep(const uint64_t &msec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

void TpTimer::timerFunction()
{
    TpTimerData *timerData = static_cast<TpTimerData *>(timerSet_);
    if (!timerData)
        return;

    while (timerData->active.load())
    {
        auto now = std::chrono::steady_clock::now();
        if (now >= timerData->nextTriggerTime)
        {
            // 定时器触发
            // std::cout << "Timer " << timerId << " triggered." << std::endl;
            // std::cout << "定时器超时事件发送 "  << std::endl;
            timeout.emit();

            // 根据定时器类型调整下一次触发时间
            switch (timerData->type.load())
            {
            case PreciseTimer:
                timerData->nextTriggerTime += std::chrono::milliseconds(timerData->intervalMs.load());
                break;
            case CoarseTimer:
                timerData->nextTriggerTime += std::chrono::milliseconds(timerData->intervalMs.load() * 1);
                break;
            case VeryCoarseTimer:
                timerData->nextTriggerTime += std::chrono::milliseconds(timerData->intervalMs.load() * 1);
                break;
            }
        }
        else
        {
            std::this_thread::sleep_until(timerData->nextTriggerTime);
        }
    }
}
