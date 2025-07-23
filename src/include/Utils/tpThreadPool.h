#ifndef __TP_THREAD_POOL_H
#define __TP_THREAD_POOL_H

#include <tpUtils.h>
#include <tpString.h>
#include <functional>

TP_DEF_VOID_TYPE_VAR(ItpThreadPoolData);
/// @brief 线程池
class tpThreadPool
{
public:
    /// @brief 创建默认线程池（线程数=CPU核心数*2）
    tpThreadPool();

    /// @brief 创建可配置的线程池
    /// @param minThreads 最小线程数量（至少为1）
    /// @param maxThreads 最大线程数量（不小于minThreads）
    explicit tpThreadPool(const uint32_t &minThreads, const uint32_t &maxThreads);

    /// @brief 销毁线程池（等待所有任务完成）
    ~tpThreadPool();

    /// @brief 向线程池添加任务
    /// @tparam F 可调用对象类型
    /// @param task 待执行的任务函数
    template <class F>
    void enqueue(F &&task)
    {
        typedef typename std::decay<F>::type TaskType;
        TaskType* taskStorage = new TaskType(std::forward<F>(task));
        enqueueInternal([taskStorage] {
            // 确保任务执行后正确释放内存
            std::unique_ptr<TaskType> ptr(taskStorage);
            (*ptr)(); 
        });
    }

    /// @brief 停止线程池（取消所有待执行任务）
    void stop();

private:
    void enqueueInternal(std::function<void()> task);
    void workerTask();

    ItpThreadPoolData *data_;
};

#endif
