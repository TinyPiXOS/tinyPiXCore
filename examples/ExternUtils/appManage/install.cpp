#include <iostream>
#include <stdio.h>
#include "tpAppInstall.h"


int install_app()
{
	tpString package_path="/home/pix/AppManage/mytestAppPackage.pik";
	tpAppInstall appmanage(package_path);


	std::cout<< "icon:" << appmanage.getIcon()<<std::endl;
	std::cout<< "uuid:" << appmanage.getAppUUID()<<std::endl;
	std::cout<< "name:" << appmanage.getAppName()<<std::endl;

/*	tpString pack_ver;
	pack_ver=appmanage.getPackVersion();	//
	std::cout<<"Pack Version: "<<pack_ver<<std::endl;
	std::cout<<"Pack Arch: "<<appmanage.getPackArch() <<std::endl;
	std::cout<<"Pack Space: "<<appmanage.getPackSpace()<<std::endl;
	std::cout<<"AppVersion:"<<appmanage.getNowVersion()<<std::endl;
	//appmanage.AppGetNowVersion();
	if(appmanage.checkAll()==false)
		std::cout<<"check error"<<std::endl;
*/

	std::cout<<"安装"<<std::endl;
	appmanage.install();
	appmanage.getInstallSchedule();
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

int remove_app()
{
	tpString uuid("f03c8f8c-dd9b-453f-b2d4-d049c073e252")
	appmanage.remove(uuid);
	return 0;
}

int main(int argc,char **argv)
{
	if(argc==2)
	{
		if(strcmp(argv[1],"install_app")==0)
            install_app();
        else if(strcmp(argv[1],"install_lib")==0)
            install_lib();
		else if(strcmp(argv[1],"remove_app")==0)
		    remove_app();
	}
	else
		install_app();
//	install_lib();
	
	return 0;
}
