#include <iostream>
#include "tpCpuManage.h"


struct tpCpuManageData
{
	tpList<tpCpu *> list;
	tpCpuManageData()
	{	
		
	}
};

tpCpuManage::tpCpuManage(tpBool enabled, tpUInt16 samp)
{
	data_ = new tpCpuManageData();
	tpCpuManageData *cmData = static_cast<tpCpuManageData *>(data_);
	cmData->list=readList();
}

tpCpuManage::~tpCpuManage()
{
	tpCpuManageData *cmData = static_cast<tpCpuManageData *>(data_);
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

tpList<tpCpu*> tpCpuManage::readList()
{
//	tpCpuManageData *cmData = static_cast<tpCpuManageData *>(data_);
	tpList<tpCpu *> list;
	/*
	执行具体的操作获取cpu列表
	*/
	tpCpu *cpu=new tpCpu(TP_FALSE);
	list.emplace_back(cpu);
	return list;
}

tpList<tpCpu*> tpCpuManage::getList()
{
	tpCpuManageData *cmData = static_cast<tpCpuManageData *>(data_);
	return cmData->list;
}

//返回第一个CPU
tpCpu *tpCpuManage::getCpu()
{
	return nullptr;
}