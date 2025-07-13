/*///------------------------------------------------------------------------------------------------------------------------//
		应用安装(解包)程序
说 明 : 
日 期 : 2024.8.20

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "../inc/archive.h"
//#include "../inc/archive_entry.h"
//#include <openssl/evp.h>
//#include <openssl/rand.h>
#include <fcntl.h>
#include "unpack.h"
#include "install.h"
#include "utilslib.h"
#include "install_check.h"


//新建文件目录
//unpack_test/control/control
void create_directories(const char *path) 
{
	char temp[1024];
	char *p = NULL;
	size_t len;

	snprintf(temp, sizeof(temp), "%s", path);
	len = strlen(temp);
	if (temp[len - 1] == '/') {
		temp[len - 1] = 0;
	}

	for (p = temp + 1; *p; p++) {
		if (*p == '/') {
			*p = 0;
			mkdir(temp, 0755);
			*p = '/';
		}
	}
 //   mkdir(temp, S_IRWXU);
}

//新建解压归档
int open_unpack_entry(struct UnpackEntry *unpack,const char *filename)
{
	unpack->a = archive_read_new();
	archive_read_support_format_all(unpack->a);
	archive_read_support_filter_all(unpack->a);			//过滤压缩文档格式

	if (archive_read_open_filename(unpack->a, filename, 10240) != ARCHIVE_OK) {
		fprintf(stderr, "Could not open archive file: %s\n", filename);
		archive_read_free(unpack->a);
		return -1;
	}
	return 0;
}

int close_unpack_entry(struct UnpackEntry *unpack)
{
	archive_read_close(unpack->a);
	archive_read_free(unpack->a);
	return 0;
}

//从归档条目(包)中查找某个条目(文件)并写入到某个文件,成功返回0
int archive_find_entry(struct archive *a,const char *file_r,const char *file_w)
{
	struct archive_entry *entry;
	int r;
	int err=-1;
	FILE *output_file=NULL;
	if(file_w != NULL)
	{
		output_file = fopen(file_w, "w+");
		if (output_file == NULL) {
			perror("fopen");
			printf("open err:%s\n",file_w);
			return -1;
		}
	}
	while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK) 
	{
		const char *current_file = archive_entry_pathname(entry);
		if(strcmp(current_file,file_r)==0)
		{
			/*if (archive_entry_filetype(entry) == AE_IFDIR) {	//如果当前条目是目录就创建文件夹否则创建文件
				mkdir(full_path, 0755);
			} */

			char *buffer = (char *)malloc(10240);
			size_t len;
//			archive_read_data_aes(a, buffer, 8192);
			while ((len = archive_read_data(a, buffer, 10240)) > 0) {		//archive_read_data_aes(a, buffer, 10240);
				fwrite(buffer, 1, len, output_file);
			}
			err=0;
			break;
		}
	}
	if (r != ARCHIVE_EOF) {
		fprintf(stderr, "archive_find_entry:Error reading archive: %s\n", archive_error_string(a));
	}

	fclose(output_file);
	return err;
}

//常规完全解包(由于会对归档遍历操作，而遍历未知不能重置，因此建议关闭后重新打开文件来获取新的struct archive *)
int extract_from_archive(struct archive *a, const char *sour_dir, const char *dest_dir)
{	
	struct archive_entry *entry;
	char full_path[1024];
	int r;

	// 创建目标目录（如果不存在）
	snprintf(full_path, sizeof(full_path), "%s", dest_dir);
	mkdir(full_path, 0755);

//	printf("[debug]:extract_from_archive\n");
	// 读取归档条目
	while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK) 
	{
//		printf("[debug]:archive_read_next_header\n");
		const char *current_file;
		FILE *output_file;
		char *buffer;
		ssize_t len;

		// 获取文件名
		current_file = archive_entry_pathname(entry);
		if(sour_dir!=NULL && strncmp(current_file, sour_dir,strlen(sour_dir))!=0)	//不为空时候需要解指定文件
		{
			continue;
		}

		snprintf(full_path, sizeof(full_path), "%s/%s", dest_dir, current_file);

		// 处理目录
		if (archive_entry_filetype(entry) == AE_IFDIR) {	//如果当前条目是目录就创建文件夹否则创建文件
			mkdir(full_path, 0755);
		} 
		else {
			//创建目录
			create_directories((const char *)full_path);
			// 创建文件
			output_file = fopen(full_path, "wb");
			if (output_file == NULL) {
				printf("open %s error\n", full_path);
				perror("fopen");
				archive_read_free(a);
				return -1;
			}

			// 写入文件数据
			buffer = (char *)malloc(8192);
//			archive_read_data_aes(a, buffer, 8192);
			while ((len = archive_read_data(a, buffer, 8192)) > 0) {	//archive_read_data_aes(a, buffer, 10240);
				fwrite(buffer, 1, len, output_file);
			}

			free(buffer);
			fclose(output_file);
		}
		break;
	}

	// 处理错误
	if (r != ARCHIVE_EOF) {
		fprintf(stderr, "extract_from_archive:Error reading archive: %s\n", archive_error_string(a));
	}
	return 0;
}

//常规完全解包
//filename:原始包
//dest_dir:解压路径
int extract_archive_file(const char *filename, const char *sour_dir, const char *dest_dir) 
{
	printf("=====新建解压归档,解包%s,%s\n",sour_dir,dest_dir);
	struct UnpackEntry unpack;
	if(open_unpack_entry(&unpack,filename)<0)		//创建归档对象
		return -1;
	if(extract_from_archive(unpack.a,sour_dir, dest_dir)<0){
		close_unpack_entry(&unpack);
		return -1;
	}
	close_unpack_entry(&unpack);
	return 0;
}




int Appm_Unpack(const char *archive_name,uint8_t type) 
{
	char dest_path[PATH_MAX_LENGTH];
	char *pik=strrchr(archive_name,'/');
	if(pik==NULL){
		perror("archive_name");
		return -1;
	}
	char *dest=strstr(pik,PACKAGE_FILE_SUFFIX);
	if(dest==NULL){		//目录不对，后缀不对
		perror("后缀 find err");
		return -1;
	}
	//	dest[0]='/0';

	switch (type)
	{
		case TYPE_PACKAGE_APP:
			snprintf(dest_path,sizeof(dest_path),"%s/%s%s",APP_INSTALL_PATH,APP_TEMP,pik);
			break;
		case TYPE_PACKAGE_SAPP:
			snprintf(dest_path,sizeof(dest_path),"%s/%s%s",APPS_INSTALL_PATH,APP_TEMP,pik);
			break;
		case TYPE_PACKAGE_LIB:
			snprintf(dest_path,sizeof(dest_path),"%s/%s%s",LIBS_INSTALL_PATH,APP_TEMP,pik);
			break;
		default:
			break;
	}
	pik=strrchr(dest_path,'/');
	dest=strstr(pik,PACKAGE_FILE_SUFFIX);
	dest[0]='\0';
	printf("Package %s extracted to %s\n", archive_name, dest_path);

	//删除MD5
//	del_md5_from_file(archive_name,NULL);	

	// 解包归档
	extract_archive_file(archive_name,NULL ,dest_path);
	printf("Package %s extracted to %s\n", archive_name, dest_path);
	return 0;
}

//单独从包pack中解出来条目entry,并写入unpack_file，如果unpac_file为空，则会创建一个临时文件用于保存entry条目的内容
//返回解出来的文件的路径
//如果unpack_file为空会产生临时文件，必须调用int close_directories_temp(char *file)删除临时文件
int extract_file_pack(const char *pack,const char *entry,char *unpack_file)
{
	struct UnpackEntry unpack;
	char *file_path=unpack_file;
	if(open_unpack_entry(&unpack,pack)<0)
		return -1;
	if(file_path ==NULL)
		return -1;
	if(archive_find_entry(unpack.a,entry,file_path)<0)		//查找文件
		return -1;
	close_unpack_entry(&unpack);
	return 0;
}

//关闭pack中的条目entry
int close_pack_entry(char *entry_file)
{
	close_directories_temp(entry_file);
	return 0;
}

//update行处理(更新升级的标志)
void package_config_handle_update(FILE *file,struct archive *a,char *line)
{
	
}
//非export行的处理（仅需要添加到json）
void package_config_handle_info(FILE *file,struct archive *a,char *line)
{

}

//解析values的文件内容并安装
//dest_dir
int install_export_files(struct AppInstallInfo *app,char *values,AppInstallPathType install_type)
{
	//config_add_to_json(type,export_obj,key,files);
	char *strtok_ptr;
	char *file_name=strtok_r(values," ",&strtok_ptr);
	while(file_name)
	{
		printf("find:%s,write:%s\n",file_name,file_name);
		switch(install_type)
		{
			case INSTALL_PATH_TYPE_NONE:
				break;
			case INSTALL_PATH_TYPE_ROOT:
				app->install_other(app,".",file_name,"./");		//解包到./目录，相对于安装包根路径为.
				break;
			case INSTALL_PATH_TYPE_RES:
				app->install_other(app,APP_INSTALL_RES,file_name,"./");
				break;
			case INSTALL_PATH_TYPE_BIN:
				app->install_other(app,".",file_name,"./");	
				break;
			default:
				break;
		}
		
		file_name=strtok_r(NULL," ",&strtok_ptr);
	}
	
	return 0;
}





//export行处理,（export行可能会比较长，导致一次读不完，line不是完整的）
int package_config_handle_export(FILE *file,struct AppInstallInfo *app,char *line,int length,uint8_t *last_line)
{
	static AppInstallPathType install_type=0;
	static PackageExportType type=EXPORT_NONE;
	static char *key=NULL;
	char *value_tmp=NULL;

	printf("安装%s\n",line);
	char *files=NULL;
	if(*last_line==0){
		//
		install_type= install_config_export_type(line);
		//if(install_type==0)
		//	continue;
		key = line + 7;
		char *flag=strchr(line,'=');	//export行必须是key=value的格式
		if(flag==NULL){
			return 0;
		}
		value_tmp=(char *)malloc(length);
		if(value_tmp==NULL)
			return -1;
		memset(value_tmp,0,length);
		memcpy(value_tmp,flag,length-(flag-line));
		files=value_tmp+1;
		type= get_config_export_key_type(key);
	}
	else{
		*last_line=0;
		files=line;
	}

	int len=strlen(files);
	if(len<2)		//没有需要复制的库
		return 0;
	if(files[len-1]!='\n')		//防止行太长这次没有读取完
	{
		printf("没有读完\n");
		char *p=strrchr(line, ' ');
		int seek_len=strlen(p);
		fseek(file, -seek_len, SEEK_CUR);
		p[0]='\0';
		*last_line=1;		//本行没有读完，下次读还是这一行
	}
	else
		files[len-1]=' ';
	install_export_files(app,files,install_type);
	//config_add_to_json(type,export_obj,key,files);
	return 0;
}

//读取包中的config文件内容到临时文件
char *appm_get_package_config(struct tpAppInfo *info)
{
	//【计划】后期优化功能
}

//根据包中的config文件解包，
//filename:原始包
//dest_dir:解包路径
int extract_archive_package_config(struct AppInstallInfo *app_install, json_object *root) 
{
	printf("根据config文件解包=====\n");
/*	uint8_t md5_r[MD5_DIGEST_LENGTH];
	if((del_md5_from_file(filename,md5_r,1))<0){
		printf("del_md5_from_file error\n");
		add_md5_to_file(filename,md5_r);
		return -1;
	}*/
	// 打开归档文件
	struct UnpackEntry unpack;
	if(open_unpack_entry(&unpack,app_install->app->path_pik)<0){
		printf("打开归档%s失败\n",app_install->app->path_pik);
//		add_md5_to_file(filename,md5_r);
		return -1;
	}
//	app_install->a=unpack.a;

	char *config_file=open_directories_temp(TMP_FILE_PATH);		//创建临时文件保存config
	if(archive_find_entry(unpack.a,"./config",config_file)<0){
		fprintf(stderr, "Error:Can't find config\n");
		close_unpack_entry(&unpack);
//		add_md5_to_file(filename,md5_r);
		return -1;
	}
	FILE *file=fopen(config_file,"r");
	if(file==NULL){
		fprintf(stderr, "Error:Can't open config temp\n");
		close_directories_temp(config_file);
		close_unpack_entry(&unpack);
//		add_md5_to_file(filename,md5_r);
		return -1;
	}

	printf("loop anylise,config=%s\n",config_file);
	app_install->app->config_file=config_file;
	char *line=(char *)malloc(PATH_MAX_LENGTH);
	uint8_t last_line=0;
	//int install_type=0;
	//PackageExportType type=EXPORT_NONE;
	//char *key=NULL;
	struct json_object *static_obj=json_object_new_object();
	while (fgets(line, PATH_MAX_LENGTH, file)) 
	{
		printf("line=%s\n",line);
		//跳过注释行
		if(line[0]=='#')
			continue;
		//update行处理
		if(strncmp(line,"update ",7)==0 && last_line==0){
			package_config_handle_update(file,app_install->a,line);
			continue;
		}
		//非export的行处理
		if(strncmp(line,"export ",7)!=0 && last_line==0){
			package_config_handle_info(file,app_install->a,line);
			continue;
		}
		printf("line=%s\n",line);
		int line_len = strlen(line);
		package_config_handle_export(file,app_install,line,line_len,&last_line);
	}
	free(line);
	//json_object_object_add(root, "static", static_obj);
	// 关闭归档
	close_unpack_entry(&unpack);
//	close_directories_temp(config_file);
//	add_md5_to_file(filename,md5_r);
	return 0;
}

//获取安装包信息
//会单独解析config文件获取信息，建议在构造函数中调用
int appm_get_package_info(const char *filename,struct PackageConfigInfo *conf) 
{
	printf("appm_get_package_info============\n");
	struct UnpackEntry unpack;
	int ret=0;
	//读取并删除MD5信息
	uint8_t *md5_r=conf->md5;
	if((ret=del_md5_from_file(filename,md5_r,1))<0){
		printf("del_md5_from_file error%d\n",ret);
		return -1;
	}
	uint8_t md5[sizeof(conf->md5)];
	compute_md5(filename, md5);
	conf->md5_flag =1;
	for(int i=0;i<sizeof(conf->md5);i++)
	{
		if(md5_r[i]!=md5[i])
		{
			conf->md5_flag =0;
			break;
		}
	}
	// 打开归档文件
	if(open_unpack_entry(&unpack,filename)<0)
	{
		fprintf(stderr, "Error:open_unpack_entry\n");
		goto ERR_ADD_MD5;
	}
	char *config_file=open_directories_temp(TMP_FILE_PATH);		//创建临时文件保存config
	if(archive_find_entry(unpack.a,"./config",config_file)<0)
	{
		fprintf(stderr, "Error:找不到./config\n");
		goto ERR_EXIT;
	}
	extract_config_info(config_file,conf);		//从config获取应用信息

	char *last_slash = strrchr(conf->app_conf.icon, '/');  // Linux路径分隔符
	char *icon_file=open_directories_temp_file_name(TMP_CACHE_FILE_PATH,last_slash+1);		//创建临时文件保存icon
	if(archive_find_entry(unpack.a,conf->app_conf.icon,icon_file)<0)	
	{
		fprintf(stderr, "Error:找不到icon文件\n");
		//free(conf->app_conf.icon);
		//conf->app_conf.icon=NULL;
		goto ERR_EXIT;
	}
	free(conf->app_conf.icon);
	conf->app_conf.icon=icon_file;

	// 关闭归档
	close_unpack_entry(&unpack);

	//写入MD5信息（）,不删除的时候不要写入
	add_md5_to_file(filename,md5_r);
	return 0;

ERR_EXIT:
	close_unpack_entry(&unpack);
ERR_ADD_MD5:
	add_md5_to_file(filename,md5_r);	//写入MD5信息（）,不删除的时候不要写入//写入MD5信息（）,不删除的时候不要写入
	return -1;
}


int appm_free_package_info(struct PackageConfigInfo *conf)
{
	switch (conf->type)
	{
		case TYPE_PACKAGE_APP:
		case TYPE_PACKAGE_SAPP:
		    free_AppPackageConfig(&conf->app_conf);
		    break;
		case TYPE_PACKAGE_LIB:
			free_LibPackageConfig(&conf->lib_conf);
			break;
		default:
			break;
	}
	return 0;
}
