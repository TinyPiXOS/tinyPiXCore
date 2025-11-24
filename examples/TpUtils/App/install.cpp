#include <iostream>
#include <stdio.h>
#include <string.h>
#include "TpUtils/AppManage/TpAppInstall.h"
#include "TpCore/TpJsonObject.h"
#include "TpCore/TpJsonValue.h"
#include "TpCore/TpJsonDocument.h"

int install_app(const char *pack_path)
{
	TpString package_path(pack_path);
	TpAppInstall appmanage(package_path);
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
	TpString package_path="/home/pix/AppManage/mytestlib/systemlib.pik";
	TpAppInstall appmanage(package_path);
	
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
		printf("命令格式：./TpInstall <安装包位置>\n");
		return -1;
	}
	install_app(argv[1]);
	
	return 0;
}


/*int main()
{
	TpJsonObject object;
	char test2[20]="test_value\n";
	char test3[11]="test_value";
	char *test4=(char *)malloc(20);
	memcpy(test4,test3,11);
	char test5[12]="test_value\0";
//	object.insert(TpString("name"), TpJsonValue(TpString("name_hello")));
//	object.insert(TpString("test"), TpJsonValue(TpString("test_value")));
	object.insert("name", "name_hello");
	object.insert("test", "test_value");
	object.insert(TpString("test1").c_str(), (TpString("test_value").c_str()));
	object.insert("test2",test2);
	object.insert("test3",test3);
	object.insert("test4",test4);
	object.insert("test5",test5);

	TpJsonDocument doc(object);
	std::cout << "json_doc:"<< doc.toJson() << std::endl;
	std::cout << "name: "   <<object.value(TpString("name")).toString() <<std::endl;
	std::cout << "test: "   <<object.value(TpString("test")).toString() <<std::endl;
}*/