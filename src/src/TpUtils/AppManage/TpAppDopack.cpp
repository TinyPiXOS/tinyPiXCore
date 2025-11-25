/*///------------------------------------------------------------------------------------------------------------------------//
        APP(库)打包
说 明 :
日 期 : 2024.11.05

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <string>
#include <stdint.h>
#include <cstring>
#include <vector>
#include "TpFileCreat.h"
#include "InstallCheck.h"
#include "TpAppDopack.h"
#include "AppmCreat.h"

// 释放
static void loop_free(void **data, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (data[i])
            free(data[i]);
    }
}

// 给路径的末尾添加“/”
static void addTrailingSlash(TpString &path)
{
    if (!path.empty() && path.back() != '/')
    {
        path += '/'; // 等价于 path.push_back('/')
    }
}

struct TpAppDopackData
{
    TpString path_s;         // 安装包相关文件生成的位置
    TpString name;           // 安装包名称
    AppPackageConfig params; // 安装包配置
    TypePackage type;        // 安装包类型
    ScriptInfo config;       // 启动脚本参数
    TpAppDopackData()
    {
        type = TYPE_PACKAGE_DEFAULT;
    }
};

TpAppDopack::TpAppDopack()
{
    data_ = new TpAppDopackData();

    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    AppPackageConfig *params = &adpData->params;

    adpData->type = TYPE_PACKAGE_NONE;
    params->diskspace = 0;
    params->version.x = 0;
    params->version.y = 0;
    params->version.z = 0;
}

TpAppDopack::~TpAppDopack()
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    if (adpData)
    {
        delete adpData;
        adpData = nullptr;
        data_ = nullptr;
    }
}

// 安装包类型
int TpAppDopack::setPackageType(TpPackageType pack_type)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    switch (pack_type)
    {
    case TP_PACKAGE_TYPE_APP:
        adpData->type = TYPE_PACKAGE_APP;
        break;
    case TP_PACKAGE_TYPE_SAPP:
        adpData->type = TYPE_PACKAGE_SAPP;
        break;
    default:
        adpData->type = TYPE_PACKAGE_NONE;
        break;
    }
    return 0;
}

// UUID/APPID
void TpAppDopack::setAppID(const TpString &id)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.appID = id;
}

void TpAppDopack::setAppID(const TpUuid id)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
}

// APP NAME
void TpAppDopack::setAppName(const TpString &name)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.appName = name;
}

// 版本
void TpAppDopack::setVersion(tpUInt8 x, tpUInt8 y, tpUInt8 z)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    //	std::strncpy(adpData->params.version, version.c_str(), sizeof(adpData->params.version) - 1);
    adpData->params.version.x = x;
    adpData->params.version.y = y;
    adpData->params.version.z = z;
}
// 硬件平台
void TpAppDopack::setArchitecture(const TpString &architecture)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.architecture = architecture;
}

void TpAppDopack::setSection(const TpString &section)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.section = section;
}

void TpAppDopack::setPriority(const TpString &priority)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.priority = priority;
}

void TpAppDopack::setEssential(const TpString &essential)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.essential = essential;
}

// 作者信息，Name
void TpAppDopack::setAuthor(const TpString &author)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.author = author;
}

// 作者联系方式,email
void TpAppDopack::setContact(const TpString &contact)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.contact = contact;
}

void TpAppDopack::setProvides(const TpString &provides)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.provides = provides;
}

// 组织，公司
void TpAppDopack::setOrganization(const TpString &organization)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.organization = organization;
}

// 安装所需空间
void TpAppDopack::setDiskSpace(int size)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.diskspace = size;
}

// 应用描述
void TpAppDopack::setDescription(const TpString &description)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.description = description;
}

// 数字签名
void TpAppDopack::setSignature(const TpString &signature)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.signature = signature;
}

// 开源库：传入格式:libname@version
void TpAppDopack::addDepend(const TpString &depend, tpUInt8 ver_x, tpUInt8 ver_y, tpUInt8 ver_z)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    AppPackageConfig *params = &adpData->params;

    TpString depend_ver = depend + "@" + TpString::number(ver_x) + "." + TpString::number(ver_y) + "." + TpString::number(ver_z);

    if (params->depend.contains(depend_ver))
        return;

    params->depend.emplace_back(depend_ver);

    // 在启动脚本中添加此依赖库
    addStartDepend(depend);
}

// 私有库:传入路径
void TpAppDopack::addLib(const TpString &lib)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);

    if (adpData->params.lib.contains(lib))
        return;

    adpData->params.lib.emplace_back(lib);
}

void TpAppDopack::setIcon(const TpString &icon)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.icon = icon;
}

void TpAppDopack::setAppPath(const TpString &app)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->params.appexecName = app;
}

// 静态文件
void TpAppDopack::addAssert(const TpString &assert)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    if (adpData->params.assertFiles.contains(assert))
        return;
    adpData->params.assertFiles.emplace_back(assert);
}

// 可执行文件
void TpAppDopack::addBin(const TpString &bin)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    if (adpData->params.binFiles.contains(bin))
        return;
    adpData->params.binFiles.emplace_back(bin);
}

// 其他文件
void TpAppDopack::addFile(const TpString &file)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    if (adpData->params.otherFiles.contains(file))
        return;
    adpData->params.otherFiles.emplace_back(file);
}

// 支持的文件后缀
void TpAppDopack::addExtension(const TpString &type)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    if (adpData->params.fileExtension.contains(type))
        return;
    adpData->params.fileExtension.emplace_back(type);
}

// 安装包的名字
void TpAppDopack::setPackageName(const TpString &name)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->name = name;
}

// 生成安装包
void TpAppDopack::creatPackage(const TpString &path)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    struct ScriptInfo *config = &adpData->config; // 启动脚本参数
    if (adpData->name.empty())
    {
        fprintf(stderr, "Installation package name not set\n");
        return;
    }
    adpData->path_s = path;
    addTrailingSlash(adpData->path_s); // 保证末尾是“/”

    TpString path_source = adpData->path_s + adpData->name; //	<path>/appname

    if (TpFileCreat::appm_generate_package_source(&adpData->params, (char *)path_source.c_str(), adpData->type) < 0)
    {
        std::cerr << "Error: Creat error" << std::endl;
        return;
    }

    // 启动脚本
    TpString path_start = path_source + "/start.sh"; //	<path>/appname/start.sh
    TpFileCreat::appmGenerateStartupScript(config, path_start.c_str());

    TpString path_package = path_source + PACKAGE_FILE_SUFFIX; //	<path>/appname.pik
    TpAppmCreat::CreatPackageFile(path_source.c_str(), path_package.c_str());
}

// 添加环境变量
void TpAppDopack::addEnvironmentVar(const TpString &key, const TpString &value)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    ScriptInfo *config = &adpData->config; // 启动脚本参数

    if (!config->env_type.contains(key))
    {
        config->env_type.emplace_back(key);
    }
    if (!config->env_vars.contains(value))
    {
        config->env_vars.emplace_back(value);
    }
}

// 添加依赖库（一般是系统通用的库）
// 库名字
void TpAppDopack::addStartDepend(const TpString &lib)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    ScriptInfo *config = &adpData->config; // 启动脚本参数
    if (config->dependencies.contains(lib))
        return;
    config->dependencies.emplace_back(lib);
}

// 添加启动参数
void TpAppDopack::addStartArg(const TpString &arg)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    ScriptInfo *config = &adpData->config; // 启动脚本参数
    if (config->args.contains(arg))
        return;
    config->args.emplace_back(arg);
}

// 添加可执行文件名称
void TpAppDopack::setExecPath(const TpString &name)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    adpData->config.exec_path = name;
}

void TpAppDopack::getAllConfig(const TpString &path_json)
{
    TpAppDopackData *adpData = static_cast<TpAppDopackData *>(data_);
    TpAppmCreat::AnalysisDopackJson(path_json, &adpData->params, &adpData->config);
}
