/*///------------------------------------------------------------------------------------------------------------------------//
		软硬件环境检查
说 明 : 包含以下内容
		1.硬件平台和包支持平台是否一致
		2.系统版本是否满足
		3.系统空间是否满足
日 期 : 2024.9.20
1
/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include "install_check.h"
#include "install.h"
#include "unpack.h"
#include "conf_json.h"
#include "../inc/appmanage_conf.h"
#include "utilslib.h"



struct ArchMapping arch_maps[] = {
	{"x86_64", "amd64"},
	{"x64", "amd64"},
	{"amd64", "amd64"},

	{"amd32", "i386"},
	{"x86_32", "i386"},
	{"i386", "i386"},
	
	{"aarch64", "arm64"},
	{"arm64,", "arm64"},

	{"aarch32", "arm32"},
	{"arm32,", "arm32"},
	
	{"risc_v", "risc_v"}
};


int hardware_get(tpEnumArchType arch)
{
	struct sysinfo info;
    if (sysinfo(&info) == 0) {
        printf("=== Hardware Information ===\n");
        printf("Total RAM: %lu MB\n", info.totalram / (1024 * 1024));
        printf("Free RAM: %lu MB\n", info.freeram / (1024 * 1024));
        printf("Total Swap: %lu MB\n", info.totalswap / (1024 * 1024));
        printf("Free Swap: %lu MB\n", info.freeswap / (1024 * 1024));
        printf("Uptime: %ld seconds\n", info.uptime);
        printf("=============================\n");
    } else {
        perror("sysinfo");
    }

	struct utsname uname_data;
    if (uname(&uname_data) == 0) {
        printf("=== Software Information ===\n");
        printf("System Name: %s\n", uname_data.sysname);
        printf("Node Name: %s\n", uname_data.nodename);
        printf("Release: %s\n", uname_data.release);
        printf("Version: %s\n", uname_data.version);
        printf("Machine: %s\n", uname_data.machine);
        printf("=============================\n");
    } else {
        perror("uname");
    }
	return 0;
}

//使用uuid获取已安装的应用的版本号(从安装目录配置文件中读取)
//返回-1，错误
//返回0，成功
//返回1，
int get_app_version_config(const char *uuid,struct tpVersion *ver)
{
	char *path_conf=(char *)malloc(PATH_MAX_LENGTH);
	if(path_conf==NULL)
		return -1;
	char ver_str[16];	//本机已安装的应用的版本号

	if(find_directory(APP_INSTALL_PATH, uuid)>0)
		snprintf(path_conf,PATH_MAX_LENGTH,"%s/%s/config.conf",APP_INSTALL_PATH,uuid);
	else if(find_directory(APPS_INSTALL_PATH, uuid)>0)
		snprintf(path_conf,PATH_MAX_LENGTH,"%s/%s/config.conf",APPS_INSTALL_PATH,uuid);
	else{
		free(path_conf);
		return 1;
	}

	extract_key_value(path_conf,"Version:",ver_str);
	string_to_version((const char*)ver_str,ver);
	return 0;
}

//获取已安装的应用的版本号(从安装后生成的json文件中读)
int get_app_version_json(const char *uuid,struct tpVersion *ver)
{
	char *path_conf=(char *)malloc(PATH_MAX_LENGTH);
	if(path_conf==NULL)
		return -1;
	char uuid_json[64];
	char ver_str[16];	//本机已安装的应用的版本号

	memset(uuid_json,0,sizeof(uuid_json));
	snprintf(uuid_json,sizeof(uuid_json),"%s.json",uuid);

	if(find_directory(APP_JSON_PATH, uuid_json)>0)
		snprintf(path_conf,PATH_MAX_LENGTH,"%s/%s",APP_JSON_PATH,uuid_json);
	else if(find_directory(APPS_JSON_PATH, uuid)>0)
		snprintf(path_conf,PATH_MAX_LENGTH,"%s/%s",APPS_JSON_PATH,uuid_json);
	else{
		free(path_conf);
		return 1;
	}
	find_key_from_file(path_conf,"Version",ver_str);
	string_to_version((const char*)ver_str,ver);
	return 0;
}

//转换arch到标准方便处理的字符串ArchMapping arch_maps
int get_standard_arch(const char *input, char *output) {
	for (size_t i = 0; i < sizeof(arch_maps) / sizeof(struct ArchMapping); i++) {
		if (strcmp(input, arch_maps[i].architecture) == 0) {
			strcpy(output, arch_maps[i].standard);
			return 1;  // 找到匹配
		}
	}
	return 0;  // 未找到匹配
}

//检查硬件环境
//envs:安装包中的硬件环境
//返回-1不支持，返回0未知，返回1支持
int install_check_arch(char *envs)
{
	struct utsname uname_data;
	if (uname(&uname_data) != 0) {
 		return 0;
    }
	char *env_local=uname_data.machine;
	char env_local_s[16];	//标准
	char envs_s[128];		//标准硬件环境字符
	memset(envs_s, 0, sizeof(envs_s));

	string_char_replace(envs,'-','_');
//	string_char_replace(envs,' ','_');
	string_to_lowercase(envs) ;
	char *envs_ptr;
	char *token = strtok_r(envs, " ", &envs_ptr);
	while(token)	//传入的安装包支持的环境可能不止一个
	{
		get_standard_arch(token, env_local_s);
		strcat(envs_s,env_local_s);
		strcat (envs_s," ");
		token=strtok_r(NULL," ",&envs_ptr);
	}

	string_char_replace(env_local,'-','_');
	string_char_replace(env_local,' ','_');
	string_to_lowercase(env_local) ;
	get_standard_arch(env_local, env_local_s);

//	printf("硬件架构%s,%s\n",envs_s,env_local_s);
	if(strcmp(envs_s,env_local_s)>0)
		return 1;
	fprintf(stderr,"硬件架构检查不通过：安装包允许架构：%s，当前环境架构：%s\n",envs_s,env_local_s);
	return -1;
}

//检查内存空间
//返回1，空间足够。返回-1空间不够，返回0，未知
int install_check_diskspace(uint32_t need_space)
{
	struct statvfs stat;	
	if(need_space==0)
	{
		return 0;
	}
	if (statvfs("/", &stat) != 0)
        return 0;
	//unsigned long int total_space = stat.f_blocks * stat.f_frsize; // 总空间
	unsigned long int free_space = stat.f_bfree * stat.f_frsize;   // 剩余空间
	//unsigned long int used_space = total_space - free_space;        // 已用空间
	//printf("剩余空间%ld,需要空间%ld\n", free_space,need_space);
	if(free_space > need_space)
		return 1;
	fprintf(stderr,"磁盘空间检查不通过：剩余空间%ld,需要空间%d\n", free_space,need_space);
	return -1;
}

//检查版本(比较版本大小)
//返回1，安装包版本更高；返回0，安装包版本更低或相同
int install_check_app_version(const char *uuid,struct tpVersion ver)
{
	struct tpVersion ver_local;
//	get_app_version_config(uuid,&ver_local);
	get_app_version_json(uuid,&ver_local);
	if(ver.x>ver_local.x)
	    return 1;
	else if(ver.x==ver_local.x && ver.y>ver_local.y)
		return 1;
	else if(ver.x==ver_local.x && ver.y==ver_local.y && ver.z>ver_local.z)
	    return 1;
	fprintf(stderr,"版本检查不通过：安装包版本：%d.%d.%d，当前环境版本：%d.%d.%d\n",ver.x,ver.y,ver.z,ver_local.x,ver_local.y,ver_local.z);
	return 0;
}

//检查库中的版本号
//三种方式：config,.so末尾，elf文件内部
int install_check_lib_version(struct LibPackageConfig *config)
{
	return 0;
}

//系统库安装包中config文件中的export lib 行内容提取
int extract_lib_config_lib(char *libs,struct LibPackageConfig *config)
{
	char *libs_ptr;
	char *lib_name=strtok_r(libs," ",&libs_ptr);
	while(lib_name)
	{
		char *ver=strchr(lib_name, '@');
		ver[0]='\0';
		config->system_lib[config->lib_count]=(char *)malloc(strlen(lib_name)+1);
		strcpy(config->system_lib[config->lib_count],lib_name);
		string_to_version(ver+1,&config->version[config->lib_count]);
		config->lib_count++;
		lib_name=strtok_r(NULL," ",&libs_ptr);
	}
	return 0;
}

//config信息中export行的内容类型
PackageExportType get_config_export_key_type(const char *key)
{
	PackageExportType type=EXPORT_NONE;
	
	if(strncmp(key,"lib",3)==0){
		type=EXPORT_LIBS;
	}
	else if(strncmp(key,"depend",6)==0){
		type=EXPORT_DEPEND;
	}
	else if(strncmp(key,"icon",4)==0 || strncmp(key,"start",5)==0 ||strncmp(key,"remove",6)==0){	//icon start remove
		type=EXPORT_MUST;
	}
	return type;
}

//安装包中的config信息提取
int extract_config_info(const char *file_config,struct PackageConfigInfo *conf)
{	
	FILE *file_s = fopen(file_config, "r");
	if (!file_s) {
		perror("fopen s_config");
		return -1;
	}

	char line[CONFIG_MAX_LENGTH];
    //安装包类型
	if(!fgets(line, CONFIG_MAX_LENGTH, file_s))
		return -1;
	if(strncmp(line,PACKAGE_TYPE_CONFIG_LIB,strlen(PACKAGE_TYPE_CONFIG_LIB))==0)
		conf->type=TYPE_PACKAGE_LIB;
	else if(strncmp(line,PACKAGE_TYPE_CONFIG_SAPP,strlen(PACKAGE_TYPE_CONFIG_SAPP))==0)
		conf->type=TYPE_PACKAGE_SAPP;
	else if(strncmp(line,PACKAGE_TYPE_CONFIG_UAPP,strlen(PACKAGE_TYPE_CONFIG_UAPP))==0)
		conf->type=TYPE_PACKAGE_APP;
	else
		conf->type=TYPE_PACKAGE_NONE;
	printf("解析config,%d\n",conf->type);
	switch(conf->type)
	{
		// 逐行读取安装包配置文件信息
		case TYPE_PACKAGE_APP:
		case TYPE_PACKAGE_SAPP:
		{
			struct AppPackageConfig *config=&(conf->app_conf);
			while (fgets(line, CONFIG_MAX_LENGTH, file_s)) 
			{
				if(line[0]=='#')	//跳过注释行
					continue;
				// 移除换行符
				trim_newline(line);
				
				if(strncmp(line,"appName:",8)==0)
				{
					strncpy(config->app_name,line+8,sizeof(config->app_name));
				}
				else if(strncmp(line,"appID:",6)==0)
				{
					strncpy(config->app_id,line+6,sizeof(config->app_id));
				}
				else if(strncmp(line,"Version:",8)==0)
				{
					//struct tpVersion ver;
					string_to_version((const char*)line+8,&(config->version));	
				}
				else if(strncmp(line,"appexecName:",12)==0)
				{
					int len=strlen(line+12)+1;
					config->appexec_name=malloc(len);
					strncpy(config->appexec_name,line+12,len);
				}
				else if(strncmp(line,"Architecture:",13)==0)
				{
					strncpy(config->architecture,line+13,sizeof(config->architecture));
				}
				else if(strncmp(line,"DiskSpace:",10)==0)
				{
					long int size;
					if(string_to_number(line+10,&size)==0)
					    config->diskspace=(uint32_t)size;
					else
						config->diskspace=0;
				}
				else if(strncmp(line,"export icon=",12)==0)
				{	
					config->icon=strdup(line+12);
					delete_end_space(config->icon);
					printf("config->icon=%s,\n",config->icon);
				}
			}
			if(find_directory(APP_INSTALL_PATH,config->app_id)>0)
			{
				printf("应用已安装\n");
				config->install_flag = 1;
				//return 0;
			}
			else
				config->install_flag = 0;
			break;
		}
		case TYPE_PACKAGE_LIB:		//系统库安装包配置文件解析
		{
			struct LibPackageConfig *config=&(conf->lib_conf);
			uint8_t last_line=0;
			while (fgets(line, CONFIG_MAX_LENGTH, file_s)) 
			{
				if(line[0]=='#')	//跳过注释行
					continue;
				// 移除换行符
				trim_newline(line);
				if(strncmp(line,"Architecture:",13)==0)
				{
					strncpy(config->architecture,line+13,sizeof(config->architecture));
				}
				else if(strncmp(line,"DiskSpace:",10)==0)
				{
					long int size;
					if(string_to_number(line+10,&size)==0)
					    config->diskspace=(uint32_t)size;
					else
						config->diskspace=0;
				}
				else if(strncmp(line,"export lib=",11)==0 || last_line==1)
				{
					last_line=0;
					char *libs=line+11;
					int len=strlen(libs);
					if(len<2)		//没有需要复制的库
						break;
					if(libs[len-1]!=' ')		//防止行太长一次没有读取完
					{
						char *p=strrchr(line, ' ');
						int seek_len=strlen(p);
						fseek(file_s, -seek_len, SEEK_CUR);
						p[0]='\0';
						last_line=1;		//本行没有读完，下次读还是这一行
					}
					extract_lib_config_lib(libs,config);
				}
			}	
			break;
		}
		default:
			break;
	}
	fclose(file_s);
}

//文件MD5检查
int install_check_md5(const char *file_path)
{
	uint8_t md5_len=MD5_DIGEST_LENGTH;
	uint8_t md5_r[md5_len];
	del_md5_from_file(file_path,md5_r,1);		

	uint8_t md5[md5_len];
	compute_md5(file_path, md5);
	for(int i=0; i<md5_len; i++) {
		if(md5[i]!= md5_r[i]) {
			add_md5_to_file(file_path,md5_r);	
            return -1;
        }
	}
	add_md5_to_file(file_path,md5_r);	//当del_md5_from_file的最后一个参数为1的时候需要调用
	return 1;
}

//AppPackageConfig结构体释放
int free_AppPackageConfig(struct AppPackageConfig *conf)
{
	if(conf->description)
		free(conf->description);
	if(conf->appexec_name)
		free(conf->appexec_name);
	if(conf->signature)
		free(conf->signature);
	for(int i=0;i<conf->depend_count;i++)
		free(conf->depend[i]);
	for(int i=0;i<conf->lib_count;i++)
		free(conf->lib[i]);
	for(int i=0;i<conf->assert_count;i++)
		free(conf->assert[i]);
	for(int i=0;i<conf->otherfile_count;i++)
		free(conf->otherfile[i]);
	if(conf->icon)
	{
		//如果是临时文件需要删除
		if(strncmp(conf->icon,TMP_CACHE_FILE_PATH,strlen(TMP_CACHE_FILE_PATH))==0)
			close_directories_temp(conf->icon);
		else
			free(conf->icon);
	}
	return 0;
}

//struct LibPackageConfig释放
int free_LibPackageConfig(struct LibPackageConfig *conf)
{
	for(int i=0;i<conf->lib_count;i++)
	{
		free(conf->system_lib[i]);
	}
	return 0;
}



int appm_check_arch(struct PackageConfigInfo *conf)
{
	char *arch;
	switch(conf->type)
	{
		case TYPE_PACKAGE_APP:
		case TYPE_PACKAGE_SAPP:
			arch=conf->app_conf.architecture;
			break;
		case TYPE_PACKAGE_LIB:
			arch=conf->lib_conf.architecture;
			break;
		default:
			return 0;
	}
	return install_check_arch(arch);
}


int appm_check_space(struct PackageConfigInfo *conf)
{
	uint32_t space;
	switch(conf->type)
	{
		case TYPE_PACKAGE_APP:
		case TYPE_PACKAGE_SAPP:
			space=conf->app_conf.diskspace;
			break;
		case TYPE_PACKAGE_LIB:
			space=conf->lib_conf.diskspace;
			break;
		default:
			return 0;
	}
	return install_check_diskspace(space);
}

//检查版本，分为库的版本和应用的版本
int appm_check_version(struct PackageConfigInfo *conf)
{
	switch(conf->type)
	{
		case TYPE_PACKAGE_APP:
		case TYPE_PACKAGE_SAPP:
			return install_check_app_version(conf->app_conf.app_id,conf->app_conf.version);
			break;
		case TYPE_PACKAGE_LIB:
			return install_check_lib_version(&conf->lib_conf);
			break;
		default:
			return 0;
	}
	return 0;
}


//获取已安装的应用的版本
int appm_get_app_version(const char *uuid,struct tpVersion *version)
{
	//get_app_version_config(uuid,version);
	get_app_version_json(uuid,version);
	return 0;
}

int appm_get_app_is_install(struct PackageConfigInfo *conf)
{
	switch(conf->type)
	{
		case TYPE_PACKAGE_APP:
		case TYPE_PACKAGE_SAPP:
			return (conf->app_conf.install_flag>0 ? 1:0);
			break;
		case TYPE_PACKAGE_LIB:
		default:
			return 0;
	}
}

