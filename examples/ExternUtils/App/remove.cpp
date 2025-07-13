#include <iostream>
#include <stdio.h>
#include "tpAppInstall.h"

int main(int argc,char **argv)
{
	if(argc!=2)
	{
		printf("命令格式：./tpRemove <uuid>\n");
		return -1;
	}
	tpString uuid=argv[1];
	tpAppInstall::remove(uuid);
	return 0;
}
