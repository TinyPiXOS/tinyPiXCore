//系统接口测试
#include <iostream>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <string>
#include <sys/mount.h>
#include <errno.h>
#include "tpSystemInfo.h"
#include "tpNetworkInfo.h"
#include "tpDiskManage.h"
#include "tpDisk.h"
#include "tpCpu.h"
#include "tpGpu.h"
#include "tpMemory.h"

std::atomic<bool> running;

void thread_disk_test()
{
	std::fstream file("/media/jiyc/9217-B09D/example.txt", std::ios::out);  // 打开文件进行读写
    if (!file) {
        std::cerr << "Error opening file!" << std::endl;
        return ;
    }
	file.close();
	while(running)
	{
		/*file=std::fstream("/media/jiyc/9217-B09D/example.txt", std::ios::out);
		if (!file) {
        	std::cerr << "Error opening file!" << std::endl;
        	return ;
   		}*/
		file << "\nThis is a new line added to the file." << std::endl;
		//file.close();
		usleep(1000*100);
	}
	file.close();
}



int main()
{
	tpSystemInfo system;
//	system.setMachineName("Ji-Yuchao Test Machine");
	std::cout << "System Test" <<  std::endl;
	std::cout << "设备名称：" << system.getMachineName() << std::endl;
	std::cout << "系统名称：" << system.getSystemName() << std::endl;
	std::cout << "系统版本：" << system.getSystemVersion() << std::endl;
	std::cout << "主板厂商：" << system.getBoardVendor() << std::endl;
	std::cout << "主板名称：" << system.getBoardName() << std::endl;
	std::cout << "主板版本：" << system.getBoardVersion() << std::endl;
	std::cout << "系统序列号：" << system.getBoardSerial() << std::endl;
	std::cout << "BIOS日期：" << system.getBiosData() << std::endl;
	std::cout << "BIOS厂商：" << system.getBiosVendor() << std::endl;
	std::cout << "BIOS版本：" << system.getBiosVersion() << std::endl;
	std::cout << "产品名称：" << system.getProductName() << std::endl;
	std::cout << "产品：" << system.getProductFamily() << std::endl;
	std::cout << "产品序列号：" << system.getProductSerial() << std::endl;
	std::cout << "产品库存单位：" << system.getProductSku() << std::endl;
	std::cout << "产品UUID：" << system.getProductUuid() << std::endl;
	std::cout << "产品版本号：" << system.getProductVersion() << std::endl;

	std::cout << "CPU信息：" << std::endl;
	tpList<tpCpuCore *> cpui=system.getCpuCoreInfo();
	for(auto &it : cpui){
		std::cout << "	Core-" << (int)it->getCoreNum() << " " << it->getName() << " " << it->getFrequency() << std::endl;

	}

	std::cout << "内存信息：" << std::endl;
	tpMemory mem;		//内存信息也可以使用system.getMemoryInfo()获取
	std::cout << "	内存总大小：" << mem.getTotalSize() << "kB" << std::endl;
	std::cout << "	剩余内存：" << mem.getAvailableSize() << "kB" << std::endl;
	std::cout << "	空闲内存：" << mem.getFreeSize() << "kB" << std::endl;
    std::cout << "	使用率：" << mem.getUsage() << "%" << std::endl;
	

	std::cout << "磁盘信息：" << std::endl;
	tpList<tpDisk *> disk_p=system.getDiskInfo();	//磁盘信息也可以使用DiskInfo("disk name")获取,但每次只能获取一个磁盘信息
	printf("信息：\n");
	for(auto &it : disk_p)
	{
		printf("\n");
		printf("	磁盘：%s\n",it->getName().c_str());
		printf("	设备：%s\n",it->getDevice().c_str());
		printf("	空间：%ld\n",it->getSpace());
		printf("	类型：%s\n",it->getType().c_str());
		printf("	只读：%d\n",it->getReadonly());
		printf("	移动：%d\n",it->getRemovable());
		printf("	厂商：%s\n",it->getVendor().c_str());
		printf("	型号：%s\n",it->getModel().c_str());
		printf("	序列号：%s\n",it->getSerial().c_str());
		printf("	挂载点：%s\n",it->getMount().c_str());
		printf("	已使用：%ld\n",it->getUsedSize());
		printf("	文件系统：%s\n",it->getFstype().c_str());
	}
	//std::cout << "CPU使用：（第一个显示的cpu总利用率）" << std::endl;

	tpString network("ens33");
	tpString disk_name("sda");

	tpCpu cpu(TP_TRUE);
	tpNetworkInfo net(network,TP_TRUE);
	tpList <tpCpuCore*> cpus=cpu.getState();
	tpDisk disk(disk_name,TP_TRUE);
	printf("creat ok\n");
	//std::thread thread_t(thread_disk_test);
	running=true;
	
	sleep(2);
	while(1)
	{
		//网络
		std::cout << "	网卡ens33：上传：" << net.getUploadSpeed() << "Byte/s  下载：" << net.getDownloadSpeed() << "Byte/s"<<std::endl;
		
		//cpu
		cpus=cpu.getState();
		for(auto &it : cpus)
		{
			std::cout << "	Core-" << (int)it->getCoreNum() << " " << it->getUsage() << "%" << std::endl;
		}

		//磁盘
		std::cout << "	磁盘：读取：" << disk.getReadSpeed() << "Byte/s  写入：" << disk.getWriteSpeed() << "Byte/s"<<std::endl;

		sleep(1);
	}
	printf("弹出磁盘\n");
	disk.popupRabDisk();
	running = false;
	//thread_t.join();
	
	return 0;
}