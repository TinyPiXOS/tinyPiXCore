#ifndef _UNLOAD_H_
#define _UNLOAD_H_

#include <stdio.h>
#include "AppManage/AppmanageConf.h"


class TpAppmUnload{
public:
    TpAppmUnload(){}
    ~TpAppmUnload(){}
public:
    static int Unload(TpAppID uuid);
};

#endif