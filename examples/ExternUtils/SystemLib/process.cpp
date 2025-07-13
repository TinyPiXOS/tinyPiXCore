//测试程序
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <array>
#include <memory>
#include "tpProcessManage.h"
#include "tpAppDataDisk.h"


int main()
{
	std::cout << "Hello, World!" << std::endl;
	tpProcessManage process(false);
	std::cout << "update process tree" << std::endl;
	process.update();
	tpProcessInfo *root=process.findProcess(1);	
	process.printProcessTree(root);		//打印进程树
	tpProcessInfo *app=process.findProcess(64953);	
	if (app) {
		process.printProcessTree(app);
	} else {
		std::cerr << "Failed to find root process .\n";
		return 0;
	}
	printf("get process information\n");
	int i=0;
	while(1)
	{
		process.update();
		std::cout << "\napp data: " << std::endl;
		std::cout << "	cpu: " << process.getCpuUsage(64953)<<"%"<<std::endl;
		std::cout << "	mem: " << process.getMemoryUsage(64953)/1024.0<<"KByte"<<std::endl;
		std::cout << "	disk_read: " << process.getDiskReadSpeed(64953)/1024<<"kByte/s"<<std::endl;
		std::cout << "	disk_write: " << process.getDiskWriteSpeed(64953)/1024<<"kByte/s"<<std::endl;
		std::cout << "	network_update: " << process.getNetUpSpeed(1)/1024<<"KByte/s"<<std::endl;
		std::cout << "	network_download: " << process.getNetDownSpeed(1)/1024<<"KByte/s"<<std::endl;
		sleep(1);
		i++;
		if(i>20)
			break;
		printf("\n");
	}

	tpAppDataDisk disk;		//磁盘占用
	std::string uuid="f03c8f8c-dd9b-453f-b2d4-d049c073e252";
	std::cout<<"app data:"<<disk.getAppDiskSpace(uuid)<<"Byte"<<std::endl;
	std::cout<<"app temp data:"<<disk.getAppDataDiskSpace(uuid)<<"Byte"<<std::endl;
	std::cout<<"all app data:"<<disk.getAllAppDiskSpace()<<"Byte"<<std::endl;
    return 0;
}

