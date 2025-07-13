#include <iostream>
#include <stdio.h>
#include "tpAppDopack.h"
#include "tpLibDopack.h"


int example_creat_app_pack()
{
	
	printf("example package and start.sh\n");
	tpAppDopack package;

	//设置应用的相关信息
	printf("set package type\n");
	package.setPackageType(tpAppDopack::TP_PACKAGE_TYPE_APP);
	printf("set uuid\n");
	package.setAppID("f03c8f8c-dd9b-453f-b2d4-d049c073e252");
	package.setAppName("apptest");
	package.setVersion(1,0,0);
	printf("set app path\n");
	package.setAppPath("/home/pix/AppManage/mytestapp/mytestapp");
	package.setArchitecture("amd64");
	package.setAuthor("Chingan");
	package.setContact("2111956539@qq.com");
	package.setProvides("tinyPiX");
	printf("set depend lib\n");
	package.addDepend("libalsa",1,5,1);//格式name@ersion
	package.addDepend("libssl",1,0,0);
	printf("set assert\n");
	package.addAssert("/home/pix/AppManage/mytestapp/assert/mysrc");
	package.addAssert("/home/pix/AppManage/mytestapp/assert/picture");
	printf("set lib\n");
	package.addLib("/home/pix/AppManage/mytestapp/lib/libmysum.so");
	printf("set icon\n");
	package.setIcon("/home/pix/AppManage/mytestapp/icon.jpeg");
	package.addExtension(".png");
	package.addExtension(".jpg");
	package.addExtension(".jpeg");
	package.addExtension(".bmp");
	package.addFile("/home/pix/AppManage/mytestapp/jiyc");

    //设置应用的启动参数
	printf("start.sh\n");
	package.addEnvironmentVar("PATH","/usr/lib/python");
	package.addEnvironmentVar("PYTHONHOME","/usr/local/python3.6");
	//package.addStartDepend("libalsa");
	package.addStartArg("--fullscreen");
	package.addStartArg("--server");
	package.setExecPath("mytestapp");

	package.setPackageName("mytestappPackage");		//必须设置安装包名称，佛则不会打包，会自动拼接后缀
	package.creatPackage("/home/pix/AppManage/");


//	appm_generate_startup_script(NULL,"/home/pix/AppManage/start.sh");
	return 0;

}


int example_creat_system_lib()
{
	tpLibDopack package;
	package.setArchitecture(tpLibDopack::TP_ARCH_TYPE_AMD64);
	package.setDiskSpace(10240);
	package.addLibrary("/home/pix/AppManage/mytestlib/libjson-c.so",1,0,0);
	package.addLibrary("/home/pix/AppManage/mytestlib/libmysum.so",1,0,8);
	package.addLibrary("/home/pix/AppManage/mytestlib/libpixnet.so",0,1,5);
	package.addLibrary("/home/pix/AppManage/mytestlib/libserial.so",5,1,0);
	package.addLibrary("/home/pix/AppManage/mytestlib/libtparchive.so",2,0,0);
	package.save("/home/pix/AppManage/mytestlib/systemlib.pik");
	return 0;
	
}

//使用json文件的内容来自动生成安装包
int example_creat_app_pack_json()
{
	tpAppDopack package;
	package.setPackageType(tpAppDopack::TP_PACKAGE_TYPE_APP);
	package.getAllConfig("/home/pix/AppManage/mytestapp/dopack.json");
	package.setPackageName("mytestappPackage");		//必须设置安装包名称，佛则不会打包，会自动拼接后缀
	package.creatPackage("/home/pix/AppManage");
}

int main()
{
	example_creat_app_pack();
//	example_creat_app_pack_json();
//	example_creat_system_lib();

	return 0;
}