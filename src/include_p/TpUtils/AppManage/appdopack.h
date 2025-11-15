#ifndef _APP_DOPACK_H
#define _APP_DOPACK_H

#include <string>
#include <stdint.h>
#include <vector>
#include "TpFileCreat.h"

class TpPackageInfo
{
public:
    virtual ~TpPackageInfo();
    TpPackageInfo();

public:
    int SetPackageType(int type);
    void SetAppID(const TpString &id);
    void SetAppName(const TpString &name);
    void SetVersion(uint8_t x, uint8_t y, uint8_t z);
    void SetArchitecture(const TpString &architecture);
    void SetSection(const TpString &section);
    void SetPriority(const TpString &priority);
    void SetEssential(const TpString &essential);
    void SetAuthor(const TpString &author);
    void SetContact(const TpString &contact);
    void SetProvides(const TpString &provides);
    void SetDiskSpace(int size);
    int SetDescription(const TpString &description);
    int SetSignature(const TpString &sig);
    int AddDepend(const TpString &depend);
    int AddLib(const TpString &lib);
    int SetIcon(const TpString &icon);
    int AddAssert(const TpString &assert);
    int AddFile(const TpString &file);
    int AddExtension(const TpString &type);
    int SetStart(const TpString &start);
    int SetRemove(const TpString &remove);
    int SetAppPath(const TpString &app);
    //	int SetMyfile(const TpString& myfile);
    int Save(const TpString &file);
    int CreatPackage(const TpString &package);

    void ClassFree();

private:
    TpString path_s; // 原始打包文件生成位置
    struct AppPackageConfig params;
    TypePackage type;
};

class TpStartShInfo
{
public:
    TpStartShInfo();
    virtual ~TpStartShInfo();

public:
    int AddEnvironmentVar(const TpString &key, const TpString &value);
    int AddDependency(const TpString &lib);
    int AddStartArg(const TpString &arg);
    int SetLogFile(const TpString &log_file);
    int SetConfigFile(const TpString &config_file);
    int SetExecPath(const TpString &name);

    int Save(const TpString &path);
    void ClassFree();

private:
    struct ScriptInfo config;
};

class TpLibPackageInfo
{
public:
    TpLibPackageInfo();
    virtual ~TpLibPackageInfo();

public:
    void SetArchitecture(const TpString &architecture);
    void SetDiskSpace(int size);
    int AddLibrary(const TpString &name, uint8_t ver_x, uint8_t ver_y, uint8_t ver_z);
    int AddFile(const TpString &file);
    int Save(const TpString &path);
    void ClassFree();

private:
    struct LibPackageConfig params;
};

#endif // CONFIGURATOR_H
