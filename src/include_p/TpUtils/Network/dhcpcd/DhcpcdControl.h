#ifndef DHCPCD_CONTROL_H
#define DHCPCD_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// 返回值定义
#define DHCPCD_SUCCESS 0
#define DHCPCD_ERROR -1
#define DHCPCD_NO_LEASE -2
#define DHCPCD_NOT_RUNNING -3

// 状态定义
typedef enum {
    DHCP_STATE_UNKNOWN = 0,
    DHCP_STATE_DYNAMIC,
    DHCP_STATE_STATIC,
    DHCP_STATE_BOUND,
    DHCP_STATE_RENEWING,
    DHCP_STATE_REBINDING
} dhcp_state_t;

// DNS状态
typedef enum {
    DNS_STATE_UNKNOWN = 0,
    DNS_STATE_DYNAMIC,
    DNS_STATE_STATIC
} dns_state_t;

// 租约信息结构
typedef struct {
    char ip_address[16];        // IPv4地址
    char subnet_mask[16];       // 子网掩码
    char gateway[16];           // 网关
    char dhcp_server[16];       // DHCP服务器
    char dns_servers[256];      // DNS服务器列表（逗号分隔）
    unsigned int lease_time;    // 租约时间（秒）
    unsigned int renew_time;    // 续租时间（秒）
    unsigned int rebind_time;   // 重新绑定时间（秒）
    unsigned int expire_time;   // 过期时间（秒）
    int remaining_seconds;      // 剩余秒数
} dhcp_lease_t;

#endif // DHCPCD_CONTROL_H