#include "TpDiskManage.h"
#include "TpApp.h"
#include "TpFixScreen.h"
#include <TpString.h>
#include <TpList.h>
#include <iostream>

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);
	TpDiskManage diskManager(TP_TRUE,500);
	std::cout << "-------------------------------" << std::endl;
	

	TpList<TpDisk *> diskInfoList = diskManager.getList();


	uint32_t index = 0;

	for (const auto &diskInfo : diskInfoList)
	{
        std::cout << "-------------------------------" << std::endl;
        std::cout << "index :" << index++ << std::endl;
        std::cout << "Name :" << diskInfo->getName() << std::endl;
        std::cout << "Device :" << diskInfo->getDevice() << std::endl;
        std::cout << "SectorSize :" << diskInfo->getSectorSize() << std::endl;
        std::cout << "SectorNum :" << diskInfo->getSectorNum() << std::endl;
        std::cout << "Space :" << diskInfo->getSpace() << std::endl;
        std::cout << "Partition :" << diskInfo->getPartition() << std::endl;
        std::cout << "Removable :" << diskInfo->getRemovable() << std::endl;
        std::cout << "Readonly :" << diskInfo->getReadonly() << std::endl;
        std::cout << "Model :" << diskInfo->getModel() << std::endl;
        std::cout << "Vendor :" << diskInfo->getVendor() << std::endl;
        std::cout << "Mount :" << diskInfo->getMount() << std::endl;
        std::cout << "UsedSize :" << diskInfo->getUsedSize() << std::endl;
	}

	std::cout << "-------------------------------" << std::endl;
	std::cout << "监测磁盘设备插拔" << std::endl;
	
	connect(&diskManager, diskRemove, [=](TpString name)
            { std::cout << "[Signal]设备弹出：" << name<< std::endl; });

	connect(&diskManager, diskAdd, [=](TpDisk *disk)
            { std::cout << "[Signal]设备插入：" << disk->getName() << std::endl; });
	
	
	app.run();
}
