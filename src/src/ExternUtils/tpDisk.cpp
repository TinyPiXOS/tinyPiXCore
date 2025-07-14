/*///------------------------------------------------------------------------------------------------------------------------//
		系统磁盘信息
说 明 :
日 期 : 2024.11.05

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <sys/mount.h> //需要在<linux/fs.h>之前，否则会冲突
#include <sys/statvfs.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <unistd.h>
#include <dirent.h>
#include "tpDisk.h"
#include "tpFileInfoWR.h"
#include "tpDiskManage.h"
#include "tpSystemInfoDef.h"
#include "tpSystemDataManage.h"

#define PATH_BLOCK_PATH "/sys/class/block/" //"/sys/class/block/"和"/sys/block/"都可以获取，后者是按照整个设备，前者按照块(分区)

struct tpDiskInfoParam
{
	tpString name;
	tpString device;	  /// 设备的名字，例如name=“sda1”的时候，device=“sda”
	tpString mount;		  // 磁盘挂载点，可用于获取磁盘使用情况
	tpString file_system; // 文件系统类型
	int16_t partition;	  //-1获取不到，0是设备，大于0是正常分区。
	uint64_t space;		  // 磁盘空间
	uint64_t sector_size; // 扇区大小
	tpString type;		  // SSD,HDD
	int8_t readonly;	  // 是否只读
	int8_t removable;	  // 是否可移动
	uint64_t space_used;  // 已用的空间
	tpString vendor;	  // 厂商
	tpString model;		  // 型号
	tpString serial;	  // 序列号
	tpDiskInfoParam(tpString &name) : name(name), space(0), type("None"), readonly(-1), removable(-1), partition(-1) {}
	tpDiskInfoParam() {
		mount="";
	};
};

struct tpDiskInfoData
{
	tpFileInfoWR fileInfoWR;
	tpSystemDataManage data;

	DiskstatsData disk_stat;
	tpDiskInfoParam param;
	//	std::atomic<bool> running;
	//	std::thread thread_disk;

	tpDiskInfoData() {}

};

void del_num_from_end(std::string &str)
{
	auto it = str.end();
	// 移动迭代器直到遇到第一个非数字字符
	while (it != str.begin() && std::isdigit(*(it - 1)))
	{
		--it;
	}
	// 截断字符串
	str.erase(it, str.end());
}

//逐级创建目录
static int mkdir_p(const char *path, mode_t mode) {
    char tmp[PATH_MAX];
    char *p = NULL;
    strncpy(tmp, path, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';

    // 逐级创建目录
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) != 0 && errno != EEXIST) 
                return -1;
            *p = '/';
        }
    }
    // 创建最后一级
    return (mkdir(tmp, mode) != 0 && errno != EEXIST) ? -1 : 0;
}


//使用分区名或者设备名得到设备名
//例如sda1返回sda，sda返回sda
tpString get_device_name(const tpString& device)
{
	if (device.find("sd") == 0 || device.find("sr")==0)
	{
		size_t pos = 0;
		// 遍历直到找到第一个数字（分区号起始位置）
		while (pos < device.size() && !std::isdigit(device[pos])) {
			++pos;
		}
		return device.substr(0, pos); // 截取数字前的部分
	} 
	else if(device.find("nvme") == 0 || device.find("mmcblk") == 0 )
	{
		size_t pos = device.find_last_of('p'); // 定位最后一个 'p'
		if (pos != tpString::npos) {
			return device.substr(0, pos); // 截取 'p' 之前的部分
		}
		return device;
	}
	else {
		fprintf(stderr,"未识别的设备类型\n");
		return "none";
	}
}



// 可以从PATH_BLOCK_PATH+disk/stat直接读取这个磁盘,也可以从/proc/diskstats读再查找
int get_disk_stat(tpString dev, struct DiskstatsData *disk)
{
	// tpString path("/proc/diskstats");
	tpString path("/sys/class/block/" + dev + "/stat");
	//printf("get_disk_stat %s\n", path.c_str());
	std::ifstream diskstats_file(path);
	if (!diskstats_file.is_open())
	{
		std::cerr << "Error get disk stat: " << path << std::endl;
		return -1;
	}
	tpString line;
	/*while (std::getline(diskstats_file, line)) {
		if (line.find(dev) !=std::string::npos) {
			sscanf(line.c_str(), "%*d %*d %*s %lu %*u %lu %lu %lu %*u %lu %lu %*u %*u %lu",
				   &(disk->reads), &(disk->read_time), &(disk->read_sector),
				   &(disk->writes), &(disk->write_time),&(disk->write_sector),
				   &(disk->weight_time));
			printf("reads:%ld,read_sector:%ld,writes:%ld,write_sector:%ld\n",disk->reads,disk->read_sector,disk->writes,disk->write_sector);
			break;
		}
	}*/
	std::getline(diskstats_file, line);
	sscanf(line.c_str(), "%lu %*u %lu %lu %lu %*u %lu %lu %*u %*u %lu",
		   &(disk->reads), &(disk->read_time), &(disk->read_sector),
		   &(disk->writes), &(disk->write_time), &(disk->write_sector),
		   &(disk->weight_time));
	//printf("reads:%ld,read_sector:%ld,writes:%ld,write_sector:%ld\n",disk->reads,disk->read_sector,disk->writes,disk->write_sector);
	diskstats_file.close();
	return 0;
}

// 获取磁盘已经使用的空间
uint64_t get_disk_usage(const char *path)
{
	struct statvfs stat;
	if(!path)
		return 0;
	// 获取指定路径的文件系统信息
	if (statvfs(path, &stat) != 0)
	{
		//std::cerr << "statvfs" << path << " error: " << strerror(errno) << std::endl;
		return 0;
	}

	// 计算总空间、已用空间和剩余空间
	unsigned long total_space = stat.f_blocks * stat.f_frsize;
	unsigned long free_space = stat.f_bfree * stat.f_frsize;
	unsigned long available_space = stat.f_bavail * stat.f_frsize;
	unsigned long used_space = total_space - free_space;
	//	printf("Disk usage for %s:\n", path);
	//    printf("  Total space: %.2f MB\n", total_space / (1024.0 * 1024.0));
	//    printf("  Used space: %.2f MB\n", used_space / (1024.0 * 1024.0));
	//    printf("  Available space (non-root): %.2f MB\n", available_space / (1024.0 * 1024.0));

	return used_space;
}

// 如果是分区直接卸载，如果是磁盘依次卸载上面所有设备
int umount_dev(const char *dev)
{
	char commond[512];
	sprintf(commond, "umount %s", dev);
	if (system(commond) < 0)
	{
		std::cerr << "Umount " << dev << " error: " << strerror(errno) << std::endl;
		return -1;
	}
	/*if (umount(dev) == -1) {
		std::cerr << "Umount " << dev << " error: " << strerror(errno) << std::endl;
		return -1;
	}*/
	return 0;
}

static int umount_disk_dev(const char *dev)
{
	tpFileInfoWR info;
	uint64_t value;
	char dev_path[266];

	int err = info.getUintFromFile(PATH_BLOCK_PATH + tpString(dev) + "/partition", &value);
	if (err < 0)
		value = 0;
	if (value != 0) // 直接卸载
	{
		memset(dev_path, 0, sizeof(dev_path));
		sprintf(dev_path, "/dev/%s", dev);
		umount_dev(dev_path);
	}
	else
	{
		const char *path = "/sys/class/block";
		DIR *dir = opendir(path);
		if (dir == nullptr)
		{
			return -1;
		}
		struct dirent *entry;
		while ((entry = readdir(dir)) != nullptr)
		{
			// shanchu shuzi
			tpString str = tpString(entry->d_name);
			del_num_from_end(str);
			tpString device = get_device_name( str);
			if (strcmp(device.c_str(), dev) != 0)
				continue;
			// 卸载
			memset(dev_path, 0, sizeof(dev_path));
			sprintf(dev_path, "dev/%s", entry->d_name);
			umount_dev(dev_path);
		}
		closedir(dir);
	}
	return 0;
}


tpDisk::tpDisk()
{
	data_ = new tpDiskInfoData();
}
tpDisk::tpDisk(const tpString &name, tpBool enabled, uint16_t time_samp)
{
	data_ = new tpDiskInfoData();
	tpDiskInit(name,enabled,time_samp);
}

tpDisk::tpDisk(const tpDisk &other)
{
	// tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	// if (this != &other)
	// {
	// 	printf("debug noexcept1\n");
	// 	data_ = other.data_; // 转移指针
	// 	printf("debug noexcept2\n");
	// 	other.data_ = nullptr; // 防止析构时释放资源
	// }

	// printf("debug noexcept3\n");
}

tpDisk::~tpDisk()
{
	updateThreadStop();
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	if (diskData != nullptr)
	{
		delete diskData;
		diskData =nullptr;
	}
}

int tpDisk::tpDiskInit(const tpString &name, bool enabled, uint16_t samp)
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	if (access(("/dev/" + name).c_str(), F_OK) == 0)
	{
		diskData->param.name = name;
		updateDeviceName();		//获取分区对应的设备名
		getDiskInfo();
		if (enabled)
		{
			diskData->data.running = true;
			diskData->data.thread_t = std::thread(&tpDisk::threadUpdateStat, this, samp, diskData->param.sector_size);
		}
	}
	else
		std::cerr << "Error Creat Class: The device " << name << "does not exist " << std::endl;
	return 0;
}
int tpDisk::setName(tpString &name)
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	diskData->param.name = name;
	return 0;
}


void tpDisk::updateThreadStop()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	if (diskData->data.running == false)
		return;

	diskData->data.running = false;
	if (diskData->data.thread_t.joinable())
		diskData->data.thread_t.join(); // 等待线程完成
}

void tpDisk::threadUpdateStat(uint16_t time_samp, uint64_t sector_size)
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	DiskstatsData stat_n, stat_l;
	get_disk_stat(diskData->param.name, &stat_l);
	while (diskData->data.running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time_samp));
		get_disk_stat(diskData->param.name, &stat_n);
		// 计算
		DiskstatsData stat;
		stat.read_sector = stat_n.read_sector - stat_l.read_sector;
		stat.write_sector = stat_n.write_sector - stat_l.write_sector;
		// stat.reads=stat_n.reads-stat_l.reads;
		// stat.writes=stat_n.writes-stat_l.writes;
		stat.read_speed = (double)(stat.read_sector * sector_size) * 1000.0 / (double)time_samp;
		stat.write_speed = (double)(stat.write_sector * sector_size) * 1000.0 / (double)time_samp;
		// printf("stat.reads=%ld,stat.writes=%ld\n",stat.reads,stat.writes);

		updateInfo(0, (void *)(&stat));
		stat_l = stat_n;
	}
}

int tpDisk::setMount(tpString &path)
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	diskData->param.mount = path;
	return 0;
}
int tpDisk::setFsType(tpString &type)
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	diskData->param.file_system = type;
	return 0;
}

tpString tpDisk::getDevice()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	return diskData->param.device;
}
tpString tpDisk::getName()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	return diskData->param.name;
}
// 获取该磁盘或分区对应的设备的名字，例如sda1对应sda
tpString tpDisk::updateDeviceName()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	diskData->param.device = get_device_name( diskData->param.name);
	return diskData->param.device;
}

// 扇区大小
uint64_t tpDisk::getSectorSize()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	uint64_t value=0;
	int err = diskData->fileInfoWR.getUintFromFile(PATH_BLOCK_PATH + diskData->param.device + "/queue/hw_sector_size", &value);
	if (err < 0)
		return 0;
	return value;
}

// 扇区数量
uint64_t tpDisk::getSectorNum()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	uint64_t value=0;
	int err = diskData->fileInfoWR.getUintFromFile(PATH_BLOCK_PATH + diskData->param.name + "/size", &value);
	if (err < 0)
		return 0;
	return value;
}

//获取设备总字节数
static long long disk_get_block_size(const tpString& name)
{
	tpString dev=tpString("/dev/")+name;
	int fd = open(dev.c_str(), O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror("open");
		return 0;
	}

	unsigned long long bytes;
	// 直接用 BLKGETSIZE64 拿设备总字节数（Linux 2.6.36+）
	if (ioctl(fd, BLKGETSIZE64, &bytes) < 0) {
		perror("ioctl(BLKGETSIZE64)");
		close(fd);
		return 0;
	}
	return bytes;
}

// 计算容量
uint64_t tpDisk::getSpace()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	uint64_t size=getSectorSize();
	uint64_t num=getSectorNum();
	if(size==0 || num==0)
		diskData->param.space = disk_get_block_size(diskData->param.name);
	else
		diskData->param.space = num * size;
	return diskData->param.space;
}

// 分区号
int16_t tpDisk::getPartition()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	uint64_t value;
	int err = diskData->fileInfoWR.getUintFromFile(PATH_BLOCK_PATH + diskData->param.name + "/partition", &value);
	if (err < 0)
		diskData->param.partition = 0;
	else
		diskData->param.partition = (int16_t)value;
	return diskData->param.partition;
}
//是否是分区，返回TRUE表示是分区，返回FALSE表示是设备
tpBool tpDisk::isPartition()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	return (diskData->param.partition>0 ? TP_TRUE: TP_FALSE);
}

// 类型（SSD/HDD）
tpString tpDisk::getType()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	if (diskData->param.partition != 0)
		return "";

	tpString type = diskData->fileInfoWR.getValueFromFile(PATH_BLOCK_PATH + diskData->param.name + "/queue/rotational");
	if (type == tpString("0"))
		diskData->param.type = "SSD";
	else if (type == tpString("1"))
		diskData->param.type = "HDD";
	return diskData->param.type;
}

// 是否可移动
tpBool tpDisk::getRemovable()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	if (diskData->param.partition < 0)
		return TP_FALSE;
	uint64_t value;
	int err = diskData->fileInfoWR.getUintFromFile(PATH_BLOCK_PATH + diskData->param.device + "/removable", &value);
	if (err < 0)
		diskData->param.removable = -1;
	else
		diskData->param.removable = (int8_t)value;
	return (diskData->param.removable==1 ? TP_TRUE: TP_TRUE);
}

// 是否只读
tpBool tpDisk::getReadonly()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	uint64_t value;
	int err = diskData->fileInfoWR.getUintFromFile(PATH_BLOCK_PATH + diskData->param.name + "/ro", &value);
	if (err < 0)
		diskData->param.readonly = -1;
	else
		diskData->param.readonly = (int8_t)value;
	return (diskData->param.readonly==1 ? TP_TRUE: TP_TRUE);
}

// 厂商
tpString tpDisk::getVendor()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	if (diskData->param.partition != 0)
		return "";
	diskData->param.vendor = diskData->fileInfoWR.getValueFromFile(PATH_BLOCK_PATH + diskData->param.name + "/device/vendor");
	return diskData->param.vendor;
}
// 设备型号
tpString tpDisk::getModel()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	if (diskData->param.partition != 0)
		return "";
	diskData->param.model = diskData->fileInfoWR.getValueFromFile(PATH_BLOCK_PATH + diskData->param.name + "/device/model");
	return diskData->param.model;
}

// 序列号
tpString tpDisk::getSerial()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	if (diskData->param.partition != 0)
		return "";
	diskData->param.serial = diskData->fileInfoWR.getValueFromFile(PATH_BLOCK_PATH + diskData->param.name + "/device/serial");
	return diskData->param.serial;
}

int tpDisk::getMountFstype()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	std::ifstream fd("/proc/mounts");
	if (!fd)
	{
		std::cerr << "open /proc/mounts error: " << strerror(errno) << std::endl;
		return -1;
	}

	tpString line;
	tpString path("/dev/" + diskData->param.name);
	while (std::getline(fd, line))
	{ // 逐行读取文件
		if (strncmp(line.c_str(), path.c_str(), path.length()) != 0)
			continue;
		// printf("cmp,%s---%s---%d\n",line.c_str(), path.c_str(), path.length());
		// printf("cmpok\n");
		tpString dev_path, dev_mount, system;
		std::stringstream ss(line);
		ss >> dev_path >> dev_mount >> system;
		diskData->param.mount = dev_mount;
		diskData->param.file_system = system;
		//printf("mount ： %s-%s\n", diskData->param.name.c_str(),dev_mount.c_str());
		break;
	}

	fd.close();
	return 0;
}
// 文件系统类型
tpString tpDisk::getFstype()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	getMountFstype();
	return diskData->param.file_system;
}
// 挂载点
tpString tpDisk::getMount()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	getMountFstype();
	return diskData->param.mount;
}

tpBool tpDisk::isMount()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	if(!diskData)
	{
		return TP_FALSE;
	}
	return diskData->param.mount.empty()? TP_FALSE:TP_TRUE;
}

// 已使用空间
uint64_t tpDisk::getUsedSize()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	if(!diskData)
	{
		return 0;
	}
	if(diskData->param.mount.empty())
	{
		//fprintf(stderr,"Error:get disk used error:disk is not mount\n");
		return 0;
	}
	diskData->param.space_used = get_disk_usage(diskData->param.mount.c_str());
	return diskData->param.space_used;
}

// 剩余空间

// 获取信息
int tpDisk::getDiskInfo()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	//printf("get disk info %s\n", diskData->param.name.c_str());
	getPartition();
	getSpace();
	getType();
	getRemovable();
	getReadonly();
	getVendor();
	getModel();
	getSerial();
	getMountFstype();
	getUsedSize();
	return 0;
}

void tpDisk::update()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	if (diskData->data.running)
		return;
	// 需要添加不使用线程的处理
	getDiskInfo();
}

double tpDisk::getReadSpeed()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	double speed = 0.0;
	update();
	diskData->data.dataReadLock();
	speed = diskData->disk_stat.read_speed;
	diskData->data.dataUnlock();
	return speed;
}

double tpDisk::getWriteSpeed()
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	double speed = 0.0;
	update();
	diskData->data.dataReadLock();
	speed = diskData->disk_stat.write_speed;
	diskData->data.dataUnlock();
	return speed;
}

void tpDisk::updateInfo(uint16_t time_s, void *stat_p)
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	DiskstatsData *stat = static_cast<DiskstatsData *>(stat_p);

	diskData->data.dataWriteLock();
	//	printf("update %d,used=%ld\n", num,stat.time_used);
	diskData->disk_stat.read_speed = stat->read_speed;
	diskData->disk_stat.write_speed = stat->write_speed;
	diskData->data.dataUnlock();
}

tpString tpDisk::getLabel()
{
	tpString label;
	return label;
}

int tpDisk::setLabel(tpString &label)
{
	return 0;
}

//超时时间，ms
int tpDisk::autoMountRabDiskThread(tpString path,tpUInt16 timeout)
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	tpUInt16 time=0;
	while(1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 每次延时100ms
		if(mountRabDisk(path.c_str())==0)
			break;
		time+=100;
		if(time>=timeout)
			break;
	}
	return 0;
}

//自动挂载
int tpDisk::autoMountRabDisk(tpString path,tpUInt16 timeout)
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	std::thread a1(&tpDisk::autoMountRabDiskThread,this, path,timeout);
    a1.detach();
	return 0;
}

// 挂载
int tpDisk::mountRabDisk(tpString &mount_path)
{
	return mountRabDisk(mount_path.c_str());
}

int tpDisk::mountRabDisk(const char *mount_path)
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	
	struct stat info;
	if (diskData->param.partition == 0)
		return -1;
	if (stat(mount_path, &info) != 0)
	{
		std::cout << "Mount point does not exist. Creating it...\n";
		if (mkdir_p(mount_path, 0755) == -1)
		{
			std::cerr << "Error creating mount point: " << strerror(errno) << std::endl;
			return -1;
		}
	}
	else if (!(info.st_mode & S_IFDIR))
	{
		std::cerr << "The mount point is not a directory!" << std::endl;
		return -1;
	}

	tpString disk_path="/dev/"+diskData->param.name;
	//使用mount函数需要检测文件系统类型，需要引入​​lsblk库来实现
/*	if (mount(disk_path.c_str(), mount_path, diskData->param.file_system.c_str(), 0, NULL) == -1)
	{
		std::cerr << disk_path << "to" << mount_path << ",Mount error: " << strerror(errno) << std::endl;
		return -1;
	}*/
	tpString command="mount "+disk_path+" "+mount_path;
	if(system(command.c_str())<0)
	{
		std::cerr << disk_path << "to" << mount_path << ",Mount error: " << strerror(errno) << std::endl;
		
		tpString command="umount "+tpString(mount_path);
		if(system(command.c_str())<0)
		{
			std::cerr << "卸载失败" << std::endl;
			return -1;
		}

		remove(mount_path);
			return -1;
	}

	printf("挂载到%s成功\n",mount_path);
	return 0;
}

// 普通卸载
int tpDisk::umountRabDisk() //
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	return umountRabDisk(diskData->param.name.c_str()); //
}
// 磁盘卸载(一般用于卸载整个设备)
int tpDisk::umountRabDisk(const char *name) //
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);
	tpString path_mount="/System/mnt/"+diskData->param.name;

	if (access(("/dev/" + diskData->param.name).c_str(), F_OK) != 0)
	{
		std::cerr << "磁盘已经不在主机上" << std::endl;
		goto UMOUNT;
	}

	if (getReadSpeed() != 0 || getWriteSpeed() != 0)
	{
		std::cerr << "磁盘正在使用中，不允许卸载" << std::endl;
		return -1;
	}

UMOUNT:
	updateThreadStop();
	/*if (umount_disk_dev(name) < 0)
	{
		std::cerr << "卸载失败" << std::endl;
		return -1;
	}*/
	tpString command="umount /System/mnt/"+tpString(name);
	if(system(command.c_str())<0)
	{
		std::cerr << "卸载失败" << std::endl;
		return -1;
	}

	remove(path_mount.c_str());
	return 0;
}

int tpDisk::popupRabDisk() // 弹出磁盘
{
	tpDiskInfoData *diskData = static_cast<tpDiskInfoData *>(data_);

	tpString dev_path("/dev/" + diskData->param.device);
	const char *device = dev_path.c_str();

	if (diskData->param.removable != 1)
	{
		std::cerr << "当前磁盘非可移动磁盘，不允许弹出" << std::endl;
		return -1;
	}

	if (umountRabDisk(diskData->param.device.c_str()) < 0) // 先卸载
		return -1;

	int fd;
	if ((fd = open(device, O_RDONLY | O_NONBLOCK)) < 0)
	{
		printf("open device %s failed!\n", device);
		return -1;
	}
	int status, k;
	sg_io_hdr_t io_hdr;
	unsigned char allowRmBlk[6] = {ALLOW_MEDIUM_REMOVAL, 0, 0, 0, 0, 0};
	unsigned char startStop1Blk[6] = {START_STOP, 0, 0, 0, 1, 0};
	unsigned char startStop2Blk[6] = {START_STOP, 0, 0, 0, 2, 0};
	unsigned char inqBuff[2];
	unsigned char sense_buffer[32];
	if ((ioctl(fd, SG_GET_VERSION_NUM, &k) < 0) || (k < 30000))
	{
		printf("not an sg device, or old sg driver\n");
		goto out;
	}
	memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = 6;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.dxfer_direction = SG_DXFER_NONE;
	io_hdr.dxfer_len = 0;
	io_hdr.dxferp = inqBuff;
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = 10000;
	io_hdr.cmdp = allowRmBlk;
	status = ioctl(fd, SG_IO, (void *)&io_hdr);
	if (status < 0)
	{
		goto out;
	}
	io_hdr.cmdp = startStop1Blk;
	status = ioctl(fd, SG_IO, (void *)&io_hdr);
	if (status < 0)
	{
		goto out;
	}
	io_hdr.cmdp = startStop2Blk;
	status = ioctl(fd, SG_IO, (void *)&io_hdr);
	if (status < 0)
	{
		goto out;
	}
	/* force kernel to reread partition table when new disc inserted */
	status = ioctl(fd, BLKRRPART);
out:
	close(fd);
	return 0;
}
