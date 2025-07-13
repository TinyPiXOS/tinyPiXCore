//多媒体服务主程序

#include <stdio.h>
#include "audio_manage.h"


int creat_process(void *)
{
	pid_t pid;
	// 创建新进程
	pid = fork();
	if (pid < 0) {  // fork 失败
		perror("fork failed");
		return -1;
	} 
	else if (pid == 0) { 
		
	} 
	else {  // 父进程代码
		printf("This is the parent process. PID: %d, Child PID: %d\n", getpid(), pid);
	}
	return pid;
}



int main()
{
	creat_process();
	return 0;
}