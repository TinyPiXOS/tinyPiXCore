/*应用安装包创建测试程序*/
#include <iostream>
#include <stdio.h>
#include "appdopack.h"


int example_creat_app_pack()
{
	
	printf("example package and start.sh\n");
	tpPackageInfo package;
	tpStartShInfo start;

	//设置应用的相关信息
	printf("set package type\n");
	package.SetPackageType(TYPE_PACKAGE_APP);
	printf("set uuid\n");
	package.SetAppID("fb1412cf-84de-4138-b401-215b6e9b1c11");
	package.SetAppName("apptest");
	package.SetVersion(1,0,0);
	printf("set app path\n");
	package.SetAppPath("/home/pix/AppManage/mytestapp/mytestapp");
	package.SetArchitecture("amd64");
	package.SetAuthor("Chingan");
	package.SetContact("2111956539@qq.com");
	printf("set depend lib\n");
	package.AddDepend("libalsa@1.5.1");//格式name@ersion
	package.AddDepend("libssl@1.0.0");
	printf("set assert\n");
	package.AddAssert("/home/pix/AppManage/mytestapp/assert/mysrc");
	package.AddAssert("/home/pix/AppManage/mytestapp/assert/picture");
	printf("set lib\n");
	package.AddLib("/home/pix/AppManage/mytestapp/lib/libmysum.so");
	printf("set icon\n");
	package.SetIcon("/home/pix/AppManage/mytestapp/icon.jpeg");
	package.AddExtension(".png");
	package.AddExtension(".jpg");
	package.AddExtension(".jpeg");
	package.AddExtension(".bmp");
	package.Save("/home/pix/AppManage/piktest");		//打包的原始文件目录生成的文件,确保该piktest文件不存在，否则会创建失败

    //设置应用的启动参数
	printf("start.sh\n");
	start.AddEnvironmentVar("PATH","/usr/lib/python");
	start.AddEnvironmentVar("PYTHONHOME","/usr/local/python3.6");
	start.AddDependency("libalsa");
	start.AddStartArg("--fullscreen");
	start.AddStartArg("--server");
	start.SetExecPath("mytestapp");
	start.Save("/home/pix/AppManage/piktest/start.sh");

	package.CreatPackage("/home/pix/AppManage/mytestapp.pik");


//	appm_generate_startup_script(NULL,"/home/pix/AppManage/start.sh");
	return 0;

}


int example_creat_system_lib()
{
	tpLibPackageInfo package;
	package.SetArchitecture("amd64");
	package.SetDiskSpace(10240);
	package.AddLibrary("/home/pix/AppManage/mytestlib/libjson-c.so",1,0,0);
	package.AddLibrary("/home/pix/AppManage/mytestlib/libmysum.so",1,0,8);
	package.AddLibrary("/home/pix/AppManage/mytestlib/libpixnet.so",0,1,5);
	package.AddLibrary("/home/pix/AppManage/mytestlib/libserial.so",5,1,0);
	package.Save("/home/pix/AppManage/mytestlib/systemlib.pik");
	package.ClassFree();
	return 0;
	
}


int main()
{
	example_creat_app_pack();
//	example_creat_system_lib();

	return 0;
}