/*///------------------------------------------------------------------------------------------------------------------------//
		应用管理的c++接口
说 明 : 
日 期 : 2024.9.7

/*///------------------------------------------------------------------------------------------------------------------------//


#include <iostream>
#include <atomic>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <thread>
#include "install.h"
#include "unload.h"
#include "unpack.h"
#include "appmanage_conf.h"
#include "install_check.h"
#include "appinfo.h"
#include "TpAppInstall.h"


struct TpAppInstallData{
	char *pack_path_c;				//安装包路径
	struct PackageConfigInfo conf;	//安装包中的信息

	struct PackageUserParam user;	//用户参数

	std::thread thread_t;
	std::atomic<bool> is_runing;
	TpAppInstallData()
	{
		is_runing=false;
		pack_path_c=NULL;
		memset(&conf,0,sizeof(struct PackageConfigInfo));
		memset(&user,0,sizeof(struct PackageUserParam));
	}
};

static bool package_is_app(struct PackageConfigInfo *conf)
{
	if(conf->type==TYPE_PACKAGE_APP || conf->type==TYPE_PACKAGE_SAPP)
		return true;
	return false;
}

TpAppInstall::TpAppInstall ()
{
	data_ = new TpAppInstallData();
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	appData->conf.type=TYPE_PACKAGE_DEFAULT;
	appData->user.schedule = creat_install_schedule();
	if (appData->user.schedule == NULL)
	{
		fprintf(stderr,"Error:creat_install_schedule\n");
		return ;
	}
}

TpAppInstall::TpAppInstall (const TpString &path)
{
	data_ = new TpAppInstallData();
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	appData->conf.type=TYPE_PACKAGE_DEFAULT;
	setPath(path);
	appData->user.schedule = creat_install_schedule();
	if (appData->user.schedule == NULL)
	{
		fprintf(stderr,"Error:creat_install_schedule\n");
		return ;
	}
		
}

TpAppInstall::~TpAppInstall ()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData)
		return ;
	if(appData->is_runing==true && appData->thread_t.joinable())
	{
		appData->thread_t.join();
	}
	appData->is_runing=false;
	appm_free_package_info(&appData->conf);
	if(appData->pack_path_c)
		free(appData->pack_path_c);
	delete_install_schedule(appData->user.schedule);
	delete(appData);
}

int TpAppInstall::setPath(const TpString &path)
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData)
		return -1;
	appData->pack_path_c=(char*)malloc(path.size() +1);
	std::strcpy(appData->pack_path_c, path.c_str());
	return appm_get_package_info(appData->pack_path_c,&appData->conf);		//获取安装包信息
}


tpBool TpAppInstall::isInstall()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData)
		return TP_FALSE;
	return (appm_get_app_is_install(&appData->conf)==0 ? TP_FALSE:TP_TRUE);
}

TpString TpAppInstall::getIcon()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData)
		return nullptr;
	TpString icon(appm_install_get_icon(&appData->conf));
	return icon;
}

TpAppInstall::TpAppPackageType TpAppInstall::getPackageType()	//获取安装包类型(system_app,user_app,lib,none)
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return TP_PACKAGE_TYPE_NONE;
	TpAppPackageType type;
	switch(appData->conf.type)
	{
		case TYPE_PACKAGE_LIB:	//系统库
			type=TP_PACKAGE_TYPE_LIB;
			break;
		case TYPE_PACKAGE_SAPP:	//系统应用
			type=TP_PACKAGE_TYPE_SYSAPP;
			break;
		case TYPE_PACKAGE_APP:	//普通应用
			type=TP_PACKAGE_TYPE_USRAPP;
			break;
		case TYPE_PACKAGE_DEFAULT:    //默认
		case TYPE_PACKAGE_NONE:	//未知类型	
		default:
			type=TP_PACKAGE_TYPE_NONE;
			break;
	}
	return type;
}

//环境检查返回1，支持或可以安装，0，未知，-1不支持或不能安装
int TpAppInstall::archCheck()			//硬件架构检查
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return -1;
	return appm_check_arch(&appData->conf);
}

int TpAppInstall::spaceCheck()		//空间检查
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return -1;
	return appm_check_space(&appData->conf);
}

int TpAppInstall::versionCheck()		//版本检查
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return -1;
	if(!isInstall())
		return 1;
	return appm_check_version(&appData->conf);
}

int TpAppInstall::completeCheck()		//安装包完整性检查
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return -1;
	return appData->conf.md5_flag;
}

TpString TpAppInstall::getPackVersion()	//获取安装包版本
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return -1;
	TpString version("0.0.0");
	char c_version[16];
	if(!package_is_app(&appData->conf))
		return version;
	snprintf(c_version,sizeof(c_version),"%d.%d.%d",appData->conf.app_conf.version.x,appData->conf.app_conf.version.y,appData->conf.app_conf.version.z);
	version=TpString(c_version);
	return version;
}

TpString TpAppInstall::getPackArch()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return "none";
	TpString arch("none");
	if(!package_is_app(&appData->conf))
		return arch;
	arch = TpString(appData->conf.app_conf.architecture);
	return arch;
}
int TpAppInstall::getPackSpace()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return -1;
	if(!package_is_app(&appData->conf))
		return 0;
	return appData->conf.app_conf.diskspace;
}

TpString TpAppInstall::getAppName()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return nullptr;
	TpString appName;
	if(!package_is_app(&appData->conf))
		return nullptr;

	appName = TpString(appData->conf.app_conf.app_name);
	return appName;
}

TpString TpAppInstall::getAppUUID()    //获取应用UUID
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return nullptr;
	TpString uuid;
	if(!package_is_app(&appData->conf))
		return uuid;

	uuid = TpString(appData->conf.app_conf.app_id);
	return uuid;
}

TpString TpAppInstall::getNowVersion()		//获取已安装的版本,根据uuid获取，暂时不支持获取系统库版本
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return "0.0.0";
	TpString version="0.0.0";
	struct TpVersion s_ver;
	appm_get_app_version(appData->conf.app_conf.app_id,&s_ver);
	char c_version[16];
	snprintf(c_version,sizeof(c_version),"%d.%d.%d",s_ver.x,s_ver.y,s_ver.z);
	version=TpString(c_version);
	return version;
}

//安装检查(全部检查)
TpAppInstall::TpInstallErrorCode TpAppInstall::checkAll()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return TpAppInstall::TP_INSTALL_ERROR_OTHER;
	FILE* fp = fopen(appData->pack_path_c, "r");
    if (fp == NULL) {
		std::cerr << "安装包不存在\n";
        return TpAppInstall::TP_INSTALL_ERROR_OTHER;
    }
	fclose(fp);          // 关闭文件流
	if(archCheck()<=0){
			std::cerr << "Arch check error\n";
			return TpAppInstall::TP_INSTALL_ERROR_ARCK;
	}

	if(spaceCheck()<0){	//==0未知
		std::cerr << "Space check error\n";
		return TpAppInstall::TP_INSTALL_ERROR_SPACE;
	}

    if(versionCheck()<=0){
		std::cerr << "Version check error\n";
		return TpAppInstall::TP_INSTALL_ERROR_VERSION;
	}

    if(completeCheck()<=0){
		std::cerr << "Complete check error\n";
		return TpAppInstall::TP_INSTALL_ERROR_MD5;
	}
		
	return TpAppInstall::TP_INSTALL_SUCCESS;
}


int TpAppInstall::threadInstall()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(appm_install_package(appData->pack_path_c,&appData->conf,&appData->user)<0)
		return -1;
	return 0;
}


TpAppInstall::TpInstallErrorCode TpAppInstall::install()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return TpAppInstall::TP_INSTALL_ERROR_OTHER;
	
	if(appData->is_runing )
		return TpAppInstall::TP_INSTALL_ERROR_OTHER;

	TpAppInstall::TpInstallErrorCode err=checkAll();
	if(err!=TP_INSTALL_SUCCESS)
		return err ;
	//创建线程
	appData->is_runing = true;
	appData->thread_t = std::thread(&TpAppInstall::threadInstall, this);

//	TpAppManage manage(appData->conf.app_conf.app_id);
//	manage.addPermission(TpAppManage::TP_APP_FILE_DATA);
	return TP_INSTALL_SUCCESS;
}

int TpAppInstall::threadInstallTest()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	sleep(5);
	if(appm_install_package(appData->pack_path_c,&appData->conf,&appData->user)<0)
		return -1;
	return 0;
}


TpAppInstall::TpInstallErrorCode TpAppInstall::installTest()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return TpAppInstall::TP_INSTALL_ERROR_OTHER;
	
	if(appData->is_runing )
		return TpAppInstall::TP_INSTALL_ERROR_OTHER;

	TpAppInstall::TpInstallErrorCode err=checkAll();
	if(err!=TP_INSTALL_SUCCESS)
		return err ;
	//创建线程
	appData->is_runing = true;
	appData->thread_t = std::thread(&TpAppInstall::threadInstall, this);

	return TP_INSTALL_SUCCESS;
}


//安装进度
int TpAppInstall::getInstallSchedule()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData)
		return -1;
	int s= appm_install_get_schedule(&appData->user);
	return s;
}

int TpAppInstall::update()
{
	TpAppInstallData *appData = static_cast<TpAppInstallData *>(data_);
	if(!appData||!appData->pack_path_c)
		return -1;
	return appm_install_package(appData->pack_path_c,&appData->conf,&appData->user);
}

int TpAppInstall::remove(TpString& uuid)
{
	TpAppID appid;
	if(appm_info_get_appid(uuid.c_str(),&appid)<0)
	{
		fprintf(stderr,"appid is error\n");
		return -1;
	}
	
	if(appm_info_is_install(&appid)!=true)
	{
		fprintf(stderr,"app not installed\n");
		return -1;
	}
	return appm_app_unload(appid);
}
