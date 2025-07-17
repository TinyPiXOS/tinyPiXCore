#ifndef __TP_THREAD_POOL_H
#define __TP_THREAD_POOL_H

#include <tpUtils.h>
#include <tpString.h>

TP_DEF_VOID_TYPE_VAR(ItpThreadPoolData);
/// @brief 线程池
class tpThreadPool
{
public:
    tpThreadPool();
    ~tpThreadPool();

    ItpThreadPoolData *data_;
};

#endif
