/*///------------------------------------------------------------------------------------------------------------------------//
			网卡管理
说 明 :
日 期 : 2024.11.06

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include "tpNetworkManage.h"
#include "tpNetworkInterface.h"


struct tpNetworkManageData{
	tpList<tpNetworkInfo *> list;
	tpNetworkManageData(){}
};


tpNetworkManage::tpNetworkManage()
{
	data_=new tpNetworkManageData();
	tpNetworkManageData *netData=static_cast<tpNetworkManageData *>(data_);

	tpList<tpNetworkInterface> interface_list=tpNetworkInterface::getAllDevice();
	for(auto &it : interface_list)
	{
		tpString name=it.getName();
		tpNetworkInfo *device = new tpNetworkInfo(name,TP_FALSE);
		netData->list.emplace_back(device);
	}
}

tpNetworkManage::~tpNetworkManage()
{
	tpNetworkManageData *netData=static_cast<tpNetworkManageData *>(data_);
	if(netData)
	{
		for(auto &it : netData->list)
		{
			netData->list.remove(it);
			delete(it);
			it=nullptr;
		}
		delete(netData);
		netData=nullptr;
	}
}

tpList<tpNetworkInfo *> tpNetworkManage::getList()
{
	tpNetworkManageData *netData=static_cast<tpNetworkManageData *>(data_);
	return netData->list;
}


