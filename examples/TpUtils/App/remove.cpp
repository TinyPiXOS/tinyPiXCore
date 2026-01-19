#include <iostream>
#include <stdio.h>
#include "TpUtils/AppManage/TpAppInstall.h"

int main(int argc,char **argv)
{
	if(argc!=2)
	{
		printf("命令格式：./TpRemove <uuid>\n");
		return -1;
	}
	TpString uuid=argv[1];
	TpAppInstall::remove(uuid);
	return 0;
}
