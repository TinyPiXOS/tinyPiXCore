#include "tpThreadPool.h"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <algorithm>

struct tpThreadPoolData
{
    std::vector<std::thread> threads_;        ///< 工作线程集合
    std::queue<std::function<void()>> tasks_; ///< 待处理任务队列
    std::mutex queueMutex_;                   ///< 任务队列互斥锁
    std::condition_variable condition_;       ///< 任务通知条件变量
    std::condition_variable idleCondition_;   ///< 空闲线程通知
    bool stop_ = false;                       ///< 停止标志
    const uint32_t minThreads_;               ///< 最小线程数
    const uint32_t maxThreads_;               ///< 最大线程数

    tpThreadPoolData(uint32_t minThreads, uint32_t maxThreads)
        : minThreads_(std::max(minThreads, uint32_t(1))),
          maxThreads_(std::max(maxThreads, minThreads_)) {}
};

// 替代std::clamp的兼容实现
template <typename T>
const T &clampCompat(const T &value, const T &min, const T &max)
{
    return (value < min) ? min : (value > max) ? max
                                               : value;
}

tpThreadPool::tpThreadPool()
{
    uint32_t cores = std::thread::hardware_concurrency();

    tpThreadPoolData *poolData = new tpThreadPoolData(cores * 2, cores * 4);
    data_ = poolData;

    // 初始化线程
    uint32_t initThreads = clampCompat(cores * 2,
                                       poolData->minThreads_, poolData->maxThreads_);

    for (uint32_t i = 0; i < initThreads; ++i)
    {
        poolData->threads_.emplace_back([this]
                                        { workerTask(); });
    }
}

tpThreadPool::tpThreadPool(const uint32_t &minThreads, const uint32_t &maxThreads)
{
    tpThreadPoolData *poolData = new tpThreadPoolData(minThreads, maxThreads);
    data_ = poolData;

    // 初始化线程
    uint32_t initThreads = poolData->minThreads_;
    for (uint32_t i = 0; i < initThreads; ++i)
    {
        poolData->threads_.emplace_back([this]
                                        { workerTask(); });
    }
}

tpThreadPool::~tpThreadPool()
{
    stop();

    tpThreadPoolData *poolData = static_cast<tpThreadPoolData *>(data_);
    if (poolData)
    {
        delete poolData;
        poolData = nullptr;
        data_ = nullptr;
    }
}

// template <class F>
// void tpThreadPool::enqueue(F &&task)
// {
//     tpThreadPoolData *poolData = static_cast<tpThreadPoolData *>(data_);

//     {
//         std::lock_guard<std::mutex> lock(poolData->queueMutex_);
//         poolData->tasks_.emplace(std::forward<F>(task));

//         // 动态扩展：任务积压且未达上限时扩容
//         if (poolData->tasks_.size() > poolData->threads_.size() &&
//             poolData->threads_.size() < poolData->maxThreads_)
//         {
//             poolData->threads_.emplace_back([this]
//                                             { workerTask(); });
//         }
//     }
//     poolData->condition_.notify_one();
// }

void tpThreadPool::stop()
{
    tpThreadPoolData *poolData = static_cast<tpThreadPoolData *>(data_);

    {
        std::lock_guard<std::mutex> lock(poolData->queueMutex_);
        if (poolData->stop_)
            return;

        poolData->stop_ = true;
        // 清空任务队列
        while (!poolData->tasks_.empty())
        {
            poolData->tasks_.pop();
        }
    }

    poolData->condition_.notify_all();
    for (std::thread &worker : poolData->threads_)
    {
        if (worker.joinable())
            worker.join();
    }
    poolData->threads_.clear();
}

void tpThreadPool::enqueueInternal(std::function<void()> task)
{
    tpThreadPoolData *poolData = static_cast<tpThreadPoolData *>(data_);
    {
        std::lock_guard<std::mutex> lock(poolData->queueMutex_);
        if (poolData->stop_)
            return;

        poolData->tasks_.emplace(std::move(task));

        // 动态扩展线程
        if (poolData->tasks_.size() > poolData->threads_.size() &&
            poolData->threads_.size() < poolData->maxThreads_)
        {
            poolData->threads_.emplace_back([this]
                                            { workerTask(); });
        }
    }
    poolData->condition_.notify_one();
}

void tpThreadPool::workerTask()
{
    while (true)
    {
        std::function<void()> task;
        {
            tpThreadPoolData *poolData = static_cast<tpThreadPoolData *>(data_);

            std::unique_lock<std::mutex> lock(poolData->queueMutex_);

            // 动态收缩：空闲线程超时处理
            if (poolData->tasks_.empty() &&
                poolData->threads_.size() > poolData->minThreads_)
            {

                // 等待10秒内是否有新任务
                if (!poolData->idleCondition_.wait_for(lock, std::chrono::seconds(10),
                                                       [this]
                                                       { return !static_cast<tpThreadPoolData *>(data_)->tasks_.empty() || static_cast<tpThreadPoolData *>(data_)->stop_; }))
                {
                    // 查找并移除当前线程
                    auto it = std::find_if(poolData->threads_.begin(), poolData->threads_.end(),
                                           [](const std::thread &t)
                                           {
                                               return t.get_id() == std::this_thread::get_id();
                                           });

                    if (it != poolData->threads_.end())
                    {
                        it->detach();
                        poolData->threads_.erase(it);
                    }
                    return; // 结束多余线程
                }
            }

            // 等待新任务或停止信号
            poolData->condition_.wait(lock, [this]
                                      { return static_cast<tpThreadPoolData *>(data_)->stop_ || !static_cast<tpThreadPoolData *>(data_)->tasks_.empty(); });

            if (poolData->stop_)
                return;
            if (poolData->tasks_.empty())
                continue;

            task = std::move(poolData->tasks_.front());
            poolData->tasks_.pop();
        }

        // 执行任务
        task();
    }
}