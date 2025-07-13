/*///------------------------------------------------------------------------------------------------------------------------//
		进程信息获取时的网络数据相关
说 明 : 原理：读取本级上所有的活跃网络连接的信息到一个结构体数组中，并且每隔一定时间区更新这个数组，需要读取所有进程的fd中的inode然后进行匹配
日 期 : 2024.12.11

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <map>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "string.h"
#include "tpNetPcap.h"



std::atomic<bool> PcapkeepRunning;
void get_process_all_connections(pid_t pid, tpNetGetHandle &net_handle);

//快速判断目录是否为空，(直接依赖linux系统接口，效率高)
int is_directory_empty(const char *path) {
    int fd = open(path, O_RDONLY | O_DIRECTORY);
    if (fd < 0) {
        // 无法打开目录（可能不存在或权限不足）
        return -1;
    }

    char buffer[1024];
#ifdef __NR_getdents
    int nread = syscall(SYS_getdents, fd, buffer, sizeof(buffer));
#else
	int nread = syscall(SYS_getdents64, fd, buffer, sizeof(buffer));
#endif
    close(fd);

    if (nread <= 0) {
        // 读取失败或目录为空
        return 1;
    }

    struct dirent *entry;
    for (int offset = 0; offset < nread;) {
        entry = (struct dirent *)(buffer + offset);
        offset += entry->d_reclen;

        // 忽略 "." 和 ".."
        if (entry->d_name[0] == '.' &&
            (entry->d_name[1] == '\0' || 
             (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) {
            continue;
        }
        return 0; // 非空目录
    }

    return 1; // 空目录
}
// 获取指定进程的网络连接
void get_process_connections(tpString net_type, tpNetConnects *connection)
{
	char path[256];
	connection_info **connections=connection->connections;
	int connection_count = connection->connection_count;
	snprintf(path, sizeof(path), "/proc/net/%s", net_type.c_str());
	//printf("path=%s", path);
	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		printf("open %s error\n", path);
		return;
	}

	char line[256];
	fgets(line, sizeof(line), file); // 跳过标题行
	while (fgets(line, sizeof(line), file) != NULL && connection_count < MAX_CONNECTIONS)
	{
		// 解析 TCP 连接信息
		char local[64], remote[64], state[4];
		unsigned long rx_queue, tx_queue;
		unsigned int refcnt=0, inode=0;

		// 示例行：  0: 00000000:1F90 00000000:0000 01 00000000:00000000 0 0 0 0 0
		sscanf(line, "%*d: %64s %64s %x %*x:%*x %*x:%*x %*x %*d %*d %d %*u", local, remote, &refcnt,&inode);
		if(refcnt!=0x01)		//ESTABLISHED=0x01
			continue;
		
		// 提取本地 IP 和端口
		char *local_ip_port = strtok(local, ":");
		char *local_port_hex = strtok(NULL, ":");
		if (local_ip_port && local_port_hex)
		{
			connections[connection_count] = (connection_info *)malloc(sizeof(connection_info));
			struct in_addr addr;
			addr.s_addr = (uint32_t)strtoul(local_ip_port, NULL, 16);
			inet_ntop(AF_INET, &addr, connections[connection_count]->local_ip, INET_ADDRSTRLEN);
			connections[connection_count]->local_port = (uint16_t)strtoul(local_port_hex, NULL, 16); // ntohs((uint16_t)strtoul(local_port_hex, NULL, 16));
			// printf("ip=%s,port=%d\n",connections[connection_count].local_ip,connections[connection_count].local_port);
		}
		else
			continue;
		//设置inode
		connections[connection_count]->inode=inode;
		// 提取远程 IP 和端口
		char *remote_ip_port = strtok(remote, ":");
		char *remote_port_hex = strtok(NULL, ":");
		if (remote_ip_port && remote_port_hex)
		{
			struct in_addr addr;
			addr.s_addr = (uint32_t)strtoul(remote_ip_port, NULL, 16);
			inet_ntop(AF_INET, &addr, connections[connection_count]->remote_ip, INET_ADDRSTRLEN);
			connections[connection_count]->remote_port = strtoul(remote_port_hex, NULL, 16); // ntohs((uint16_t)strtoul(remote_port_hex, NULL, 16));
			connection_count++;
		}
		//printf("count=%d,ips=%s,ips=%s,inode=%d\n",connection_count-1,connections[connection_count-1]->local_ip,connections[connection_count-1]->remote_ip,connections[connection_count-1]->inode);
	}
	fclose(file);
	connection->connection_count = connection_count;
}



void get_process_all_connections(tpNetConnects *connection)
{
	pthread_mutex_lock(&connection->mutex);
	connection->connection_count = 0;
	char path[32];
	snprintf(path, sizeof(path), "/proc/net");
	if(is_directory_empty(path)==1)		//目录为空直接返回
	{
		pthread_mutex_unlock(&connection->mutex);
		return ;
	}
	get_process_connections("tcp", connection);
	get_process_connections("udp", connection);
	get_process_connections("tcp6", connection);
	get_process_connections("udp6", connection);
	//	get_process_connections(pid,"unix",net_handle);
	pthread_mutex_unlock(&connection->mutex);
}



tpNetPcap::tpNetPcap()
{
	net_handle = new tpNetGetHandle();

}

tpNetPcap::~tpNetPcap()
{
	if(net_handle)
		delete net_handle;
	net_handle = nullptr;

}


int findMappingPid(tpNetConnects *connect ,const char *src_ip, uint16_t src_port,const char *dst_ip, uint16_t dst_port)
{
	connection_info **connections = connect->connections;

	for (int i = 0; i < connect->connection_count; i++)
	{ 
		//printf("Proc:%d Captured packet: Src IP: %s, Src Port: %d, Dst IP: %s, Dst Port: %d\n",
		//						connections[i]->pid,src_ip, src_port, dst_ip, dst_port);
		//printf("debug:pid=%d,ips=%s,port=%d,ips=%s,port=%d\n", connections[i]->pid,connections[i]->local_ip,connections[i]->local_port,connections[i]->remote_ip,connections[i]->remote_port );
		if ((strcmp(src_ip, connections[i]->local_ip) == 0 && src_port == connections[i]->local_port) &&
			(strcmp(dst_ip, connections[i]->remote_ip) == 0 && dst_port == connections[i]->remote_port))
		{
			//printf("Proc:%d Captured packet: Src IP: %s, Src Port: %d, Dst IP: %s, Dst Port: %d\n",connections[i]->pid,src_ip, src_port, dst_ip, dst_port);
			return connections[i]->pid;
		}
		else if((strcmp(src_ip, connections[i]->remote_ip) == 0 && src_port == connections[i]->remote_port) &&
			(strcmp(dst_ip, connections[i]->local_ip) == 0 && dst_port == connections[i]->local_port))
		{
			//printf("Proc:%d Captured packet: Src IP: %s, Src Port: %d, Dst IP: %s, Dst Port: %d\n",connections[i]->pid,src_ip, src_port, dst_ip, dst_port);
			return connections[i]->pid;
		}
	}
	return -1;
}


int tpNetPcap::setDataByteu(uint32_t len)
{
	pthread_rwlock_wrlock(&(net_handle->rwlock));
	net_handle->data_byte_u += len;
	pthread_rwlock_unlock(&(net_handle->rwlock)); // 解锁
	return 0;
}

int tpNetPcap::setDataByted(uint32_t len)
{
	pthread_rwlock_wrlock(&(net_handle->rwlock));
	net_handle->data_byte_d += len;
	pthread_rwlock_unlock(&(net_handle->rwlock)); // 解锁
	return 0;
}

int tpNetPcap::init()
{
	if(net_handle->net_init.load())
		return 0;
	if (net_handle->init() < 0)
		return -1;
	return 0;
}
void tpNetPcap::tpfree()
{
	net_handle->tpfree();
}


long int tpNetPcap::getDataByteu()
{
	pthread_rwlock_wrlock(&net_handle->rwlock); // 写
	long int len=net_handle->data_byte_u;
	net_handle->data_byte_u=0;
	pthread_rwlock_unlock(&net_handle->rwlock); // 解锁
	return len;
}
long int tpNetPcap::getDataByted()
{
	pthread_rwlock_wrlock(&net_handle->rwlock); // 写
	long int len=net_handle->data_byte_d;
	net_handle->data_byte_d=0;
	pthread_rwlock_unlock(&net_handle->rwlock); // 解锁
	return len;
}
