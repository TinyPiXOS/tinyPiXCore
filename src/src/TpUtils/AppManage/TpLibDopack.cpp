/*///------------------------------------------------------------------------------------------------------------------------//
        APP(库)打包
说 明 :
日 期 : 2024.11.05

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "AppmanageConf.h"
#include "InstallCheck.h"
#include "TpFileCreat.h"
#include "TpLibDopack.h"
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

struct TpLibDopackData
{
    struct LibPackageConfig params;
    TpLibDopackData() {}
};

TpLibDopack::TpLibDopack()
{
    data_ = new TpLibDopackData();
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);

    libData->params.systemLib.clear();
    libData->params.file.clear();
}

TpLibDopack::~TpLibDopack()
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);
    if (libData)
    {
        delete libData;
        libData = nullptr;
        data_ = nullptr;
    }
}

void TpLibDopack::setArchitecture(const TpString &architecture)
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);
    libData->params.architecture = architecture;
}

void TpLibDopack::setArchitecture(TpArchType type)
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);
    TpEnumArchType type_c;
    switch (type)
    {
    case TP_ARCH_TYPE_AMD64:
        type_c = TYPE_ARCH_AMD64;
        break;
    case TP_ARCH_TYPE_I386:
        type_c = TYPE_ARCH_I386;
        break;
    case TP_ARCH_TYPE_ARM64:
        type_c = TYPE_ARCH_ARM64;
        break;
    case TP_ARCH_TYPE_ARM32:
        type_c = TYPE_ARCH_ARM32;
        break;
    case TP_ARCH_TYPE_RISCV:
        type_c = TYPE_ARCH_RV64GC;
        break;
    default:
        type_c = TYPE_ARCH_NONE;
        break;
    }
    libData->params.arch = type_c;
}

void TpLibDopack::setDiskSpace(int size)
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);

    libData->params.diskspace = size;
}

int TpLibDopack::addLibrary(const TpString &lib, uint8_t ver_x, uint8_t ver_y, uint8_t ver_z)
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);

    if (libData->params.systemLib.size() >= MAX_ITEMS_LIB)
        return -1;

    //	std::string ver=std::to_string(ver_x)+"."+std::to_string(ver_y)+"."+std::to_string(ver_z);
    if (!libData->params.systemLib.contains(lib))
    {
        libData->params.systemLib.emplace_back(lib);
        libData->params.version[libData->params.systemLib.size() - 1].x = ver_x;
        libData->params.version[libData->params.systemLib.size() - 1].y = ver_y;
        libData->params.version[libData->params.systemLib.size() - 1].z = ver_z;
    }

    return 0;
}

int TpLibDopack::addFile(const TpString &file)
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);

    if (libData->params.file.size() >= MAX_ITEMS_LIB)
        return -1;

    if (!libData->params.file.contains(file))
    {
        libData->params.file.emplace_back(file);
    }

    return 0;
}

int TpLibDopack::save(const TpString &path)
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);
    appm_creat_libpackage_config(path.c_str(), &libData->params);
    return 0;
}
