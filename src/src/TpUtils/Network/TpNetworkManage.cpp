/*///------------------------------------------------------------------------------------------------------------------------//
            网卡管理
说 明 :
日 期 : 2024.11.06

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include "TpNetworkManage.h"
#include "TpNetworkInterface.h"

struct TpNetworkManageData
{
    TpList<TpNetworkInfo *> list;
    TpNetworkManageData() {}
};

TpNetworkManage::TpNetworkManage()
{
    data_ = new TpNetworkManageData();
    TpNetworkManageData *netData = static_cast<TpNetworkManageData *>(data_);

    TpList<tpShared<TpNetworkInterface>> interfaceList = TpNetworkInterface::allDevice();
    for (auto &it : interfaceList)
    {
        TpString name = it->name();
        TpNetworkInfo *device = new TpNetworkInfo(name, TP_FALSE);
        netData->list.emplace_back(device);
    }
}

TpNetworkManage::~TpNetworkManage()
{
    TpNetworkManageData *netData = static_cast<TpNetworkManageData *>(data_);
    if (netData)
    {
        for (auto &it : netData->list)
        {
            netData->list.remove(it);
            delete (it);
            it = nullptr;
        }
        delete (netData);
        netData = nullptr;
    }
}

TpList<TpNetworkInfo *> TpNetworkManage::getList()
{
    TpNetworkManageData *netData = static_cast<TpNetworkManageData *>(data_);
    return netData->list;
}
