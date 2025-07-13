#include <iostream>
#include <stdio.h>
#include "tpAppDopack.h"
#include "tpLibDopack.h"


//使用json文件的内容来自动生成安装包
int example_creat_app_pack_json(const char *json,const char *pack,const char *path)
{
	tpAppDopack package;
	package.setPackageType(tpAppDopack::TP_PACKAGE_TYPE_APP);
	package.getAllConfig(json);///home/pix/AppManage/keyboard/dopack.json
	package.setPackageName(pack);		//必须设置安装包名称，佛则不会打包，会自动拼接后缀keyboardPackage
	package.creatPackage(path);///home/pix/AppManage
}

int main(int argc,char **argv)
{
	if(argc!=4)
	{
		printf("命令格式：./tpDopack <json文件位置> <安装包名称> <安装包生成位置>\n");
		return -1;
	}


	example_creat_app_pack_json(argv[1],argv[2],argv[3]);

	return 0;
}