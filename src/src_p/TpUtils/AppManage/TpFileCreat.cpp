/*///------------------------------------------------------------------------------------------------------------------------//
        安装包中必要文件的生成
说 明 :
日 期 : 2024.9.2

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
#include "TpFileCreat.h"
#include "appm_creat.h"
#include "AppManage/install.h"
#include "AppManage/utilslib.h"
#include "TpFile.h"
#include "TpFileInfo.h"
#include "TpDir.h"

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

// 使用TpFile拷贝单个文件
bool fileCopySingle(const TpString &destDir, const TpString &sourceFile)
{
    if (sourceFile.empty())
        return false;

    if (!TpFile::exists(sourceFile))
        return false;

    // 构建目标文件路径
    TpString destPath = destDir + "/" + TpFileInfo::fileName(sourceFile);

    // 使用TpFile的静态copy方法
    return TpFile::copy(sourceFile, destPath);
}

// 使用TpFile拷贝文件列表到指定目录
bool fileCopyList(const TpString &destDir, const TpVector<TpString> &fileList)
{
    if (fileList.isEmpty())
        return true;

    bool success = true;
    for (int i = 0; i < fileList.size(); i++)
    {
        if (!fileCopySingle(destDir, fileList[i]))
        {
            success = false;
            // 记录错误日志，但继续处理其他文件
        }
    }
    return success;
}

// 生成安装包中的配置文件
int fileConfigCreate(const TpString &path, const AppPackageConfig &conf, TypePackage type)
{
    TpString filename = TpString(path) + "/config";
    TpFile file(filename);

    if (!file.open(TpFile::WriteOnly))
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // 安装包类型
    switch (type)
    {
    case TYPE_PACKAGE_APP:
        file.write(TpString(PACKAGE_TYPE_CONFIG_UAPP) + "\n");
        break;
    case TYPE_PACKAGE_SAPP:
        file.write(TpString(PACKAGE_TYPE_CONFIG_SAPP) + "\n");
        break;
    default:
        file.write(TpString(PACKAGE_TYPE_CONFIG_UAPP) + "\n");
        break;
    }

    file.write("appID:" + conf.app_id + "\n");
    file.write("appName:" + conf.app_name + "\n");
    file.write("organization:" + conf.organization + "\n");

    // 格式化版本号
    TpString versionStr = TpString("version:") + TpString::number(conf.version.x) +
                          "." + TpString::number(conf.version.y) +
                          "." + TpString::number(conf.version.z);
    file.write(versionStr);

    if (!conf.appexec_name.empty())
    {
        int lastSlashPos = conf.appexec_name.lastIndexOf('/');
        if (lastSlashPos != -1)
        {
            TpString execName = conf.appexec_name.substr(lastSlashPos + 1);
            file.write("appexecName:" + execName + "\n");
        }
    }

    file.write("diskSpace:" + TpString::number(conf.diskspace) + "\n");
    file.write("architecture:" + conf.architecture + "\n");

    file.write("author:" + conf.author + " <" + conf.contact + ">\n");
    file.write("provides:" + conf.provides + "\n");

    // 支持的文件类型
    if (conf.file_extension.size() > 0)
    {
        file.write("fileExtension:");
        for (int i = 0; i < conf.file_extension.size(); i++)
        {
            file.write(conf.file_extension[i] + " ");
        }
        file.write("\n");
    }

    if (!conf.description.empty())
    {
        file.write("description:" + conf.description + "\n");
    }

    // 图标
    if (!conf.icon.empty())
    {
        int lastSlashPos = conf.icon.lastIndexOf('/');
        if (lastSlashPos != -1)
        {
            TpString iconName = conf.icon.substr(lastSlashPos + 1);
            file.write("export icon=./" + iconName + "\n");
        }
    }

    // 可执行文件
    if (!conf.appexec_name.empty())
    {
        int lastSlashPos = conf.appexec_name.lastIndexOf('/');
        if (lastSlashPos != -1)
        {
            TpString execPath = conf.appexec_name.substr(lastSlashPos);
            file.write("export appexec=." + execPath + "\n");
        }
    }

    // 依赖开源库
    if (conf.depend.size() > 0)
    {
        file.write("export depend=");
        for (int i = 0; i < conf.depend.size(); i++)
        {
            file.write(conf.depend[i] + " ");
        }
        file.write("\n");
    }

    file.write("export bin=./bin \n");
    file.write("export lib=./lib \n");
    file.write("export start=./start.sh\n");

    // 其他文件
    if (conf.otherfile.size() > 0)
    {
        file.write("export userfile=");
        for (int i = 0; i < conf.otherfile.size(); i++)
        {
            int lastSlashPos = conf.otherfile[i].lastIndexOf('/');
            if (lastSlashPos != -1)
            {
                TpString filePath = conf.otherfile[i].substr(lastSlashPos);
                file.write("." + filePath + " ");
            }
        }
        file.write("\n");
    }

    file.close();

    return 0;
}

int TpFileCreat::appm_generate_package_source(AppPackageConfig *config, const TpString &path, TypePackage type)
{
    // 检查配置

    // 生成目录
    TpDir::mkpath(path);

    // 根据结构体内容拷贝
    if (config->appexec_name.empty())
        return -1;

    fileCopySingle(path + "/bin", config->appexec_name);
    fileCopySingle(path, config->icon);
    fileCopyList(path + "/bin", config->bin);
    fileCopyList(path + "/lib", config->lib);
    fileCopyList(path + "/assert", config->assert);
    fileCopyList(path, config->otherfile);

    // 计算文件大小
    if (config->diskspace == 0)
    {
        long long total_size = TpDir::size(path);
        config->diskspace = total_size;
    }

    // 根据结构体内容写入配置文件
    fileConfigCreate(path, *config, type);

    return 0;
}

int TpFileCreat::appmGenerateStartupScript(ScriptInfo *config, const TpString &outputFile)
{
    TpFile file(outputFile);

    if (!file.open(TpFile::WriteOnly))
    {
        perror("Failed to create startup script");
        return -1;
    }

    // 写入
    file.write("#!/bin/bash\n\n");

    // 设置其他环境变量
    for (int i = 0; i < config->env_type.size(); i++)
    {
        TpString envLine = "export " + config->env_type.at(i) + "=$" +
                           config->env_type.at(i) + ":" + config->env_vars.at(i) + "\n";
        file.write(envLine);
    }

    // 设置环境变量中的依赖库路径
    file.write("\n# Dependencies\n");

    TpString libPath = "export LD_LIBRARY_PATH=./lib";
    for (int i = 0; i < config->dependencies.size(); i++)
    {
        libPath += ":" + config->dependencies.at(i);
    }
    libPath += ":" + TpString(PATH_SYSTEM_LIB);
    file.write(libPath);

    printf("startup script  depend ok\n");

    // 写入启动参数
    if (config->args.size() > 0)
    {
        file.write("\n# Startup Parameters\nARG=");
        for (int i = 0; i < config->args.size(); i++)
        {
            file.write(config->args.at(i) + " ");
        }
        file.write("\n");
    }

    // 写入执行路径
    file.write("\n# Execute Application\n");
    file.write("APP_PATH =\"./bin/" + config->exec_path);

    // 检查app是否存在
    file.write("\n" + TpString(CHECK_APP_OK) + "\n");

    // 启动应用
    file.write("./$APP_PATH \"$ARG\"\n");

    // 进程id打印
    file.write("\nPID=$!\n");
    file.write("echo \"应用程序PID: $PID\"\n");

    file.close();

    // 设置文件权限
    TpString command = "chmod 777 " + outputFile;
    if (system(command.c_str()) == -1)
        return -1;

    printf("startup script ok\n");
    return 0;
}

// 写入指令集类型
const char *architecture_map[] = {"none", "amd64", "i386", "arm64", "arm32", "risc_v"};
const char *TpFileCreat::get_architecture_string(TpEnumArchType arch)
{
    return architecture_map[arch];
}

void TpFileCreat::init_script_config(ScriptInfo *config)
{
    memset(config, 0, sizeof(ScriptInfo));
}

void TpFileCreat::add_env_var(ScriptInfo *config, const TpString &key, const TpString &value)
{
    TpString entry = key + "=" + value;
    if (config->env_vars.contains(entry))
        return;

    config->env_vars.emplace_back(entry);
}

void TpFileCreat::add_dependency(ScriptInfo *config, const char *lib)
{
    if (config->dependencies.contains(lib))
        return;
    config->dependencies.emplace_back(lib);
}

void TpFileCreat::add_arg(ScriptInfo *config, const char *arg)
{
    if (config->args.contains(arg))
        return;
    config->args.emplace_back(arg);
}

void TpFileCreat::set_log_file(ScriptInfo *config, const char *log_file)
{
    config->log_file = strdup(log_file);
}

void TpFileCreat::set_config_file(ScriptInfo *config, const char *config_file)
{
    config->config_file = strdup(config_file);
}

void TpFileCreat::set_exec_path(ScriptInfo *config, const char *exec_path)
{
}

int TpFileCreat::file_startsh_creat(char *path)
{
    return 0;
}

int TpFileCreat::file_config_creat_lib(archive *a, const char *path, LibPackageConfig *conf)
{
    //	char filename[MAX_PATH];
    //	memset(filename,0,MAX_PATH);
    //	snprintf(filename, MAX_PATH, "%s/config",path);
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        perror("Failed to open file\n");
        return -1;
    }
    fprintf(file, "#TinyPix SystemLib\n");

    fprintf(file, "architecture:%s\n", conf->architecture);
    //	fprintf(file, "architecture:%s\n", get_architecture_string(conf->arch));
    fprintf(file, "diskSpace:%u\n", conf->diskspace);
    char buf[MAX_LEN_APP_NAME];
    // 库文件
    if (conf->lib_count > 0)
    {
        char *lib_name;
        fprintf(file, "export lib=");
        for (int i = 0; i < conf->lib_count; i++)
        {
            TpFileInfo libPathInfo(conf->system_lib[i]);
            if (libPathInfo.isDir())
            {
                fclose(file);
                return -1;
            }
            if ((lib_name = strrchr(conf->system_lib[i], '/')) == NULL)
                lib_name = conf->system_lib[i];
            fprintf(file, "%s@%d.%d.%d ", lib_name, conf->version[i].x, conf->version[i].y, conf->version[i].z);
            snprintf(buf, sizeof(buf), ".%s", lib_name);
            add_file_to_archive(a, conf->system_lib[i], buf); // config打包
        }
        fprintf(file, "\n");
    }

    // 其他文件
    if (conf->file_count > 0)
    {
        char *file_name;
        fprintf(file, "export file=");
        for (int i = 0; i < conf->file_count; i++)
        {
            TpFileInfo confPathInfo(conf->file[i]);
            if (confPathInfo.isDir())
            {
                fclose(file);
                return -1;
            }
            if ((file_name = strrchr(conf->file[i], '/')) == NULL)
                file_name = conf->file[i];
            fprintf(file, "./%s ", file_name);
            snprintf(buf, sizeof(buf), ".%s", file_name);
            add_file_to_archive(a, conf->file[i], buf); // config打包
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}
