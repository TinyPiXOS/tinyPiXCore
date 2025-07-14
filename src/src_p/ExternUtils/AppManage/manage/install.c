/*///------------------------------------------------------------------------------------------------------------------------//
		应用安装程序
说 明 :
日 期 : 2024.8.26

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include "fileUtils.h"
#include "install.h"
#include "unpack.h"
#include "conf_json.h"
#include "utilslib.h"
#include "purview.h"
#include "../inc/appmanage_conf.h"

uint8_t buf_temp[1024];

const char *InstallCreatFile[] = {
	APP_INSTALL_RES,
	APP_INSTALL_BIN,
	APP_INSTALL_CONFIG,
	APP_INSTALL_DATA,
	APP_INSTALL_TEMP,
	NULL};

const char *InstallMustFile[][3] = {
	{"./assert", "./res/"},
	{"./start.sh", "./"},
	{"./bin", "./"},
	//...
	{NULL, NULL}};

//【计划】
struct tpAppInfo *appm_install_app_info_create()
{
	return NULL;
}

//【计划】
int appm_install_app_info_delete(struct tpAppInfo *app)
{

	if(app->config_file)
	{
		close_directories_temp(app->config_file);
	}


}

// 写入安装进度
static int write_install_schedule(struct InstallSchedule *sch_lock, uint8_t schedule)
{
	pthread_rwlock_wrlock(&sch_lock->rwlock);
	sch_lock->schedule = schedule;
	pthread_rwlock_unlock(&sch_lock->rwlock);
	return 0;
}

// 读取安装进度
static int read_install_schedule(struct InstallSchedule *sch_lock)
{
	uint8_t schedule;
	pthread_rwlock_rdlock(&sch_lock->rwlock);
	schedule = sch_lock->schedule;
	pthread_rwlock_unlock(&sch_lock->rwlock);
	return schedule;
}

// 创建安装进度结构
struct InstallSchedule *creat_install_schedule()
{
	struct InstallSchedule *sch = (struct InstallSchedule *)malloc(sizeof(struct InstallSchedule));
	if (!sch)
		return NULL;
//	pthread_rwlock_init(&sch->rwlock, NULL);
	pthread_rwlockattr_t rwlock_attr;
	pthread_rwlockattr_init(&rwlock_attr);								 // 初始化属性对象
	pthread_rwlockattr_setpshared(&rwlock_attr, PTHREAD_PROCESS_SHARED); // 设置进程间共享属性
	pthread_rwlock_init(&sch->rwlock, &rwlock_attr);
	pthread_rwlockattr_destroy(&rwlock_attr);


	return sch;
}

int delete_install_schedule(struct InstallSchedule *sch)
{
	if (!sch)
		return -1;
	pthread_rwlock_destroy(&sch->rwlock);
}

// 绝对路径拼接,传入应用的相对路径和uuid
static int montage_absolute_path(char *path_a, char *path_r, size_t size_a, char *uuid)
{
	snprintf(path_a, size_a, "%s/%s/%s", APP_INSTALL_PATH, uuid, path_r);
	return 0;
}

// 拼接配置文件的配置项字符串
static int montage_config_key_value(char *key_prefix, int size, uint8_t type, const char *key, const char *value)
{
	memset(key_prefix, 0, size);
	switch (type)
	{
	case CONFIG_KEYTYPE_TERM:
		snprintf(key_prefix, size, "%s:%s\n", key, value);
		break;
	case CONFIG_KEYTYPE_PATH:
		snprintf(key_prefix, size, "export %s=%s\n", key, value);
		break;
	}
	return 0;
}

// 从安装包的路径或名字中去掉后缀提取名字
static int get_pikname_from_path(const char *path_pik, char *pikname, int len)
{
	const char *pik = strrchr(path_pik, '/');
    const char *filename_start; // 指向文件名起始位置

    if (pik == NULL) {
        // 没有斜杠，整个路径就是文件名
        filename_start = path_pik;
    } else {
        // 有斜杠，文件名在斜杠后
        filename_start = pik + 1;
    }

	snprintf(pikname, len, "%s", filename_start);
	char *dest = strstr(pikname, PACKAGE_FILE_SUFFIX);
	if (dest == NULL) // 目录不对，文件后缀名不对
		return -1;
	dest[0] = '\0';
	return 0;
}

// 修改可执行文件的链接路径
static void modify_rpath(const char *filename, const char *new_rpath)
{
	int fd = open(filename, O_RDWR);
	if (fd < 0)
	{
		perror("open");
		return;
	}

	struct stat st;
	if (fstat(fd, &st) < 0)
	{
		perror("fstat");
		close(fd);
		return;
	}

	// 映射 ELF 文件到内存
	void *mapped = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mapped == MAP_FAILED)
	{
		perror("mmap");
		close(fd);
		return;
	}

	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)mapped;
	Elf64_Phdr *phdr = (Elf64_Phdr *)(mapped + ehdr->e_phoff);

	// 查找动态段
	Elf64_Dyn *dynamic = NULL;
	for (int i = 0; i < ehdr->e_phnum; i++)
	{
		if (phdr[i].p_type == PT_DYNAMIC)
		{
			dynamic = (Elf64_Dyn *)(mapped + phdr[i].p_offset);
			break;
		}
	}

	if (!dynamic)
	{
		fprintf(stderr, "No dynamic section found\n");
		munmap(mapped, st.st_size);
		close(fd);
		return;
	}

	// 查找 RPATH 或 RUNPATH
	Elf64_Off rpath_offset = 0;
	char *strtab = NULL;
	for (Elf64_Dyn *dyn = dynamic; dyn->d_tag != DT_NULL; dyn++)
	{
		if (dyn->d_tag == DT_STRTAB)
		{
			strtab = (char *)(mapped + dyn->d_un.d_ptr);
		}
		if (dyn->d_tag == DT_RPATH || dyn->d_tag == DT_RUNPATH)
		{
			rpath_offset = dyn->d_un.d_val;
			break;
		}
	}

	if (!rpath_offset || !strtab)
	{
		fprintf(stderr, "No RPATH or RUNPATH found\n");
		munmap(mapped, st.st_size);
		close(fd);
		return;
	}

	// 打印当前的 RPATH
	printf("Old RPATH: %s\n", strtab + rpath_offset);

	// 检查新的 RPATH 是否比旧的长，不能超过原来预留的空间
	size_t old_rpath_len = strlen(strtab + rpath_offset);
	size_t new_rpath_len = strlen(new_rpath);

	if (new_rpath_len > old_rpath_len)
	{
		fprintf(stderr, "New RPATH is too long!\n");
		munmap(mapped, st.st_size);
		close(fd);
		return;
	}

	// 用新的 RPATH 覆盖旧的 RPATH
	memcpy(strtab + rpath_offset, new_rpath, new_rpath_len);
	memset(strtab + rpath_offset + new_rpath_len, '\0', old_rpath_len - new_rpath_len); // 填充空字符

	printf("New RPATH: %s\n", strtab + rpath_offset);

	// 释放资源
	munmap(mapped, st.st_size);
	close(fd);
}

// 向应用内的配置文件添加信息
// mode:写入配置项/写入路径
// key：配置项的名字
// value：值
int write_config_file(const char *config, char *uuid, uint8_t type, const char *key, const char *value)
{
	char path_conf[128]; // config目录
	char key_prefix[128];

	// 拼接UUID目录
	memset(path_conf, 0, sizeof(path_conf));
	strcat(path_conf, APP_INSTALL_PATH);
	strcat(path_conf, "/");
	strcat(path_conf, uuid);
	strcat(path_conf, config);
	int fd = open(path_conf, O_RDWR | O_APPEND | O_CREAT, 0644);
	if (fd < 0)
	{
		perror("open configure");
		printf("path:%s\n", path_conf);
		return -1;
	}
	// 拼接完整的前缀的key，例如：uuid:, export lib=
	montage_config_key_value(key_prefix, sizeof(key_prefix), type, key, value);
	write(fd, key_prefix, strlen(key_prefix));

	close(fd);
	return 0;
}

// 安装文件,源文件可能是条目也可以是文件
int install_file(const char *path_s, const char *path_d)
{
}

// 根据安装包配置文件信息生成新的配置文件和json文件到安装目录,并且解析配置文件中的关键信息
// config_s:原始配置文件
int install_config_file(const struct tpAppInfo *app)
{
	int err=0;
	struct json_object *root = json_object_new_object();
	char path_conf[PATH_MAX_LENGTH]; // config目录

	// 拼接UUID目录
	//	memset(path_conf,0,sizeof(path_conf));
	snprintf(path_conf, sizeof(path_conf), "%s/%s/%s", APP_INSTALL_PATH, app->uuid, APP_CONFIG);
	FILE *file_d = fopen(path_conf, "w");
	if (!file_d)
	{
		fprintf(stderr,"fopen file %s error\n",path_conf);
		err = -1;
		goto quit;
	}

	// 拼接安装包config目录
	//	memset(path_conf,0,sizeof(path_conf));
	snprintf(path_conf, sizeof(path_conf), "%s/%s/%s/config", APP_INSTALL_PATH, APP_TEMP, app->pikname);
	FILE *file_s = fopen(app->config_file, "r");
	if (!file_s)
	{
		fprintf(stderr,"fopen file %s error\n",path_conf);
		err = -1;
		goto quit;
	}

	// 拼接json文件目录
	//	memset(path_conf,0,sizeof(path_conf));
	/*snprintf(path_conf, sizeof(path_conf), "%s/%s.json", APP_JSON_PATH, app->uuid);
	FILE *file_j = fopen(path_conf, "w");
	if (!file_j)
	{
		perror("fopen json");
		printf("path:%s\n", path_conf);
		fclose(file_d);
		fclose(file_s);
		return -1;
	}*/
	printf("===安装配置和json文件\n");

	struct json_object *static_obj = json_object_new_object();
	char line[CONFIG_MAX_LENGTH];
	// 逐行读取安装包配置文件信息
	while (fgets(line, CONFIG_MAX_LENGTH, file_s))
	{
		if (line[0] == '#') // 跳过注释行
			continue;
		// 移除换行符
		trim_newline(line);
		// export和update开头的不拷贝
		int ret = strncmp(line, "export ", 7);
		if (ret == 0)
		{
			fputs((const char *)line, file_d);
			fputs("\n", file_d);
			config_export_analysis_json(line, static_obj);
			continue;
		}
		ret = strncmp(line, "update ", 7);
		if (ret == 0)
		{
			continue;
		}
		fputs((const char *)line, file_d);
		fputs("\n", file_d);
		config_keyvalue_analysis_json(line, static_obj);
		// fputs((const char *)line, file_d);
	}
	json_object_object_add(root, "static", static_obj);

	snprintf(path_conf, sizeof(path_conf), "%s/%s.json", APP_JSON_PATH, app->uuid);
	write_json_object_file(root,path_conf);	//不加密写入json
//	write_json_object_file_encryption(root,path_conf);	//加密写入json

quit:
	json_object_put(root);
	if(file_s)
		fclose(file_s);
	if(file_d)
		fclose(file_d);
	return err;
}

// 删除安装路径(用于安装过程中出错的删除)
static int install_remove_appfile(const char *uuid)
{
	printf("[debug]:安装过程出错，删除安装文件\n");
	char path[PATH_MAX_LENGTH];
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), APP_INSTALL_PATH "/%s", uuid);
	remove_dir(path);
	return 0;
}

// 创建应用的安装目录和必要目录
int install_creat_path(char *uuid)
{
	char path[PATH_MAX_LENGTH];
	int err;
	// uuid(本应用安装的根目录),/System/app/<uuid>/
	snprintf(path, sizeof(path), APP_INSTALL_PATH "/%s", uuid);
	if (mkdir(path, 0777) == -1)
	{
		fprintf(stderr,"创建应用根目录失败：%s\n",path);
		return -1;
	}

	// 创建文件，如果需要增删修改InstallCreatFile即可
	for (int i = 0;; i++)
	{
		if (InstallCreatFile[i] == NULL)
			break;
		snprintf(path, sizeof(path), APP_INSTALL_PATH "/%s/%s", uuid, InstallCreatFile[i]);
		if (mkdir(path, 0777) == -1)
		{
			fprintf(stderr,"创建应用目录失败：%s\n",path);
			goto remove;
		}
	}
	return 0;
remove:
	install_remove_appfile(uuid);
	return -1;
}

// 解包的形式安装文件到目标位置
int install_must_file_extract(struct AppInstallInfo *app, const char *file_s, const char *file_d)
{
	return install_file_extract(app, ".", file_s, file_d);
}
// path:相对应用安装目录的路径
// fild_d:通常都是./
int install_file_extract(struct AppInstallInfo *app_install, const char *path, const char *file_s, const char *file_d)
{
	int ret = 0;
	struct tpAppInfo *app = app_install->app;
	char *path_d = (char *)malloc(strlen(file_d) + 100);
	if (path_d == NULL)
		return -1;

	char *install_path;
	if (app->type == TYPE_PACKAGE_SAPP)
		install_path = APPS_INSTALL_PATH;
	else
		install_path = APP_INSTALL_PATH;

	sprintf(path_d, "%s/%s/%s/%s", install_path, app->uuid, path, file_d);

	if (app_install->a != NULL)
		ret = extract_from_archive(app_install->a, file_s, path_d); // 已经创建好归档最直接解包
	else
		ret = extract_archive_file(app->path_pik, file_s, path_d); // 先创建归档再解包
	return ret;
}

// 复制的形式安装文件到目标位置
int install_must_file_copy(struct AppInstallInfo *app, const char *file_s, const char *file_d)
{
	return install_file_copy(app, ".", file_s, file_d);
}
// path:相对应用安装目录的路径
int install_file_copy(struct AppInstallInfo *app_install, const char *path, const char *file_s, const char *file_d)
{
	int ret = 0;
	struct tpAppInfo *app = app_install->app;
	size_t len_s = strlen(file_s) + MAX_LEN_APP_NAME + 100;
	char *path_s = (char *)malloc(len_s);
	if (path_s == NULL)
		return -1;
	char *path_d = (char *)malloc(strlen(file_d) + strlen(path) + 100);
	if (path_d == NULL)
		return -1;

	char *install_path;
	if (app->type == TYPE_PACKAGE_SAPP)
		install_path = APPS_INSTALL_PATH;
	else
		install_path = APP_INSTALL_PATH;

	snprintf(path_s, len_s, "%s/%s/%s/%s", install_path, APP_TEMP, app->pikname, file_s);
	sprintf(path_d, "%s/%s/%s/%s", install_path, app->uuid, path, file_d);

	ret = system_copy_file(path_s, path_d);
	free(path_s);
	free(path_d);
	return ret;
}

// 必要文件拷贝到指定位置
// cp -r /System/app/<UUID>/temp/<path> /System/app/<UUID>/
int install_must_file(struct AppInstallInfo *app)
{
	int ret;
	// 拷贝通用文件（改为在.h中配置）
	char line[CONFIG_MAX_LENGTH];
	for (int i = 0; InstallMustFile[i][0] != NULL; i++)
	{
		const char *str1 = InstallMustFile[i][0];
		const char *str2 = InstallMustFile[i][1];
		printf("安装%s\n", str1);
		if (app->install_must(app, str1, str2) < 0)
			ret = -1;
	}
	// 权限管理，需要修改
	/*snprintf(command,PATH_MAX_LENGTH*2,"chmod 755 %s/%s/start.sh",APP_INSTALL_PATH,uuid);
	ret = system(command);

	snprintf(command,PATH_MAX_LENGTH*2,"chmod 755 %s/%s/%s",APP_INSTALL_PATH,uuid,execname);
	ret = system(command);*/
	return ret;
}

static int install_icon_file(struct AppInstallInfo *app)
{
	app->install_must(app,app->app->icon,app->app->icon);
}


// 安装的配置文件中export部分需要做的处理判断
// 返回0不需要复制，返回1需要复制到根目录，返回2复制到资源目录
AppInstallPathType install_config_export_type(char *line)
{
	char *flag = strstr(line, "depend="); // 系统依赖库，需要跳过不复制
	if (flag != NULL)
		return INSTALL_PATH_TYPE_NONE;
	flag = strstr(line, "export start="); // 启动脚本跳过
	if (flag != NULL)
		return INSTALL_PATH_TYPE_NONE;
	flag = strstr(line, "export remove=");
	if (flag != NULL)
		return INSTALL_PATH_TYPE_ROOT;
	flag = strstr(line, "export lib=");
	if (flag != NULL)
		return INSTALL_PATH_TYPE_ROOT;
	flag = strstr(line, "export appexec");
	if (flag != NULL)
		return INSTALL_PATH_TYPE_ROOT;
	flag = strstr(line, "export icon=");
	if (flag != NULL)
		return INSTALL_PATH_TYPE_ROOT;
	flag = strstr(line, "export userfile=");
	if (flag != NULL)
		return INSTALL_PATH_TYPE_RES;
	flag = strstr(line, "export bin=");
	if (flag != NULL)
		return INSTALL_PATH_TYPE_BIN;
	return 2;
}

// 安装的拷贝，所有需要安装的文件必须在安装包的config文件中声明
int install_other_copy_file(struct AppInstallInfo *app_install)
{
	int ret;
	struct tpAppInfo *app = app_install->app;
	char path_conf[128]; // config目录
	char *install_path;
	if (app == NULL)
		return -1;
	if (app->type == TYPE_PACKAGE_SAPP)
		install_path = APPS_INSTALL_PATH;
	else
		install_path = APP_INSTALL_PATH;
	// 拼接安装包config目录
	memset(path_conf, 0, sizeof(path_conf));
	snprintf(path_conf, sizeof(path_conf), "%s/%s/%s/config", install_path, APP_TEMP, app->pikname);
	FILE *file = fopen(path_conf, "r");
	if (!file)
	{
		perror("fopen s_config");
		fprintf(stderr,"Error:找不到保存的临时配置文件，%s\n",path_conf);
		return -1;
	}
	fseek(file, 0, SEEK_SET);
	app_install->app->config_file=strdup(path_conf);

	char line[CONFIG_MAX_LENGTH];
	// 逐行读取安装包中配置文件
	while (fgets(line, CONFIG_MAX_LENGTH, file))
	{
		if (line[0] == '#') // 跳过注释行
			continue;
		// 移除换行符
		trim_newline(line);
		char *flag = strstr(line, "export "); // 只复制export行说明的文件
		if (flag == NULL)
			continue;
		AppInstallPathType inst_type = install_config_export_type(line);
		flag = strstr(line, "=");
		if (flag == NULL)
			continue;
		char *path_str = flag + 1;
		switch (inst_type)
		{
		case INSTALL_PATH_TYPE_ROOT:
			app_install->install_other(app_install, ".", path_str, path_str); // install_file_extract
			break;
		case INSTALL_PATH_TYPE_RES:
			app_install->install_other(app_install, APP_INSTALL_RES, path_str, path_str); // install_file_extract
			break;
		case INSTALL_PATH_TYPE_BIN:
			app_install->install_other(app_install, ".", path_str, path_str); // install_file_extract
			break;
		default:
			continue;
		}
	}
	fclose(file);
	return 0;
}

// 安装其他文件(通过解析export文件)
int install_other_file(struct AppInstallInfo *app)
{
	install_other_copy_file(app);
	return 0;
}

// 删除安装临时目录
// name:解压后文件的名字
int Appm_Remove_Package(char *name)
{
	char command[PATH_MAX_LENGTH];
	snprintf(command, sizeof(command), "%s/%s/%s", APP_INSTALL_PATH, APP_TEMP, name);
	remove_dir(command);
	//	printf("rm -rf %s/%s\n",APP_INSTALL_PATH, APP_TEMP);
	system(command);
	return 0;
}

// 安装解压好的包
int Appm_Install_Unpack(struct tpAppInfo *app, struct PackageUserParam *user)
{
	struct AppInstallInfo app_install;
	struct InstallSchedule *schedule = user->schedule;

	app_install.app = app;
	app_install.a = NULL;
	// app.path_file =
	app_install.install_must = install_must_file_copy;
	app_install.install_other = install_file_copy;

	write_install_schedule(schedule, 60);
	// 安装普通文件()
	install_must_file(&app_install);
	write_install_schedule(schedule, 80);
	// 安装其他文件
	install_other_file(&app_install);
	write_install_schedule(schedule, 90);
	// 安装配置文件和json文件
	install_config_file(app);
	// 其他流程
	return 0;
}
// 从未解压的文件直接安装
int Appm_Install_Archive(struct tpAppInfo *app, struct PackageUserParam *user)
{
	struct AppInstallInfo app_install;
	struct InstallSchedule *schedule = user->schedule;
	printf("[debug]:开始安装文件\n");
	app_install.app = app;
	app_install.a = NULL;
	// app.path_file =
	app_install.install_must = install_must_file_extract;
	// app_install.install_other= install_file_copy;
	app_install.install_other = install_file_extract;

	//提取icon以供显示图标
//	install_icon_file(&app_install);

	write_install_schedule(schedule, 60);
	// 安装普通文件()
	install_must_file(&app_install);
	write_install_schedule(schedule, 80);
	// 安装其他文件
	// struct json_object *root=json_object_new_object();
	extract_archive_package_config(&app_install, NULL);
	// printf("%s\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
	// json_object_put(root);
	write_install_schedule(schedule, 90);
	// 安装配置文件和json文件
	printf("[debug]\n");
	install_config_file(app);

	// 其他流程
	return 0;
}

// 获取安装进度
int appm_install_get_schedule(struct PackageUserParam *user)
{
	return read_install_schedule(user->schedule);
}

// 直接安装app
// 分为系统app和用户app
int appm_install_pik(const char *path_pik, TypePackage type, struct AppPackageConfig *conf, struct PackageUserParam *user)
{
	tpAppID uuid;
	struct tpAppInfo app;
	struct InstallSchedule *schedule = user->schedule;
	

	memcpy(uuid.value, conf->app_id, sizeof(uuid.value));
	app.path_pik = path_pik;
	app.type = type;
	app.uuid = conf->app_id;
	app.icon = conf->icon;

	write_install_schedule(schedule, 0);

	// 应用未安装，需要新建文件目录
	if (conf->install_flag != 1)
	{
		printf("应用未安装\n");
		if (install_creat_path(app.uuid) < 0)
			return -1;
	}
	// 获取应用名
	if ((app.pikname = (char *)malloc(strlen(path_pik))) == NULL)
	{
		fprintf(stderr,"Error:获取应用安装包名失败\n");
		install_remove_appfile(app.uuid);
		return -1;
	}
	if (get_pikname_from_path(path_pik, app.pikname, strlen(path_pik)) < 0)
	{
		fprintf(stderr,"Error:获取应用安装包名(无后缀)失败，%s\n",path_pik);
		install_remove_appfile(app.uuid);
		free(app.pikname);
		return -1;
	}
	// 解包到/System/app/temp
	/*if(Appm_Unpack(path_pik,TYPE_PACKAGE_APP)<0){
		install_remove_appfile(app.uuid);
		free(app.pikname);
		return -1;
	}*/
	write_install_schedule(schedule, 50);
	// 安装
	Appm_Install_Archive(&app, user);
	// Appm_Install_Unpack(&app,user);
	// 移除安装包
	//	Appm_Remove_Package(pik_name);
	if (conf->install_flag == 0)
	{
		// add_appuuid_install_safe(uuid,APP_INSTALL_CONF_PATH);		//写入install.conf
		printf("应用未安装，设置用户权限\n");
		Appm_Install_Purview(uuid, type);
	}
	write_install_schedule(schedule, 100);
	free(app.pikname);

	close_directories_temp(app.config_file);
	return 0;
}

// 库的安装就是解包的过程
int Appm_Install_Library(const char *path_lib, struct LibPackageConfig *conf, struct PackageUserParam *user)
{
	char *lib_path = (char *)malloc(PATH_MAX_LENGTH);

	for (int i = 0; i < conf->lib_count; i++)
	{
		snprintf(lib_path, PATH_MAX_LENGTH, LIBS_INSTALL_PATH "/%s", conf->system_lib[i]);
		extract_file_pack(path_lib, conf->system_lib[i], lib_path);
	}
	return 0;
}

// 安装
int appm_install_package(const char *path_pack, struct PackageConfigInfo *conf, struct PackageUserParam *user)
{
	//	printf("package type: %d\n",conf->type);
	if (conf->md5_flag != 1)
	{
		fprintf(stderr, "Install package damage\n");
		return -1;
	}
	if (mkdir_path(TMP_FILE_PATH, 0755) < 0) // 创建/System/tmp
		return -1;

	printf("安装类型%d\n", conf->type);
	switch (conf->type)
	{
	case TYPE_PACKAGE_APP:
		appm_install_pik(path_pack, conf->type, &(conf->app_conf), user);
		break;
	case TYPE_PACKAGE_SAPP:
		appm_install_pik(path_pack, conf->type, &(conf->app_conf), user);
		break;
	case TYPE_PACKAGE_LIB:
		Appm_Install_Library(path_pack, &(conf->lib_conf), user);
		break;
	default:
		appm_install_pik(path_pack, TYPE_PACKAGE_APP, &(conf->app_conf), user); // 未知类型按照普通APP安装
		break;
	}
	printf("[debug]appm_install_package\n");
	return 0;
}



const char *appm_install_get_icon(struct PackageConfigInfo *conf)
{
	return conf->app_conf.icon;
}

