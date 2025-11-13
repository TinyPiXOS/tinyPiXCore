#ifndef _NETWORK_PCAP_H_
#define _NETWORK_PCAP_H_


#include <atomic>
#include <sys/time.h>
#include <sys/stat.h> //stat
#include <net/if.h>
#include <ifaddrs.h>
#include "pcap/pcap.h"
#include <pthread.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <TpCore.h>

#define MAX_CONNECTIONS 256

struct TpNetGetHandle;
struct TpNetConnects;
typedef unsigned long TpNetInode;
void get_process_all_connections(TpNetConnects *connection);
int findMappingPid(TpNetConnects *connect ,const char *src_ip, uint16_t src_port,const char *dst_ip, uint16_t dst_port);
void *thread_pcap_cpature(void *param);

extern std::atomic<bool> PcapkeepRunning;

// 结构体存储连接信息
typedef struct
{
	char local_ip[INET_ADDRSTRLEN];
	uint16_t local_port;
	char remote_ip[INET_ADDRSTRLEN];
	uint16_t remote_port;
	TpNetInode inode;
	int pid;
} connection_info;

struct TpNetConnects{
	connection_info *connections[MAX_CONNECTIONS]; // 进程的所有网络链接
	int connection_count;
	pthread_mutex_t mutex;
	TpNetConnects()
	{
		if (pthread_mutex_init(&mutex, NULL) < 0)
		{
			return ;
		}
	}
	void TpFree()
	{
		for(int i=0;i<connection_count;i++)
			free(connections[i]);
	}
};

struct TpNetGetHandle
{
//	connection_info *connections[MAX_CONNECTIONS]; // 进程的所有网络链接
//	int connection_count;
	pthread_mutex_t mutex;
	long int last_data_byte_u; // 上次下载字节数
	long int last_data_byte_d; // 上次上传字节数
	long int data_byte_u;	   // 当前下载字节数
	long int data_byte_d;	   // 当前上传字节数
	pthread_rwlock_t rwlock;
	std::atomic<bool> net_init;
	TpNetGetHandle() : data_byte_u(0), data_byte_d(0), last_data_byte_u(0), last_data_byte_d(0) { net_init.store(false); }

	int init()
	{
		if (pthread_rwlock_init(&rwlock, NULL) < 0)
			return -1;
		if (pthread_mutex_init(&mutex, NULL) < 0)
		{
			pthread_rwlock_destroy(&rwlock);
			return -1;
		}
		net_init.store(true);
		return 0;
	}
	void TpFree()
	{
		if (net_init.load() == true)
		{
			// printf("销毁锁\n");
			pthread_mutex_lock(&mutex);
			pthread_mutex_unlock(&mutex);
			pthread_mutex_destroy(&mutex);
			pthread_rwlock_destroy(&rwlock);
		}
	}
};



//pcap库抓包，
class TpNetPcap{
public:
	TpNetPcap();
	~TpNetPcap();
public:
	void getProcessConnections(pid_t pid, TpString net_type);
	TpNetGetHandle *getNetHandle();
	long int getDataByteu();
	long int getDataByted();
	int setDataByteu(uint32_t len);
	int setDataByted(uint32_t len);
	void TpFree();
	int init();
	int update();

private:
	TpNetGetHandle *net_handle;
};



#endif
