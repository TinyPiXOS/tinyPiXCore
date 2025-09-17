#include "TpShareTrans.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <memory>
#include <iostream>
#include <cstring>

struct TpShareTransData
{
    tpUInt32 width = 0;
    tpUInt32 height = 0;
    tpInt32 format = 0;
    tpUInt32 Rmask = 0;
    tpUInt32 Gmask = 0;
    tpUInt32 Bmask = 0;
    tpUInt32 Amask = 0;

    TpShareTransData()
    {
    }
};

tpUInt64 TpShareTrans::createShareTrans(IPiWFSurface *surface)
{
    if (!surface)
    {
        return 0;
    }

    tpUInt32 size = tinyPiX_surface_get_size(surface) + sizeof(TpShareTransData);
    tpInt64 id = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT);

    if (id < 0)
    {
        return 0;
    }

    void *buffer = shmat(id, NULL, 0);

    if (!buffer)
    {
        shmctl(id, IPC_RMID, NULL);
        return 0;
    }

    TpShareTransData *memory = (TpShareTransData *)buffer;

    memory->width = tinyPiX_surface_get_width(surface);
    memory->height = tinyPiX_surface_get_height(surface);

    memory->format = tinyPiX_surface_get_format(surface);

    memory->Rmask = tinyPiX_surface_get_rmask(surface);
    memory->Gmask = tinyPiX_surface_get_gmask(surface);
    memory->Bmask = tinyPiX_surface_get_bmask(surface);
    memory->Amask = tinyPiX_surface_get_amask(surface);

    void *address = (tpUInt8 *)buffer + sizeof(TpShareTransData);
    memcpy(address, tinyPiX_surface_get_matrix(surface), tinyPiX_surface_get_size(surface));
    shmdt(buffer);

    return ((((tpUInt64)id << 32) & 0xffffffff00000000) | size);
}

tpBool TpShareTrans::checkShareTrans(tpUInt64 shmid)
{
    return tpBool(shmid && ((shmid >> 32) & 0xffffffff) && (shmid & 0xffffffff));
}

IPiWFSurface *TpShareTrans::surfaceShareTrans(tpUInt64 shmid, tpBool autoFree)
{
    tpBool ret = checkShareTrans(shmid);

    if (!ret)
    {
        return NULL;
    }

    tpInt32 id = (tpInt32)((shmid >> 32) & 0xffffffff);
    tpUInt32 size = (tpUInt32)(shmid & 0xffffffff);

    void *buffer = shmat(id, NULL, 0);

    if (!buffer)
    {
        shmdt(buffer);

        if (autoFree)
        {
            shmctl(id, IPC_RMID, NULL);
        }
        return NULL;
    }

    TpShareTransData *memory = (TpShareTransData *)buffer;
    IPiWFSurface *surface = tinyPiX_surface_create(NULL, memory->width, memory->height, memory->format,
                                                   memory->Rmask, memory->Gmask, memory->Bmask, memory->Amask);

    if (!surface)
    {
        shmdt(buffer);

        if (autoFree)
        {
            shmctl(id, IPC_RMID, NULL);
        }
        return NULL;
    }

    memcpy(tinyPiX_surface_get_matrix(surface), (char*)buffer + sizeof(TpShareTransData), tinyPiX_surface_get_size(surface));
    shmdt(buffer);

    if (autoFree)
    {
        shmctl(id, IPC_RMID, NULL);
    }

    return surface;
}

TpShareTrans::TpShareTrans()
{
}

TpShareTrans::~TpShareTrans()
{
}
