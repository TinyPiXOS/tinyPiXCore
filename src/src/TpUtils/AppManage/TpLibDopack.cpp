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
#include "appmanage_conf.h"
#include "InstallCheck.h"
#include "TpFileCreat.h"
#include "TpLibDopack.h"
#include "appm_creat.h"

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

    libData->params.lib_count = 0;
    libData->params.file_count = 0;
}

TpLibDopack::~TpLibDopack()
{
}

void TpLibDopack::setArchitecture(const TpString &architecture)
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);

    std::strncpy(libData->params.architecture, architecture.c_str(), sizeof(libData->params.architecture) - 1);
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

    if (libData->params.lib_count == MAX_ITEMS_LIB)
        return -1;
    if ((libData->params.system_lib[libData->params.lib_count] = (char *)malloc(lib.size() + 1)) == NULL)
        return -1;
    //	std::string ver=std::to_string(ver_x)+"."+std::to_string(ver_y)+"."+std::to_string(ver_z);
    std::strcpy(libData->params.system_lib[libData->params.lib_count], lib.c_str());
    libData->params.version[libData->params.lib_count].x = ver_x;
    libData->params.version[libData->params.lib_count].y = ver_y;
    libData->params.version[libData->params.lib_count].z = ver_z;
    // std::strcpy(params.version[params.lib_count],ver.c_str());
    libData->params.lib_count++;
    return 0;
}

int TpLibDopack::addFile(const TpString &file)
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);

    if (libData->params.file_count == MAX_ITEMS_LIB)
        return -1;
    if ((libData->params.file[libData->params.file_count] = (char *)malloc(file.size() + 1)) == NULL)
        return -1;
    std::strcpy(libData->params.file[libData->params.file_count], file.c_str());
    libData->params.file_count++;
    return 0;
}

int TpLibDopack::save(const TpString &path)
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);

    char *path_c = (char *)malloc(path.size() + 1);
    std::strcpy(path_c, path.c_str());
    appm_creat_libpackage_config(path_c, &libData->params);
    free(path_c);
    return 0;
}

void TpLibDopack::classFree()
{
    TpLibDopackData *libData = static_cast<TpLibDopackData *>(data_);

    if (libData->params.lib_count > 0)
        loop_free((void **)libData->params.system_lib, libData->params.lib_count);
    if (libData->params.file_count > 0)
        loop_free((void **)libData->params.file, libData->params.file_count);
}
