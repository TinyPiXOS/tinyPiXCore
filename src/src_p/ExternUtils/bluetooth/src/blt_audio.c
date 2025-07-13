/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙音频服务的启动关联等
说 明 : 
日 期 : 2025.5.13

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include "blt_audio.h"

//启动bluealsa守护进程
int bluet_audio_start_blue_alsa(DesktopSystem *system,GError **error)
{
	return desktop_system_start_unit(system,"bluealsa.service","replace",error);
}

//查询bluealsa守护进程状态
char *bluet_audio_get_active_state(DesktopSystem *system, GError **error)
{
	return desktop_system_get_service_active_state(system,"bluealsa.service",error);
}

//获取bluealsa守护进程是否在运行
uint8_t bluet_audio_blue_alsa_is_runing(DesktopSystem *system,GError **error)
{
	char *state=bluet_audio_get_active_state(system,error);
	uint8_t runing=0;

	printf("state=%s\n",state);
	if(strncmp(state,"active",strlen("active"))==0)
	{
		runing=1;
	}
	else if(strncmp(state,"failed",strlen("failed"))==0)
	{
		runing=0;
	}
	else
	{
		runing=0;
	}
	free(state);
	return runing;
}

//停止bluealsa守护进程
int bluet_audio_stop_blue_alsa(DesktopSystem *system,GError **error)
{
	return desktop_system_stop_unit(system,"bluealsa.service","replace",error);
}

//重启
int bluet_audio_restart_blue_alsa(DesktopSystem *system,GError **error)
{
	return desktop_system_restart_unit(system,"bluealsa.service","replace",error);
}


