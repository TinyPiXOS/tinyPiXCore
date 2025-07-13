#include <iostream>
#include <stdio.h>
#include "tpAppInstall.h"


int install_app(const char *pack_path)
{
	tpString package_path(pack_path);
	tpAppInstall appmanage(package_path);
	std::cout<< "icon:" << appmanage.getIcon()<<std::endl;
	std::cout<< "uuid:" << appmanage.getAppUUID()<<std::endl;
	std::cout<< "name:" << appmanage.getAppName()<<std::endl;
	std::cout<<"安装"<<std::endl;
	appmanage.install();
	while(1)
	{
		if(appmanage.getInstallSchedule()==100)
			break;
		usleep(500000);
	}
	std::cout<<"安装完成\n";
	return 0;
}

int install_lib()
{
	tpString package_path="/home/pix/AppManage/mytestlib/systemlib.pik";
	tpAppInstall appmanage(package_path);
	
	if(appmanage.completeCheck()!=1)
		std::cout<<"MD5 Check error"<<std::endl;	

	if(appmanage.archCheck()!=1)
		std::cout<<"硬件环境不支持"<<std::endl;	
	if(appmanage.spaceCheck()!=1)
		std::cout<<"空间检查"<<std::endl;	
	if(appmanage.versionCheck()!=1)
		std::cout<<"已有新版本或相同版本"<<std::endl;

	appmanage.install();
	return 0;
}



int main(int argc,char **argv)
{
	if(argc!=2)
	{
		printf("命令格式：./tpInstall <安装包位置>\n");
		return -1;
	}
	install_app(argv[1]);
	
	return 0;
}
