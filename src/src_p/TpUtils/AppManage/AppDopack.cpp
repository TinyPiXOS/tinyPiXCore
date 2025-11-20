/*///------------------------------------------------------------------------------------------------------------------------//
		应用安装报文件生成的c++接口
说 明 : 
日 期 : 2024.9.4

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include "AppDopack.h"
#include "AppmCreat.h"


//释放
void loop_free(void **data,int count)
{
	for(int i=0;i<count;i++)
	{
		if(data[i])	free(data[i]);
	}
}

//安装包信息以及生成安装包所需资源



TpPackageInfo::TpPackageInfo() {
//	std::strncpy(params.app_id, appId.c_str(), sizeof(params.app_id) - 1);
//	std::strncpy(params.app_name, appName.c_str(), sizeof(params.app_name) - 1);
//	std::strncpy(params.version, version.c_str(), sizeof(params.version) - 1);

	type=TYPE_PACKAGE_NONE;
	params.description=NULL;
	// params.icon=NULL;
	// params.appexec_name=NULL;
	// params.signature=NULL;
	params.diskspace=0;
	params.version.x=0;
	params.version.y=0;
	params.version.z=0;

	// Initialize other parameters with empty strings
/*	std::memset(params.app_id, 0, sizeof(params.app_id));
	std::memset(params.app_name, 0, sizeof(params.app_name));
//	std::memset(params.version, 0, sizeof(params.version));
	std::memset(params.architecture, 0, sizeof(params.architecture));
	std::memset(params.section, 0, sizeof(params.section));
	std::memset(params.priority, 0, sizeof(params.priority));
	std::memset(params.essential, 0, sizeof(params.essential));
	std::memset(params.author, 0, sizeof(params.author));
	std::memset(params.provides, 0, sizeof(params.provides));
//	std::memset(params.description, 0, sizeof(params.description));
	std::memset(params.depend, 0, sizeof(params.depend));
	std::memset(params.lib, 0, sizeof(params.lib));
	std::memset(params.assert, 0, sizeof(params.assert));
//	std::memset(params.icon, 0, sizeof(params.icon));
//	std::memset(params.start, 0, sizeof(params.start));
//	std::memset(params.remove, 0, sizeof(params.remove));
	std::memset(params.otherfile, 0, sizeof(params.otherfile));
	std::memset(params.file_extension, 0, sizeof(params.file_extension));*/

/*	params.otherfile_count = 0;
	params.lib_count = 0;
	params.depend_count = 0;
	params.assert_count = 0;
	params.extension_count = 0;*/
}

TpPackageInfo::~TpPackageInfo() {
	ClassFree();
}

//安装包类型
int TpPackageInfo::SetPackageType(int pack_type) {
	switch(pack_type) {
		case TYPE_PACKAGE_APP:
		case TYPE_PACKAGE_SAPP:
			type=(TypePackage)pack_type;
		default:
			return -1;
	}
	return 0;
}

//UUID/APPID
void TpPackageInfo::SetAppID(const TpString& id){
	params.appID=id;
}
//APP NAME
void TpPackageInfo::SetAppName(const TpString& name){
	params.appName=name;
}
//版本
void TpPackageInfo::SetVersion(uint8_t x,uint8_t y,uint8_t z){
//	std::strncpy(params.version, version.c_str(), sizeof(params.version) - 1);
	params.version.x=x;
	params.version.y=y;
	params.version.z=z;
}
//硬件平台
void TpPackageInfo::SetArchitecture(const TpString& architecture) {
	params.architecture=architecture;
}

void TpPackageInfo::SetSection(const TpString& section) {
	params.section=section;
}

void TpPackageInfo::SetPriority(const TpString& priority) {
	params.priority=priority;
}

void TpPackageInfo::SetEssential(const TpString& essential) {
	params.essential=essential;
}
//作者信息，Name
void TpPackageInfo::SetAuthor(const TpString& author) {
	params.author=author;
}
//作者联系方式,email
void TpPackageInfo::SetContact(const TpString& contact) {
	params.contact=contact;
}
//组织，公司
void TpPackageInfo::SetProvides(const TpString& provides) {
	params.provides=provides;
}
//安装所需空间
void TpPackageInfo::SetDiskSpace(int size){
	params.diskspace = size;
}
//应用描述
int TpPackageInfo::SetDescription(const TpString& description) 
{
	params.description=description;
	return 0;
}

//数字签名
int TpPackageInfo::SetSignature(const TpString& signature) 
{
	params.signature=signature;
    return 0;
}
//开源库：传入格式:libname@version
int TpPackageInfo::AddDepend(const TpString& depend) 
{
	params.depend.emplace_back(depend);
	return 0;
}
//私有库:传入路径
int TpPackageInfo::AddLib(const TpString& lib) 
{
/*	if(params.lib_count==MAX_ITEMS)
		return -1;
	if((params.lib[params.lib_count] = (char *)malloc(lib.size() + 1))==NULL)
	    return -1;
	std::strcpy(params.lib[params.lib_count], lib.c_str());
	params.lib_count++;*/
	params.lib.emplace_back(lib);
	return 0;
}
//图标
int TpPackageInfo::SetIcon(const TpString& icon) 
{
	params.icon=icon;
	return 0;
}
//可执行文件路径
int TpPackageInfo::SetAppPath(const TpString& app) 
{
	params.appexecName=app;
	return 0;
}
//静态文件
int TpPackageInfo::AddAssert(const TpString& assert)
{
	params.assertFiles.emplace_back(assert);
	return 0;
}
//其他文件
int TpPackageInfo::AddFile(const TpString& file)
{
	params.otherFiles.emplace_back(file);
	return 0;
}
//支持的文件后缀
int TpPackageInfo::AddExtension(const TpString& type)
{
	params.fileExtension.emplace_back(type);
	return 0;
}

/*
void Configurator::setStart(const std::string& start) {
    std::strncpy(params.start, start.c_str(), sizeof(params.start) - 1);
}
*/
/*
void Configurator::setRemove(const std::string& remove) {
    std::strncpy(params.remove, remove.c_str(), sizeof(params.remove) - 1);
}
*/


int TpPackageInfo::Save(const TpString& path) 
{
	if(TpFileCreat::appm_generate_package_source(&params,path,type)<0){
		fprintf(stderr,"[Error]: Creat error");
		return -1;
	}
	return 0;
}

int TpPackageInfo::CreatPackage(const TpString& package)
{
	char *pack_c=(char*)malloc(package.size() +1);
	char *path_c=(char*)malloc(path_s.size() +1);
	std::strcpy(pack_c, package.c_str());
	std::strcpy(path_c, path_s.c_str());
	appm_creat_package_path(path_c,pack_c);
	free(path_c);
	free(pack_c);
}

void TpPackageInfo::ClassFree() 
{
	;
}


//启动文件部分==============================

TpStartShInfo::TpStartShInfo()
{
	;
}

TpStartShInfo::~TpStartShInfo()
{
	ClassFree();
}

// 添加环境变量
int TpStartShInfo::AddEnvironmentVar(const TpString& key, const TpString& value) 
{
	config.env_type.emplace_back(key);
	config.env_vars.emplace_back(value);
	return 0;
}

// 添加依赖库（一般是系统通用的库）
//库名字
int TpStartShInfo::AddDependency(const TpString& lib) 
{
	config.dependencies.emplace_back(lib);
	return 0;
}

// 添加启动参数
int TpStartShInfo::AddStartArg(const TpString& arg)
{
	config.args.emplace_back(arg);
	return 0;
}

//添加可执行文件名称
int TpStartShInfo::SetExecPath(const TpString& name)
{
	config.exec_path=name;
	return 0;
}

int TpStartShInfo::Save(const TpString& path) {

	char *path_c=(char*)malloc(path.size() +1);
	std::strcpy(path_c, path.c_str());
	printf("cvreat:%s\n", path_c);
	//std::cout << "creat:" << addr << std::endl;
	TpFileCreat::appmGenerateStartupScript(&config, path_c);
	free(path_c);
	return 0;
}


void TpStartShInfo::ClassFree()
{
	/*if(config.arg_count > 0){
		loop_free((void**)config.args, config.arg_count);
	}
	config.arg_count=0;

	if(config.dep_count > 0){
		loop_free((void**)config.dependencies, config.dep_count);
	}
	config.dep_count=0;

	if(config.env_var_count > 0){
		loop_free((void**)config.env_vars, config.env_var_count);
		loop_free((void**)config.env_type, config.env_var_count);
	}
	config.env_var_count=0;*/
}


//系统库打包===========================================================================

TpLibPackageInfo::TpLibPackageInfo()
{

}

TpLibPackageInfo::~TpLibPackageInfo()
{

}

void TpLibPackageInfo::SetArchitecture(const TpString& architecture)
{
	params.architecture=architecture;
}
void TpLibPackageInfo::SetDiskSpace(int size)
{
	params.diskspace = size;
}

int TpLibPackageInfo::AddLibrary(const TpString& lib,uint8_t ver_x,uint8_t ver_y,uint8_t ver_z)
{
	params.systemLib.emplace_back(lib);
	TpVersion ver{.x=ver_x, .y=ver_y, .z=ver_z};
	params.version.emplace_back(ver);
	return 0;
}

int TpLibPackageInfo::AddFile(const TpString& file)
{
	params.file.emplace_back(file);
	return 0;
}

int TpLibPackageInfo::Save(const TpString &path)
{
	char *path_c=(char*)malloc(path.size() +1);
	std::strcpy(path_c,path.c_str());
	appm_creat_libpackage_config(path_c,&params);
	free(path_c);
}

void TpLibPackageInfo::ClassFree()
{
/*	if(params.lib_count >0)
		loop_free((void**)params.system_lib, params.lib_count);
	if(params.file_count >0)
		loop_free((void**)params.file, params.file_count);*/
}
