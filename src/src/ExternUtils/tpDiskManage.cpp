/*///------------------------------------------------------------------------------------------------------------------------//
		系统磁盘管理
说 明 :
日 期 : 2024.11.05

/*/
//------------------------------------------------------------------------------------------------------------------------//

// #include <sys/mount.h>
#include <iostream>
#include <pthread.h>
#include <atomic>
#include <libgen.h>    // basename
#include <set>
#include <mntent.h>
#include <thread>
#include <dirent.h>
#include "tpDiskManage.h"
#include "tpSystemInfoDef.h"

struct tpDiskManageData
{
	tpList<tpDisk *> list;
	std::thread thread_t;
	std::atomic<bool> is_runing;
	std::atomic<bool> is_geting;
	tpString root_devname;
	tpDiskManageData() {
		is_runing=false;
		is_geting=false;
	}
};


// 读取挂载在 / 的块设备 basename
static std::string get_root_partition() {
    FILE *fp = setmntent("/proc/mounts", "r");
    if (!fp) return "";

    struct mntent *ent;
    std::string root_part;
    while ((ent = getmntent(fp))) {
        if (ent->mnt_dir && std::string(ent->mnt_dir) == "/") {
            // basename 会修改输入，所以先复制到缓冲
            char buf[256];
            strncpy(buf, ent->mnt_fsname, sizeof(buf)-1);
            buf[sizeof(buf)-1] = '\0';
            root_part = basename(buf);
            break;
        }
    }
    endmntent(fp);
    return root_part;
}

tpDiskManage::tpDiskManage(tpBool enabled, tpUInt16 samp )
{
	data_ = new tpDiskManageData;
	tpDiskManageData *dmData = static_cast<tpDiskManageData *>(data_);
	if(!dmData)
		return ;
	tpString diskname= get_root_partition();
	tpDisk disk_root = tpDisk(diskname, TP_FALSE);
	dmData->root_devname=disk_root.getDevice();
	if(enabled)
	{
		dmData->is_runing=true;
		dmData->thread_t = std::thread(&tpDiskManage::monitorList, this, TP_FALSE,samp);
	}

	else
	{
		monitorList(TP_TRUE,samp);
		if(!dmData->is_geting)
			usleep(1000);
	}
}

tpDiskManage::~tpDiskManage()
{
	tpDiskManageData *dmData = static_cast<tpDiskManageData *>(data_);
	if (dmData == nullptr)
		return;
	dmData->is_runing=false;
	for (auto &it : dmData->list)
	{
		if (it)
		{
			delete it;
			it = nullptr;
		}
	}
	if (dmData->thread_t.joinable())
		dmData->thread_t.join(); // 等待线程完成
}

static tpList<tpString> readListString()
{
	tpList<tpString> info_list;
	// 读所有磁盘设备
	const char *path = "/sys/class/block";

	DIR *dir = opendir(path); // C++11不支持打开目录
	if (dir == nullptr)
	{
		fprintf(stderr,"opendir error:%s\n",path);
		return info_list;
	}
	struct dirent *entry;
	int count = 0;
	while ((entry = readdir(dir)) != nullptr)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue;
		}
		if (strncmp(entry->d_name, "loop", 4) == 0)
			continue;
		tpString diskName(entry->d_name);
		
		//printf("push %s\n", diskName.c_str());
		info_list.emplace_back(diskName); //

		count++;
	}
	closedir(dir);
	//printf("return \n");
	return std::move(info_list);
}


tpList<tpDisk *> tpDiskManage::getList()
{
	tpDiskManageData *dmData = static_cast<tpDiskManageData *>(data_);
	monitorList(TP_TRUE,0);
	return dmData->list;
}

tpDisk *tpDiskManage::getDisk(const tpString &name)
{
	tpDiskManageData *dmData = static_cast<tpDiskManageData *>(data_);
	for(auto it :dmData->list)
	{
		if(it->getName()==name)
			return it;
	}
	return nullptr;
}


//更新列表以发出弹出磁盘提醒信号
void tpDiskManage::monitorList(tpBool once,uint16_t samp)
{
	tpDiskManageData *dmData = static_cast<tpDiskManageData *>(data_);
	tpList<tpString > list_l;
	tpString root_devname = dmData->root_devname;
	tpString None="none";
	while(dmData->is_runing || once==TP_TRUE)
	{
		tpList<tpString > list_n=readListString();
		for(const tpString &disk_l:list_l)
		{
			auto it=find_if(list_n.begin(),list_n.end(),[disk_l](const tpString &disk_n){return (disk_n==disk_l);});
			if(it==list_n.end())		//已经弹出
			{
				printf("%s已弹出\n",disk_l.c_str());
				diskRemove.emit(disk_l);
				printf("卸载%s\n",disk_l.c_str());
				tpDisk *disk_obj=getDisk(disk_l);
				if(!disk_obj)
				{
					printf("get %s error\n",disk_l.c_str());
					continue;
				}
					
				disk_obj->umountRabDisk();
				printf("remove\n");
				dmData->list.remove(disk_obj);
				printf("delete\n");
				if (disk_obj != nullptr) delete(disk_obj);
			}
		}

		for(const tpString &disk_n:list_n)
		{
			auto it=find_if(list_l.begin(),list_l.end(),[disk_n](const tpString &disk_l){return (disk_l==disk_n);});
			if(it==list_l.end())		//新设备
			{
				tpString name=disk_n;
				tpDisk *disk_obj=new tpDisk(name,TP_FALSE);
				tpString disk_devname=disk_obj->getDevice();
				if(root_devname==disk_devname)
				{
					printf("root:%s,disk:%s\n",root_devname.c_str(),disk_devname.c_str());
					delete(disk_obj);
					continue;
				}
				if(disk_obj->getRemovable()!=TP_TRUE || disk_obj->getDevice()==None || !disk_obj->isPartition())
					continue;
				printf("监测到新设备%s\n",name.c_str());
				diskAdd.emit(disk_obj);
				dmData->list.emplace_back(disk_obj);
				tpString path_mount="/System/mnt/"+name;
				
				printf("DEBUG before mount: disk_n=%p, getName()='%s'\n",
					(void*)disk_obj, disk_obj->getName().c_str());
				disk_obj->mountRabDisk(path_mount);
				printf("DEBUG after  mount: disk_n=%p, getName()='%s'\n",
					(void*)disk_obj, disk_obj->getName().c_str());

				//disk_n->autoMountRabDisk(path_mount,5000);
			}
		}

		list_l=std::move(list_n);

		if(once==TP_TRUE)
		{
			dmData->is_geting=true;
			break;
		}
			
		std::this_thread::sleep_for(std::chrono::milliseconds(samp));
	}
}


// 磁盘列表以及每个磁盘的参数(获取详细使用情况使用DiskInfo接口)
/*tpList<tpDisk*> tpDiskManage::getDeviceListInfo()
{
	tpList<tpDisk*> info_list = getDeviceList();
	std::ifstream fd("/proc/mounts");
	if (!fd)
		return info_list;
	tpString line;
	while (std::getline(fd, line))
	{ // 逐行读取文件
		if (strncmp(line.c_str(), "/dev/", 5) != 0)
			continue;
		if (strncmp(line.c_str(), "/dev/loop", 9) == 0)
			continue;
		tpString dev_path, dev_mount, system;
		std::stringstream ss(line);
		ss >> dev_path >> dev_mount >> system;

		size_t pos = dev_path.rfind('/');
		if (pos == std::string::npos)
			continue;
		tpString dev = dev_path.substr(pos + 1);

		for (int i = 0; i < info_list.size(); ++i)
		{
			if (info_list[i]->getName() == dev)
			{
				//printf("debug:%s,%s\n",dev_mount.c_str(), system.c_str());
				//info_list[i]->setMount(dev_mount);
				//info_list[i]->setFsType(system);
				info_list[i]->getDiskInfo();
				break;
			}
		}
	}
	fd.close();

	return info_list;
}*/
