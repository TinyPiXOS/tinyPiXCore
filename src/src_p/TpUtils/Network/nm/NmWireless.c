
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "Network/NetworkConf.h"
#include "Network/nm/NmWireless.h"



int nm_wireless_connect_ssid(const char *name,const char *passwd, uint32_t timeout)
{
    int err = -1;
    char *buff = (char *)malloc(1024);
    char *buf = (char *)malloc(200);
    memset(buf, 0, 200);
    char *all = buf;
    FILE *outstream = NULL;
    sprintf(buf, "nmcli dev wifi con %s password %s name %s", name, passwd, name);
    if (systemCmdTimeout((const char *)buf, timeout) < 0) // 执行all字符串所包含的命令
        goto EXIT_FREE;
    //	printf("1\n");
    sprintf(buf, "nmcli con mod %s wifi-sec.psk %s", name, passwd); // ke'yi'sheng'lue
    if (systemCmdTimeout((const char *)buf, timeout) < 0)
    {
        goto EXIT_FREE;
    }
    //	printf("2\n");

    sprintf(buf, "nmcli con up %s", name); // 启用网络连接
    if ((outstream = popen(buf, "r")) == NULL)
    {
        fprintf(stderr, "get command information error\n");
        goto EXIT_FREE;
    }
    while (fgets(buff, 1024, outstream) != NULL)
    {
        if (strstr(buff, "error") != NULL || strstr(buff, "错误") != NULL)
        {
            goto EXIT;
        }
    }
    //	printf("3\n");
    // sprintf(buf, "nmcli con mod %s connection.autoconnect yes", name); // 修改网络连接的单项参数，修改为自动连接
    if (passwd == NULL || strlen(passwd) == 0)
    {
        sprintf(buf, "nmcli dev wifi connect '%s' name '%s'", name, name);
    }
    else
    {
        sprintf(buf, "nmcli dev wifi connect '%s' password '%s' name '%s'", name, passwd, name);
    }
    if (systemCmdTimeout((const char *)buf, timeout) < -1)
        goto EXIT;

    err = 0;
EXIT:
    pclose(outstream);
EXIT_FREE:
    free(buf);
    free(buff);
    return err;
}



int nm_wireless_disconnect(const char *name)
{
    char buff[1024];
    sprintf(buff, "nmcli dev disconnect %s", name);
    if (system(buff) < 0) //
        return -1;
    return 0;
}
