/*///------------------------------------------------------------------------------------------------------------------------//
		安装包中必要文件的生成
说 明 : 
日 期 : 2024.9.2

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "file_creat.h"
#include "appm_creat.h"
#include "../manage/install.h"
#include "../manage/utilslib.h"

static long calculate_directory_size(const char* dir_path);
/*
		示例：
		appID:f03c8f8c-dd9b-453f-b2d4-d049c073e252
		appName:mytestapp
		organization:MyCompany
		Version:1.0.0
		appexecName:MyAppLication
		Architecture:amd64 i386
		DiskSpace:1024000
		FileExtension:.pdf .png .jpg
		Section:free
		Priority:optional
		Essential:no
		Author:Chingan 2111956539@qq.com
		Provides:MyAdcSoftware
		Description:adc detect
		export depend:libalsa@1.1.0 libbluez-5@5.0.21 libmylib@0.0.1
		export lib=./lib
		export icon=./icon
		export start=./start.sh
		export remove=./remove.sh
		export myfile=./myfile 

*/

/**
 * 计算指定路径的文件或目录大小
 * @param path 文件/目录路径
 * @return 文件/目录大小（字节数），出错返回-1
 */
static long calculate_path_size(const char* path) {
    struct stat path_stat;
    // 获取文件/目录信息
    if (lstat(path, &path_stat) < 0) {
        perror("lstat failed");
        return -1;
    }

    // 如果是普通文件，直接返回大小
    if (S_ISREG(path_stat.st_mode)) {
        return path_stat.st_size;
    }
    // 如果是目录，递归计算大小
    else if (S_ISDIR(path_stat.st_mode)) {
        return calculate_directory_size(path);
    }
    // 忽略其他类型（符号链接等）
    return 0;
}

/**
 * 递归计算目录大小
 * @param dir_path 目录路径
 * @return 目录总大小（字节数）
 */
static long calculate_directory_size(const char* dir_path) {
    DIR* dir;
    struct dirent* entry;
    long total_size = 0;
    char full_path[MAX_PATH];

    // 打开目录
    if ((dir = opendir(dir_path)) == NULL) {
        perror("opendir failed");
        return -1;
    }

    // 遍历目录内容
    while ((entry = readdir(dir)) != NULL) {
        // 忽略当前目录和上级目录
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 构建完整路径
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        // 递归计算每个项目的大小
        long size = calculate_path_size(full_path);
        if (size < 0) {
            closedir(dir);
            return -1;
        }
        total_size += size;
    }

    closedir(dir);
    return total_size;
}

//写入指令集类型
const char *architecture_map[]={"none","amd64","i386","arm64","arm32","risc_v"};

const char *get_architecture_string(tpEnumArchType arch)
{
	return architecture_map[arch];
}

static int is_directory(const char *path) 
{
	struct stat statbuf;
	if (stat(path, &statbuf) != 0) {
		return 0;
	}
	return S_ISDIR(statbuf.st_mode);
}

//拷贝文件或文件夹到目标路径
//path:安装包路径（目标路径）
//file:要拷贝进去的文件（源文件）
static int file_copy(const char *path, const char *file)
{
	int ret = 0;
	char command[MAX_PATH*2];
	memset(command,0,MAX_PATH*2);
	snprintf(command,MAX_PATH*2,"cp -rp %s %s/",file,path);
	printf("command: %s\n",command);
	ret = system(command);
	if (ret != 0) {
		printf("File copy failed.\n");
		return -1;
	}
	return 0;
}

//拷贝一系列文件或文件夹到目标路径
static int file_list_copy(const char *path, char *file[],int count)
{
	for(int i=0;i<count;i++) 
	{
		int ret=file_copy(path,file[i]);
		if(ret<0)
			return -1;
	}
	return 0;
}

//检查配置是否合法
static int package_config_check(struct AppPackageConfig *config)
{

}

//目录生成
int file_dir_creat(const char *path)
{
	char filename[MAX_PATH];
	memset(filename,0,MAX_PATH);
	snprintf(filename, MAX_PATH, "%s",path);
	int len_path=strlen(filename);

	if (mkdir(filename, 0755) == -1) 
	{
		printf("creat %s error\n",filename);
		perror("install file creat:");
		return -1;
	}
	printf("install path:%s\n",filename);
	//assert,
	filename[len_path] = '\0';
	strcat(filename,"/assert");
	if (mkdir(filename, 0755) == -1)
	{
		printf("assert path:%s error\n",filename);
		return -1;
	}
	//lib 
	filename[len_path] = '\0';
	strcat(filename,"/lib");
	if (mkdir(filename, 0755) == -1)
	{
		printf("lib path:%s error\n",filename);
		return -1;
	}
	//bin
	filename[len_path] = '\0';
	strcat(filename,"/bin");
	if (mkdir(filename, 0755) == -1)
	{
		printf("bin path:%s error\n",filename);
		return -1;
	}

	return 0;
}

//path:安装包路径，file:要拷贝进去的文件,拷贝完成后会修改源字符串
int file_copy_icon(const char *path_pack, const char *path_s)
{
//	char filename[MAX_PATH];
//	memset(filename,0,MAX_PATH);
//	snprintf(filename, MAX_PATH, "%s/icon.jpg",path_pack);
	return file_copy(path_pack,path_s);
}

int file_copy_app(const char *path_pack, const char *path_s)
{
	char filename[MAX_PATH];
	memset(filename,0,MAX_PATH);
	snprintf(filename, MAX_PATH, "%s/bin",path_pack);
	file_copy(filename,path_s);
}

int file_copy_lib(const char *path_pack, char *file[],int count)
{
	char filename[MAX_PATH];
	memset(filename,0,MAX_PATH);
	snprintf(filename, MAX_PATH, "%s/lib",path_pack);
	return file_list_copy(filename,file,count);
}

int file_copy_assert(const char *path_pack, char *file[],int count)
{
	char filename[MAX_PATH];
	memset(filename,0,MAX_PATH);
	snprintf(filename, MAX_PATH, "%s/assert",path_pack);
	return file_list_copy(filename,file,count);
}

int file_copy_bin(const char *path_pack, char *file[],int count)
{
	char filename[MAX_PATH];
	memset(filename,0,MAX_PATH);
	snprintf(filename, MAX_PATH, "%s/bin",path_pack);
	return file_list_copy(filename,file,count);
}

int file_copy_other(const char *path_pack, char *file[],int count)
{
	char filename[MAX_PATH];
	memset(filename,0,MAX_PATH);
	snprintf(filename, MAX_PATH, "%s",path_pack);
	return file_list_copy(filename,file,count);
}

//生成安装包中的配置文件
int file_config_creat(const char *path,struct AppPackageConfig *conf,TypePackage type)
{
	char filename[MAX_PATH];
	memset(filename,0,MAX_PATH);
	snprintf(filename, MAX_PATH, "%s/config",path);
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}
	switch(type)	//安装包类型
	{
		case TYPE_PACKAGE_APP:
			fprintf(file, "%s\n", PACKAGE_TYPE_CONFIG_UAPP);
			break;
		case TYPE_PACKAGE_SAPP:
			fprintf(file, "%s\n", PACKAGE_TYPE_CONFIG_SAPP);
			break;
		default:
			fprintf(file, "%s\n", PACKAGE_TYPE_CONFIG_UAPP);
			break;
	}
	fprintf(file, "appID:%s\n", conf->app_id);
	fprintf(file, "appName:%s\n", conf->app_name);
	fprintf(file, "organization:%s\n", conf->organization);
	fprintf(file, "Version:%d.%d.%d\n", conf->version.x,conf->version.y,conf->version.z);
	if(conf->appexec_name){
		char *str_p=strrchr(conf->appexec_name,'/')+1;
		fprintf(file, "appexecName:%s\n", str_p);
	}
	fprintf(file, "DiskSpace:%u\n", conf->diskspace);
	fprintf(file, "Architecture:%s\n", conf->architecture);
	fprintf(file, "Section:%s\n", conf->section);
	fprintf(file, "Priority:%s\n", conf->priority);
	fprintf(file, "Essential:%s\n", conf->essential);
	fprintf(file, "Author:%s <%s>\n", conf->author,conf->contact);
	fprintf(file, "Provides:%s\n", conf->provides);
	//支持的文件类型
	if(conf->extension_count>0)
	{
		//printf("extension_count:%d\n", conf->extension_count);
		fprintf(file, "FileExtension:");
		for(int i=0;i<conf->extension_count;i++) {
			fprintf(file, "%s ", conf->file_extension[i]);
		}
		fprintf(file,"\n");
	}
	if(conf->description)
		fprintf(file, "Description:%s\n", conf->description);
	//图标
	if(conf->icon){
		char *str_p=strrchr(conf->icon,'/')+1;
		fprintf(file, "export icon=./%s\n", str_p);
	}
	//可执行文件
	if(conf->appexec_name){
		char *str_p=strrchr(conf->appexec_name,'/');
		fprintf(file, "export appexec=.%s\n", str_p);
	}
	//依赖开源库
	if(conf->depend_count>0){
		fprintf(file, "export depend=");
		for(int i=0;i<conf->depend_count;i++) {
			fprintf(file, "%s ", conf->depend[i]);
		}
		fprintf(file,"\n");
	}

	fprintf(file, "export bin=./bin \n");
	fprintf(file, "export lib=./lib \n");
	fprintf(file, "export start=./start.sh\n");

//    fprintf(file, "export remove=%s\n", conf->remove);

	//其他文件
	if(conf->otherfile_count>0){
		fprintf(file, "export userfile=");
		for(int i=0;i<conf->otherfile_count;i++) {
			fprintf(file, ".%s ", strrchr(conf->otherfile[i],'/'));
		}
		fprintf(file,"\n");
	}

	fclose(file);
	return 0;
}

//生成原始打包文件(未压缩的源文件)
//config:配置信息
//path:源文件生成的路径
//type:安装包类型，app，sapp，lib，default
int appm_generate_package_source(struct AppPackageConfig *config,char *path,TypePackage type)
{
	//1.检查配置

	//2.生成目录
//	const char *path="/home/pix/AppManage/piktest";
	if(path[strlen(path)-1]=='/')
	    path[strlen(path)-1]='\0';
	if(file_dir_creat(path)<0)
		return -1;
	//3.根据结构体内容拷贝
	if(config->appexec_name==NULL)
		return -1;
	file_copy_app(path, config->appexec_name);
	if(config->icon!=NULL)
		file_copy_icon(path, config->icon);
	if(config->bin_count)
		file_copy_bin(path, config->bin, config->bin_count);
	if(config->lib_count)
		file_copy_lib(path, config->lib, config->lib_count);
	if(config->assert_count>0)
		file_copy_assert(path, config->assert, config->assert_count);
	if(config->otherfile_count>0)
		file_copy_other(path, config->otherfile, config->otherfile_count);
 

	//3.计算文件大小
	if(config->diskspace==0)
	{
		long long total_size = calculate_directory_size(path);
		config->diskspace=total_size;
	}

	//4.根据结构体内容写入配置文件
	printf("=====write config file\n");
	file_config_creat(path,config,type);

	//5.生成启动文件
	return 0;
}

//=============================启动脚本=====================================


void init_script_config(struct ScriptInfo *config) {
    memset(config, 0, sizeof(struct ScriptInfo));
}

// 添加环境变量
void add_env_var(struct ScriptInfo *config, const char *key, const char *value) {
	if (config->env_var_count < MAX_ITEMS) {
		char *entry = (char*)malloc(strlen(key) + strlen(value) + 2); // +2 for '=' and '\0'
		sprintf(entry, "%s=%s", key, value);
		config->env_vars[config->env_var_count++] = entry;
	}
}

// 添加依赖库（一般是系统通用的库）
//库名字，版本
void add_dependency(struct ScriptInfo *config, const char *lib) {
    if (config->dep_count < MAX_ITEMS) {
        config->dependencies[config->dep_count++] = strdup(lib);
    }
}

// 添加启动参数
void add_arg(struct ScriptInfo *config, const char *arg) {
    if (config->arg_count < MAX_ITEMS) {
        config->args[config->arg_count++] = strdup(arg);
    }
}

// 设置日志文件路径
void set_log_file(struct ScriptInfo *config, const char *log_file) {
    config->log_file = strdup(log_file);
}

// 设置配置文件路径
void set_config_file(struct ScriptInfo *config, const char *config_file) {
    config->config_file = strdup(config_file);
}


//启动文件生成
//
//output_file:启动文件生成位置，通常在安装包根目录
int appm_generate_startup_script(struct ScriptInfo *config, const char *output_file) {
	FILE *fp = fopen(output_file, "w");
	if (fp==NULL) {
		perror("Failed to create startup script");
		return -1;
	}
	fprintf(fp, "#!/bin/bash\n\n");
	
	// 设置其他环境变量
//	fprintf(fp, "export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH\n");		//
	for (int i = 0; i < config->env_var_count; i++) {
		fprintf(fp, "export %s=$%s:%s\n", config->env_type[i],config->env_type[i],config->env_vars[i]);
	}
//	printf("startup script  export ok\n");
	// 设置环境变量中的依赖库路径
	fprintf(fp, "\n# Dependencies\n");
	fprintf(fp, "export LD_LIBRARY_PATH=./lib");
	for (int i = 0; i < config->dep_count; i++) {
		fprintf(fp, ":%s", config->dependencies[i]);
	}
	fprintf(fp, ":%s\n", PATH_SYSTEM_LIB);
	printf("startup script  depend ok\n");
	// 写入日志文件配置
//	if (config->log_file) {
//		fprintf(fp, "\n# Log File\n");
//		fprintf(fp, "LOG_FILE=%s\n", config->log_file);
//	}

	// 写入配置文件路径
//	if (config->config_file) {
//		fprintf(fp, "\n# Config File\n");
//		fprintf(fp, "CONFIG_FILE=%s\n", config->config_file);
//	}

	// 写入启动参数
	if (config->arg_count > 0) {
		fprintf(fp, "\n# Startup Parameters\nARG=");
		for (int i = 0; i < config->arg_count; i++) {
			fprintf(fp, "%s ", config->args[i]);
		}
		fprintf(fp, "\n");
	}
	// 写入执行路径
	fprintf(fp, "\n# Execute Application\n");
	fprintf(fp, "APP_PATH=\"./bin/%s\"\n", config->exec_path);
	//检查app是否存在
	fprintf(fp,"\n%s\n",CHECK_APP_OK);
	//启动应用
	fprintf(fp,"./$APP_PATH \"$ARG\"\n");
	//进程id打印
	fprintf(fp,"\nPID=$!\n");
	fprintf(fp,"echo \"应用程序PID: $PID\"\n");
//	fflush(fp);
	fclose(fp);
	char command[MAX_PATH];
	snprintf(command,MAX_PATH,"chmod 777 %s\n",output_file);
	if(system(command)==-1)
	    return -1;
	printf("startup script ok\n");
	return 0;
}



//系统库打包===============================================================

//生成库安装包中的配置文件并打包
int file_config_creat_lib(struct archive *a,const char *path,struct LibPackageConfig *conf)
{
//	char filename[MAX_PATH];
//	memset(filename,0,MAX_PATH);
//	snprintf(filename, MAX_PATH, "%s/config",path);
	FILE *file = fopen(path, "w");
	if (file == NULL) {
		perror("Failed to open file\n");
		return -1;
	}
	fprintf(file, "#TinyPix SystemLib\n");

	fprintf(file, "Architecture:%s\n", conf->architecture);
//	fprintf(file, "Architecture:%s\n", get_architecture_string(conf->arch));
	fprintf(file, "DiskSpace:%u\n", conf->diskspace);
	char buf[MAX_LEN_APP_NAME];
	//库文件
	if(conf->lib_count>0)
	{
		char *lib_name;
		fprintf(file, "export lib=");
		for(int i=0;i<conf->lib_count;i++) {
			if(is_directory(conf->system_lib[i])!=0){
				fclose(file);
				return -1;
			}
			if((lib_name=strrchr(conf->system_lib[i],'/'))==NULL)
			    lib_name=conf->system_lib[i];
			fprintf(file, "%s@%d.%d.%d ", lib_name,conf->version[i].x,conf->version[i].y,conf->version[i].z);
			snprintf(buf,sizeof(buf),".%s",lib_name);
			add_file_to_archive(a, conf->system_lib[i], buf);		//config打包
		}
		fprintf(file,"\n");
	}

	//其他文件
	if(conf->file_count>0)
	{
		char *file_name;
		fprintf(file, "export file=");
		for(int i=0;i<conf->file_count;i++) {
			if(is_directory(conf->file[i])!=0){
				fclose(file);
				return -1;
			}
			if((file_name=strrchr(conf->file[i],'/'))==NULL)
			    file_name=conf->file[i];
			fprintf(file, "./%s ", file_name);
			snprintf(buf,sizeof(buf),".%s",file_name);
			add_file_to_archive(a, conf->file[i], buf);		//config打包
		}
		fprintf(file,"\n");
	}

	fclose(file);
	return 0;
}


