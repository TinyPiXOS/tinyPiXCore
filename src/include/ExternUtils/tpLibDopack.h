#ifndef __TP_LIB_DOPACK_H
#define __TP_LIB_DOPACK_H

#include "tpUtils.h"
#include "tpString.h"

TP_DEF_VOID_TYPE_VAR(ItpLibDopackData);

class tpLibDopack{
public:
	enum tpPackageType{
		TP_PACKAGE_TYPE_DEFAULT,
		TP_PACKAGE_TYPE_LIB	
	};
	enum tpArchType{
		TP_ARCH_TYPE_AMD64,
		TP_ARCH_TYPE_I386,
		TP_ARCH_TYPE_ARM64,
		TP_ARCH_TYPE_ARM32,
		TP_ARCH_TYPE_RISCV
	};

public:
	tpLibDopack();
	virtual ~tpLibDopack();

public:
	void setArchitecture(const tpString& architecture);
	void setArchitecture(tpArchType arch);
	void setDiskSpace(int size);
	int addLibrary(const tpString& name,uint8_t x,uint8_t y,uint8_t z);
	int addFile(const tpString& file);
	int save(const tpString& path);
	
private:
	void classFree();
	ItpLibDopackData *data_;
};

#endif