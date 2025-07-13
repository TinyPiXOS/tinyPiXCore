#ifndef __TP_HOST_ADDRESS_H
#define __TP_HOST_ADDRESS_H

#include "tpString.h"

class tpHostAddress
{
public:
	enum SpecialAddress{
		Any,
	};
public:
	tpHostAddress(SpecialAddress addr);
	~tpHostAddress();
	
};





#endif
