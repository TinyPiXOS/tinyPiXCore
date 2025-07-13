/*///------------------------------------------------------------------------------------------------------------------------//
		APP(库)打包
说 明 :
日 期 : 2024.11.05

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <string>
#include <stdint.h>
#include <cstring>
#include <vector>
#include "file_creat.h"
#include "install_check.h"
#include "tpAppDopack.h"


//释放
static void loop_free(void **data,int count)
{
	for(int i=0;i<count;i++)
	{
		if(data[i])	free(data[i]);
	}
}

void free_ScriptInfo(struct ScriptInfo *config)
{
	if(!config)
		return;
	if(config->arg_count > 0){
		loop_free((void**)config->args, config->arg_count);
	}
	config->arg_count=0;

	if(config->dep_count > 0){
		loop_free((void**)config->dependencies, config->dep_count);
	}
	config->dep_count=0;

	if(config->env_var_count > 0){
		loop_free((void**)config->env_vars, config->env_var_count);
		loop_free((void**)config->env_type, config->env_var_count);
	}
	config->env_var_count=0;
}

//给路径的末尾添加“/”
static void addTrailingSlash(tpString& path) {
    if (!path.empty() && path.back() != '/') {
        path += '/';  // 等价于 path.push_back('/')
    }
}




struct tpAppDopackData{
    tpString path_s;		//安装包相关文件生成的位置
	tpString name;			//安装包名称
    struct AppPackageConfig params;	//安装包配置
	TypePackage type;				//安装包类型
	struct ScriptInfo config;	//启动脚本参数
	tpAppDopackData(){
		type=TYPE_PACKAGE_DEFAULT;
	}
};


tpAppDopack::tpAppDopack() 
{
	data_ = new tpAppDopackData();
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	struct AppPackageConfig *params=&adpData->params;

//	std::strncpy(params->app_id, appId.c_str(), sizeof(params->app_id) - 1);
//	std::strncpy(params->app_name, appName.c_str(), sizeof(params->app_name) - 1);
//	std::strncpy(params->version, version.c_str(), sizeof(params->version) - 1);

	adpData->type=TYPE_PACKAGE_NONE;
	params->description=NULL;
	params->icon=NULL;
	params->appexec_name=NULL;
	params->signature=NULL;
	params->diskspace=0;
	params->version.x=0;
	params->version.y=0;
	params->version.z=0;

	// Initialize other parameters with empty strings
	std::memset(params->app_id, 0, sizeof(params->app_id));
	std::memset(params->app_name, 0, sizeof(params->app_name));
//	std::memset(params->version, 0, sizeof(params->version));
	std::memset(params->architecture, 0, sizeof(params->architecture));
	std::memset(params->section, 0, sizeof(params->section));
	std::memset(params->priority, 0, sizeof(params->priority));
	std::memset(params->essential, 0, sizeof(params->essential));
	std::memset(params->author, 0, sizeof(params->author));
	std::memset(params->provides, 0, sizeof(params->provides));
//	std::memset(params->description, 0, sizeof(params->description));
	std::memset(params->depend, 0, sizeof(params->depend));
	std::memset(params->lib, 0, sizeof(params->lib));
	std::memset(params->assert, 0, sizeof(params->assert));
//	std::memset(params->icon, 0, sizeof(params->icon));
//	std::memset(params->start, 0, sizeof(params->start));
//	std::memset(params->remove, 0, sizeof(params->remove));
	std::memset(params->otherfile, 0, sizeof(params->otherfile));
	std::memset(params->file_extension, 0, sizeof(params->file_extension));

	params->otherfile_count = 0;
	params->lib_count = 0;
	params->depend_count = 0;
	params->assert_count = 0;
	params->extension_count = 0;

	memset(&adpData->config,0,sizeof(struct ScriptInfo));
}

tpAppDopack::~tpAppDopack() {
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	classFree();
}


//安装包类型
int tpAppDopack::setPackageType(tpPackageType pack_type) 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	switch(pack_type) {
		case TP_PACKAGE_TYPE_APP:
			adpData->type=TYPE_PACKAGE_APP;
			break;
		case TP_PACKAGE_TYPE_SAPP:
			adpData->type=TYPE_PACKAGE_SAPP;
			break;
		default:
			adpData->type=TYPE_PACKAGE_NONE;
			break;
	}
	return 0;
}

//UUID/APPID
void tpAppDopack::setAppID(const tpString& id)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	std::strncpy(adpData->params.app_id, id.c_str(), sizeof(adpData->params.app_id) - 1);
}
void tpAppDopack::setAppID(const tpUUID id)
{
		tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
}

//APP NAME
void tpAppDopack::setAppName(const tpString& name)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	std::strncpy(adpData->params.app_name, name.c_str(), sizeof(adpData->params.app_name) - 1);
}
//版本
void tpAppDopack::setVersion(tpUInt8 x,tpUInt8 y,tpUInt8 z){
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
//	std::strncpy(adpData->params.version, version.c_str(), sizeof(adpData->params.version) - 1);
	adpData->params.version.x=x;
	adpData->params.version.y=y;
	adpData->params.version.z=z;
}
//硬件平台
void tpAppDopack::setArchitecture(const tpString& architecture) {
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	std::strncpy(adpData->params.architecture, architecture.c_str(), sizeof(adpData->params.architecture) - 1);
}

void tpAppDopack::setSection(const tpString& section) {
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	std::strncpy(adpData->params.section, section.c_str(), sizeof(adpData->params.section) - 1);
}

void tpAppDopack::setPriority(const tpString& priority) {
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	std::strncpy(adpData->params.priority, priority.c_str(), sizeof(adpData->params.priority) - 1);
}

void tpAppDopack::setEssential(const tpString& essential) {
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	std::strncpy(adpData->params.essential, essential.c_str(), sizeof(adpData->params.essential) - 1);
}
//作者信息，Name
void tpAppDopack::setAuthor(const tpString& author) {
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	std::strncpy(adpData->params.author, author.c_str(), sizeof(adpData->params.author) - 1);
}
//作者联系方式,email
void tpAppDopack::setContact(const tpString& contact) {
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
    std::strncpy(adpData->params.contact, contact.c_str(), sizeof(adpData->params.contact) - 1);
}

void tpAppDopack::setProvides(const tpString& provides) {
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	std::strncpy(adpData->params.provides, provides.c_str(), sizeof(adpData->params.provides) - 1);
}
//组织，公司
void tpAppDopack::setOrganization(const tpString& organization){
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	std::strncpy(adpData->params.organization, organization.c_str(), sizeof(adpData->params.organization) - 1);
}
//安装所需空间
void tpAppDopack::setDiskSpace(int size){
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	adpData->params.diskspace = size;
}
//应用描述
int tpAppDopack::setDescription(const tpString& description) 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	if((adpData->params.description = (char *)malloc(description.size() + 1))==NULL)
		return -1;
	printf("addr desc:%p\n",adpData->params.description);
	std::strcpy(adpData->params.description, description.c_str());
	return 0;
}

//数字签名
int tpAppDopack::setSignature(const tpString& signature) 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
    if((adpData->params.signature = (char *)malloc(signature.size() + 1))==NULL)
        return -1;
    std::strcpy(adpData->params.signature, signature.c_str());
    return 0;
}
//开源库：传入格式:libname@version
int tpAppDopack::addDepend(const tpString& depend,tpUInt8 ver_x,tpUInt8 ver_y,tpUInt8 ver_z) 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	struct AppPackageConfig *params=&adpData->params;

	tpString depend_ver = depend+"@"+tpString::number(ver_x)+"."+tpString::number(ver_y)+"."+tpString::number(ver_z);
	if(params->depend_count==MAX_ITEMS)
		return -1;
	if((params->depend[params->depend_count] = (char *)malloc(depend_ver.size() + 1))==NULL)
		return -1;
	std::strcpy(params->depend[params->depend_count], depend_ver.c_str());
	params->depend_count++;
	//在启动脚本中添加此依赖库
	addStartDepend(depend);
	return 0;
}
//私有库:传入路径
int tpAppDopack::addLib(const tpString& lib) 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	struct AppPackageConfig *params=&adpData->params;
	if(adpData->params.lib_count==MAX_ITEMS)
		return -1;
	if((adpData->params.lib[adpData->params.lib_count] = (char *)malloc(lib.size() + 1))==NULL)
	    return -1;
	std::strcpy(adpData->params.lib[adpData->params.lib_count], lib.c_str());
	adpData->params.lib_count++;
	return 0;
}
//图标
int tpAppDopack::setIcon(const tpString& icon) 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	if((adpData->params.icon = (char *)malloc(icon.size() + 1))==NULL)
	    return -1;
    std::strcpy(adpData->params.icon, icon.c_str());
	return 0;
}
//可执行文件路径
int tpAppDopack::setAppPath(const tpString& app) 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);

	if((adpData->params.appexec_name = (char *)malloc(app.size() + 1))==NULL)
		return -1;
	std::strcpy(adpData->params.appexec_name, app.c_str());
	return 0;
}
//静态文件
int tpAppDopack::addAssert(const tpString& assert)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	if(adpData->params.assert_count==MAX_ITEMS)
		return -1;
	if((adpData->params.assert[adpData->params.assert_count] = (char *)malloc(assert.size() + 1))==NULL)
	    return -1;
	std::strcpy(adpData->params.assert[adpData->params.assert_count], assert.c_str());
	adpData->params.assert_count++;
	return 0;
}

//可执行文件
int tpAppDopack::addBin(const tpString& bin)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	if(adpData->params.bin_count==MAX_ITEMS)
		return -1;
	if((adpData->params.bin[adpData->params.bin_count] = (char *)malloc(bin.size() + 1))==NULL)
	    return -1;
	std::strcpy(adpData->params.bin[adpData->params.bin_count], bin.c_str());
	adpData->params.bin_count++;
	return 0;
}

//其他文件
int tpAppDopack::addFile(const tpString& file)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	if(adpData->params.otherfile_count==MAX_ITEMS)
		return -1;
	if((adpData->params.otherfile[adpData->params.otherfile_count] = (char *)malloc(file.size() + 1))==NULL)
	    return -1;
	std::strcpy(adpData->params.otherfile[adpData->params.otherfile_count], file.c_str());
	adpData->params.otherfile_count++;
	return 0;
}
//支持的文件后缀
int tpAppDopack::addExtension(const tpString& type)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	if(adpData->params.extension_count==MAX_ITEMS)
		return -1;
	if((adpData->params.file_extension[adpData->params.extension_count] = (char *)malloc(type.size() + 1))==NULL)
	    return -1;
	std::strcpy(adpData->params.file_extension[adpData->params.extension_count], type.c_str());
	adpData->params.extension_count++;
	return 0;
}

/*
void Configurator::setStart(const std::string& start) {
    std::strncpy(adpData->params.start, start.c_str(), sizeof(adpData->params.start) - 1);
}
*/
/*
void Configurator::setRemove(const std::string& remove) {
    std::strncpy(adpData->params.remove, remove.c_str(), sizeof(adpData->params.remove) - 1);
}
*/

//安装包的名字
int tpAppDopack::setPackageName(const tpString& name)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	adpData->name=name;
	return 0;
}

//生成安装包
int tpAppDopack::creatPackage(const tpString& path)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	struct ScriptInfo *config=&adpData->config;	//启动脚本参数
	if(adpData->name.empty())
	{
		fprintf(stderr,"Installation package name not set\n");
		return -1;
	}
	adpData->path_s=path;
	addTrailingSlash(adpData->path_s);	//保证末尾是“/”

	tpString path_source=adpData->path_s + adpData->name;				//	<path>/appname
	char *path_source_c=(char*)malloc(path_source.size() + 1);		
	std::strcpy(path_source_c, path_source.c_str());

	if(appm_generate_package_source(&adpData->params,path_source_c,adpData->type)<0){
		std::cerr<<"Error: Creat error"<<std::endl;
		free(path_source_c);
		return -1;
	}

	//启动脚本
	tpString path_start=path_source+"/start.sh";							//	<path>/appname/start.sh
	char *path_start_c=(char *)malloc(path_start.size()+1);
	std::strcpy(path_start_c, path_start.c_str());
	appm_generate_startup_script(config, path_start_c);
	free(path_start_c);


	tpString path_package=path_source + PACKAGE_FILE_SUFFIX;			//	<path>/appname.pik
	char *path_package_c=(char*)malloc(path_package.size() + 1);		
	std::strcpy(path_package_c, path_package.c_str());
	appm_creat_package_path(path_source_c,path_package_c);

	free(path_source_c);
	free(path_package_c);
	return 0;
}

void tpAppDopack::classFree() 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	struct AppPackageConfig *params=&adpData->params;

    if(params->description)	free(params->description);
	if(params->icon)	free(params->icon);
	if(params->appexec_name) free(params->appexec_name);
	if(params->signature) free(params->signature);
	loop_free((void **)params->depend, params->depend_count);
	params->depend_count=0;
	loop_free((void **)params->lib, params->lib_count);
	params->lib_count=0;
	loop_free((void **)params->assert, params->assert_count);
	params->assert_count=0;
	loop_free((void **)params->otherfile, params->otherfile_count);
	params->otherfile_count=0;

	free_ScriptInfo(&adpData->config);
}


// 添加环境变量
int tpAppDopack::addEnvironmentVar(const tpString& key, const tpString& value) 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	struct ScriptInfo *config=&adpData->config;	//启动脚本参数

	if (config->env_var_count == MAX_ITEMS) 
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
	config->env_type[config->env_var_count] = entry_k;
	config->env_vars[config->env_var_count] = entry_v;
	config->env_var_count++;
	return 0;
}

// 添加依赖库（一般是系统通用的库）
//库名字
int tpAppDopack::addStartDepend(const tpString& lib) 
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	struct ScriptInfo *config=&adpData->config;	//启动脚本参数

	if (config->dep_count == MAX_ITEMS) 
		return -1;
	if((config->dependencies[config->dep_count]=(char*)malloc(lib.size() + 1))==NULL)
		return -1;
	std::strcpy(config->dependencies[config->dep_count], lib.c_str());
	config->dep_count++;
	return 0;
}

// 添加启动参数
int tpAppDopack::addStartArg(const tpString& arg)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	struct ScriptInfo *config=&adpData->config;	//启动脚本参数

	if (config->arg_count == MAX_ITEMS)
		return -1;
	if((config->args[config->arg_count]=(char*)malloc(arg.size() + 1))==NULL)
		return -1;
	std::strcpy(config->args[config->arg_count], arg.c_str());
	config->arg_count++;
	return 0;
}

//添加可执行文件名称
int tpAppDopack::setExecPath(const tpString& name)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);
	struct ScriptInfo *config=&adpData->config;	//启动脚本参数

	std::strncpy(config->exec_path, name.c_str(), sizeof(config->exec_path) - 1);
	return 0;
}

int tpAppDopack::getAllConfig(const tpString& path_json)
{
	tpAppDopackData *adpData = static_cast<tpAppDopackData *>(data_);

	char *path_json_c=(char *)malloc(path_json.size()+1);
	std::strcpy(path_json_c, path_json.c_str());
	
	appm_analysis_dopack_json(path_json_c, &adpData->params, &adpData->config);

	free(path_json_c);
	return 0;
}

