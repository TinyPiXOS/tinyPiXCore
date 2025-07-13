/*///------------------------------------------------------------------------------------------------------------------------//
		进程管理
说 明 : 存在的问题：无法应对ppid被修改的情况，假设进程100创建了进程500,然后修改其ppid=200,那只能认为其属于200的子进程
日 期 : 2024.12.11
/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <sys/mount.h> //需要在<linux/fs.h>之前，否则会冲突
#include <sys/statvfs.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <unistd.h>
#include <dirent.h>
#include "tpProcessManage.h"
#include "tpSystemInfoDef.h"
#include "tpSystemDataManage.h"
#include "tpNetPcap.h"
#include "tpVector.h"

typedef std::map<int, tpProcessInfo *> ProcInfoMap;

struct PcapThreadHandle
{
	tpString name;
	ProcInfoMap *data_map;
	pcap_t *pcap_handle;
///	pthread_t pthread;
	std::thread thread_t;
	char local_ip[INET_ADDRSTRLEN];		//主机ip
	char local_ip6[INET6_ADDRSTRLEN];
	tpNetConnects *net_connect;			//本机所有连接的信息
	PcapThreadHandle(tpString n,pcap_t *pt,ProcInfoMap *d_m) : name(n),pcap_handle(pt),data_map(d_m) {}
};


struct tpProcessManageData
{
	tpHash<std::string, DiskstatsDataHandle *> disk_map;

	tpList<PcapThreadHandle> pcap_list;		//网卡pcap列表
	std::map<int,tpNetPcap *> netMap;
	ProcInfoMap processMap; // 主机上所有进程
	tpSystemDataManage lock;				//		

	tpNetConnects net_connect;	
	tpProcessManageData()
	{
	}
};

namespace{
// pcap初始化(网络使用)
pcap_t *pcap_init(const char *dev)
{
	// 打开网络设备
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle = pcap_open_live(dev, 65535, 0, 1000, errbuf);
	if (handle == NULL)
	{
		fprintf(stderr, "Could not open device: %s\n", errbuf);
		return NULL;
	}
	// 打印handle地址
	struct bpf_program fp;
	pcap_compile(handle, &fp, "tcp or udp", 1, PCAP_NETMASK_UNKNOWN); // 协议类型可以使用"any"来获取所有类型
	pcap_setfilter(handle, &fp);
	return handle;
}

// 判断是否虚拟网卡
bool is_virtual_interface(const std::string &interface_name)
{
	std::string type_path = "/sys/class/net/" + interface_name + "/type";
	std::ifstream type_file(type_path);

	if (type_file)
	{
		int type;
		type_file >> type;
		// type == 1 表示以太网，type == 2 表示无线，其他可能是虚拟网卡
		if (type == 1 || type == 2)
			return false;
	}
	return true; // 如果无法读取，默认认为是虚拟网卡
}

// 获取主机上所有网卡
void get_all_network_devices(tpVector<tpString> &devices)
{
	const char *net_path = "/sys/class/net/";
	// 使用 opendir 和 readdir 遍历目录
	DIR *dir = opendir(net_path);
	struct dirent *entry;
	if (dir != nullptr)
	{
		while ((entry = readdir(dir)) != nullptr)
		{
			tpString interface_name(entry->d_name);

			// 过滤虚拟网卡和环回接口
			if (!is_virtual_interface(interface_name) && interface_name != "lo")
			{

				if (interface_name.find("docker") != tpString::npos)
					continue;
				if (interface_name.find("lo") != tpString::npos)
					continue;
				devices.push_back(interface_name);
			}
		}
		closedir(dir);
	}
	else
	{
		fprintf(stderr, "Could not open get netwoork device\n");
	}
}



// 仅提取vector
// 功能约等于getAllProcessMap，但是vector格式
/*
int getAllProcess(tpVector<tpProcessInfo *> &process_list)
{
	DIR *proc_dir = opendir("/proc");
	if (!proc_dir)
	{
		std::cerr << "Error: open /proc" << std::endl;
		return -1;
	}

	struct dirent *entry;
	while ((entry = readdir(proc_dir)) != nullptr)
	{
		if (!isdigit(entry->d_name[0]))
			continue;

		int pid = std::stoi(entry->d_name);
		tpProcessInfo *process = new tpProcessInfo(pid, -1, "");
		if (process->readBasicInfo() == 0)
		{
			// processData->processMap[pid] = process;
			process_list.push_back(process);
		}
		else
		{
			std::cerr << "read process basic info error,can't add to tree" << std::endl;
			delete process;
		}
		//process->readMemoryInfo();
	}
	closedir(proc_dir);
	return 0;
}*/

long getTotalCpuTime()
{
	std::ifstream statFile("/proc/stat");
	if (!statFile.is_open())
	{
		std::cerr << "Error opening /proc/stat." << std::endl;
		return -1;
	}

	std::string line;
	std::getline(statFile, line); // 读取第一行
	std::istringstream iss(line);
	std::string cpu;
	long user, nice, system, idle, iowait, irq, softirq;
	// 提取各字段
	iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
	// 计算总 CPU 时间
	return user + nice + system + idle + iowait + irq + softirq;

}

// 遍历 /proc/<pid>/fd 目录，查找 inode 对应的进程
std::map<tpNetInode, int> mapInodeToPid() 
{
    std::map<tpNetInode, int> inodeToPid;
    DIR* procDir = opendir("/proc");

    if (!procDir) {
        std::cerr << "Failed to open /proc directory" << std::endl;
        return inodeToPid;
    }

    struct dirent* entry;
    while ((entry = readdir(procDir)) != nullptr) 
	{
        if (entry->d_type != DT_DIR)
            continue;

        // 跳过 . 和 ..
        int pid = atoi(entry->d_name);
        if (pid <= 0)
            continue;

        std::string fdPath = "/proc/" + std::to_string(pid) + "/fd";
        DIR* fdDir = opendir(fdPath.c_str());
        if (!fdDir)
            continue;

        struct dirent* fdEntry;
        while ((fdEntry = readdir(fdDir)) != nullptr) {
            if (fdEntry->d_type != DT_LNK)
                continue;

            std::string linkPath = fdPath + "/" + fdEntry->d_name;
            char linkTarget[256] = {0};
            ssize_t len = readlink(linkPath.c_str(), linkTarget, sizeof(linkTarget) - 1);

            if (len != -1) {
                linkTarget[len] = '\0';

                // 查找 "socket:[inode]"
                std::string linkStr = linkTarget;
                if (linkStr.find("socket:[") == 0) {
                    tpNetInode inode = std::stoul(linkStr.substr(8, linkStr.size() - 9));
                    inodeToPid[inode] = pid;
                }
            }
        }

        closedir(fdDir);
    }

    closedir(procDir);
    return inodeToPid;
}




} //命名空间结束


// 根据/proc文件的进程得到每个进程的pid，为进程树做准备
// 生成map格式的进程列表
int tpProcessManage::getAllProcessMap(ProcInfoMap &processMap)
{
	printf("get map of process\n");
	//tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);

	DIR *proc_dir = opendir("/proc");
	if (!proc_dir)
	{
		std::cerr << "opendir error" << std::endl;
		return -1;
	}

	struct dirent *entry;
	while ((entry = readdir(proc_dir)) != nullptr)
	{
		if (!isdigit(entry->d_name[0]))
			continue;

		int pid = std::stoi(entry->d_name);
		tpProcessInfo *process = new tpProcessInfo(pid, -1, "");
		if (process->readBasicInfo() == 0)
		{
			//processData->processMap[pid] = process;
			processMap[pid] = process;
		}
		else
		{
			std::cerr << "read process basic info error,can't add to tree" << std::endl;
			delete process;
		}
		//process->readMemoryInfo();
	}
	closedir(proc_dir);
	return 0;
}
// 使用智能指针
/*int getAllProcessMap(std::map<int, std::shared_ptr<tpProcessData>>& processData->processMap)
{
	//std::map<int, std::shared_ptr<ProcessInfo>> newMap;
	return 0;
}*/

// 数据包处理回调函数
void tpProcessManage::packet_handler(unsigned char *args, const struct pcap_pkthdr *header, const unsigned char *packet)
{
	//printf("Packet captured! Length: %d,\n", header->len); // 调试信息
	PcapThreadHandle *handle = (PcapThreadHandle *)args;
	ProcInfoMap *data_map =handle->data_map;
	pcap_t *pcap_handle = handle->pcap_handle;
	uint8_t direction=0;
	struct ip *ip_header = (struct ip *)(packet + sizeof(struct ether_header));
	struct tcphdr *tcp_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));
	//	int header_len=sizeof(struct ether_header)+sizeof(struct ip)+sizeof(struct tcphdr);
	//	const unsigned char *data=packet+header_len;

	char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ip_header->ip_src, src_ip, sizeof(src_ip));
	inet_ntop(AF_INET, &ip_header->ip_dst, dst_ip, sizeof(dst_ip));
	uint16_t src_port = ntohs(tcp_header->source);
	uint16_t dst_port = ntohs(tcp_header->dest);

	//printf("Captured packet: Src IP: %s, Src Port: %d, Dst IP: %s, Dst Port: %d, Len: %d\n", src_ip, src_port, dst_ip, dst_port, header->len);
	if (PcapkeepRunning == false)
	{
		pcap_breakloop(pcap_handle);
	}
	int pid=findMappingPid(handle->net_connect ,src_ip, src_port,dst_ip, dst_port);
	if(pid<0)
		return ;
	if(strcmp(src_ip,handle->local_ip) == 0)	//上传
	{
		direction=1;
	}
	else if(strcmp(dst_ip,handle->local_ip) == 0)//下载
	{
		direction=2;
	}
	else
		return ;
	auto process = data_map->find(pid);
	if (process != data_map->end())
	{
		tpNetPcap *net_handle=(tpNetPcap *)process->second->getNetHandle();
		if(direction==1)
			net_handle->setDataByteu(header->len);
		else
			net_handle->setDataByted(header->len);
	}

}

// pcap循环捕获
void *tpProcessManage::thread_pcap_cpature(void *param)
{
	PcapThreadHandle *pcap_thread = (PcapThreadHandle *)param;
	pcap_t *pcap_handle = pcap_thread->pcap_handle;
	pcap_loop(pcap_handle, -1, tpProcessManage::packet_handler, reinterpret_cast<unsigned char *>(param));
	return NULL;
}

std::map<int, tpProcessInfo *> *tpProcessManage::getProcessInfoMap()
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	return &procData->processMap;
}

tpProcessManage::tpProcessManage(bool enabled,uint16_t samp)
{
	data_ = new tpProcessManageData();
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);

	// 网卡
	tpVector<tpString> devices;
	get_all_network_devices(devices);
	for (const auto &dev : devices)
	{
		pcap_t *pcap_handle = pcap_init(dev.c_str());
		//PcapThreadHandle pcap_thread_handle(pcap_handle);
		
		procData->pcap_list.push_back(PcapThreadHandle(dev,pcap_handle,&procData->processMap));
	}
	initNetworkMonitor(); // 开启网卡的监测
	if (enabled)
	{
		procData->lock.running = true;
		procData->lock.thread_t = std::thread(&tpProcessManage::threadUpdateStat, this, samp);
	}
}

tpProcessManage::~tpProcessManage()
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);

	if (procData->lock.running == false)
		return;

	procData->lock.running = false;
	if (procData->lock.thread_t.joinable())
		procData->lock.thread_t.join(); // 等待线程完成

	PcapkeepRunning = false; ///
	// 关闭监测
	deinitNetworkMonitor();
	// 关闭pcap
	for (auto &pcap : procData->pcap_list)
		pcap_close(pcap.pcap_handle);

	// 删除进程树
	for (auto it = procData->processMap.begin(); it != procData->processMap.end(); ++it)
	{
		delete it->second;
	}

	if (procData)
	{
		delete procData;
		procData = nullptr;
		data_ = nullptr;
	}
}

void tpProcessManage::threadUpdateStat(uint16_t time_samp)
{
	while(1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time_samp));
		updateProcessTree();
		getProcessInfo(1);
	}
}

void tpProcessManage::update()
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	if(procData->lock.running)
		return ;
	printf("get tree\n");
	updateProcessTree();
	printf("get info\n");
	getProcessInfo(1);
}


// 更新进程列表
int tpProcessManage::updateProcessTree(uint16_t samp_time)
{
	return 0;
}
int tpProcessManage::updateProcessTree()
{
	tpProcessManageData* processData = static_cast<tpProcessManageData *>(data_);
	static long int cputotal_last = 0, cputotal = 0;
	cputotal = getTotalCpuTime();
	double cputime_diff = cputotal - cputotal_last;
	cputotal_last = cputotal;
	static int debug_flag=0;
	ProcInfoMap map_temp;
	getAllProcessMap(map_temp); // 临时map

	for (auto it = map_temp.begin(); it != map_temp.end(); ++it)
	{ // 更新和新增
		auto process = processData->processMap.find(it->first);
		if (process != processData->processMap.end())
		{
			process->second->updateBasicInfo(it->second,cputime_diff);
		}
		else
		{
			tpProcessInfo *process_new=new tpProcessInfo(it->second->getPid(),it->second->getPpid(),it->second->getName());
			process_new->updateBasicInfo(it->second,cputime_diff);
			processData->processMap[it->first] = process_new;
			printf("新增pid=%d,ppid=%d,addr=%p\n", it->first, it->second->getPpid(),processData->processMap[it->first]);
		}
	}
	for (auto it = processData->processMap.begin(); it != processData->processMap.end();)
	{											 // 删除
		auto process = map_temp.find(it->first); // 如果进程在新的进程map中没有
		if (process != map_temp.end())
		{
			// 能找到;
			++it;
		}
		else
		{
			printf("删除进程%d\n", it->first);
			if (it->second->getParent())
			{
				it->second->getParent()->removeChild(it->second);
			}
			//it->second->tpfree();
			delete it->second;
			it = processData->processMap.erase(it); // 从 map 中删除
		}
	}
	//printf("add pid1 addr=%p,(update tree%ld)\n", processData->processMap[1], processData->processMap.size() );
	tpProcessInfo *root = nullptr;
	for (auto it = processData->processMap.begin(); it != processData->processMap.end(); ++it)
	{
		int pid=it->first;
		tpProcessInfo *process = it->second;
		if (process->getPpid() > 0 && processData->processMap.count(process->getPpid() ))
		{
			tpProcessInfo *parent = processData->processMap[process->getPpid()];
			tpList<tpProcessInfo *> children=parent->getChildren();
			auto it_=std::find_if(children.begin(),children.end(),[pid](tpProcessInfo *obj){ return obj->getPid() == pid;});
			if(it_==children.end())
			{
				// 如果没有，则添加子进
				parent->addChild(process);
				//printf("Added PID %d as child of PID %d\n", process->getPid(), process->getPpid());
			}
		}
		else if (process->getPpid()  == 0 && process->getPid()  == 1)
		{
			root = process;
		}
	}
	debug_flag=1;
	for (auto& pair : map_temp) {
		delete pair.second;
    }
	return 0;
}

// 从map中找到进程在设备树中的地址
tpProcessInfo *tpProcessManage::findProcess(int pid)
{
	tpProcessManageData* processData = static_cast<tpProcessManageData *>(data_);

	auto it = processData->processMap.find(pid);
	if (it != processData->processMap.end())
		return it->second;
	return NULL;
}

// 打印某个进程的进程树
void tpProcessManage::printProcessTree(tpProcessInfo *data, int level) const
{
	if (data == NULL)
		return;
	for (int i = 0; i < level; ++i)
		std::cout << "  ";
	std::cout << "PID: " << data->getPid() << ", Name: " << data->getName()
			  << ", PPID: " << data->getPpid()
			  << ", CPU: " << data->getCpuUsage() << ", Mem: " << data->getMemoryUsage()
			  << " kB"
			  << ", GPU Usage: " << data->getGpuUsage() << "%\n";

	tpList<tpProcessInfo *>children=data->getChildren();
	for (tpProcessInfo *child : children)
	{
		printProcessTree(child, level + 1);
	}
}
int tpProcessManage::updateNetLocalAddr()
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	tpList<PcapThreadHandle> *pcap_list=&procData->pcap_list;

	struct ifaddrs *interfaces = nullptr, *ifa = nullptr;

	// 获取本机的所有网络接口
	if (getifaddrs(&interfaces) == -1) {
		perror("getifaddrs");
		return -1;
	}

	// 遍历所有接口，查找指定网卡的 IP 地址
	for (ifa = interfaces; ifa != nullptr; ifa = ifa->ifa_next) 
	{
		if (ifa->ifa_addr == nullptr) 
			continue;
		for(auto it=pcap_list->begin(); it != pcap_list->end(); it++)
		{
			tpString interfaceName=it->name;
			if (interfaceName == tpString(ifa->ifa_name)) 
			{
				int family = ifa->ifa_addr->sa_family;
				char *addr = it->local_ip6;
				// 只获取 IPv4 和 IPv6 地址
				
				if (family == AF_INET) 
				{
					addr = it->local_ip;
					if (inet_ntop(family, (void*)&(((struct sockaddr_in*)ifa->ifa_addr)->sin_addr),addr,INET_ADDRSTRLEN)) {
						//std::cout << "Interface: " << ifa->ifa_name << " | IP Address: " << addr << std::endl;
					} 
					else {
						perror("inet_ntop");
					}
				}
				else if(family == AF_INET6)
				{
					addr = it->local_ip6;
					if (inet_ntop(family,(void*)&(((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr),addr,INET6_ADDRSTRLEN)) {
						//std::cout << "Interface: " << ifa->ifa_name << " | IP Address: " << addr << std::endl;
					} 
					else {
						perror("inet_ntop");
					}
				}
			}
		}
	}
	// 释放内存
	freeifaddrs(interfaces);
	return 0;
}

int tpProcessManage::updateConnectInfo()
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	tpNetConnects *connects=&procData->net_connect;
	get_process_all_connections(connects);
	auto inodeToPid = mapInodeToPid();
	connection_info **connections=connects->connections ; // 进程的所有网络链接
	for (int i=0;i<connects->connection_count;i++)
	{
		if (inodeToPid.count(connections[i]->inode)) {
			connections[i]->pid = inodeToPid[connections[i]->inode];
			//printf("debug:pid=%d,ips=%s,port=%d,ips=%s,port=%d\n", connections[i]->pid,connections[i]->local_ip,connections[i]->local_port,connections[i]->remote_ip,connections[i]->remote_port );
		}
	}
	return 0;
}

int tpProcessManage::initNetworkMonitor()
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);

	if (PcapkeepRunning == true)
		return 0;
	PcapkeepRunning = true;
	for (auto &pcap_thread : procData->pcap_list)
	{
		pcap_thread.data_map = &procData->processMap;
		pcap_thread.net_connect= &procData->net_connect;
		printf("debug addr:%p\n",pcap_thread.data_map);
		pcap_thread.thread_t = std::thread(&tpProcessManage::thread_pcap_cpature, this,(void *)(&pcap_thread));
	}
	return 0;
}

// 关闭网络监测
int tpProcessManage::deinitNetworkMonitor()
{
	tpProcessManageData* processData = static_cast<tpProcessManageData *>(data_);

	if (PcapkeepRunning == false)
		return 0;
	PcapkeepRunning = false;
	for (auto &pcap_thread : processData->pcap_list)
	{
		if (pcap_thread.thread_t.joinable())
			pcap_thread.thread_t.join(); // 等待线程完成
		printf("线程结束\n");
	}
	return 0;
}

// 计算某个进程(应用)及其子进程的资源信息（）
int tpProcessManage::countProcessInfo(tpProcessInfo *proc,double samp)
{
	// 读取进程其他资源信息
	proc->updateOtherInfo(samp);

	proc->clearAppUsage();
	tpList<tpProcessInfo *>children=proc->getChildren();
	for (tpProcessInfo *child : children)
	{
		countProcessInfo(child,samp);
		proc->sumUsage(
			child->getAppCpuUsage(),
			0,
			child->getAppMemoryUsage(),
			child->getAppDiskReadSpeed(),
			child->getAppDiskWriteSpeed(),
			child->getAppNetUpSpeed(),
			child->getAppNetDownSpeed());

	}
	return 0;
}

int tpProcessManage::getProcessInfo(int pid)
{
	tpProcessInfo *app = findProcess(pid);
	if (app == NULL)
	{
		printf("没有找到PID=%d的应用\n", pid);
		std::cerr << "找不到pid为" << pid << "的进程" << std::endl;
        return -1;
	}
	return getProcessInfo(app);
}

int tpProcessManage::getProcessInfo(tpProcessInfo *process)
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	
	// 计算
	static struct timeval last_time;
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0)
	{
		return -1;
	}
	if (last_time.tv_sec == 0 && last_time.tv_usec == 0)
	{
		last_time = tv;
		return 0;
	}

	double time_samp = (tv.tv_sec * 1000LL) + ((double)tv.tv_usec / 1000.0) - (last_time.tv_sec * 1000LL) - ((double)last_time.tv_usec / 1000.0);
//	printf("time_samp=%lf\n", time_samp);
	//计算进程树的资源信息
	procData->lock.dataWriteLock();
	printf("计算\n");
	//读ip
	updateNetLocalAddr();
	updateConnectInfo();
	countProcessInfo(process,time_samp);
	printf("计算ok\n");
	procData->lock.dataUnlock();
	last_time = tv;
	return 0;
}

int tpProcessManage::updateInfo()
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	return 0;
}


//cpu
double tpProcessManage::getCpuUsage(int pid)
{
	tpProcessInfo *app = findProcess(pid);
	return getCpuUsage(app);
}
double tpProcessManage::getCpuUsage(tpProcessInfo *app)
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	double usage ;
	procData->lock.dataReadLock();
	usage=app->getAppCpuUsage();
	procData->lock.dataUnlock();
	return usage;
}
//gpu
double tpProcessManage::getGpuUsage(int pid)
{
	tpProcessInfo *app = findProcess(pid);
	return getGpuUsage(app);
}
double tpProcessManage::getGpuUsage(tpProcessInfo *app)
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	double usage ;
	procData->lock.dataReadLock();
	usage=app->getAppGpuUsage();
	procData->lock.dataUnlock();
	return usage;
}
//memory
double tpProcessManage::getMemoryUsage(int pid)
{
	tpProcessInfo *app = findProcess(pid);
	return getMemoryUsage(app);
}
double tpProcessManage::getMemoryUsage(tpProcessInfo *app)
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	double usage ;
	procData->lock.dataReadLock();
	usage=app->getAppMemoryUsage();
	procData->lock.dataUnlock();
	return usage;
}
//disk
double tpProcessManage::getDiskReadSpeed(int pid)
{
	tpProcessInfo *app = findProcess(pid);
	return getDiskReadSpeed(app);
}
double tpProcessManage::getDiskReadSpeed(tpProcessInfo *app)
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	double speed ;
	procData->lock.dataReadLock();
	speed=app->getAppDiskReadSpeed();
	procData->lock.dataUnlock();
	return speed;
}
//disk
double tpProcessManage::getDiskWriteSpeed(int pid)
{
	tpProcessInfo *app = findProcess(pid);
	return getDiskWriteSpeed(app);
}
double tpProcessManage::getDiskWriteSpeed(tpProcessInfo *app)
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	double speed ;
	procData->lock.dataReadLock();
	speed=app->getAppDiskWriteSpeed();
	procData->lock.dataUnlock();
	return speed;
}
//network
double tpProcessManage::getNetUpSpeed(int pid)
{
	tpProcessInfo *app = findProcess(pid);
	return getNetUpSpeed(app);
}
double tpProcessManage::getNetUpSpeed(tpProcessInfo *app)
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	double speed ;
	procData->lock.dataReadLock();
	speed=app->getAppNetUpSpeed();
	procData->lock.dataUnlock();
	return speed;
}
//network
double tpProcessManage::getNetDownSpeed(int pid)
{
	tpProcessInfo *app = findProcess(pid);
	return getNetDownSpeed(app);
}
double tpProcessManage::getNetDownSpeed(tpProcessInfo *app)
{
	tpProcessManageData* procData = static_cast<tpProcessManageData *>(data_);
	double speed ;
	procData->lock.dataReadLock();
	speed=app->getAppNetDownSpeed();
	procData->lock.dataUnlock();
	return speed;
}