/*///------------------------------------------------------------------------------------------------------------------------//
		进程/应用信息
说 明 : cpu使用情况读取/proc/pid/stat文件,cpu使用率为采样时刻利用率和采样时间没有关系。
	   内存使用情况读取/proc/pid/status文件，内存使用率为采样时刻的利用率和采样时间没有关系。
	   网络使用情况需要使用工具在后台抓包监测实现
	   磁盘使用/proc/pid/io文件,具体的根据总速率来获取每个进程的占用百分比程序未使用
	   cpu，内存为进程树更新时候实时更新
日 期 : 2024.10.24

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>
#include <fstream>
#include <sys/mount.h> //需要在<linux/fs.h>之前，否则会冲突
#include <sys/statvfs.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <unistd.h>
#include <dirent.h>

#include "tpProcessInfo.h"
#include "tpList.h"
#include "tpSystemInfoDef.h"
#include "tpNetPcap.h"
#include "tpVector.h"

struct tpProcessData;
struct tpNetGetHandle;

typedef std::map<int, tpProcessInfo *> ProcInfoMap;



// 进程磁盘读取使用的句柄（文件的信息）
struct tpDiskGetHandle
{
	unsigned long int disk_read;
	unsigned long int disk_write;
	unsigned long int last_disk_read;
	unsigned long int last_disk_write;
	std::string disk_name;
	tpDiskGetHandle() : disk_read(0), disk_write(0), last_disk_read(0), last_disk_write(0) {}
};

//pcap传参使用
struct PcapThreadHandle
{
	ProcInfoMap *data_map;
	pcap_t *pcap_handle;
	pthread_t pthread;
	PcapThreadHandle(pcap_t *pt) : pcap_handle(pt) {}
};

struct tpCpuGetHandle
{
	long int cpu_time;
	long int cpu_time_last;
	tpCpuGetHandle() : cpu_time(0), cpu_time_last(0) {}
};

//进程资源
struct tpProcessResource
{
	double cpu_usage;
	double gpu_usage;
	long memory_usage;
	double disk_read;
	double disk_write;
	double network_upload;
	double network_download;
	tpProcessResource(double cpu, double gpu, double memory,double disk_r,double disk_w,double net_up,double net_down):
				cpu_usage(cpu), gpu_usage(gpu), memory_usage(memory), disk_read(disk_r), disk_write(disk_w), network_upload(net_up), network_download(net_down)
	{}
	tpProcessResource() : cpu_usage(0.0), gpu_usage(0.0), memory_usage(0), disk_read(0.0), disk_write(0.0), network_upload(0.0), network_download(0.0) {}
	void clear()
	{cpu_usage=0.0;gpu_usage=0.0; memory_usage=0.0; disk_read=0.0; disk_write=0.0; network_upload=0.0; network_download=0.0; }
};

struct tpProcessData : public std::enable_shared_from_this<tpProcessData>
{
	int pid;
	int ppid;
	std::string name;
	std::string status;
	struct timeval last_time;

	//进程资源使用
	double cpu_usage;
	double gpu_usage;
	long memory_usage;
	double disk_read_speed;
	double disk_write_speed;
	double network_upload_speed;
	double network_download_speed;

	double disk_read;
	double disk_write;
	double network_upload;
	double network_download;
	
	//进程以及其子进程的资源使用总和
	tpProcessResource resource_s;

	tpNetPcap net_handle;				//class
	tpDiskGetHandle disk_handle;		//struct
	tpCpuGetHandle cpu_handle;			//struct

	void updateInfo(int new_pid, int new_ppid, const std::string &new_name, double cpu_time, double new_memory)
	{
		ppid = new_ppid;
		pid = new_pid;
		name = new_name; // 更新内部信息
		cpu_handle.cpu_time = cpu_time;
		memory_usage = new_memory; // 更新内部信息
	}

	// 智能指针
	//	std::shared_ptr<tpProcessData> parent; // 指向父进程的指针
	//	tpVector<std::shared_ptr<tpProcessData>> children;
	//	void addChild(const std::shared_ptr<tpProcessData>& child) {
	//		children.push_back(child);
	//		child->parent = shared_from_this(); // 设置父进程
	//	}

	tpProcessData(int pid, int ppid, const tpString &name)
		: pid(pid), ppid(ppid), name(name), cpu_usage(0.0),
		  memory_usage(0),
		  network_upload(0), network_download(0),
		  gpu_usage(0.0),
		  disk_read(0), disk_write(0)
	{
		last_time.tv_sec = 0;
		last_time.tv_usec = 0;
	}
	tpProcessData(){}
	void tpfree()
	{
		net_handle.tpfree();
	}
};


struct tpProcessInfoData
{
	tpHash<tpString, DiskstatsDataHandle *> disk_map;

	struct tpProcessData data;
	tpProcessInfo *parent;
	tpList<tpProcessInfo *> children;

	tpProcessInfoData(int pid, int ppid, const tpString &name):data(pid,ppid,name)
	{
		parent = nullptr;
	}
};
/*
// 遍历所有进程网络信息句柄查找符合的ip
void get_process_connection_info(ProcInfoMap *processMap, uint32_t len,
								 const char *src_ip, uint16_t src_port,
								 const char *dst_ip, uint16_t dst_port)
{
	printf("debug pid 21 addr:%p\n",processMap[1]);
	printf("process count%d\n",processMap->size());
	for (auto it = processMap->begin(); it != processMap->end(); ++it)
	{
		//printf("debug test pid:%d  addr=%p  count:%d\n",info->getPid(),info,it->second->getNetConCount());	
		if (it->second->getNetConCount() == 0)
			continue;
		printf("debug test pid:%d  addr=%p  count:%d\n",it->first,processMap[it->first],it->second->getNetConCount());	
		tpNetGetHandle *net_handle = &it->second->net_handle;
		tpNetPcap *net_handle=&it->second->net_handle;
		connection_info **connections = net_handle->connections;
		
		pthread_mutex_lock(&net_handle->mutex);
		for (int i = 0; i < it->second->net_handle.connection_count; i++)
		{
			if ((strcmp(src_ip, connections[i]->local_ip) == 0 && src_port == connections[i]->local_port))
			{
				//			printf("Captured packet: Src IP: %s, Src Port: %d, Dst IP: %s, Dst Port: %d, Size: %d\n",
				//					src_ip, src_port, dst_ip, dst_port, header->len);
				pthread_rwlock_wrlock(&(net_handle->rwlock));
				net_handle->data_byte_u += len;
				pthread_rwlock_unlock(&(net_handle->rwlock)); // 解锁
			}
			else if ((strcmp(dst_ip, connections[i]->local_ip) == 0 && dst_port == connections[i]->local_port))
			{
				//			printf("Captured packet: Src IP: %s, Src Port: %d, Dst IP: %s, Dst Port: %d, Size: %d\n",
				//					src_ip, src_port, dst_ip, dst_port, header->len);
				pthread_rwlock_wrlock(&(net_handle->rwlock));
				net_handle->data_byte_d += len;
				pthread_rwlock_unlock(&(net_handle->rwlock)); // 解锁
			}
		}
		pthread_mutex_unlock(&net_handle->mutex);
	}
}

// 数据包处理回调函数
void packet_handler(unsigned char *args, const struct pcap_pkthdr *header, const unsigned char *packet)
{
	//	printf("Packet captured! Length: %d,\n", header->len); // 调试信息
	PcapThreadHandle *handle = (PcapThreadHandle *)args;
	ProcInfoMap *data_map = handle->data_map;
	pcap_t *pcap_handle = handle->pcap_handle;
	printf("debug addr:%p\n",data_map);
	printf("add pid1 addr=%p,\n", data_map[1]);
	//	struct ether_header *eth_header = (struct ether_header *)packet;
	struct ip *ip_header = (struct ip *)(packet + sizeof(struct ether_header));
	struct tcphdr *tcp_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));
	//	int header_len=sizeof(struct ether_header)+sizeof(struct ip)+sizeof(struct tcphdr);
	//	const unsigned char *data=packet+header_len;

	char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ip_header->ip_src, src_ip, sizeof(src_ip));
	inet_ntop(AF_INET, &ip_header->ip_dst, dst_ip, sizeof(dst_ip));
	uint16_t src_port = ntohs(tcp_header->source);
	uint16_t dst_port = ntohs(tcp_header->dest);

	// printf("Captured packet: Src IP: %s, Src Port: %d, Dst IP: %s, Dst Port: %d, Size: %d\n", src_ip, src_port, dst_ip, dst_port, header->len);
	if (PcapkeepRunning == false)
	{
		pcap_breakloop(pcap_handle);
	}
	get_process_connection_info(data_map, header->len, src_ip, src_port, dst_ip, dst_port);
}
*/


// 获取设备名和设备号的映射
std::map<dev_t, std::string> getDeviceMap()
{
	std::map<dev_t, std::string> deviceMap;
	FILE *mounts = fopen("/proc/mounts", "r");
	if (!mounts)
	{
		std::cerr << "Failed to open /proc/mounts" << std::endl;
		return deviceMap;
	}

	char device[256], mountPoint[256];
	while (fscanf(mounts, "%255s %255s %*s %*s %*d %*d\n", device, mountPoint) == 2)
	{
		struct stat statBuf;
		if (stat(mountPoint, &statBuf) == 0)
		{
			deviceMap[statBuf.st_dev] = std::string(device);
		}
	}
	fclose(mounts);
	return deviceMap;
}

// 读取文件所在的磁盘
std::string get_device_path(const std::string &file_path)
{
	struct stat file_stat;

	// 获取文件的元数据
	if (stat(file_path.c_str(), &file_stat) != 0)
	{
		std::cerr << "Failed to stat file" << std::endl;
		return "";
	}

	// 读取 /proc/mounts 文件
	std::ifstream mounts_file("/proc/mounts");
	if (!mounts_file.is_open())
	{
		std::cerr << "Failed to open /proc/mounts" << std::endl;
		return "";
	}
	std::string line;
	std::string device_path;
	dev_t file_dev = file_stat.st_dev; // 获取文件的设备号

	while (std::getline(mounts_file, line))
	{
		std::istringstream iss(line);
		std::string dev, mount_point, fs_type, options;

		// 解析 /proc/mounts 的每一行
		if (iss >> dev >> mount_point >> fs_type >> options)
		{
			struct stat mount_stat;
			// 获取挂载点的设备号
			if (stat(mount_point.c_str(), &mount_stat) == 0)
			{
				// 如果设备号匹配，返回设备路径
				if (file_dev == mount_stat.st_dev)
				{
					device_path = dev;
					break;
				}
			}
		}
	}
	return device_path;
}

// 获取文件的设备号
dev_t getDeviceID(const std::string &filePath)
{
	struct stat statBuf;
	if (stat(filePath.c_str(), &statBuf) == 0)
	{
		return statBuf.st_dev;
	}
	return 0;
}

// 读取某个进程使用的文件（需要根据该文件所在磁盘来读取磁盘情况）
/// proc/<pid>/fd
tpVector<std::string> get_process_disk_info(int pid)
{
	tpVector<std::string> openFiles;
	std::string path = "/proc/" + std::to_string(pid) + "/fd"; // 构建文件描述符目录路径
	char buf[MAX_PATH_LENGTH];								   // Buffer for storing the resolved path
	struct stat statBuf;

	DIR *dir = opendir(path.c_str()); // 打开该目录
	if (dir == nullptr)
	{
		perror("Failed to open directory");
		return openFiles;
	}

	struct dirent *entry;
	//	std::cout << "Open file descriptors for process " << pid << ":\n";
	while ((entry = readdir(dir)) != nullptr)
	{
		// 跳过 . 和 ..
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			// 获取文件描述符的实际路径
			std::string fd_path = path + "/" + entry->d_name;
			ssize_t len = readlink(fd_path.c_str(), buf, sizeof(buf) - 1);
			if (len != -1)
			{
				buf[len] = '\0';
				std::string filePath = std::string(buf);

				if (lstat(fd_path.c_str(), &statBuf) == 0)
				{
					// 检查文件类型
					if (S_ISREG(statBuf.st_mode))
					{
						// Additionally, filter out /dev entries
						if (filePath.find("/dev/") != 0)
						{
							openFiles.push_back(filePath);
						}
					}
				}
				else
				{
					std::cerr << "Failed to stat: " << fd_path << std::endl;
				}
			}
			else
			{
				std::cerr << "Failed to read link: " << fd_path << std::endl;
			}
		}
	}
	closedir(dir); // 关闭目录
	return openFiles;
}

// 读取某个设备的使用情况
// proc/diskstats(所有磁盘的使用情况，包括试题磁盘和虚拟块设备磁盘)
int get_disk_info(std::string dev, struct DiskstatsData *disk)
{
	std::string path = "/proc/diskstats";
	std::ifstream diskstats_file(path);
	if (!diskstats_file.is_open())
	{
		return -1;
	}
	struct timeval time_now;
	if (gettimeofday(&disk->time_now, NULL) != 0)
	{
		diskstats_file.close();
		return -1;
	}
	std::string line;
	while (std::getline(diskstats_file, line))
	{
		if (line.find(dev) == 0)
		{
			sscanf(line.c_str(), "%*d %*d %*s %lu %*u %lu %lu %lu %*u %lu %lu %*u %*u %lu",
				   &(disk->reads), &(disk->read_time), &(disk->read_sector),
				   &(disk->writes), &(disk->write_time), &(disk->write_sector),
				   &(disk->weight_time));
			break;
		}
	}
	diskstats_file.close();
	return 0;
}

// 进程对磁盘的占用计算()
int process_disk_occupy(std::string dev,
						struct DiskstatsDataHandle *disk,
						unsigned long int read, unsigned long int write)
{
	get_disk_info(dev, &(disk->data));

	struct timeval tv = disk->data.time_now;
	struct timeval tv_l = disk->last_data.time_now;
	long int time_samp = (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000) - (tv_l.tv_sec * 1000LL) - (tv_l.tv_usec / 1000);
	long int time_acti = disk->data.weight_time - disk->last_data.weight_time;

	unsigned long int read_disk_byte = (disk->data.read_sector - disk->last_data.read_sector) * (disk->data.reads - disk->last_data.reads);
	unsigned long int write_disk_byte = (disk->data.write_sector - disk->last_data.write_sector) * (disk->data.writes - disk->last_data.writes);

	disk->occupy = time_acti / time_samp * (read + write) / (read_disk_byte + write_disk_byte);
	return 0;
}
//
int _(int pid, std::unordered_map<std::string, struct DiskstatsDataHandle *> disk_map)
{
	tpVector<std::string> file_list = get_process_disk_info(pid); // 进程使用的文件列表
	std::map<dev_t, std::string> device_map = getDeviceMap();		 // 磁盘映射列表
	std::map<std::string, int> deviceAccessCount;					 //

	for (const auto &file : file_list)
	{
		dev_t devId = getDeviceID(file);
		if (device_map.count(devId))
		{
			std::string deviceName = device_map[devId];
			disk_map[deviceName]->count++;
			disk_map[deviceName]->dev_num = devId;
		}
		std::cout << file << std::endl;
	}
	return 0;
}

// 时间计算并且更新时间
double get_time_sampl(struct timeval *tv_l)
{
	struct timeval tv;
	double time_samp = 0;
	if (gettimeofday(&tv, NULL) != 0)
		return -1;
	if (tv_l->tv_sec != 0 || tv_l->tv_usec != 0)
	{
		time_samp = (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000) - (tv_l->tv_sec * 1000LL) - (tv_l->tv_usec / 1000);
	}
	tv_l->tv_sec = tv.tv_sec;
	tv_l->tv_usec = tv.tv_usec;
	return time_samp;
}

namespace{

// 读单个进程基础信息(cpu信息以及进程状态，父进程id，进程名)
int read_process_info(int pid, tpProcessData &data)
{
	std::string stat_path = "/proc/" + std::to_string(pid) + "/stat";
	std::ifstream stat_file(stat_path);
	if (!stat_file.is_open())
	{
		return -1;
	}
	std::string name;
	long utime, stime;
	stat_file.ignore(256, '(');			// Skip to the name part
	std::getline(stat_file, name, ')'); // Read the name
	stat_file >> data.status;
	stat_file >> data.ppid;
	for (int i = 0; i < 10; ++i)
		stat_file.ignore(256, ' '); // Skip to utime/stime
	stat_file >> utime >> stime;
	stat_file.close();

	data.name = name;
	data.cpu_handle.cpu_time = utime + stime;
	//	data.cpu_usage = (data.cpu_handle.cpu_time - data.cpu_handle.cpu_time_last) /cputime_diff*100;// sysconf(_SC_CLK_TCK)
	//	data.cpu_handle.cpu_time_last=data.cpu_handle.cpu_time;
	data.pid = pid;
	//	printf("ppid=%d,pid=%d,name=%s,cpu=%lf\n",data.ppid ,pid,name.c_str(),data.cpu_usage);
	//	printf("pid=%d,utime=%ld,stime=%ld,sys=%ld\n",pid,utime,stime,sysconf(_SC_CLK_TCK));
	return 0;
}
// 读单个进程内存信息
int read_memory_info(int pid, tpProcessData &data)
{
	std::string status_path = "/proc/" + std::to_string(pid) + "/status";
	std::ifstream status_file(status_path);
	if (!status_file.is_open())
	{
		std::cerr << "内存信息读取失败: " << strerror(errno) << std::endl;
		return -1;
	}
	std::string line;
	while (std::getline(status_file, line))
	{
		if (line.find("VmRSS:") == 0)
		{
			std::istringstream iss(line);
			std::string key;
			iss >> key >> data.memory_usage;
			break;
		}
	}
	status_file.close();
	return 0;
}





}	//匿名命名空间结束



tpProcessInfo::tpProcessInfo()
{
	std::cerr << "禁止自行构造" << std::endl;
}

tpProcessInfo::tpProcessInfo(int pid, int ppid, const tpString &name)
{
	data_ = new tpProcessInfoData(pid, ppid, name);
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
}

tpProcessInfo::~tpProcessInfo()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	if (procData)
	{
		delete procData;
		procData = nullptr;
		data_ = nullptr;
	}
}

// GPU，
// 启动gpu监测
int tpProcessInfo::initGpuUsage()
{
	return 0;
}
int tpProcessInfo::deinitGpuUsage()
{
	return 0;
}

int tpProcessInfo::readGpuUsage()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);


	// Here we call 'nvidia-smi' to get the GPU usage
	std::array<char, 128> buffer;
	std::string result;

	// Run nvidia-smi and get the output
	std::shared_ptr<FILE> pipe(popen("nvidia-smi --query-gpu=utilization.gpu --format=csv,noheader,nounits", "r"), pclose);
	if (!pipe)
	{
		return false;
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}

	// Extract the GPU usage percentage
	if (!result.empty())
	{
		setGpuUsage(std::stod(result));
	}
	else
	{
		setGpuUsage(0.0); // No GPU usage found
	}
	return 0;
}

int tpProcessInfo::setGpuUsage(double usage)
{
	return 0;
}


// 磁盘读写字节数量
// 对/proc/<pid>/io文件文件解析(进程对文件io的读写字节数量)
int tpProcessInfo::readDiskInfo()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	tpProcessData *data=&procData->data;

	std::string status_path = "/proc/" + std::to_string(data->pid) + "/io";
	std::ifstream status_file(status_path);
	if (!status_file.is_open())
	{
		return -1;
	}
	std::string line;
	double time_samp = get_time_sampl(&data->last_time);
	while (std::getline(status_file, line))
	{
		if (line.find("read_bytes:") == 0)
		{
			std::istringstream iss(line);
			std::string key;
			iss >> key >> data->disk_handle.disk_read;
			data->disk_read = (data->disk_handle.disk_read - data->disk_handle.last_disk_read) / time_samp;
			data->disk_handle.last_disk_read = data->disk_handle.disk_read;
			// break;
		}
		else if (line.find("write_bytes:") == 0)
		{
			std::istringstream iss(line);
			std::string key;
			iss >> key >> data->disk_handle.disk_write;
			data->disk_write = (data->disk_handle.disk_write - data->disk_handle.last_disk_write) / time_samp;
			data->disk_handle.last_disk_write = data->disk_handle.disk_write;
			// break;
		}
	}
	status_file.close();
	return 0;
}

// 网络
// 初始化网络监测
int tpProcessInfo::initNetworkSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);

	if(procData->data.net_handle.init() < 0)
	    return -1;
	return 0;
}

// 获取网络使用情况
int tpProcessInfo::readNetworkSpeed(double samp)
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
//	pthread_rwlock_wrlock(&procData->data.net_handle.rwlock); // 写
	//	data.network_upload=data.net_handle.data_byte_u-data.net_handle.last_data_byte_u;
	//	data.network_download=data.net_handle.data_byte_d-data.net_handle.last_data_byte_d;
	procData->data.network_upload = procData->data.net_handle.getDataByteu();
	procData->data.network_download = procData->data.net_handle.getDataByted();
//	pthread_rwlock_unlock(&procData->data.net_handle.rwlock); // 解锁

	//	data.net_handle.last_data_byte_u=data.network_upload;
	//	data.net_handle.last_data_byte_d=data.network_download;
	if(samp!=0)
	{
		if(procData->data.network_upload!=0)
			procData->data.network_upload_speed= procData->data.network_upload/ samp;
		if(procData->data.network_download!=0)
       		procData->data.network_download_speed=  procData->data.network_download/ samp;
	}

	return 0;
}
//结束网络监测
int tpProcessInfo::deinitNetworkSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	procData->data.net_handle.tpfree();
	return 0;
}

void *tpProcessInfo::getNetHandle()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return &procData->data.net_handle;
}


void tpProcessInfo::sumUsage(double cpu, double gpu, double memory,double disk_r,double disk_w,double net_up,double net_down)
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	
	procData->data.resource_s.cpu_usage += cpu;
	procData->data.resource_s.gpu_usage += gpu;
	procData->data.resource_s.memory_usage += memory;
	procData->data.resource_s.disk_read += disk_r;
	procData->data.resource_s.disk_write += disk_w;
	procData->data.resource_s.network_upload += net_up;
	procData->data.resource_s.network_download += net_down;
}
void tpProcessInfo::clearAppUsage()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	//procData->data.resource_s.clear();
	procData->data.resource_s.cpu_usage = procData->data.cpu_usage;
	procData->data.resource_s.gpu_usage = procData->data.gpu_usage;
	procData->data.resource_s.memory_usage = procData->data.memory_usage;
	procData->data.resource_s.disk_read = procData->data.disk_read;
	procData->data.resource_s.disk_write = procData->data.disk_write;
	procData->data.resource_s.network_upload = procData->data.network_upload;
	procData->data.resource_s.network_download = procData->data.network_download;
}


int tpProcessInfo::readBasicInfo()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return read_process_info(procData->data.pid,procData->data);
}

int tpProcessInfo::readMemoryInfo()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return read_memory_info(procData->data.pid,procData->data);
}

void tpProcessInfo::addChild(tpProcessInfo *child)
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	procData->children.push_back(child);
	child->setParent(this);// 设置父进程
}

void tpProcessInfo::removeChild(tpProcessInfo *child)
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	procData->children.erase(std::remove(procData->children.begin(), procData->children.end(), child), procData->children.end());
	child->setParent(nullptr);// 删除父进程的引用
}
void tpProcessInfo::setParent(tpProcessInfo *parent)
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	procData->parent = parent;
}

void tpProcessInfo::updateBasicInfo(int new_ppid, int new_pid, const std::string &new_name, double cpu_time, double new_memory,double cpu)
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	procData->data.updateInfo(new_ppid,new_pid,new_name, cpu_time, new_memory);
}

//cputime:cpu总时间的变化量
void tpProcessInfo::updateBasicInfo(tpProcessInfo *info,double cputime)
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	tpProcessInfoData* otherData = static_cast<tpProcessInfoData *>(info->data_);
	procData->data.ppid=otherData->data.ppid;
	double cpu_usage = ((double)(otherData->data.cpu_handle.cpu_time - procData->data.cpu_handle.cpu_time_last)) / cputime * 100;
	if(procData->data.cpu_handle.cpu_time_last!=0)
		procData->data.cpu_usage = cpu_usage;
	procData->data.cpu_handle.cpu_time_last = otherData->data.cpu_handle.cpu_time;
	procData->data.memory_usage = otherData->data.memory_usage;
	read_memory_info(procData->data.pid,procData->data);
}

void tpProcessInfo::updateOtherInfo()
{
//	initNetworkSpeed();		//初始化网络交互数据获取，会获取本进程所有连接
//	readNetworkSpeed();
//	readMemoryInfo(data->pid,*data);
	readDiskInfo();
}
void tpProcessInfo::updateOtherInfo(double samp)
{
	initNetworkSpeed();		//初始化网络交互数据获取，会获取本进程所有连接
	readNetworkSpeed(samp);
//	readMemoryInfo(data->pid,*data);
	readDiskInfo();
}

int tpProcessInfo::getPid()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.pid;
}
int tpProcessInfo::getPpid()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.ppid;
}

tpString tpProcessInfo::getName()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.name;
}

double tpProcessInfo::getCpuUsage()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.cpu_usage;
}
double tpProcessInfo::getGpuUsage()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.gpu_usage;
}
double tpProcessInfo::getMemoryUsage()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.memory_usage;
}
double tpProcessInfo::getDiskReadSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.disk_read;
}
double tpProcessInfo::getDiskWriteSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.disk_write;
}
double tpProcessInfo::getNetUpSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.network_upload_speed;
}
double tpProcessInfo::getNetDownSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.network_download_speed;
}
tpList<tpProcessInfo *> tpProcessInfo::getChildren()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->children;
}
tpProcessInfo *tpProcessInfo::getParent()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->parent;
}

int tpProcessInfo::getDiskReadByte()
{
	return 0;
}
int tpProcessInfo::getDiskWriteByte()
{
	return 0;
}

int tpProcessInfo::getNetUpByte()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.network_upload;
}
int tpProcessInfo::getNetDownByte()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.network_download;
}

//只允许manage调用（获取进程以及子进程总信息，由manage生成，manage调用）
double tpProcessInfo::getAppCpuUsage()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.resource_s.cpu_usage;
}
double tpProcessInfo::getAppGpuUsage()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.resource_s.gpu_usage;
}
double tpProcessInfo::getAppMemoryUsage()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.resource_s.memory_usage;
}
double tpProcessInfo::getAppDiskReadSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.resource_s.disk_read;
}
double tpProcessInfo::getAppDiskWriteSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.resource_s.disk_write;
}
double tpProcessInfo::getAppNetUpSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.resource_s.network_upload;
}
double tpProcessInfo::getAppNetDownSpeed()
{
	tpProcessInfoData* procData = static_cast<tpProcessInfoData *>(data_);
	return procData->data.resource_s.network_download;
}
