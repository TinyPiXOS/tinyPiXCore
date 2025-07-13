#include <iostream>
#include <stdio.h>

#include "appmanage.h"


int install_app()
{
	tpString package_path="/home/pix/AppManage/mytestapp.pik";
	tpAppManage appmanage(package_path);

	tpString pack_ver;
	pack_ver=appmanage.AppGetPackVersion();	//
	std::cout<<"Pack Version: "<<pack_ver<<std::endl;
	std::cout<<"Pack Arch: "<<appmanage.AppGetPackArch() <<std::endl;
	std::cout<<"Pack Space: "<<appmanage.AppGetPackSpace()<<std::endl;
	std::cout<<"AppVersion:"<<appmanage.AppGetNowVersion()<<std::endl;
	//appmanage.AppGetNowVersion();
	if(appmanage.AppCheckAll()==false)
		std::cout<<"check error"<<std::endl;

	std::cout<<"安装"<<std::endl;
	appmanage.AppInstall();
	return 0;
}

int install_lib()
{
	tpString package_path="/home/pix/AppManage/mytestlib/systemlib.pik";
	tpAppManage appmanage(package_path);
	
	if(appmanage.AppCompleteCheck()!=1)
		std::cout<<"MD5 Check error"<<std::endl;	

	if(appmanage.AppArchCheck()!=1)
		std::cout<<"硬件环境不支持"<<std::endl;	
	if(appmanage.AppSpaceCheck()!=1)
		std::cout<<"空间检查"<<std::endl;	
	if(appmanage.AppVersionCheck()!=1)
		std::cout<<"已有新版本或相同版本"<<std::endl;

	appmanage.AppInstall();
	return 0;
}

int remove_app()
{
	tpAppManage appmanage;
	appmanage.AppRemove("fb1412cf-84de-4138-b401-215b6e9b1c11");
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
