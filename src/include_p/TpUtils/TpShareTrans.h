#ifndef __TP_SHARED_TRANS_H
#define __TP_SHARED_TRANS_H

#include <TpString.h>
#include <tinyPiXUtils.h>
#include <TpCore.h>

/// @brief 画布共享内存相关接口；用于跨进程共享surface画布
class TpShareTrans
{
public:
    /// @brief 生成一个基于共享内存的存储句柄;可以跨进程使用
    /// @param surface 任意表面
    /// @return 成功返回共享内存id，失败时可以使用函数进行校验
    static tpUInt64 createShareTrans(IPiWFSurface *surface);

    /// @brief 校验内存ID是否有效
    /// @param shmid 共享内存ID
    /// @return 成功返回TP_TRUE，失败返回TP_FALSE
    static tpBool checkShareTrans(tpUInt64 shmid);

    /// @brief 通过共享内存ID直接生成surface表面
    /// @param shmid 共享内存ID
    /// @param autoFree 是否自动释放共享内存
    /// @return 成功返回非空表面，失败返回NULL;在获得surface后，需要使用sufface_free释放
    static IPiWFSurface *surfaceShareTrans(tpUInt64 shmid, tpBool autoFree);

private:
    TpShareTrans();
    virtual ~TpShareTrans();
};

#endif
