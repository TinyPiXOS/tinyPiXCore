#ifndef __TP_LIB_DOPACK_H
#define __TP_LIB_DOPACK_H

#include <TpCore.h>
#include "TpString.h"

TP_DEF_VOID_TYPE_VAR(ITpLibDopackData);

class TpLibDopack
{
public:
    enum TpPackageType
    {
        TP_PACKAGE_TYPE_DEFAULT,
        TP_PACKAGE_TYPE_LIB
    };
    enum TpArchType
    {
        TP_ARCH_TYPE_AMD64,
        TP_ARCH_TYPE_I386,
        TP_ARCH_TYPE_ARM64,
        TP_ARCH_TYPE_ARM32,
        TP_ARCH_TYPE_RISCV
    };

public:
    TpLibDopack();
    virtual ~TpLibDopack();

public:
    void setArchitecture(const TpString &architecture);
    void setArchitecture(TpArchType arch);
    void setDiskSpace(int size);
    int addLibrary(const TpString &name, uint8_t x, uint8_t y, uint8_t z);
    int addFile(const TpString &file);
    int save(const TpString &path);

private:
    ITpLibDopackData *data_;
};

#endif