#ifndef __TP_ARCHITECTURE_H
#define __TP_ARCHITECTURE_H

#include "tpString.h"


class {
public:
	enum tpArchType{
		TP_ARCH_TYPE_AMD64,
		TP_ARCH_TYPE_I386,
		TP_ARCH_TYPE_ARM64,
		TP_ARCH_TYPE_ARM32,
		TP_ARCH_TYPE_RISCV
	};

public:
	getArchString(enum tpArchType);


private:

};

#endif