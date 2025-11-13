#include <iostream>
#include "TpCpuManage.h"


struct TpCpuManageData
{
	TpList<TpCpu *> list;
	TpCpuManageData()
	{	
		
	}
};

TpCpuManage::TpCpuManage(tpBool enabled, tpUInt16 samp)
{
	data_ = new TpCpuManageData();
	TpCpuManageData *cmData = static_cast<TpCpuManageData *>(data_);
	cmData->list=readList();
}

TpCpuManage::~TpCpuManage()
{
	TpCpuManageData *cmData = static_cast<TpCpuManageData *>(data_);
	if(cmData==nullptr)
		return ;
	for(auto &it:cmData->list)
	{
		if(it)
		{
			delete it;
			it=nullptr;
		}
	}
}

TpList<TpCpu*> TpCpuManage::readList()
{
//	TpCpuManageData *cmData = static_cast<TpCpuManageData *>(data_);
	TpList<TpCpu *> list;
	/*
	执行具体的操作获取cpu列表
	*/
	TpCpu *cpu=new TpCpu(TP_FALSE);
	list.emplace_back(cpu);
	return list;
}

TpList<TpCpu*> TpCpuManage::getList()
{
	TpCpuManageData *cmData = static_cast<TpCpuManageData *>(data_);
	return cmData->list;
}

//返回第一个CPU
TpCpu *TpCpuManage::getCpu()
{
	return nullptr;
}