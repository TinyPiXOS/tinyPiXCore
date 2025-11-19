/*///------------------------------------------------------------------------------------------------------------------------//
		应用打包程序
说 明 : 
日 期 : 2024.8.19

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include "md5.h"	//#include <openssl/md5.h>
#include "../manage/utilslib.h"
#include "appm_creat.h"
#include "file_creat.h"
#include "json_conf.h"


//根据私钥生成签名文件
//需要生成签名的文件，私钥文件，返回的签名文件
/*
int generate_signature_file(const char *input_file,const char *private_key,  const char *signature)
{
	FILE *file_signature;
	unsigned char *data = NULL;
	size_t data_len = 0;    
	data = read_file(input_file, &data_len);
	if (!data) {
		return 1;
	}

	if((file_signature=fopen(signature,"r"))==NULL)
		return -1;

	PEM_read_PrivateKey();
}
*/

//删除换行符
static void trim_newline_(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

//归档对象，需要归档的文件目录，配置文件名字（即对单个文件打包）
//a:归档
//filepath：需要归档的文件目录
int add_file_to_archive(struct archive *a, const char *file_path, const char *entry_name) {
	struct archive_entry *entry;
	struct stat st;
	char buffer[8192];
	ssize_t len;

    // 打开文件
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return -1;
    }

    // 获取文件状态
    if (fstat(fd, &st) != 0) {
        perror("fstat error");
        close(fd);
        return -1;
    }
	printf("打包%s文件到条目%s\n", file_path,entry_name);
    // 创建新的归档条目
	entry = archive_entry_new();
	archive_entry_set_pathname(entry, entry_name);
	archive_entry_set_size(entry,st.st_size);
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0644);

    // 写入归档条目头部
    archive_write_header(a, entry);

    // 写入文件数据
    while ((len = read(fd, buffer, sizeof(buffer))) > 0) {
        archive_write_data(a, buffer, len);
    }

    // 释放资源
    archive_entry_free(entry);
    close(fd);
	return 0;
}

//判断目录以及文件合法性（最小应用程序文件应该包含的东西）
int validity_check()
{
    //数字签名（必须），里面包含必须的配置项，如允许运行的系统版本
    
    //依赖库（非必须）

    //静态资源

    //配置文件（必须），包含用户私自定义的配置项，用户随意更改
        //需要读取配置文件
        //
    //可执行文件
        //
    //
    return 0;
}


//向文件尾部写入MD5校验值
//file_path:文件路径
static int write_md5_file_end(const char* file_path)
{
	uint8_t md5_len=MD5_DIGEST_LENGTH;
	FILE *file = fopen(file_path, "a");
	if (!file) {
		perror("Unable to open file");
		return -1;
	}

	uint8_t md5[md5_len];
	if(compute_md5(file_path,md5)<0)
	{
		fclose(file);
		return -1;
	}
	if(fwrite(md5,1,md5_len,file)<md5_len)
	{
		fclose(file);
		return -1;
	}
	fclose(file);
	return 0;
}


//遍历文件夹进行打包
//path_source:打包的原始目录
//path_target:生成的安装包
int appm_ergodic_source_dopack(struct archive *a,const char *path_source,const char *path_target)
{
	DIR *dir;
//	char *path_s,*path_t;
    struct stat entry_info;
	struct dirent *dirt;

	if((dir=opendir(path_source))==NULL)
	{
		fprintf(stderr,"源文件不存在或打不开，文件：%s\n",path_source);		
		return -1;
	}

	while((dirt=readdir(dir))!=NULL)   //循环查看目录下所有文件
	{
		char path_next_s[MAX_LEN_PATH];     //下一个要处理的源路径
		char path_next_t[MAX_LEN_PATH];     //源路径对应的目标路径

        snprintf(path_next_s, sizeof(path_next_s), "%s/%s", path_source, dirt->d_name);
		
		if((strcmp(dirt->d_name,".")==0)||(strcmp(dirt->d_name,"..")==0))  //如果是..就直接跳过
			continue;	

        snprintf(path_next_t, sizeof(path_next_t), "%s/%s",path_target, dirt->d_name);	
        
        printf("打包：%s 到 %s\n",path_next_s,path_next_t);
        if (stat(path_next_s, &entry_info) == 0) 
        {
            if (S_ISDIR(entry_info.st_mode)) {
                // 如果是目录，递归进入子目录
                appm_ergodic_source_dopack(a, path_next_s, path_next_t);
            } else if (S_ISREG(entry_info.st_mode)) {
                // 如果是文件，添加到压缩包中
                add_file_to_archive(a, (const char *)path_next_s, (const char *)path_next_t);
            }
        } 
        else 
        {
            perror("stat");
        }
	}
	return 0;
}



//普通打包（使用file_creat生成原始文件夹后直接打包）
//path_s：用于生成的安装包的源文件路径和名字
//archive_name：生成的安装包路径和名字
int appm_creat_package_path(const char *path_s,const char *archive_name)
{
	struct archive *a = archive_write_new();
	archive_write_set_format_pax_restricted(a);  // 设置为PAX格式

	// 打开归档文件
	if (archive_write_open_filename(a, archive_name) != ARCHIVE_OK) {
		printf("Could not open archive file: %s\n", archive_name);
		return -1;
	}

	//增加计算文件大小的功能

	appm_ergodic_source_dopack(a,path_s,".");

    // 关闭归档文件
    archive_write_close(a);
    archive_write_free(a);
    printf("Package %s created successfully.\n", archive_name);

	//写入MD5校验
	write_md5_file_end(archive_name);

	return 0;
}

//lib打包(根据config结构体直接打包，不生成原始文件夹)
//archive_name:生成的包的路径和名字
int appm_creat_libpackage_config(const char *archive_name,struct LibPackageConfig *conf)
{
	struct archive *a = archive_write_new();
	int ret=0;
	archive_write_set_format_pax_restricted(a);  // 设置为PAX格式

	// 打开归档文件
	if (archive_write_open_filename(a, archive_name) != ARCHIVE_OK) {
		printf("Could not open archive file: %s\n", archive_name);
		return -1;
	}

	char *config_file=open_directories_temp("/tmp");
	//printf("生成%s\n", config_file);
	if((ret=file_config_creat_lib(a,config_file,conf))==0)				//生成config文件并打包库文件
	{
		add_file_to_archive(a, config_file, "./config");		//打包config
	}
	
	close_directories_temp(config_file);
	
    // 关闭归档文件
	archive_write_close(a);
	archive_write_free(a);

	//写入MD5校验
	if(ret==0)
		write_md5_file_end(archive_name);
	return ret;
}

//根据config结构体直接打包成安装包(不生成中间文件)
int appm_creat_apppackage_config(const char *archive_name,struct AppPackageConfig *conf)
{

	return 0;
}


//解析用于打包的json文件，用于无配置界面通过json脚本直接打包
//json_path:json文件路径和名字
//conf:应用配置参数
//script:启动脚本参数
int appm_analysis_dopack_json(const char *json_path,struct AppPackageConfig *conf,struct ScriptInfo *script)
{
	printf("解析%s文件\n",json_path);
	printf("get AppPackageConfig\n");
	json_conf_get_package_config(json_path, conf);
	printf("get ScriptInfo\n");
	json_conf_get_startup(json_path, script);

	return 0;
}



