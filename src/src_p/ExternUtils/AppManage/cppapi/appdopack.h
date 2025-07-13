#ifndef _APP_DOPACK_H
#define _APP_DOPACK_H

#include <string>
#include <stdint.h>
#include <vector>
#include "file_creat.h"

#ifdef __cplusplus
extern "C" {                //  告诉编译器下列代码要以C链接约定的模式进行链接
#endif

int appm_generate_package_source(struct AppPackageConfig *config,char *path,TypePackage type);
int appm_generate_startup_script(struct ScriptInfo *config, const char *output_file);
int appm_creat_package_path(const char *path_s,const char *archive_name);
int appm_creat_libpackage_config(const char *archive_name,struct LibPackageConfig *conf);
int appm_creat_apppackage_config(const char *archive_name,struct AppPackageConfig *conf);

#ifdef __cplusplus
}
#endif

#define tpString std::string

class tpPackageInfo {
public:
	virtual ~tpPackageInfo();
	tpPackageInfo();
    
public:
	int SetPackageType(int type);
	void SetAppID(const tpString& id);
	void SetAppName(const tpString& name);
	void SetVersion(uint8_t x,uint8_t y,uint8_t z);
	void SetArchitecture(const tpString& architecture);
	void SetSection(const tpString& section);
	void SetPriority(const tpString& priority);
	void SetEssential(const tpString& essential);
	void SetAuthor(const tpString& author);
	void SetContact(const tpString& contact);
	void SetProvides(const tpString& provides);
	void SetDiskSpace(int size);
	int SetDescription(const tpString& description);
	int SetSignature(const tpString& sig);
	int AddDepend(const tpString& depend);
	int AddLib(const tpString& lib);
	int SetIcon(const tpString& icon);
	int AddAssert(const tpString& assert);
	int AddFile(const tpString& file);
	int AddExtension(const tpString& type);
	int SetStart(const tpString& start);
	int SetRemove(const tpString& remove);
	int SetAppPath(const tpString& app);
//	int SetMyfile(const tpString& myfile);
	int Save(const tpString& file);
	int CreatPackage(const tpString& package);

	void ClassFree();

private:
    tpString path_s;		//原始打包文件生成位置
    struct AppPackageConfig params;
	TypePackage type;
};


class tpStartShInfo{
public:
	tpStartShInfo();
	virtual ~tpStartShInfo();

public:
    int AddEnvironmentVar(const tpString& key, const tpString& value) ;
    int AddDependency(const tpString& lib);
    int AddStartArg(const tpString& arg);
    int SetLogFile(const tpString& log_file);
    int SetConfigFile(const tpString& config_file);
    int SetExecPath(const tpString& name);

	int Save(const tpString& path);
    void ClassFree();
private:
    struct ScriptInfo config;
};


class tpLibPackageInfo{
public:
	tpLibPackageInfo();
	virtual ~tpLibPackageInfo();

public:
	void SetArchitecture(const tpString& architecture);
	void SetDiskSpace(int size);
    int AddLibrary(const tpString& name,uint8_t ver_x,uint8_t ver_y,uint8_t ver_z);
	int AddFile(const tpString& file);
	int Save(const tpString& path);
	void ClassFree();
private:
	struct LibPackageConfig params;
};

#endif // CONFIGURATOR_H


