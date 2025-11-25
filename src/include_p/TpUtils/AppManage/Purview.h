#ifndef _PURVIEW_H_
#define _PURVIEW_H_

#include "AppManage/AppmanageConf.h"


class TpAppmPurview{
public:
    TpAppmPurview(){}
    ~TpAppmPurview(){}
    
public:
    static int InstallPurviewInit();
    static int InstallPurviewSet(TpAppID uuid, TypePackage type);
    static int RemovePurview(TpAppID uuid);

};

#endif
