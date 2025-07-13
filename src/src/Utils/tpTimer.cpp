#include "tpTimer.h"
#include "tpUtils.h"

#include <atomic>
#include <chrono>
#include <thread>
#include <functional>

struct tpTimerData
{
    // 定时器是否正在执行
    std::atomic<bool> active;
    // 定时器间隔事件Ms
    std::atomic<uint32_t> intervalMs;
    // 定时器类型
    std::atomic<tpTimer::TimerType> type;
    // 定时器全局ID，进程内唯一
    uint32_t timerId;

    std::thread timerThread;
    std::chrono::steady_clock::time_point nextTriggerTime;

    virtual ~tpTimerData() 
    {
        if (timerThread.joinable()) 
        {
            timerThread.join();  // 等待线程结束
            // 或 t.detach();  // 分离线程（后台运行）
        }
    }
};

tpTimer::tpTimer(int32_t msec)
{
    timerSet_ = new tpTimerData();

    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);

    timerData->active = false;
    timerData->intervalMs = msec;
    timerData->type = tpTimer::CoarseTimer;
    timerData->timerId = generateTimerId();
}

tpTimer::~tpTimer()
{
    if (isActive())
        stop();

    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);

    if (timerData)
    {
        delete timerData;
        timerData = nullptr;
        timerSet_ = nullptr;
    }
}

bool tpTimer::isActive() const
{
    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);
    if (!timerData)
        return false;

    return timerData->active.load();
}

uint32_t tpTimer::timerId() const
{
    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);
    if (!timerData)
        return 0;

    return timerData->timerId;
}

void tpTimer::setInterval(uint32_t msec)
{
    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);
    if (!timerData)
        return;

    timerData->intervalMs.store(msec);
}

uint32_t tpTimer::interval() const
{
    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);
    if (!timerData)
        return 0;

    return timerData->intervalMs.load();
}

void tpTimer::setTimerType(tpTimer::TimerType atype)
{
    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);
    if (!timerData)
        return;

    timerData->type.store(atype);
}

tpTimer::TimerType tpTimer::timerType()
{
    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);
    if (!timerData)
        return tpTimer::CoarseTimer;

    return timerData->type.load();
}

void tpTimer::start(int32_t msec)
{
    setInterval(msec);
    start();
}

void tpTimer::start()
{
    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);
    if (!timerData)
        return;

    if (!timerData->active.load())
    {
        timerData->active.store(true);
        timerData->nextTriggerTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(timerData->intervalMs.load());
        timerData->timerThread = std::thread(&tpTimer::timerFunction, this);
        if (!timerData->timerThread.joinable())
        {
            // 线程创建失败
            std::cerr << "Failed to create timer thread." << std::endl;
            timerData->active.store(false);
        }
    }
}

void tpTimer::stop()
{
    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);
    if (!timerData)
        return;

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

void tpTimer::sleep(const uint64_t &msec)
{
    // 休眠 500 毫秒
    std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

void tpTimer::timerFunction()
{
    tpTimerData *timerData = static_cast<tpTimerData *>(timerSet_);
    if (!timerData)
        return;

    while (timerData->active.load())
    {
        auto now = std::chrono::steady_clock::now();
        if (now >= timerData->nextTriggerTime)
        {
            // 定时器触发
            // std::cout << "Timer " << timerId << " triggered." << std::endl;
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
