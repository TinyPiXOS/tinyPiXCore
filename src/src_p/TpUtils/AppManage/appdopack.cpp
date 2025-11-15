/*///------------------------------------------------------------------------------------------------------------------------//
		应用安装报文件生成的c++接口
说 明 : 
日 期 : 2024.9.4

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include "appdopack.h"



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
	params.icon=NULL;
	params.appexec_name=NULL;
	params.signature=NULL;
	params.diskspace=0;
	params.version.x=0;
	params.version.y=0;
	params.version.z=0;

	// Initialize other parameters with empty strings
	std::memset(params.app_id, 0, sizeof(params.app_id));
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
	std::memset(params.file_extension, 0, sizeof(params.file_extension));

	params.otherfile_count = 0;
	params.lib_count = 0;
	params.depend_count = 0;
	params.assert_count = 0;
	params.extension_count = 0;
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
	std::strncpy(params.app_id, id.c_str(), sizeof(params.app_id) - 1);
}
//APP NAME
void TpPackageInfo::SetAppName(const TpString& name){
	std::strncpy(params.app_name, name.c_str(), sizeof(params.app_name) - 1);
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
	std::strncpy(params.architecture, architecture.c_str(), sizeof(params.architecture) - 1);
}

void TpPackageInfo::SetSection(const TpString& section) {
	std::strncpy(params.section, section.c_str(), sizeof(params.section) - 1);
}

void TpPackageInfo::SetPriority(const TpString& priority) {
	std::strncpy(params.priority, priority.c_str(), sizeof(params.priority) - 1);
}

void TpPackageInfo::SetEssential(const TpString& essential) {
	std::strncpy(params.essential, essential.c_str(), sizeof(params.essential) - 1);
}
//作者信息，Name
void TpPackageInfo::SetAuthor(const TpString& author) {
	std::strncpy(params.author, author.c_str(), sizeof(params.author) - 1);
}
//作者联系方式,email
void TpPackageInfo::SetContact(const TpString& contact) {
    std::strncpy(params.contact, contact.c_str(), sizeof(params.contact) - 1);
}
//组织，公司
void TpPackageInfo::SetProvides(const TpString& provides) {
	std::strncpy(params.provides, provides.c_str(), sizeof(params.provides) - 1);
}
//安装所需空间
void TpPackageInfo::SetDiskSpace(int size){
	params.diskspace = size;
}
//应用描述
int TpPackageInfo::SetDescription(const TpString& description) 
{
	if((params.description = (char *)malloc(description.size() + 1))==NULL)
		return -1;
	printf("addr desc:%p\n",params.description);
	std::strcpy(params.description, description.c_str());
	return 0;
}

//数字签名
int TpPackageInfo::SetSignature(const TpString& signature) 
{
    if((params.signature = (char *)malloc(signature.size() + 1))==NULL)
        return -1;
    std::strcpy(params.signature, signature.c_str());
    return 0;
}
//开源库：传入格式:libname@version
int TpPackageInfo::AddDepend(const TpString& depend) 
{
	if(params.depend_count==MAX_ITEMS)
		return -1;
	if((params.depend[params.depend_count] = (char *)malloc(depend.size() + 1))==NULL)
		return -1;
	std::strcpy(params.depend[params.depend_count], depend.c_str());
	params.depend_count++;
	return 0;
}
//私有库:传入路径
int TpPackageInfo::AddLib(const TpString& lib) 
{
	if(params.lib_count==MAX_ITEMS)
		return -1;
	if((params.lib[params.lib_count] = (char *)malloc(lib.size() + 1))==NULL)
	    return -1;
	std::strcpy(params.lib[params.lib_count], lib.c_str());
	params.lib_count++;
	return 0;
}
//图标
int TpPackageInfo::SetIcon(const TpString& icon) 
{
	if((params.icon = (char *)malloc(icon.size() + 1))==NULL)
	    return -1;
    std::strcpy(params.icon, icon.c_str());
	return 0;
}
//可执行文件路径
int TpPackageInfo::SetAppPath(const TpString& app) 
{
	if((params.appexec_name = (char *)malloc(app.size() + 1))==NULL)
		return -1;
	std::strcpy(params.appexec_name, app.c_str());
	return 0;
}
//静态文件
int TpPackageInfo::AddAssert(const TpString& assert)
{
	if(params.assert_count==MAX_ITEMS)
		return -1;
	if((params.assert[params.assert_count] = (char *)malloc(assert.size() + 1))==NULL)
	    return -1;
	std::strcpy(params.assert[params.assert_count], assert.c_str());
	params.assert_count++;
	return 0;
}
//其他文件
int TpPackageInfo::AddFile(const TpString& file)
{
	if(params.otherfile_count==MAX_ITEMS)
		return -1;
	if((params.otherfile[params.otherfile_count] = (char *)malloc(file.size() + 1))==NULL)
	    return -1;
	std::strcpy(params.otherfile[params.otherfile_count], file.c_str());
	params.otherfile_count++;
	return 0;
}
//支持的文件后缀
int TpPackageInfo::AddExtension(const TpString& type)
{
	if(params.extension_count==MAX_ITEMS)
		return -1;
	if((params.file_extension[params.extension_count] = (char *)malloc(type.size() + 1))==NULL)
	    return -1;
	std::strcpy(params.file_extension[params.extension_count], type.c_str());
	params.extension_count++;
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
	path_s=path;
	char *path_c=(char*)malloc(path.size() +1);
	memset(path_c, 0, path.size()+1);
	std::strcpy(path_c, path.c_str());
	if(appm_generate_package_source(&params,path_c,type)<0){
		std::cerr<<"Error: Creat error"<<std::endl;
		free(path_c);
		return -1;
	}
	free(path_c);
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
    if(params.description)	free(params.description);
	if(params.icon)	free(params.icon);
	if(params.appexec_name) free(params.appexec_name);
	if(params.signature) free(params.signature);
	loop_free((void **)params.depend, params.depend_count);
	params.depend_count=0;
	loop_free((void **)params.lib, params.lib_count);
	params.lib_count=0;
	loop_free((void **)params.assert, params.assert_count);
	params.assert_count=0;
	loop_free((void **)params.otherfile, params.otherfile_count);
	params.otherfile_count=0;
}


//启动文件部分==============================

TpStartShInfo::TpStartShInfo()
{
	config.env_var_count=0;
	config.dep_count=0;
	config.arg_count=0;
	memset(&config,0,sizeof(config));
}

TpStartShInfo::~TpStartShInfo()
{
	ClassFree();
}

// 添加环境变量
int TpStartShInfo::AddEnvironmentVar(const TpString& key, const TpString& value) 
{
	if (config.env_var_count == MAX_ITEMS) 
		return -1;
	char *entry_k = (char*)malloc(key.size() + 1); // +1 for'\0'
	char *entry_v = (char*)malloc(value.size() + 1); // +1
	if(entry_k == NULL)
	    return -1;
	if(entry_v == NULL)
		return -1;
	memset(entry_k, 0,key.size() + 1);
	memset(entry_v, 0, value.size() + 1);
//	printf("entry=%s,len=%d\n",entry,(int)(key.size() + value.size() + 2));
	std::strcpy(entry_k, key.c_str());
	std::strcat(entry_v, value.c_str());
	printf("export:%s=%s\n",entry_k,entry_v);
	config.env_type[config.env_var_count] = entry_k;
	config.env_vars[config.env_var_count] = entry_v;
	config.env_var_count++;
	return 0;
}

// 添加依赖库（一般是系统通用的库）
//库名字
int TpStartShInfo::AddDependency(const TpString& lib) 
{
	if (config.dep_count == MAX_ITEMS) 
		return -1;
	if((config.dependencies[config.dep_count]=(char*)malloc(lib.size() + 1))==NULL)
		return -1;
	std::strcpy(config.dependencies[config.dep_count], lib.c_str());
	config.dep_count++;
	return 0;
}

// 添加启动参数
int TpStartShInfo::AddStartArg(const TpString& arg)
{
	if (config.arg_count == MAX_ITEMS)
		return -1;
	if((config.args[config.arg_count]=(char*)malloc(arg.size() + 1))==NULL)
		return -1;
	std::strcpy(config.args[config.arg_count], arg.c_str());
	config.arg_count++;
	return 0;
}

//添加可执行文件名称
int TpStartShInfo::SetExecPath(const TpString& name)
{
	std::strncpy(config.exec_path, name.c_str(), sizeof(config.exec_path) - 1);
	return 0;
}

int TpStartShInfo::Save(const TpString& path) {

	char *path_c=(char*)malloc(path.size() +1);
	std::strcpy(path_c, path.c_str());
	printf("cvreat:%s\n", path_c);
	//std::cout << "creat:" << addr << std::endl;
	appmGenerateStartupScript(&config, path_c);
	free(path_c);
	return 0;
}


void TpStartShInfo::ClassFree()
{
	if(config.arg_count > 0){
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
	config.env_var_count=0;
}


//系统库打包===========================================================================

TpLibPackageInfo::TpLibPackageInfo()
{
	params.lib_count=0;
	params.file_count=0;
}

TpLibPackageInfo::~TpLibPackageInfo()
{

}

void TpLibPackageInfo::SetArchitecture(const TpString& architecture)
{
	std::strncpy(params.architecture, architecture.c_str(), sizeof(params.architecture) - 1);
}
void TpLibPackageInfo::SetDiskSpace(int size)
{
	params.diskspace = size;
}

int TpLibPackageInfo::AddLibrary(const TpString& lib,uint8_t ver_x,uint8_t ver_y,uint8_t ver_z)
{
	if (params.lib_count == MAX_ITEMS_LIB) 
		return -1;
	if((params.system_lib[params.lib_count]=(char*)malloc(lib.size() + 1))==NULL)
		return -1;
//	std::string ver=std::to_string(ver_x)+"."+std::to_string(ver_y)+"."+std::to_string(ver_z);
	std::strcpy(params.system_lib[params.lib_count], lib.c_str());
	params.version[params.lib_count].x=ver_x;
	params.version[params.lib_count].y=ver_y;
	params.version[params.lib_count].z=ver_z;
	//std::strcpy(params.version[params.lib_count],ver.c_str());
	params.lib_count++;
	return 0;
}

int TpLibPackageInfo::AddFile(const TpString& file)
{
	if (params.file_count == MAX_ITEMS_LIB) 
		return -1;
	if((params.file[params.file_count]=(char*)malloc(file.size() + 1))==NULL)
		return -1;
	std::strcpy(params.file[params.file_count], file.c_str());
	params.file_count++;
	return 0;

}

int TpLibPackageInfo::Save(const std::string& path)
{
	char *path_c=(char*)malloc(path.size() +1);
	std::strcpy(path_c,path.c_str());
	appm_creat_libpackage_config(path_c,&params);
	free(path_c);
}

void TpLibPackageInfo::ClassFree()
{
	if(params.lib_count >0)
		loop_free((void**)params.system_lib, params.lib_count);
	if(params.file_count >0)
		loop_free((void**)params.file, params.file_count);
}
